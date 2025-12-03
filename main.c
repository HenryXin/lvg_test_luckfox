#include "lvgl/lvgl.h"
#include "lv_demos/lv_demo.h"
#include "lv_drivers/display/fbdev.h"
//#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#define DISP_BUF_SIZE (240 * 240)

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
#define NUM_SNOWFLAKES 30
#define GROUND_HEIGHT 40

/*Snowflake structure*/
typedef struct {
    lv_obj_t * obj;
    int x;
    int y;
    int speed;
    int size;
} snowflake_t;

static snowflake_t snowflakes[NUM_SNOWFLAKES];
static lv_obj_t * sky_bg = NULL;
static lv_obj_t * ground = NULL;

/*Function to create a snowman*/
static void create_snowman(int x, int y)
{
    lv_obj_t * part;
    
    /*Bottom circle (largest)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 50, 50);
    lv_obj_set_pos(part, x - 25, y - 50);
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Middle circle*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 40, 40);
    lv_obj_set_pos(part, x - 20, y - 90);
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Top circle (head)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 30, 30);
    lv_obj_set_pos(part, x - 15, y - 120);
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Left eye*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 4, 4);
    lv_obj_set_pos(part, x - 8, y - 115);
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Right eye*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 4, 4);
    lv_obj_set_pos(part, x + 4, y - 115);
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Carrot nose (triangle approximated as small rectangle)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 6, 4);
    lv_obj_set_pos(part, x - 3, y - 110);
    lv_obj_set_style_radius(part, 2, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFF8C00), 0);  /*Orange*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Button 1 (top)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 4, 4);
    lv_obj_set_pos(part, x - 2, y - 85);
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Button 2 (middle)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 4, 4);
    lv_obj_set_pos(part, x - 2, y - 75);
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Button 3 (bottom)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 4, 4);
    lv_obj_set_pos(part, x - 2, y - 65);
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Hat brim*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 35, 5);
    lv_obj_set_pos(part, x - 17, y - 125);
    lv_obj_set_style_radius(part, 2, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Hat top*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, 20, 15);
    lv_obj_set_pos(part, x - 10, y - 140);
    lv_obj_set_style_radius(part, 2, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
}

/*Timer callback to update snowflake positions*/
static void snow_anim_timer(lv_timer_t * timer)
{
    for(int i = 0; i < NUM_SNOWFLAKES; i++) {
        if(snowflakes[i].obj == NULL) continue;
        
        /*Move snowflake down*/
        snowflakes[i].y += snowflakes[i].speed;
        
        /*Add slight horizontal drift*/
        snowflakes[i].x += (snowflakes[i].speed % 3) - 1;
        
        /*Reset if snowflake reaches bottom*/
        if(snowflakes[i].y > SCREEN_HEIGHT - GROUND_HEIGHT) {
            snowflakes[i].y = -snowflakes[i].size;
            snowflakes[i].x = (snowflakes[i].x % SCREEN_WIDTH) + (rand() % SCREEN_WIDTH);
        }
        
        /*Wrap around horizontally*/
        if(snowflakes[i].x < 0) snowflakes[i].x = SCREEN_WIDTH;
        if(snowflakes[i].x > SCREEN_WIDTH) snowflakes[i].x = 0;
        
        /*Update snowflake position*/
        lv_obj_set_pos(snowflakes[i].obj, snowflakes[i].x - snowflakes[i].size/2, 
                       snowflakes[i].y - snowflakes[i].size/2);
    }
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
    
    /*Create snow scene background - dark blue sky*/
    sky_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sky_bg, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_HEIGHT);
    lv_obj_set_pos(sky_bg, 0, 0);
    lv_obj_set_style_bg_color(sky_bg, lv_color_hex(0x1a1a2e), 0);  /*Dark blue sky*/
    lv_obj_set_style_bg_opa(sky_bg, LV_OPA_COVER, 0);
    lv_obj_clear_flag(sky_bg, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Create snow ground*/
    ground = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ground, SCREEN_WIDTH, GROUND_HEIGHT);
    lv_obj_set_pos(ground, 0, SCREEN_HEIGHT - GROUND_HEIGHT);
    lv_obj_set_style_bg_color(ground, lv_color_hex(0xFFFFFF), 0);  /*White snow*/
    lv_obj_set_style_bg_opa(ground, LV_OPA_COVER, 0);
    lv_obj_clear_flag(ground, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Initialize snowflakes*/
    srand(time(NULL));
    for(int i = 0; i < NUM_SNOWFLAKES; i++) {
        snowflakes[i].size = 2 + (rand() % 4);  /*Size between 2-5 pixels*/
        snowflakes[i].x = rand() % SCREEN_WIDTH;
        snowflakes[i].y = -(rand() % SCREEN_HEIGHT);  /*Start at random heights above screen*/
        snowflakes[i].speed = 1 + (rand() % 3);  /*Speed between 1-3 pixels per frame*/
        
        /*Create snowflake object*/
        snowflakes[i].obj = lv_obj_create(lv_scr_act());
        lv_obj_set_size(snowflakes[i].obj, snowflakes[i].size, snowflakes[i].size);
        lv_obj_set_pos(snowflakes[i].obj, snowflakes[i].x - snowflakes[i].size/2, 
                       snowflakes[i].y - snowflakes[i].size/2);
        lv_obj_set_style_radius(snowflakes[i].obj, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(snowflakes[i].obj, lv_color_hex(0xFFFFFF), 0);  /*White*/
        lv_obj_set_style_bg_opa(snowflakes[i].obj, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(snowflakes[i].obj, 0, 0);  /*No border*/
        lv_obj_clear_flag(snowflakes[i].obj, LV_OBJ_FLAG_SCROLLABLE);
    }
    
    /*Create snowman on the ground (centered horizontally)*/
    create_snowman(SCREEN_WIDTH / 2, SCREEN_HEIGHT - GROUND_HEIGHT);
    
    /*Create animation timer (runs every 50ms for smooth animation)*/
    lv_timer_create(snow_anim_timer, 50, NULL);
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
