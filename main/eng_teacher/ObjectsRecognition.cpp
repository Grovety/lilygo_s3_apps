#include <cstddef>
#include <cstdlib>
#include <string>

#include "freertos/projdefs.h"

#include "bootloader_random.h"
#include "esp_random.h"

#include "Lcd_GC9D01N.hpp"
#include "ObjectsRecognition.h"
#include "VoiceMsgPlayer.hpp"
#include "bitmaps.h"
#include "kws_event_task.h"
#include "kws_task.h"

#define OBJECT_SWITCH_TIMEOUT_MS (size_t(1000) * 7)
#define MAX_ATTEMPT_NUM          4

extern const unsigned char *objects_model_ptr;
extern const char *objects_labels[];
extern unsigned int objects_labels_num;

extern const unsigned char *numbers_model_ptr;
extern const char *numbers_labels[];
extern unsigned int numbers_labels_num;

extern const unsigned char *g_ref_objects_samples[];
extern unsigned int g_ref_objects_samples_num;

extern const unsigned char *g_ref_numbers_samples[];
extern unsigned int g_ref_numbers_samples_num;

extern const unsigned char *g_voice_msg_samples[];
extern unsigned int g_voice_msg_samples_num;

static const struct wav_samples_table_t voice_msg_samples_table = {
  .wav_samples = g_voice_msg_samples,
  .samples_num = g_voice_msg_samples_num,
};

static constexpr char TAG[] = "ObjectsRecognition";

static nn_model_handle_t s_model_handle = NULL;

static object_info_t objects_table[] = {
  {
    .label = "cat",
    .real_label_idx = -1,
    .ref_pronunciation =
      {
        .samples_table = nullptr,
        .sample_idx = -1,
      },
    .data =
      {
        .type = object_info_t::IMAGE_BITMAP,
        .value =
          {
            .image = cat_64_x_64_bits,
          },
      },
  },
  {
    .label = "dog",
    .real_label_idx = -1,
    .ref_pronunciation =
      {
        .samples_table = nullptr,
        .sample_idx = -1,
      },
    .data =
      {
        .type = object_info_t::IMAGE_BITMAP,
        .value =
          {
            .image = dog_64_x_64_bits,
          },
      },
  },
  {
    .label = "car",
    .real_label_idx = -1,
    .ref_pronunciation =
      {
        .samples_table = nullptr,
        .sample_idx = -1,
      },
    .data =
      {
        .type = object_info_t::IMAGE_BITMAP,
        .value =
          {
            .image = car_64_x_64_bits,
          },
      },
  },
  {
    .label = "house",
    .real_label_idx = -1,
    .ref_pronunciation =
      {
        .samples_table = nullptr,
        .sample_idx = -1,
      },
    .data =
      {
        .type = object_info_t::IMAGE_BITMAP,
        .value =
          {
            .image = house_64_x_64_bits,
          },
      },
  },
};

static object_info_t numbers_table[] = {
  {
    .label = "one",
    .real_label_idx = -1,
    .ref_pronunciation =
      {
        .samples_table = nullptr,
        .sample_idx = -1,
      },
    .data =
      {
        .type = object_info_t::STRING,
        .value =
          {
            .str = "1",
          },
      },
  },
  {
    .label = "two",
    .real_label_idx = -1,
    .ref_pronunciation =
      {
        .samples_table = nullptr,
        .sample_idx = -1,
      },
    .data =
      {
        .type = object_info_t::STRING,
        .value =
          {
            .str = "2",
          },
      },
  },
  {
    .label = "three",
    .real_label_idx = -1,
    .ref_pronunciation =
      {
        .samples_table = nullptr,
        .sample_idx = -1,
      },
    .data =
      {
        .type = object_info_t::STRING,
        .value =
          {
            .str = "3",
          },
      },
  },
};

