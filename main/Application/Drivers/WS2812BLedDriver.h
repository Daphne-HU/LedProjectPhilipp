// ---------------------------------------------------------------------------
//
// Filename:
//   WS2812BLedDriver.cpp
//
// Product or product-subsystem:
//   led project Philipp
//
// Original author:
//   Daphne Annink
//
// Description:
//
//
// ---------------------------------------------------------------------------

#ifndef BC_APPLICATION_WS2812B_LED_DRIVER_H
#define BC_APPLICATION_WS2812B_LED_DRIVER_H

#include "Application/ApplicationTypes.h"
#include <driver/rmt_tx.h>
#include <stdint.h>

class WS2812BLedDriver {
public:
  WS2812BLedDriver();

  void SetPower(bool powerOn);
  void init();
  void deinit();
  void setColor(RGB_t color);

private:
  rmt_channel_handle_t m_txChannel{};
  rmt_encoder_handle_t m_ledEncoder{};
  rmt_tx_channel_config_t m_txChnConfig{};
  rmt_bytes_encoder_config_t m_encoderConfig{};
};

#endif // BC_APPLICATION_WS2812B_LED_DRIVER_H