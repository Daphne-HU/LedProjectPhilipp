// ---------------------------------------------------------------------------
//
// Filename:
//   LedService.cpp
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

#include "LedService.h"
#include "Application/ApplicationTypes.h"

#include <esp_log.h>

namespace {
constexpr auto LOG_TAG = "LedService";
} // namespace

LedService::LedService()
    : m_nimBLEDriver(std::bind(&LedService::NewRGBValueReceived, this,
                               std::placeholders::_1),
                     std::bind(&LedService::NewLedPowerMode, this,
                               std::placeholders::_1)) {}

void LedService::Start() {
  m_ledDriver.init();
  m_nimBLEDriver.Init();

  m_ledDriver.SetPower(true);
  m_ledDriver.setColor(RGB_t{.red = 255, .green = 100, .blue = 30});
}

void LedService::NewRGBValueReceived(RGB_t newRGBVal) {
  m_ledDriver.setColor(newRGBVal);
};

void LedService::NewLedPowerMode(bool powerOn) {
  ESP_LOGI(LOG_TAG, "%d", powerOn);
  m_ledDriver.SetPower(powerOn);
};
