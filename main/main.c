#include <stdio.h>
#include <string.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event_loop.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "cJSON.h"

#include "fetch.h"
#include "connect.h"
#include "display.h"
#include "sntp-time.h"

#define TAG "DATA"
#define COUNTRY CONFIG_COUNTRY

void fetchTask(void *args);

xSemaphoreHandle connectionSemaphore;

void OnGotData(char *incomingBuffer, char *output)
{

  ESP_LOGI(TAG, "OnGotData called with incomingBuffer = %s", incomingBuffer);

  cJSON *payload = cJSON_Parse(incomingBuffer);
  cJSON *entry;
  cJSON_ArrayForEach(entry, payload)
  {
    ESP_LOGI(TAG, "foreach called");

    int confirmed = cJSON_GetObjectItem(entry, "Confirmed")->valueint;
    int deaths = cJSON_GetObjectItem(entry, "Deaths")->valueint;
    int recovered = cJSON_GetObjectItem(entry, "Recovered")->valueint;

    ESP_LOGI(TAG, "confirmed = %d", confirmed);
    ESP_LOGI(TAG, "deaths = %d", deaths);
    ESP_LOGI(TAG, "recovered = %d", recovered);

    displayInit();
    displayPrintConfirmed(confirmed);
    displayPrintDeaths(deaths);
    displayPrintRecovered(recovered);
  }

  cJSON_Delete(payload);
}

void OnConnected(void *para)
{
  while (true)
  {
    if (xSemaphoreTake(connectionSemaphore, 10000 / portTICK_RATE_MS))
    {
      ESP_LOGI(TAG, "Processing");

      xTaskCreate(&fetchTask, "fetchTask", 8000, NULL, 1, NULL);

      ESP_LOGI(TAG, "Done!");
      xSemaphoreTake(connectionSemaphore, portMAX_DELAY);
    }
    else
    {
      ESP_LOGE(TAG, "Failed to connect. Retry in");
      for (int i = 0; i < 5; i++)
      {
        ESP_LOGE(TAG, "...%d", i);
        vTaskDelay(1000 / portTICK_RATE_MS);
      }
      esp_restart();
    }
  }
}

void fetchTask(void *args)
{
  ESP_LOGI(TAG, "fetch task called");

  struct FetchParms fetchParams;
  fetchParams.OnGotData = OnGotData;

  char fromDate[100]; // should equal to yesterday
  char toDate[100];
  char url[300];

  struct tm *time = sntpGetTime();

  sprintf(fromDate, "%d-%d-%dT00:00:00Z", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday - 1);
  sprintf(toDate, "%d-%d-%dT00:00:00Z", time->tm_year + 1900, time->tm_mon + 1, time->tm_mday);
  sprintf(url, "https://api.covid19api.com/country/%s?from=%s&to=%s", COUNTRY, fromDate, toDate);

  strcpy(fetchParams.url, url);

  fetch(&fetchParams);

  vTaskDelete(NULL);
}

void app_main()
{
  esp_log_level_set(TAG, ESP_LOG_DEBUG);
  connectionSemaphore = xSemaphoreCreateBinary();
  wifiInit();
  xTaskCreate(&OnConnected, "handel comms", 1024 * 3, NULL, 5, NULL);
}