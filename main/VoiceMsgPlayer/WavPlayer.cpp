#include "freertos/projdefs.h"

#include "driver/gpio.h"

#include "I2sTx.hpp"
#include "Types.hpp"
#include "WavPlayer.hpp"
#include "i2s_rx_slot.h"

static const char *TAG = "WavPlayer";

static TaskHandle_t s_task_handle = NULL;
EventGroupHandle_t xWavPlayerEventGroup;
QueueHandle_t xWavPlayerQueue;

static void wp_task(void *pvParameters) {
  for (;;) {
    Sample_t sample;
    xQueuePeek(xWavPlayerQueue, &sample, portMAX_DELAY);
    if (xSemaphoreTake(xMicSema, portMAX_DELAY) == pdPASS) {
      xEventGroupClearBits(xWavPlayerEventGroup, WAV_PLAYER_STOP_MSK);
      while (xQueueReceive(xWavPlayerQueue, &sample, 100) == pdPASS) {
        if (sample.data) {
          i2s_play_wav(sample.data, sample.bytes);
        } else {
          ESP_LOGE(TAG, "wav data is not allocated");
        }
      }
      vTaskDelay(pdMS_TO_TICKS(500));
      xSemaphoreGive(xMicSema);
      xEventGroupSetBits(xWavPlayerEventGroup, WAV_PLAYER_STOP_MSK);
    }
  }
}

int initWavPlayer() {
  ESP_LOGD(TAG, "Setting up i2s");
  i2s_init();

  xWavPlayerQueue = xQueueCreate(100, sizeof(Sample_t));
  if (xWavPlayerQueue == NULL) {
    ESP_LOGE(TAG, "Error creating wav queue");
    return -1;
  }

  xWavPlayerEventGroup = xEventGroupCreate();
  if (xWavPlayerEventGroup == NULL) {
    ESP_LOGE(TAG, "Error creating xWavPlayerEventGroup");
    return -1;
  }
  xEventGroupSetBits(xWavPlayerEventGroup, WAV_PLAYER_STOP_MSK);

  auto xReturned =
    xTaskCreate(wp_task, "wp_task", configMINIMAL_STACK_SIZE + 1024, NULL, 3,
                &s_task_handle);
  if (xReturned != pdPASS) {
    ESP_LOGE(TAG, "Error creating wp_task");
    i2s_release();
    return -1;
  }
  return 0;
}

void releaseWavPlayer() {
  xEventGroupWaitBits(xWavPlayerEventGroup, WAV_PLAYER_STOP_MSK, pdFALSE,
                      pdFALSE, portMAX_DELAY);
  if (s_task_handle) {
    vTaskDelete(s_task_handle);
    s_task_handle = NULL;
  }
  if (xWavPlayerQueue) {
    vQueueDelete(xWavPlayerQueue);
    xWavPlayerQueue = NULL;
  }
  if (xWavPlayerEventGroup) {
    vEventGroupDelete(xWavPlayerEventGroup);
    xWavPlayerEventGroup = NULL;
  }
  i2s_release();
}
