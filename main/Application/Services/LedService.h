// ---------------------------------------------------------------------------
//
// Filename:
//   LedService.h
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

#ifndef BC_APPLICATION_LED_SERVICE_H
#define BC_APPLICATION_LED_SERVICE_H

#include "Application/ApplicationTypes.h"
#include "Application/Drivers/NimBLEDriver.h"
#include "Application/Drivers/WS2812BLedDriver.h"

class LedService {
public:
  LedService();

  void Start();

private:
  void NewRGBValueReceived(RGB_t newRGBVal);
  void NewLedPowerMode(bool powerOn);

  NimBleDriver m_nimBLEDriver;
  WS2812BLedDriver m_ledDriver{};
};

#endif // BC_APPLICATION_LED_SERVICE_H