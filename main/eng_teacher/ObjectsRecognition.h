#ifndef _OBJECTS_RECOGNITION_H_
#define _OBJECTS_RECOGNITION_H_

#include "App.hpp"
#include "VoiceMsgPlayer.hpp"
#include <cstddef>

#define EXTRA_LABELS_OFFSET 2

#ifndef _countof
#define _countof(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

struct object_info_t {
  enum object_data_t {
    NOTHING,
    STRING,
    NUMBER,
    IMAGE_BITMAP,
    SAMPLE_IDX, // maybe use sample data instead of table idx
  };

  const char *label;
  int real_label_idx;

  struct {
    const wav_samples_table_t *samples_table;
    int sample_idx;
  } ref_pronunciation;

  struct {
    object_data_t type;
    union {
      const unsigned char *image;
      const char *str;
      size_t sample_idx;
      int number;
    } value;
  } const data;
};

struct ObjectsRecognition : State {
  ObjectsRecognition(const object_info_t *const object_info)
    : attempt_num_(0), object_info_(object_info) {}
  void enterAction(App *app) override = 0;
  void exitAction(App *app) override final;
  void handleEvent(App *app, eEvent ev) override final;

protected:
  size_t attempt_num_;
  virtual void check_kws_result(App *app);
  virtual void reset_attempt(App *app);
  const object_info_t *const object_info_;
};

struct Objects : ObjectsRecognition {
  using ObjectsRecognition::ObjectsRecognition;
  State *clone() override final { return new Objects(*this); }
  void enterAction(App *app) override final;
};

struct Numbers : ObjectsRecognition {
  using ObjectsRecognition::ObjectsRecognition;
  State *clone() override final { return new Numbers(*this); }
  void enterAction(App *app) override final;
};

#endif // _OBJECTS_RECOGNITION_H_
