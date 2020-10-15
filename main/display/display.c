#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>
#include <u8g2.h>

#include "display.h"
#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"

#define TAG "DISPLAY"

#define PIN_SDA 5
#define PIN_SCL 4


static u8g2_t u8g2; // a structure which will contain all the data for one display


void displayInit()
{
    ESP_LOGI(TAG, "starting display....\n");

    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
    u8g2_esp32_hal.sda = PIN_SDA;
    u8g2_esp32_hal.scl = PIN_SCL;
    u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_Setup_ssd1306_128x32_univision_f(
        &u8g2,
        U8G2_R2,
        //u8x8_byte_sw_i2c,
        u8g2_esp32_msg_i2c_cb,
        u8g2_esp32_msg_i2c_and_delay_cb); // init u8g2 structure
    u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);

    ESP_LOGI(TAG, "u8g2_InitDisplay");
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

    ESP_LOGI(TAG, "u8g2_SetPowerSave");
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    ESP_LOGI(TAG, "u8g2_ClearBuffer");
    u8g2_ClearBuffer(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
}

void displayPrintConfirmed(int confirmed)
{
    ESP_LOGI(TAG, "displayPrintConfirmed");

    char firstItemText[50];
    sprintf(firstItemText, "Confirmed: %d", confirmed);

    u8g2_DrawStr(&u8g2, 2, 10, firstItemText);

    u8g2_SendBuffer(&u8g2);
}

void displayPrintDeaths(int deaths)
{
    ESP_LOGI(TAG, "displayPrintDeaths");

    char firstItemText[50];
    sprintf(firstItemText, "Deaths: %d", deaths);

    u8g2_DrawStr(&u8g2, 2, 20, firstItemText);

    u8g2_SendBuffer(&u8g2);
}

void displayPrintRecovered(int recovered)
{
    ESP_LOGI(TAG, "displayPrintRecovered");

    char firstItemText[50];
    sprintf(firstItemText, "Recovered: %d", recovered);

    u8g2_DrawStr(&u8g2, 2, 30, firstItemText);

    u8g2_SendBuffer(&u8g2);
}