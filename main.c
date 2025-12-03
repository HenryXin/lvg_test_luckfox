#include "lvgl/lvgl.h"
#include "lv_demos/lv_demo.h"
#include "lv_drivers/display/fbdev.h"
//#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define DISP_BUF_SIZE (240 * 240)

/*Bouncing ball variables*/
static lv_obj_t * ball = NULL;
static int ball_x = 120;  /*Start at center*/
static int ball_y = 120;
static int ball_vx = 3;   /*Velocity in pixels per frame*/
static int ball_vy = 2;
#define BALL_SIZE 20
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

/*Timer callback to update ball position*/
static void ball_anim_timer(lv_timer_t * timer)
{
    if(ball == NULL) return;
    
    /*Update position*/
    ball_x += ball_vx;
    ball_y += ball_vy;
    
    /*Bounce off horizontal walls*/
    if(ball_x <= BALL_SIZE/2 || ball_x >= SCREEN_WIDTH - BALL_SIZE/2) {
        ball_vx = -ball_vx;
        /*Clamp position to prevent going out of bounds*/
        if(ball_x < BALL_SIZE/2) ball_x = BALL_SIZE/2;
        if(ball_x > SCREEN_WIDTH - BALL_SIZE/2) ball_x = SCREEN_WIDTH - BALL_SIZE/2;
    }
    
    /*Bounce off vertical walls*/
    if(ball_y <= BALL_SIZE/2 || ball_y >= SCREEN_HEIGHT - BALL_SIZE/2) {
        ball_vy = -ball_vy;
        /*Clamp position to prevent going out of bounds*/
        if(ball_y < BALL_SIZE/2) ball_y = BALL_SIZE/2;
        if(ball_y > SCREEN_HEIGHT - BALL_SIZE/2) ball_y = SCREEN_HEIGHT - BALL_SIZE/2;
    }
    
    /*Update ball position*/
    lv_obj_set_pos(ball, ball_x - BALL_SIZE/2, ball_y - BALL_SIZE/2);
}

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 240;
    disp_drv.ver_res    = 240;
    lv_disp_drv_register(&disp_drv);
#if 0
    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_1.read_cb = evdev_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);
#endif
    
#if 0
    /*Set a cursor for the mouse*/
    LV_IMG_DECLARE(mouse_cursor_icon)
    lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
    lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
    lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/
#endif
    
#if 1
    /*Create a Demo*/
    lv_demo_widgets();
#endif

    /*Create a bouncing ball*/
    ball = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ball, BALL_SIZE, BALL_SIZE);
    lv_obj_set_pos(ball, ball_x - BALL_SIZE/2, ball_y - BALL_SIZE/2);
    lv_obj_set_style_radius(ball, LV_RADIUS_CIRCLE, 0);  /*Make it perfectly circular*/
    lv_obj_set_style_bg_color(ball, lv_color_hex(0xFF0000), 0);  /*Set red color*/
    lv_obj_set_style_bg_opa(ball, LV_OPA_COVER, 0);  /*Make it opaque*/
    lv_obj_clear_flag(ball, LV_OBJ_FLAG_SCROLLABLE);  /*Disable scrolling for better performance*/
    
    /*Create animation timer (runs every 16ms for ~60fps)*/
    lv_timer_create(ball_anim_timer, 16, NULL);
    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
