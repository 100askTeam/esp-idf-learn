/*
 ***************************************
    CLOCK_PIN	----- ID
    LATCH_PIN	----- D+
    DATA_PIN	----- D-
 ***************************************
    A:      0000 0001 1     (1 << 0)
    B:      0000 0010 2     (1 << 1)
    start: 	0000 0100 8     (1 << 2)
    select:	0000 1000 4     (1 << 3)
    Up:     0001 0000 16    (1 << 4)
    Down:   0010 0000 32    (1 << 5)
    Left:   0100 0000 64    (1 << 6)
    Right:  1000 0000 128   (1 << 7)
 ***************************************
*/

#include <stdio.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

#include "fc_joypad_100ask_drivers.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void show_joypad_state_info(fc_joypad_100ask_btn_t btn, fc_joypad_100ask_state_t btn_state);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char *TAG = "main";

/**********************
 *   APPLICATION MAIN
 **********************/
void app_main(void)
{
    uint8_t count = 0;
    fc_joypad_100ask_state_t btn_state = FC_JOYPAD_100ASK_RELEASE;

    fc_joypad_100ask_init();

    while(1) {
        fc_joypad_100ask_read();
        btn_state = fc_joypad_100ask_state();
        for (count = 0; count < 8; count++)
        {
            if (fc_joypad_100ask_is_pressed((fc_joypad_100ask_btn_t)count))
                show_joypad_state_info((fc_joypad_100ask_btn_t)count, btn_state);
            else if (fc_joypad_100ask_is_release((fc_joypad_100ask_btn_t)count))
                show_joypad_state_info((fc_joypad_100ask_btn_t)count, btn_state);
        }

        //vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


static void show_joypad_state_info(fc_joypad_100ask_btn_t btn, fc_joypad_100ask_state_t btn_state)
{
  switch (btn)
  {
    case FC_JOYPAD_100ASK_BTN_A:
      ESP_LOGI(TAG, "Key FC_JOYPAD_BTN_A is ");
      break;
    case FC_JOYPAD_100ASK_BTN_B:
      ESP_LOGI(TAG, "Key FC_JOYPAD_BTN_B is ");
      break;
    case FC_JOYPAD_100ASK_BTN_START:
      ESP_LOGI(TAG, "Key FC_JOYPAD_BTN_START is ");
      break;
    case FC_JOYPAD_100ASK_BTN_SELECT:
      ESP_LOGI(TAG, "Key FC_JOYPAD_BTN_SELECT is ");
      break;
    case FC_JOYPAD_100ASK_BTN_UP:
      ESP_LOGI(TAG, "Key FC_JOYPAD_BTN_UP is ");
      break;
    case FC_JOYPAD_100ASK_BTN_DOWN:
      ESP_LOGI(TAG, "Key FC_JOYPAD_BTN_DOWN is ");
      break;
    case FC_JOYPAD_100ASK_BTN_LEFT:
      ESP_LOGI(TAG, "Key FC_JOYPFC_JOYPAD_BTN_LEFTAD_BTN_A is ");
      break;
    case FC_JOYPAD_100ASK_BTN_RIGHT:
      ESP_LOGI(TAG, "Key FC_JOYPAD_BTN_RIGHT is ");
      break;
    default:
      ESP_LOGW (TAG, "FC Joypad data error!");
      break;
  }

  if (btn_state == FC_JOYPAD_100ASK_PRESSED)
    ESP_LOGI(TAG, "pressed!");
  else
    ESP_LOGI(TAG, "release!");

}
