// ---------------------------------------------------------------------------
//
// Filename:
//   NimBLEDriver.h
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
//
// ---------------------------------------------------------------------------
#ifndef BC_APPLICATION_NIMBLE_DRIVER_H
#define BC_APPLICATION_NIMBLE_DRIVER_H

#include "Application/ApplicationTypes.h"

#include <cstdint>
#include <functional>
#include <host/ble_gatt.h>
#include <host/ble_uuid.h>
#include <string_view>

constexpr auto MAX_BLE_DEVICE_NAME_LENG = 109;
constexpr auto MAX_ADDR_VAL_LENGTH = 20;

class NimBleDriver {
public:
  using NewRGBValueCallback = std::function<void(RGB_t newRGBValue)>;
  using NewLedPowerModeCallback = std::function<void(bool powerOn)>;
  explicit NimBleDriver(
      const NewRGBValueCallback &NewRGBValueCallbackFunc,
      const NewLedPowerModeCallback &NewLedPowerModeCallbackfunc);

  void Init() const;

private:
  void GattSvrInit() const;
  static int GapEvent(struct ble_gap_event *event, void *arg);
  static void HostTask(void *param);
  static void Advertise();
  static void OnSync();
  static void OnReset(int reason);
  static int GattAccessRGB(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);
  static int GattAccessLedOnOff(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);
  static int GattSvrChrWrite(struct os_mbuf *data, uint16_t min_len,
                             uint16_t max_len, char *dst, uint16_t *len);

private:
  NewRGBValueCallback m_newRGBValueCallback;
  NewLedPowerModeCallback m_newLedPowerModeCallback;

  constexpr static const ble_uuid128_t gattUuidSvr =
      BLE_UUID128_INIT(0x04, 0x00, 0x34, 0xbc, 0x64, 0x04, 0x9a, 0xda, 0xcf,
                       0x33, 0xfe, 0x68, 0x98, 0xbe, 0xcf, 0xe2);
  // e2cfbe98-68fe-33cf-da9a-0464bc340004
  constexpr static const ble_uuid128_t gattUuidRGB =
      BLE_UUID128_INIT(0x05, 0x00, 0x45, 0xcd, 0x75, 0x05, 0xab, 0xeb, 0xbf,
                       0x44, 0xef, 0x79, 0xa9, 0xcf, 0xdf, 0xf3);
  // f3dfcfa9-79ef-44bf-ebab-0575cd450005
  constexpr static const ble_uuid128_t gattUuidLedOnOff =
      BLE_UUID128_INIT(0x03, 0x00, 0x21, 0xab, 0x53, 0x03, 0x89, 0xc9, 0xde,
                       0x22, 0xed, 0x57, 0x87, 0xad, 0xbf, 0xd1);
  // d1bfad87-57ed-22de-c989-0353ab210003

  const ble_gatt_chr_def m_gattCharacteristics[3] = {
      {
          .uuid = &gattUuidRGB.u,
          .access_cb = GattAccessRGB,
          .arg = this,
          .descriptors = nullptr,
          .flags = BLE_GATT_CHR_F_WRITE, // | BLE_GATT_CHR_F_READ
          .min_key_size = 0,
          .val_handle = nullptr,
          .cpfd = nullptr,
      },
      {
          .uuid = &gattUuidLedOnOff.u,
          .access_cb = GattAccessLedOnOff,
          .arg = this,
          .descriptors = nullptr,
          .flags = BLE_GATT_CHR_F_WRITE, // | BLE_GATT_CHR_F_READ
          .min_key_size = 0,
          .val_handle = nullptr,
          .cpfd = nullptr,
      },
      {
          // NOTE: no more characteristics
      }};

  const ble_gatt_svc_def m_gattSvr[2] = {
      {
          .type = BLE_GATT_SVC_TYPE_PRIMARY,
          .uuid = &gattUuidSvr.u,
          .includes = nullptr,
          .characteristics = m_gattCharacteristics,
      },
      {
          // NOTE: no more services
      }};
};

#endif // BC_APPLICATION_NIMBLE_DRIVER_H
