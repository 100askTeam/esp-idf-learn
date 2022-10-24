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
//#include "esp_system.h"
#include "esp_log.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/* 100ask specific */
#include "tft_lcd_100ask_drivers.h"
#include "fc_joypad_100ask_drivers.h"
#include "lv_port_fs_fatfs.h"
#include "lv_lib_100ask.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TICK_PERIOD_MS 1

//#define LV_DISP_BUF_SIZE  (CONFIG_SPI_TFT_LCD_100ASK_DISP_WIDTH * CONFIG_SPI_TFT_LCD_100ASK_DISP_HEIGHT)
#define LV_DISP_BUF_SIZE  (CONFIG_SPI_TFT_LCD_100ASK_DISP_WIDTH * 20)
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void create_demo_application(void);
static void get_joypad_value(lv_indev_data_t *data, fc_joypad_100ask_btn_t btn, bool btn_state);
static void my_fc_joypad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);

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
 
    /* esp-100ask-components initialize */
    tft_lcd_100ask_drivers_init();
    
    fc_joypad_100ask_init();

    lv_port_fs_100ask_init();
 
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
 
#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820 || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306
    size_in_px *= 8;
#endif
 
    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);
 
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = tft_lcd_100ask_display_flush;
 
    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif
 
    disp_drv.draw_buf = &disp_buf;
    disp_drv.hor_res = CONFIG_SPI_TFT_LCD_100ASK_DISP_WIDTH;
    disp_drv.ver_res = CONFIG_SPI_TFT_LCD_100ASK_DISP_HEIGHT;
    lv_disp_drv_register(&disp_drv);
 
    /* Register an input device when enabled on the menuconfig */
#if CONFIG_LV_TOUCH_CONTROLLER != TOUCH_CONTROLLER_NONE
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

    /*Initialize the (dummy) input device driver*/
    lv_indev_drv_t joypad_drv;
    lv_indev_t * lv_joypad_device_object;
    lv_indev_drv_init(&joypad_drv);
    joypad_drv.type = LV_INDEV_TYPE_KEYPAD;
    joypad_drv.read_cb = my_fc_joypad_read;
    lv_joypad_device_object = lv_indev_drv_register(&joypad_drv);

    // 创建一个组，稍后将需要使用键盘或编码器或按钮控制的部件(对象)添加进去，并且将输入设备和组关联
    // 如果将这个组设置为默认组，那么对于那些在创建时会添加到默认组的部件(对象)就可以省略 lv_group_add_obj()
    // 视频教程学习：https://www.bilibili.com/video/BV1Ya411r7K2?p=19
    lv_group_t *g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(lv_joypad_device_object, g);  // joypad
 
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
    lv_100ask_file_explorer_simple_test();
    //lv_100ask_2048_simple_test();
}

static void get_joypad_value(lv_indev_data_t *data, fc_joypad_100ask_btn_t btn, bool btn_state)
{

  data->state = (lv_indev_state_t)((btn_state) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL);

  switch (btn) {
    case FC_JOYPAD_100ASK_BTN_A:
      data->key = LV_KEY_ENTER;
      //printf("Key FC_JOYPAD_100ASK_BTN_A is pressed!\n");
      break;
    case FC_JOYPAD_100ASK_BTN_B:
      data->key = LV_KEY_BACKSPACE;
      //printf("Key FC_JOYPAD_100ASK_BTN_B is pressed!\n");
      break;
    case FC_JOYPAD_100ASK_BTN_START:
      data->key = LV_KEY_NEXT;
      //printf("Key FC_JOYPAD_100ASK_BTN_START is pressed!\n");
      break;
    case FC_JOYPAD_100ASK_BTN_SELECT:
      data->key = LV_KEY_PREV;
      //printf("Key FC_JOYPAD_100ASK_BTN_SELECT is pressed!\n");
      break;
    case FC_JOYPAD_100ASK_BTN_UP:
      data->key = LV_KEY_UP;
      //printf("Key FC_JOYPAD_100ASK_BTN_UP is pressed!\n");
      break;
    case FC_JOYPAD_100ASK_BTN_DOWN:
      data->key = LV_KEY_DOWN;
      //printf("Key FC_JOYPAD_100ASK_BTN_DOWN is pressed!\n");
      break;
    case FC_JOYPAD_100ASK_BTN_LEFT:
      data->key = LV_KEY_LEFT;
      //printf("Key FC_JOYPAD_100ASK_BTN_LEFT is pressed!\n");
      break;
    case FC_JOYPAD_100ASK_BTN_RIGHT:
      data->key = LV_KEY_RIGHT;
      //printf("Key FC_JOYPAD_100ASK_BTN_RIGHT is pressed!\n");
      break;
    default:
      ESP_LOGW (TAG, "FC Joypad data error!");
      break;
  }
}

/*Read the touchpad*/
static void my_fc_joypad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  static uint8_t count = 0;
  fc_joypad_100ask_state_t btn_state = FC_JOYPAD_100ASK_RELEASE;

  fc_joypad_100ask_read();
  btn_state = fc_joypad_100ask_state();
  for (count = 0; count < 8; count++) {
    if (fc_joypad_100ask_is_pressed((fc_joypad_100ask_btn_t)count))
      get_joypad_value(data, (fc_joypad_100ask_btn_t)count, btn_state);
    else if (fc_joypad_100ask_is_release((fc_joypad_100ask_btn_t)count))
      get_joypad_value(data, (fc_joypad_100ask_btn_t)count, btn_state);
  }
}

static void lv_tick_task(void *arg)
{
    (void)arg;
 
    lv_tick_inc(LV_TICK_PERIOD_MS);
}