/**
 * @file epd_100ask_lvgl_main.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"

#include "driver/gpio.h"

#include "epd_100ask_hal.h"
#include "epd_100ask_paint.h"

/* lvgl specific */
#include "lvgl.h"

#include "lv_demos.h"
#include "lv_examples.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TICK_PERIOD_MS 1
#define DISP_BUF_SIZE (CONFIG_EPD_100ASK_PAINT_WIDTH  * CONFIG_EPD_100ASK_PAINT_HEIGHT)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void epd_disp_driver_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
static void guiTask(void *pvParameter);
static void create_demo_application(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char *TAG = "main";

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;
static uint8_t * g_epd_paint_image = NULL;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void app_main(void)
{
    g_epd_paint_image = (uint8_t *)malloc(((CONFIG_EPD_100ASK_PAINT_WIDTH % 8 == 0) ? (CONFIG_EPD_100ASK_PAINT_WIDTH / 8 ) : (CONFIG_EPD_100ASK_PAINT_WIDTH / 8 + 1)) * CONFIG_EPD_100ASK_PAINT_HEIGHT);
    assert(g_epd_paint_image != NULL);

    epd_100ask_hal_init();
    epd_100ask_paint_init(g_epd_paint_image, EPD_100ASK_COLOR_WHITE);
    // 通过画布清屏
    epd_100ask_paint_clear(EPD_100ASK_COLOR_WHITE);
    epd_100ask_hal_display_image(g_epd_paint_image, 240, 360);
    epd_100ask_hal_refresh(EPD_100ASK_LUT_GC);
    
    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
    * Otherwise there can be problem such as memory corruption and so on.
    * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
}



/**********************
 *   STATIC FUNCTIONS
 **********************/
static void guiTask(void *pvParameter)
{
    (void)pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();
 
    lv_init();
 
    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);
 
    /* Use double buffered when not working with monochrome displays */

    //lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    //assert(buf2 != NULL);
 
    static lv_disp_draw_buf_t disp_buf;
 
    uint32_t size_in_px = DISP_BUF_SIZE;
 
    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    //lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);
    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, size_in_px);
 
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);  
    disp_drv.draw_buf = &disp_buf;
    disp_drv.hor_res = CONFIG_EPD_100ASK_PAINT_WIDTH;
    disp_drv.ver_res = CONFIG_EPD_100ASK_PAINT_HEIGHT;
    disp_drv.full_refresh = 1;
    disp_drv.flush_cb = epd_disp_driver_flush;
    lv_disp_drv_register(&disp_drv);

#if LV_USE_THEME_MONO
    lv_disp_t * disp = lv_disp_get_default();
    if(lv_theme_mono_is_inited() == false) {
        disp->theme = lv_theme_mono_init(disp, 0, LV_FONT_DEFAULT);
    }
#endif

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui",
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "LVGL initialized successfully!");
 
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
    //free(buf2);
    vTaskDelete(NULL);
}

static void create_demo_application(void)
{
#if 1
    /* Create simple label */
    lv_obj_t *label = lv_label_create( lv_scr_act() );
    lv_label_set_text_fmt(label, "Hello DShanMCU-Mio! \nLVGL V%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
    lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );
#else
    lv_demo_widgets();
    //lv_example_get_started_1();
    //lv_example_calendar_1();
    //lv_example_btnmatrix_2();
#endif
}



/* Display flushing */
static void epd_disp_driver_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t x;
    int32_t y;

    for (y = area->y1; y <= area->y2; y++) {
        for (x = area->x1; x <= area->x2; x++) {
            /*Put a pixel to the display. For example:*/
            /*put_px(x, y, *color_p)*/
            epd_100ask_paint_set_pixel(x, y, *((uint8_t *)color_p));
            color_p++;
        }
    }

    if (lv_disp_flush_is_last(disp))
    {
        epd_100ask_hal_display_image(g_epd_paint_image, 240, 360);
        epd_100ask_hal_refresh(EPD_100ASK_LUT_GC);

        lv_disp_flush_ready( disp );
    }

    
}


static void lv_tick_task(void *arg)
{
    (void)arg;
 
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

