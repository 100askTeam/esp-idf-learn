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
#include "display_100ask_hal.h"

/*********************
 *      DEFINES
 *********************/
#define LCD_COLOR_BLACK     0x0000
#define LCD_COLOR_WHITE     0xFFFF
#define LCD_COLOR_RED       0xF800
#define LCD_COLOR_GREEN     0x07E0
#define LCD_COLOR_BLUE      0x001F

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static const char *TAG = "DISPLAY_100ASK_SMILE_TEST";

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void app_main(void)
{
    display_100ask_hal_init();

    while (1) {
        ESP_LOGI(TAG, "Color white.");
        display_100ask_hal_set_clear(LCD_COLOR_WHITE);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Color red.");
        display_100ask_hal_set_clear(LCD_COLOR_RED);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Color green.");
        display_100ask_hal_set_clear(LCD_COLOR_GREEN);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Color blue.");
        display_100ask_hal_set_clear(LCD_COLOR_BLUE);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        ESP_LOGI(TAG, "Color black.");
        display_100ask_hal_set_clear(LCD_COLOR_BLACK);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