struct sub_scenario_desc_t {
  object_info_t *object_info_table;
  const size_t object_info_table_len;
  wav_samples_table_t ref_pronunciation;
  void (*transition_func)(App *app, const object_info_t *object_info);
  const float inference_threshold;
  const unsigned char *model_ptr;
  const char **const labels;
  const unsigned int labels_num;
  const size_t mel_low_freq;
  const size_t mel_high_freq;
} static const s_sub_scenario_descs[]{
  {
    .object_info_table = objects_table,
    .object_info_table_len = _countof(objects_table),
    .ref_pronunciation =
      {
        .wav_samples = g_ref_objects_samples,
        .samples_num = g_ref_objects_samples_num,
      },
    .transition_func =
      [](App *app, const object_info_t *object_info) {
        app->transition(new Objects(object_info));
      },
    .inference_threshold = OBJECTS_INFERENCE_THRESHOLD,
    .model_ptr = objects_model_ptr,
    .labels = objects_labels,
    .labels_num = objects_labels_num,
    .mel_low_freq = 40,
    .mel_high_freq = 8000,
  },
  {
    .object_info_table = numbers_table,
    .object_info_table_len = _countof(numbers_table),
    .ref_pronunciation =
      {
        .wav_samples = g_ref_numbers_samples,
        .samples_num = g_ref_numbers_samples_num,
      },
    .transition_func =
      [](App *app, const object_info_t *object_info) {
        app->transition(new Numbers(object_info));
      },
    .inference_threshold = NUMBERS_INFERENCE_THRESHOLD,
    .model_ptr = numbers_model_ptr,
    .labels = numbers_labels,
    .labels_num = numbers_labels_num,
    .mel_low_freq = 20,
    .mel_high_freq = 4000,
  },
};
static size_t s_objects_groups_len_arr[_countof(s_sub_scenario_descs) + 1] = {
  0};
static size_t s_total_objects_num = 0;
static size_t s_random_object_idx = size_t(-1);

void initSubScenario(const sub_scenario_desc_t *desc);
void releaseSubScenario();
void switchSubScenario(App *app);

void ObjectsRecognition::exitAction(App *app) {
  app->p_display->clear();
  app->p_display->send();
  VoiceMsgWaitStop(portMAX_DELAY);
}
void ObjectsRecognition::handleEvent(App *app, eEvent ev) {
  switch (ev) {
  case eEvent::TOUCH_CLICK:
  case eEvent::TOUCH_SWIPE_LEFT:
  case eEvent::TOUCH_SWIPE_RIGHT:
  case eEvent::TIMEOUT:
    xTimerStop(xTimer, 0);
    kws_req_cancel();
    releaseSubScenario();
    switchSubScenario(app);
    break;
  case eEvent::KWS_WORD:
    xTimerStop(xTimer, 0);
    check_kws_result(app);
    break;
  default:
    break;
  }
}
void ObjectsRecognition::check_kws_result(App *app) {
  int category;
  if (xQueueReceive(xKWSResultQueue, &category, pdMS_TO_TICKS(10)) == pdPASS) {
    if (category == object_info_->real_label_idx) {
      VoiceMsgPlay(voice_msg_samples_table, 1);
      xEventGroupSetBits(xStatusEventGroup, STATUS_EVENT_GOOD_MSK);
      releaseSubScenario();
      switchSubScenario(app);
    } else {
      xEventGroupSetBits(xStatusEventGroup, STATUS_EVENT_BAD_MSK);
      if (attempt_num_++ >= (MAX_ATTEMPT_NUM - 1)) {
        reset_attempt(app);
      } else {
        kws_req_word(1);
        xTimerChangePeriod(xTimer, pdMS_TO_TICKS(OBJECT_SWITCH_TIMEOUT_MS), 0);
      }
    }
  } else {
    ESP_LOGW(TAG, "Unable to receive KWS word");
  }
}
void ObjectsRecognition::reset_attempt(App *app) {
  attempt_num_ = 0;
  assert(object_info_);
  const auto &ref_pron = object_info_->ref_pronunciation;
  assert(ref_pron.samples_table);
  VoiceMsgPlay(*ref_pron.samples_table, ref_pron.sample_idx);
  ESP_LOGI(TAG, "label=%s", object_info_->label);

  app->p_display->clear();
  app->p_display->print_string(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2,
                               object_info_->label);
  app->p_display->send();

  vTaskDelay(pdMS_TO_TICKS(1000));
  app->transition(clone());
}

