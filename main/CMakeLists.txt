set(3RDPARTY_DIR "${PROJECT_DIR}/3rdparty")

if(${CONFIG_APP_VOICE_RELAY})
  set(VOICE_RELAY_SRC "kws/kws_event_task.cpp" "kws/kws_task.cpp"
                      "voice_relay/VoiceRelay.cpp" "voice_relay/model.cpp")
  set(VOICE_RELAY_INC "kws")

  add_compile_definitions(KWS_INFERENCE_THRESHOLD=0.9)

  set(APP_SCENARIO_SRC ${VOICE_RELAY_SRC})
  set(APP_SCENARIO_INC ${VOICE_RELAY_INC})
elseif(${CONFIG_APP_SOUND_EVENTS_DETECTION})
  set(SED_SRC
      "sed/SED.cpp" "sed/sed_task.cpp" "sed/sed_model_baby_cry.cpp"
      "sed/sed_model_glass_breaking.cpp" "sed/sed_model_bark.cpp"
      "sed/sed_model_coughing.cpp")
  set(SED_INC "sed")

  add_compile_definitions(SED_INFERENCE_THRESHOLD=0.9)

  set(APP_SCENARIO_SRC ${SED_SRC})
  set(APP_SCENARIO_INC ${SED_INC})
elseif(${CONFIG_APP_ENG_TEACHER})
  set(WAV_PLAYER_DIR "${PROJECT_DIR}/main/VoiceMsgPlayer")
  set(WAV_PLAYER_SRC
      "${WAV_PLAYER_DIR}/I2sTx.cpp"
      "${WAV_PLAYER_DIR}/VoiceMsgPlayer.cpp"
      "${WAV_PLAYER_DIR}/WavPlayer.cpp"
      "${WAV_PLAYER_DIR}/voice_msg_samples.cpp"
      "${WAV_PLAYER_DIR}/ref_objects_samples.cpp"
      "${WAV_PLAYER_DIR}/ref_numbers_samples.cpp")
  set(WAV_PLAYER_INC "${WAV_PLAYER_DIR}/")

  set(ENG_TEACHER_SRC
      "kws/kws_task.cpp" "kws/kws_event_task.cpp"
      "eng_teacher/ObjectsRecognition.cpp" "eng_teacher/objects_model.cpp" "eng_teacher/numbers_model.cpp"
      "eng_teacher/bitmaps.cpp")
  set(ENG_TEACHER_INC "eng_teacher" "kws")

  add_compile_definitions(OBJECTS_INFERENCE_THRESHOLD=0.9)
  add_compile_definitions(NUMBERS_INFERENCE_THRESHOLD=0.9)
  add_compile_definitions(VOICE_MSGS_VOLUME=0.5)

  set(APP_SCENARIO_SRC ${ENG_TEACHER_SRC} ${WAV_PLAYER_SRC})
  set(APP_SCENARIO_INC ${ENG_TEACHER_INC} ${WAV_PLAYER_INC})
endif()

set(GFX_DIR "${3RDPARTY_DIR}/Arduino_GFX-1.3.7")
set(GFX_SRC
    "${GFX_DIR}/src/databus/Arduino_ESP32SPIDMA.cpp"
    "${GFX_DIR}/src/Arduino_DataBus.cpp"
    "${GFX_DIR}/src/Arduino_GFX.cpp"
    "${GFX_DIR}/src/Arduino_TFT.cpp"
    "${GFX_DIR}/src/Arduino_G.cpp"
    "${GFX_DIR}/src/display/Arduino_GC9D01N.cpp")
set(GFX_INC "${GFX_DIR}/src" "${GFX_DIR}/src/display" "${GFX_DIR}/src/databus")

set(FAST_LED_DIR "${3RDPARTY_DIR}/FastLED-3.6.0")
set(FAST_LED_SRC "${FAST_LED_DIR}/src/FastLED.cpp")
set(FAST_LED_INC "${FAST_LED_DIR}/src")

set(BUS_DIR "${3RDPARTY_DIR}/Arduino_DriveBus-1.1.12")
set(TOUCH_SRC
    "${BUS_DIR}/src/hardware/Arduino_HWIIC.cpp"
    "${BUS_DIR}/src/touch_chip/Arduino_CST816x.cpp"
    "${BUS_DIR}/src/Arduino_DriveBus.cpp"
    "${BUS_DIR}/src/Arduino_DriveBus_Library.cpp"
    "${BUS_DIR}/src/Arduino_IIC.cpp")
set(TOUCH_INC "${BUS_DIR}/src")

idf_component_register(
  SRCS
  "main.cpp"
  "./App/App.cpp"
  "./App/Event.cpp"
  "./App/Status.cpp"
  "./App/Touch.cpp"
  "./Hardware/Led_APA102.cpp"
  "./Hardware/Lcd_GC9D01N.cpp"
  ${GFX_SRC}
  ${FAST_LED_SRC}
  ${TOUCH_SRC}
  ${APP_SCENARIO_SRC}
  INCLUDE_DIRS
  "./"
  "./App/include"
  "./Hardware"
  ${GFX_INC}
  ${FAST_LED_INC}
  ${TOUCH_INC}
  ${APP_SCENARIO_INC}
  REQUIRES
  "mic_reader"
  "nn_model"
  "esp_timer"
  "esp_lcd"
  "driver")

target_compile_options(
  ${COMPONENT_LIB}
  PRIVATE -Wno-error=unused-const-variable -Wno-error=delete-non-virtual-dtor
          -Wno-error=implicit-function-declaration -fpermissive)

add_compile_definitions(SUSPEND_TIMEOUT_S=10)
