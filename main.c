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
#define NUM_SNOWFLAKES 0  /*No snowflakes in summer*/
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
static void create_snowman(int x, int y, float scale)
{
    lv_obj_t * part;
    
    /*Bottom circle (largest)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(50 * scale), (int)(50 * scale));
    lv_obj_set_pos(part, x - (int)(25 * scale), y - (int)(50 * scale));
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Middle circle*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(40 * scale), (int)(40 * scale));
    lv_obj_set_pos(part, x - (int)(20 * scale), y - (int)(90 * scale));
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Left arm (stick) - positioned diagonally - made thicker and more visible*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(4 * scale), (int)(22 * scale));
    lv_obj_set_pos(part, x - (int)(22 * scale), y - (int)(86 * scale));
    lv_obj_set_style_radius(part, (int)(2 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B4513), 0);  /*Brown*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Left arm extension (to make it look longer and angled)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(4 * scale), (int)(14 * scale));
    lv_obj_set_pos(part, x - (int)(32 * scale), y - (int)(77 * scale));
    lv_obj_set_style_radius(part, (int)(2 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B4513), 0);  /*Brown*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Right arm (stick) - positioned diagonally - made thicker and more visible*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(4 * scale), (int)(22 * scale));
    lv_obj_set_pos(part, x + (int)(18 * scale), y - (int)(86 * scale));
    lv_obj_set_style_radius(part, (int)(2 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B4513), 0);  /*Brown*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Right arm extension (to make it look longer and angled)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(4 * scale), (int)(14 * scale));
    lv_obj_set_pos(part, x + (int)(28 * scale), y - (int)(77 * scale));
    lv_obj_set_style_radius(part, (int)(2 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B4513), 0);  /*Brown*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Top circle (head)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(30 * scale), (int)(30 * scale));
    lv_obj_set_pos(part, x - (int)(15 * scale), y - (int)(120 * scale));
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Left eye - scaled and positioned correctly on head*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(4 * scale), (int)(4 * scale));
    lv_obj_set_pos(part, x - (int)(8 * scale), y - (int)(115 * scale));
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Right eye - scaled and positioned correctly on head*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(4 * scale), (int)(4 * scale));
    lv_obj_set_pos(part, x + (int)(4 * scale), y - (int)(115 * scale));
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Carrot nose (triangle approximated as small rectangle) - scaled*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(6 * scale), (int)(4 * scale));
    lv_obj_set_pos(part, x - (int)(3 * scale), y - (int)(110 * scale));
    lv_obj_set_style_radius(part, (int)(2 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFF8C00), 0);  /*Orange*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Button 1 (top) - scaled*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(4 * scale), (int)(4 * scale));
    lv_obj_set_pos(part, x - (int)(2 * scale), y - (int)(85 * scale));
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Button 2 (middle) - scaled*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(4 * scale), (int)(4 * scale));
    lv_obj_set_pos(part, x - (int)(2 * scale), y - (int)(75 * scale));
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Button 3 (bottom) - scaled*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(4 * scale), (int)(4 * scale));
    lv_obj_set_pos(part, x - (int)(2 * scale), y - (int)(65 * scale));
    lv_obj_set_style_radius(part, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Hat brim*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(35 * scale), (int)(5 * scale));
    lv_obj_set_pos(part, x - (int)(17 * scale), y - (int)(125 * scale));
    lv_obj_set_style_radius(part, (int)(2 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Hat top*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(20 * scale), (int)(15 * scale));
    lv_obj_set_pos(part, x - (int)(10 * scale), y - (int)(140 * scale));
    lv_obj_set_style_radius(part, (int)(2 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
}

/*Function to create a pine tree*/
static void create_tree(int x, int y, float scale)
{
    lv_obj_t * part;
    
    /*Tree trunk (brown rectangle)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(8 * scale), (int)(25 * scale));
    lv_obj_set_pos(part, x - (int)(4 * scale), y - (int)(25 * scale));
    lv_obj_set_style_radius(part, 1, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B4513), 0);  /*Brown*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Bottom foliage layer (largest - pine tree base)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(55 * scale), (int)(35 * scale));
    lv_obj_set_pos(part, x - (int)(27 * scale), y - (int)(60 * scale));
    lv_obj_set_style_radius(part, 0, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x228B22), 0);  /*Forest green*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Middle foliage layer*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(45 * scale), (int)(32 * scale));
    lv_obj_set_pos(part, x - (int)(22 * scale), y - (int)(92 * scale));
    lv_obj_set_style_radius(part, 0, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x228B22), 0);  /*Forest green*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Upper middle foliage layer*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(38 * scale), (int)(28 * scale));
    lv_obj_set_pos(part, x - (int)(19 * scale), y - (int)(120 * scale));
    lv_obj_set_style_radius(part, 0, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x228B22), 0);  /*Forest green*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Top foliage layer (smallest - pine tree tip)*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(32 * scale), (int)(25 * scale));
    lv_obj_set_pos(part, x - (int)(16 * scale), y - (int)(145 * scale));
    lv_obj_set_style_radius(part, 0, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x228B22), 0);  /*Forest green*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
}

/*Function to create a fireplace with logs*/
static void create_fireplace(int x, int y, float scale)
{
    lv_obj_t * part;
    
    /*Fireplace base/bottom*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(40 * scale), (int)(8 * scale));
    lv_obj_set_pos(part, x - (int)(20 * scale), y - (int)(8 * scale));
    lv_obj_set_style_radius(part, 0, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x696969), 0);  /*Gray stone*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Fireplace left wall*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(6 * scale), (int)(25 * scale));
    lv_obj_set_pos(part, x - (int)(20 * scale), y - (int)(33 * scale));
    lv_obj_set_style_radius(part, 0, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B7355), 0);  /*Brown brick*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Fireplace right wall*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(6 * scale), (int)(25 * scale));
    lv_obj_set_pos(part, x + (int)(14 * scale), y - (int)(33 * scale));
    lv_obj_set_style_radius(part, 0, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B7355), 0);  /*Brown brick*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Fireplace back wall*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(28 * scale), (int)(25 * scale));
    lv_obj_set_pos(part, x - (int)(14 * scale), y - (int)(33 * scale));
    lv_obj_set_style_radius(part, 0, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x654321), 0);  /*Dark brown*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Fireplace mantel/top*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(42 * scale), (int)(6 * scale));
    lv_obj_set_pos(part, x - (int)(21 * scale), y - (int)(39 * scale));
    lv_obj_set_style_radius(part, 0, 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x696969), 0);  /*Gray stone*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Log 1 - bottom log*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(20 * scale), (int)(6 * scale));
    lv_obj_set_pos(part, x - (int)(10 * scale), y - (int)(15 * scale));
    lv_obj_set_style_radius(part, (int)(3 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B4513), 0);  /*Brown wood*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Log 2 - middle log, slightly angled*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(18 * scale), (int)(5 * scale));
    lv_obj_set_pos(part, x - (int)(8 * scale), y - (int)(22 * scale));
    lv_obj_set_style_radius(part, (int)(2 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B4513), 0);  /*Brown wood*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Log 3 - top log*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(16 * scale), (int)(5 * scale));
    lv_obj_set_pos(part, x - (int)(6 * scale), y - (int)(28 * scale));
    lv_obj_set_style_radius(part, (int)(2 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0x8B4513), 0);  /*Brown wood*/
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Fire glow - orange/yellow glow behind logs*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(12 * scale), (int)(8 * scale));
    lv_obj_set_pos(part, x - (int)(6 * scale), y - (int)(18 * scale));
    lv_obj_set_style_radius(part, (int)(4 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFF4500), 0);  /*Orange red*/
    lv_obj_set_style_bg_opa(part, LV_OPA_70, 0);  /*Semi-transparent*/
    lv_obj_set_style_border_width(part, 0, 0);
    lv_obj_clear_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Fire center - brighter yellow/orange*/
    part = lv_obj_create(lv_scr_act());
    lv_obj_set_size(part, (int)(8 * scale), (int)(6 * scale));
    lv_obj_set_pos(part, x - (int)(4 * scale), y - (int)(19 * scale));
    lv_obj_set_style_radius(part, (int)(3 * scale), 0);
    lv_obj_set_style_bg_color(part, lv_color_hex(0xFFD700), 0);  /*Gold/yellow*/
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
    
    /*Create summer scene background - bright summer sky*/
    sky_bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(sky_bg, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_HEIGHT);
    lv_obj_set_pos(sky_bg, 0, 0);
    lv_obj_set_style_bg_color(sky_bg, lv_color_hex(0x87CEEB), 0);  /*Bright summer sky blue*/
    lv_obj_set_style_bg_opa(sky_bg, LV_OPA_COVER, 0);
    lv_obj_clear_flag(sky_bg, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Add white clouds*/
    lv_obj_t * cloud;
    /*Cloud 1 - left side*/
    cloud = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cloud, 30, 20);
    lv_obj_set_pos(cloud, 20, 30);
    lv_obj_set_style_radius(cloud, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cloud, lv_color_hex(0xFFFFFF), 0);  /*White*/
    lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cloud, 0, 0);
    lv_obj_clear_flag(cloud, LV_OBJ_FLAG_SCROLLABLE);
    
    cloud = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cloud, 25, 18);
    lv_obj_set_pos(cloud, 35, 25);
    lv_obj_set_style_radius(cloud, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cloud, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cloud, 0, 0);
    lv_obj_clear_flag(cloud, LV_OBJ_FLAG_SCROLLABLE);
    
    cloud = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cloud, 28, 19);
    lv_obj_set_pos(cloud, 48, 28);
    lv_obj_set_style_radius(cloud, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cloud, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cloud, 0, 0);
    lv_obj_clear_flag(cloud, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Cloud 2 - center*/
    cloud = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cloud, 35, 22);
    lv_obj_set_pos(cloud, 100, 40);
    lv_obj_set_style_radius(cloud, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cloud, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cloud, 0, 0);
    lv_obj_clear_flag(cloud, LV_OBJ_FLAG_SCROLLABLE);
    
    cloud = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cloud, 30, 20);
    lv_obj_set_pos(cloud, 115, 35);
    lv_obj_set_style_radius(cloud, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cloud, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cloud, 0, 0);
    lv_obj_clear_flag(cloud, LV_OBJ_FLAG_SCROLLABLE);
    
    cloud = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cloud, 32, 21);
    lv_obj_set_pos(cloud, 128, 38);
    lv_obj_set_style_radius(cloud, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cloud, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cloud, 0, 0);
    lv_obj_clear_flag(cloud, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Cloud 3 - right side*/
    cloud = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cloud, 28, 18);
    lv_obj_set_pos(cloud, 180, 25);
    lv_obj_set_style_radius(cloud, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cloud, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cloud, 0, 0);
    lv_obj_clear_flag(cloud, LV_OBJ_FLAG_SCROLLABLE);
    
    cloud = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cloud, 25, 17);
    lv_obj_set_pos(cloud, 195, 22);
    lv_obj_set_style_radius(cloud, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cloud, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cloud, 0, 0);
    lv_obj_clear_flag(cloud, LV_OBJ_FLAG_SCROLLABLE);
    
    cloud = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cloud, 27, 19);
    lv_obj_set_pos(cloud, 208, 24);
    lv_obj_set_style_radius(cloud, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(cloud, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(cloud, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(cloud, 0, 0);
    lv_obj_clear_flag(cloud, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Create grass ground*/
    ground = lv_obj_create(lv_scr_act());
    lv_obj_set_size(ground, SCREEN_WIDTH, GROUND_HEIGHT);
    lv_obj_set_pos(ground, 0, SCREEN_HEIGHT - GROUND_HEIGHT);
    lv_obj_set_style_bg_color(ground, lv_color_hex(0x228B22), 0);  /*Forest green grass*/
    lv_obj_set_style_bg_opa(ground, LV_OPA_COVER, 0);
    lv_obj_clear_flag(ground, LV_OBJ_FLAG_SCROLLABLE);
    
    /*Create multiple trees on the ground - varying sizes for depth*/
    create_tree(30, SCREEN_HEIGHT - GROUND_HEIGHT, 0.5f);  /*Far left, smaller*/
    create_tree(50, SCREEN_HEIGHT - GROUND_HEIGHT, 0.6f);  /*Left side*/
    create_tree(75, SCREEN_HEIGHT - GROUND_HEIGHT, 0.55f); /*Left-center, medium*/
    create_tree(105, SCREEN_HEIGHT - GROUND_HEIGHT, 0.65f); /*Center-left, slightly larger*/
    create_tree(135, SCREEN_HEIGHT - GROUND_HEIGHT, 0.58f); /*Center, medium*/
    create_tree(160, SCREEN_HEIGHT - GROUND_HEIGHT, 0.52f); /*Center-right, smaller*/
    create_tree(200, SCREEN_HEIGHT - GROUND_HEIGHT, 0.48f); /*Right side, smaller*/
    create_tree(220, SCREEN_HEIGHT - GROUND_HEIGHT, 0.45f); /*Far right, smallest*/
    
    /*Create fireplace with logs - positioned in center-left*/
    create_fireplace(90, SCREEN_HEIGHT - GROUND_HEIGHT, 0.7f);
    
    /*Initialize snowflakes - disabled for summer scene*/
    srand(time(NULL));
    for(int i = 0; i < NUM_SNOWFLAKES; i++) {
        snowflakes[i].size = 2 + (rand() % 7);  /*Size between 2-8 pixels (more variety)*/
        snowflakes[i].x = rand() % SCREEN_WIDTH;
        snowflakes[i].y = -(rand() % (SCREEN_HEIGHT * 2));  /*Start at random heights above screen*/
        snowflakes[i].speed = 1 + (rand() % 4);  /*Speed between 1-4 pixels per frame (more variety)*/
        
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
