#pragma once
#define I2S_TX_SAMPLE_RATE  16000
#define I2S_TX_SAMPLE_LEN   800 // 50ms
#define I2S_TX_AUDIO_BUFFER I2S_TX_SAMPLE_LEN

/*!
 * \brief Init i2s tx driver.
 */
void i2s_init(void);
/*!
 * \brief Release i2s tx driver.
 */
void i2s_release(void);
/*!
 * \brief Play wav sample.
 * \param data Data pointer.
 * \param bytes Sample size.
 */
void i2s_play_wav(const void *data, size_t bytes);
