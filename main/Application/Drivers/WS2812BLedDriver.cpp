// ---------------------------------------------------------------------------
//
// Filename:
//   WS2812BLedDriver.cpp
//
// Product or product-subsystem:
//   Led project Philipp
//
// Original author:
//   Daphne Annink
//
// Description:
//
//
// ---------------------------------------------------------------------------

#include "WS2812BLedDriver.h"
#include "Application/ApplicationTypes.h"
#include "soc/gpio_num.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <driver/gpio.h>
#include <driver/rmt_encoder.h>
#include <driver/rmt_tx.h>
#include <esp_log.h>
#include <sys/types.h>

namespace {
constexpr auto LOG_TAG = "LedDriver";

constexpr auto WS2812B_PIN = GPIO_NUM_18;
constexpr auto POWER_PIN = GPIO_NUM_13;
constexpr auto RESOLUTION_HZ = (10 * 1000 * 1000); // RMT resolution: 10 MHz
} // namespace

WS2812BLedDriver::WS2812BLedDriver() {
  gpio_set_direction(POWER_PIN, GPIO_MODE_OUTPUT);
}

void WS2812BLedDriver::SetPower(bool powerOn) {
  ESP_LOGI(LOG_TAG, "%d", powerOn);
  if (powerOn) {
    ESP_LOGI(LOG_TAG, "power ON");
    gpio_set_level(POWER_PIN, 1);
  } else {
    ESP_LOGI(LOG_TAG, "power OFF");
    gpio_set_level(POWER_PIN, 0);
  }
}

void WS2812BLedDriver::init() {
  m_txChnConfig.gpio_num = WS2812B_PIN;
  m_txChnConfig.clk_src = RMT_CLK_SRC_DEFAULT;
  m_txChnConfig.resolution_hz = RESOLUTION_HZ;
  m_txChnConfig.mem_block_symbols = 64;
  m_txChnConfig.trans_queue_depth = 4;
  m_txChnConfig.flags.with_dma = false;

  ESP_ERROR_CHECK(rmt_new_tx_channel(&m_txChnConfig, &m_txChannel));
  ESP_ERROR_CHECK(rmt_enable(m_txChannel));

  m_encoderConfig.bit0 = {.duration0 = static_cast<uint16_t>(
                              0.4 * RESOLUTION_HZ / 1e6), // 400 ns HIGH
                          .level0 = 1,
                          .duration1 = static_cast<uint16_t>(
                              0.85 * RESOLUTION_HZ / 1e6), // 850 ns LOW
                          .level1 = 0};
  m_encoderConfig.bit1 = {.duration0 = static_cast<uint16_t>(
                              0.8 * RESOLUTION_HZ / 1e6), // 800 ns HIGH
                          .level0 = 1,
                          .duration1 = static_cast<uint16_t>(
                              0.45 * RESOLUTION_HZ / 1e6), // 450 ns LOW
                          .level1 = 0};
  m_encoderConfig.flags = {.msb_first = true};

  ESP_ERROR_CHECK(rmt_new_bytes_encoder(&m_encoderConfig, &m_ledEncoder));

  ESP_LOGI(LOG_TAG, "Initialized WS2812B LED strip on GPIO %d with %d LEDs",
           WS2812B_PIN, LED_COUNT);
}

void WS2812BLedDriver::setColor(RGB_t color) {
  ESP_LOGI(LOG_TAG, "set Colors");
  std::array<uint8_t, LED_COUNT * 3> led_data = {};

  for (int i = 0; i < LED_COUNT; i++) {
    led_data[i * 3 + 0] = color.green; // Green first
    led_data[i * 3 + 1] = color.red;   // Red second
    led_data[i * 3 + 2] = color.blue;  // Blue last
  }

  rmt_transmit_config_t tx_config = {
      .loop_count = 0 // No looping
  };

  ESP_ERROR_CHECK(rmt_transmit(m_txChannel, m_ledEncoder, led_data.data(),
                               led_data.size(), &tx_config));
  ESP_LOGI(LOG_TAG, "Set color to R:%d, G:%d, B:%d", color.red, color.green,
           color.blue);
}

void WS2812BLedDriver::deinit() {
  if (m_txChannel)
    rmt_del_channel(m_txChannel);
  if (m_ledEncoder)
    rmt_del_encoder(m_ledEncoder);
}
