/**
 * @file lcd_100ask_simple_test_main.c
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
#include "esp_system.h"
#include "esp_log.h"

/* lcd 100ask drivers specific */
#include "tft_lcd_100ask_hal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static const char *TAG = "TFT_LCD_100ASK_SMILE_TEST";

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void app_main(void)
{
    tft_lcd_100ask_hal_init();

    while (1) {
        ESP_LOGI(TAG, "Color white.");
        tft_lcd_100ask_hal_clear(TFT_LCD_100ASK_COLOR_WHITE);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Color red.");
        tft_lcd_100ask_hal_clear(TFT_LCD_100ASK_COLOR_RED);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Color green.");
        tft_lcd_100ask_hal_clear(TFT_LCD_100ASK_COLOR_GREEN);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Color blue.");
        tft_lcd_100ask_hal_clear(TFT_LCD_100ASK_COLOR_BLUE);
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Color black.");
        tft_lcd_100ask_hal_clear(TFT_LCD_100ASK_COLOR_BLACK);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
