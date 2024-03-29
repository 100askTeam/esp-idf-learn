/* LVGL Example project
 *
 * Basic project to test LVGL on ESP32 based projects.
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"
 
/* lvgl specific */
#include "lvgl.h"
#include "lv_examples.h"
#include "lv_demos.h"
 
/* lcd 100ask drivers specific */
#include "display_100ask_hal.h"
 
/*********************
 *      DEFINES
 *********************/
#define LV_TICK_PERIOD_MS 1

#define LV_DISP_BUF_SIZE  (CONFIG_DISPLAY_SCREEN_100ASK_WIDTH * CONFIG_DISPLAY_SCREEN_100ASK_HEIGHT)
//#define LV_DISP_BUF_SIZE  (CONFIG_DISPLAY_SCREEN_100ASK_HEIGHT * 80)

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void create_demo_application(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char *TAG = "main";

/**********************
 *   APPLICATION MAIN
 **********************/
void app_main()
{
 
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
}
 
/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;
 
static void guiTask(void *pvParameter)
{
 
    (void)pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();
 
    lv_init();
 
    /* Initialize display */
    display_100ask_hal_init();

    lv_color_t *buf1 = heap_caps_malloc(LV_DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if(buf1 == NULL)
        buf1 = malloc(LV_DISP_BUF_SIZE * sizeof(lv_color_t));
    
    assert(buf1 != NULL);
 
    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t *buf2 = heap_caps_malloc(LV_DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    if (buf2 == NULL)
        buf2 = malloc(LV_DISP_BUF_SIZE * sizeof(lv_color_t));
    
    assert(buf2 != NULL);

#else
    static lv_color_t *buf2 = NULL;
#endif
 
    static lv_disp_draw_buf_t disp_buf;
 
    uint32_t size_in_px = LV_DISP_BUF_SIZE;
 
    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);
 
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = display_100ask_hal_lvgl_flush;
 
    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.draw_buf = &disp_buf;
#if ((CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 0) || (CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 180))
    disp_drv.hor_res = CONFIG_DISPLAY_SCREEN_100ASK_WIDTH;
    disp_drv.ver_res = CONFIG_DISPLAY_SCREEN_100ASK_HEIGHT;
#elif ((CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 90) || (CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 270))
    disp_drv.hor_res = CONFIG_DISPLAY_SCREEN_100ASK_HEIGHT;
    disp_drv.ver_res = CONFIG_DISPLAY_SCREEN_100ASK_WIDTH;
#endif
    lv_disp_drv_register(&disp_drv);
 
    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif
 
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui",
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
 
    /* Create the demo application */
    create_demo_application();
 
    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));
 
        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
 
    /* A task should NEVER return */
    free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    free(buf2);
#endif
    vTaskDelete(NULL);
}
 
static void create_demo_application(void)
{
#if 1
    /* Create simple label */
    lv_obj_t *label = lv_label_create( lv_scr_act() );
    lv_label_set_text_fmt( label, "Hello DShanMCU-Mio!\nLVGL V%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
#else
    //lv_demo_widgets();
    //lv_demo_benchmark();
    lv_demo_music();
#endif
}
 
static void lv_tick_task(void *arg)
{
    (void)arg;
 
    lv_tick_inc(LV_TICK_PERIOD_MS);
}