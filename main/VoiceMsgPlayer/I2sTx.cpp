#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#include "I2sTx.hpp"

#include "driver/gpio.h"
#include "driver/i2s.h"
#include "esp_log.h"

static const char *TAG = "I2sTx";

// I2S Configuration
#if CONFIG_TARGET_LILYGO_T_CIRCLE
#define I2S_BLK_PIN      GPIO_NUM_5
#define I2S_WS_PIN       GPIO_NUM_4
#define I2S_DATA_OUT_PIN GPIO_NUM_6

#define GAIN_PIN   GPIO_NUM_NC
#define SDMODE_PIN GPIO_NUM_45
#else
#error "unknown target"
#endif

#define I2S_DATA_IN_PIN I2S_PIN_NO_CHANGE
#define I2S_SCLK_PIN    I2S_PIN_NO_CHANGE

void i2s_init(void) {
  if (GAIN_PIN != GPIO_NUM_NC) {
    gpio_set_direction(GAIN_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(GAIN_PIN, 0);
  }
  if (SDMODE_PIN != GPIO_NUM_NC) {
    gpio_set_direction(SDMODE_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(SDMODE_PIN, 1);
  }

  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = I2S_TX_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = I2S_TX_AUDIO_BUFFER,
    .use_apll = 0,
    .tx_desc_auto_clear = true,
    .fixed_mclk = I2S_PIN_NO_CHANGE,
  };

  // Set the pinout configuration (set using menuconfig)
  i2s_pin_config_t pin_config = {
    .mck_io_num = I2S_SCLK_PIN,
    .bck_io_num = I2S_BLK_PIN,
    .ws_io_num = I2S_WS_PIN,
    .data_out_num = I2S_DATA_OUT_PIN,
    .data_in_num = I2S_DATA_IN_PIN,
  };

  // Call driver installation function before any I2S R/W operation.
  ESP_ERROR_CHECK(i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL));
  ESP_ERROR_CHECK(i2s_set_pin(I2S_NUM_0, &pin_config));
  ESP_ERROR_CHECK(i2s_set_clk(I2S_NUM_0, I2S_TX_SAMPLE_RATE,
                              I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO));
  ESP_ERROR_CHECK(i2s_start(I2S_NUM_0));
}

void i2s_play_wav(const void *data, size_t bytes) {
  size_t total_wrote_bytes = 0;
  const size_t samples = bytes / sizeof(int16_t);
  const int16_t *audio_data = static_cast<const int16_t *>(data);
  for (size_t i = 0; i < samples; i++) {
    size_t wrote_bytes = 0;
    int16_t value = float(audio_data[i]) * VOICE_MSGS_VOLUME;
    i2s_write(I2S_NUM_0, &value, sizeof(int16_t), &wrote_bytes, portMAX_DELAY);
    total_wrote_bytes += wrote_bytes;
  }
  ESP_LOGV(TAG, "wrote bytes=%d/%d", total_wrote_bytes, bytes);
}

void i2s_release(void) {
  ESP_ERROR_CHECK(i2s_stop(I2S_NUM_0));
  ESP_ERROR_CHECK(i2s_driver_uninstall(I2S_NUM_0));
}