void Objects::enterAction(App *app) {
  assert(object_info_);
  assert(object_info_->data.type == object_info_t::IMAGE_BITMAP);
  ESP_LOGI(TAG, "Object: %s", object_info_->label);
  app->p_display->draw(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, 64, 64,
                       object_info_->data.value.image);
  app->p_display->send();
  kws_req_word(1);
  xTimerChangePeriod(xTimer, pdMS_TO_TICKS(OBJECT_SWITCH_TIMEOUT_MS), 0);
}

void Numbers::enterAction(App *app) {
  assert(object_info_);
  assert(object_info_->data.type == object_info_t::STRING);
  ESP_LOGI(TAG, "Say number %s", object_info_->data.value.str);
  app->p_display->print_string(DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2, "%s",
                               object_info_->data.value.str);
  app->p_display->send();
  kws_req_word(1);
  xTimerChangePeriod(xTimer, pdMS_TO_TICKS(OBJECT_SWITCH_TIMEOUT_MS), 0);
}

void switchSubScenario(App *app) {
  size_t n;
  for (n = 0;; n++) {
    const size_t random_object_idx = esp_random() % s_total_objects_num;
    if (random_object_idx != s_random_object_idx) {
      s_random_object_idx = random_object_idx;
      break;
    }
  }
  ESP_LOGV(TAG, "random object idx gen attempts: %u", n);

  size_t idx = 1;
  while (s_random_object_idx >= s_objects_groups_len_arr[idx]) {
    idx++;
  }
  idx--;

  const size_t object_idx = s_random_object_idx - s_objects_groups_len_arr[idx];
  ESP_LOGD(TAG, "s_random_object_idx=%u, object_idx=%u, sub_scenario_idx=%u",
           s_random_object_idx, object_idx, idx);

  const auto &desc = s_sub_scenario_descs[idx];
  int errors = nn_model_init(&s_model_handle,
                             nn_model_config_t{
                               .model_ptr = desc.model_ptr,
                               .labels = desc.labels,
                               .labels_num = desc.labels_num,
                               .is_quantized = true,
                               .inference_threshold = desc.inference_threshold,
                             }) < 0;
  errors += kws_task_init(kws_task_conf_t{
              .model_handle = s_model_handle,
              .mic_gain = 30,
              .ns_level = 2,
              .mel_low_freq = desc.mel_low_freq,
              .mel_high_freq = desc.mel_high_freq,
            }) < 0;
  errors += kws_event_task_init();
  if (errors) {
    ESP_LOGE(TAG, "KWS model init error");
    app->transition(nullptr);
  } else {
    desc.transition_func(app, &desc.object_info_table[object_idx]);
  }
}

void initScenario(App *app) {
  bootloader_random_enable();
  if (initWavPlayer() < 0) {
    ESP_LOGE(TAG, "Unable to init wav player");
    return;
  }

  ESP_LOGI(TAG, "Enter Objects Recongnition scenario");
  for (size_t i = 0; i < _countof(s_sub_scenario_descs); i++) {
    auto &desc = s_sub_scenario_descs[i];

    const size_t objects_num = desc.object_info_table_len;
    s_total_objects_num += objects_num;
    s_objects_groups_len_arr[i + 1] = s_total_objects_num;

    for (size_t j = 0; j < objects_num; j++) {
      auto &object_info = desc.object_info_table[j];
      const char *label = object_info.label;
      object_info.real_label_idx = -1;
      for (size_t k = EXTRA_LABELS_OFFSET; k < desc.labels_num; k++) {
        if (strcmp(label, desc.labels[k]) == 0) {
          object_info.real_label_idx = k;
          object_info.ref_pronunciation.samples_table = &desc.ref_pronunciation;
          if (object_info.ref_pronunciation.sample_idx == -1) {
            object_info.ref_pronunciation.sample_idx =
              k - EXTRA_LABELS_OFFSET + 1;
          }
        }
      }
    }
  }
  ESP_LOGD(TAG, "total_objects_num=%u", s_total_objects_num);

  switchSubScenario(app);
}

void releaseSubScenario() {
  kws_event_task_release();
  kws_task_release();
  if (s_model_handle) {
    nn_model_release(s_model_handle);
    s_model_handle = NULL;
  }
}

void releaseScenario(App *app) {
  ESP_LOGI(TAG, "Exit Objects Recongnition scenario");
  releaseSubScenario();

  bootloader_random_disable();
  releaseWavPlayer();
}
