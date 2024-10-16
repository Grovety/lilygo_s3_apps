#include "VoiceMsgPlayer.hpp"
#include "I2sTx.hpp"
#include "Types.hpp"

#include "string.h"

static const char *TAG = "VoiceMsgPlayer";

void VoiceMsgPlay(wav_samples_table_t table, VoiceMsgId id) {
  ESP_LOGD(TAG, "play msg: %u", id);
  if (id == 0 || id > table.samples_num)
    return;
  const auto xBits = xEventGroupGetBits(xWavPlayerEventGroup);
  if (xBits & WAV_PLAYER_MUTED_MSK) {
    return;
  }
  VoiceMsgId array_idx = id - 1;
  wav_header_t header;
  memcpy(&header, table.wav_samples[array_idx], sizeof(wav_header_t));
  Sample_t wav = {.data = table.wav_samples[array_idx] + sizeof(wav_header_t),
                  .bytes = header.subchunk2Size};
  for (size_t i = 0; i < wav.bytes / I2S_TX_AUDIO_BUFFER; i++) {
    const uint8_t *ptr =
      static_cast<const uint8_t *>(wav.data) + i * I2S_TX_AUDIO_BUFFER;
    Sample_t sample = {.data = ptr, .bytes = I2S_TX_AUDIO_BUFFER};
    xQueueSend(xWavPlayerQueue, &sample, portMAX_DELAY);
    ESP_LOGV(TAG, "send[%d]=%d", i, I2S_TX_AUDIO_BUFFER);
  }
  const size_t rem = wav.bytes % I2S_TX_AUDIO_BUFFER;
  if (rem != 0) {
    const uint8_t *ptr =
      static_cast<const uint8_t *>(wav.data) + wav.bytes - rem;
    Sample_t sample = {.data = ptr, .bytes = rem};
    xQueueSend(xWavPlayerQueue, &sample, portMAX_DELAY);
    ESP_LOGV(TAG, "send[-]=%d", rem);
  }
}

void VoiceMsgStop() {
  if (size_t samples_num = uxQueueMessagesWaiting(xWavPlayerQueue)) {
    ESP_LOGD(TAG, "dropped samples=%d", samples_num);
    xQueueReset(xWavPlayerQueue);
    VoiceMsgWaitStop(portMAX_DELAY);
  }
}

bool VoiceMsgWaitStop(size_t xTicks) {
  const auto xBits = xEventGroupWaitBits(
    xWavPlayerEventGroup, WAV_PLAYER_STOP_MSK, pdFALSE, pdFALSE, xTicks);
  return xBits & WAV_PLAYER_STOP_MSK;
}

void VoiceMsgMutePlayer(bool state) {
  if (state) {
    ESP_LOGD(TAG, "player is muted");
    xEventGroupSetBits(xWavPlayerEventGroup, WAV_PLAYER_MUTED_MSK);
  } else {
    ESP_LOGD(TAG, "player is unmuted");
    xEventGroupClearBits(xWavPlayerEventGroup, WAV_PLAYER_MUTED_MSK);
  }
}
