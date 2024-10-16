#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include "string.h"

#include <driver/i2s.h>

#include "i2s_rx_slot.h"

static const char *TAG = "i2s_rx_slot";

#define MIC_INIT_FRAME_NUM 100

EventGroupHandle_t xMicEventGroup = NULL;
SemaphoreHandle_t xMicSema = NULL;

void i2s_rx_slot_init() {
  xMicEventGroup = xEventGroupCreate();
  if (xMicEventGroup == NULL) {
    ESP_LOGE(TAG, "Error creating xMicEventGroup");
  }

  xMicSema = xSemaphoreCreateBinary();
  if (xMicSema) {
    xSemaphoreGive(xMicSema);
  } else {
    ESP_LOGE(TAG, "Error creating xMicSema");
  }

  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = CONFIG_MIC_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = MIC_FRAME_LEN,
    .use_apll = 0,
    .mclk_multiple = I2S_MCLK_MULTIPLE_DEFAULT,
    .bits_per_chan = I2S_BITS_PER_CHAN_32BIT,
  };

  i2s_pin_config_t pin_config = {
    .mck_io_num = I2S_PIN_NO_CHANGE,
    .bck_io_num = MIC_BCLK_PIN,
    .ws_io_num = MIC_WS_PIN,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = MIC_DATA_PIN,
  };

  ESP_ERROR_CHECK(i2s_driver_install(I2S_RX_PORT_NUMBER, &i2s_config, 0, NULL));
  ESP_ERROR_CHECK(i2s_set_pin(I2S_RX_PORT_NUMBER, &pin_config));

  i2s_rx_slot_start();
  raw_audio_t buffer[MIC_FRAME_LEN];
  for (size_t i = 0; i < MIC_INIT_FRAME_NUM; i++) {
    i2s_rx_slot_read(buffer, sizeof(buffer), MIC_FRAME_LEN_MS * 2);
  }
}

void i2s_rx_slot_start() { ESP_ERROR_CHECK(i2s_start(I2S_RX_PORT_NUMBER)); }

void i2s_rx_slot_stop() { ESP_ERROR_CHECK(i2s_stop(I2S_RX_PORT_NUMBER)); }

void i2s_rx_slot_release() {
  i2s_rx_slot_stop();

  if (xMicEventGroup) {
    vEventGroupDelete(xMicEventGroup);
    xMicEventGroup = NULL;
  }
  if (xMicSema) {
    vSemaphoreDelete(xMicSema);
    xMicSema = NULL;
  }

  ESP_ERROR_CHECK(i2s_driver_uninstall(I2S_RX_PORT_NUMBER));
}

int i2s_rx_slot_read(void *buffer, size_t bytes, size_t timeout_ms) {
  size_t read = 0;
  esp_err_t ret =
    i2s_read(I2S_RX_PORT_NUMBER, buffer, bytes, &read, timeout_ms);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "failed to receive item, %s", esp_err_to_name(ret));
    return -1;
  }
  if (read != bytes) {
    ESP_LOGE(TAG, "read %u/%u bytes", read, bytes);
    return -1;
  }
  return 0;
};
