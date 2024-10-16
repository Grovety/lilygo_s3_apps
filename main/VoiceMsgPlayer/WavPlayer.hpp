#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_log.h"

#define WAV_PLAYER_STOP_MSK  BIT0
#define WAV_PLAYER_MUTED_MSK BIT1

/*! \brief Global WavPlayer events. */
extern EventGroupHandle_t xWavPlayerEventGroup;
/*! \brief Global WavPlayer queue. */
extern QueueHandle_t xWavPlayerQueue;

/*!
 * \brief Initialize WavPlayer.
 * \return Result.
 */
int initWavPlayer();
/*!
 * \brief Release WavPlayer.
 */
void releaseWavPlayer();