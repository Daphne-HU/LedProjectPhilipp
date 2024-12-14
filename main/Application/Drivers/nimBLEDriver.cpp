// ---------------------------------------------------------------------------
//
// Filename:
//   nimBLEDriver.cpp
//
// Product or product-subsystem:
//   led project Philipp
//
// Original author:
//   Daphne Annink
//
// Description:
//   Driver for the use of the nimBLE host
//
// ---------------------------------------------------------------------------

#include "NimBLEDriver.h"
#include "Application/ApplicationTypes.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <esp_log.h>
#include <host/ble_hs.h>
#include <host/ble_hs_id.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include <services/gap/ble_svc_gap.h>
#include <services/gatt/ble_svc_gatt.h>

namespace {
constexpr auto LOG_TAG = "NimBLE_DRIVER";
constexpr char DEVICE_NAME[] = "LedsPhilipp";

uint8_t blehrAddrType{};
} // namespace

NimBleDriver::NimBleDriver(
    const NewRGBValueCallback &NewRGBValueCallbackFunc,
    const NewLedPowerModeCallback &NewLedPowerModeCallbackfunc)
    : m_newRGBValueCallback(NewRGBValueCallbackFunc),
      m_newLedPowerModeCallback(NewLedPowerModeCallbackfunc) {}

void NimBleDriver::Init() const {
  ESP_LOGI(LOG_TAG, "Initializing BT Controller and NimBLE stack");
  if (const esp_err_t resultCode = nimble_port_init(); resultCode != ESP_OK) {
    ESP_LOGE(LOG_TAG, "Failed nimble_port_init, error: %d", resultCode);
    return;
  }
  ble_hs_cfg.sync_cb = OnSync;
  ble_hs_cfg.reset_cb = OnReset;
  GattSvrInit();

  ESP_LOGI(LOG_TAG, "BLE device name '%s' set", DEVICE_NAME);
  assert(ble_svc_gap_device_name_set(DEVICE_NAME) == 0);
  nimble_port_freertos_init(HostTask);
}

void NimBleDriver::GattSvrInit() const {
  ble_svc_gap_init();
  ble_svc_gatt_init();
  if (ble_gatts_count_cfg(m_gattSvr) != 0) {
    assert(false &&
           "m_gattSvr contains at least one invalid resource definition");
  }
  if (ble_gatts_add_svcs(m_gattSvr) != 0) {
    assert(false && "Couldn't add all resource definitions, out of RAM!");
  }
}

int NimBleDriver::GapEvent(struct ble_gap_event *event,
                           [[maybe_unused]] void *arg) {
  switch (event->type) {
  case BLE_GAP_EVENT_CONNECT: {
    ESP_LOGI(LOG_TAG, "Connection %s; status=%d",
             event->connect.status == 0 ? "established" : "failed",
             event->connect.status);
    if (event->connect.status != 0) {
      Advertise();
    }
    break;
  }
  case BLE_GAP_EVENT_DISCONNECT: {
    ESP_LOGI(LOG_TAG, "Disconnect; reason=%d", event->disconnect.reason);
    Advertise();
    break;
  }
  case BLE_GAP_EVENT_ADV_COMPLETE: {
    ESP_LOGI(LOG_TAG, "Advertising complete");
    Advertise();
    break;
  }
  case BLE_GAP_EVENT_SUBSCRIBE: {
    ESP_LOGI(LOG_TAG, "Subscribe event, cur_notify=%d",
             event->subscribe.cur_notify);
    break;
  }
  case BLE_GAP_EVENT_MTU: {
    ESP_LOGI(LOG_TAG, "MTU update event; conn_handle=%d mtu=%d",
             event->mtu.conn_handle, event->mtu.value);
    break;
  }
  default:
    break;
  }
  return 0;
}

void NimBleDriver::HostTask([[maybe_unused]] void *param) {
  ESP_LOGI(LOG_TAG, "BLE Host Task Started");
  nimble_port_run();
  nimble_port_freertos_deinit();
}

void NimBleDriver::Advertise() {
  int resultCode = 0;
  ble_hs_adv_fields fields{};
  fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
  fields.name = reinterpret_cast<const unsigned char *>(DEVICE_NAME);
  fields.name_len = sizeof(DEVICE_NAME);
  fields.name_is_complete = 1;
  fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
  fields.tx_pwr_lvl_is_present = 1;

  resultCode = ble_gap_adv_set_fields(&fields);
  if (resultCode != 0) {
    ESP_LOGE(LOG_TAG, "Error setting advertisement data; rc=%d", resultCode);
    return;
  }

  constexpr ble_gap_adv_params advParams = {.conn_mode = BLE_GAP_CONN_MODE_UND,
                                            .disc_mode = BLE_GAP_DISC_MODE_GEN,
                                            .itvl_min = 0,
                                            .itvl_max = 0,
                                            .channel_map = 0,
                                            .filter_policy = 0,
                                            .high_duty_cycle = 0};

  resultCode = ble_gap_adv_start(blehrAddrType, nullptr, BLE_HS_FOREVER,
                                 &advParams, GapEvent, nullptr);
  if (resultCode != 0) {
    ESP_LOGE(LOG_TAG, "Error enabling advertisement; rc=%d", resultCode);
    return;
  }
}

void NimBleDriver::OnSync() {
  int resultCode = ble_hs_id_infer_auto(0, &blehrAddrType);
  assert(resultCode == 0 && "ble_hs_id_infer_auto failed");

  std::array<uint8_t, MAX_ADDR_VAL_LENGTH> addrVal{};
  resultCode = ble_hs_id_copy_addr(blehrAddrType, addrVal.data(), nullptr);
  assert(resultCode == 0 && "ble_hs_id_copy_addr failed");

  ESP_LOGI(LOG_TAG, "Device Address: %s", addrVal.data());
  Advertise();
}

void NimBleDriver::OnReset(int reason) {
  ESP_LOGE(LOG_TAG, "Resetting state; reason=%d", reason);
}

int NimBleDriver::GattAccessRGB([[maybe_unused]] uint16_t conn_handle,
                                [[maybe_unused]] uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg) {
  switch (ctxt->op) {
    //   case BLE_GATT_ACCESS_OP_READ_CHR: {
    //     // RGB_t currentRGBval{};
    //     const char *currentRGBvalStr = "40,100,80";
    //     return os_mbuf_append(ctxt->om, currentRGBvalStr,
    //     strlen(currentRGBvalStr));
    //   }
  case BLE_GATT_ACCESS_OP_WRITE_CHR: {
    std::array<char, 11> newData{};
    uint16_t newDataLength = 0;
    const int resultCode = GattSvrChrWrite(ctxt->om, 0, newData.max_size(),
                                           &newData[0], &newDataLength);
    RGB_t newRGBval{};
    int red, green, blue;
    if (sscanf(newData.data(), "%d,%d,%d", &red, &green, &blue) == 3) {
      newRGBval.red = static_cast<uint8_t>((red < 0)     ? 0
                                           : (red > 255) ? 255
                                                         : red);
      newRGBval.green = static_cast<uint8_t>((green < 0)     ? 0
                                             : (green > 255) ? 255
                                                             : green);
      newRGBval.blue = static_cast<uint8_t>((blue < 0)     ? 0
                                            : (blue > 255) ? 255
                                                           : blue);

      ESP_LOGI(LOG_TAG, "NewRGBVal: %s", newData.begin());
      auto *nimBLEDriver = static_cast<NimBleDriver *>(arg);
      nimBLEDriver->m_newRGBValueCallback(newRGBval);
    } else {
      ESP_LOGE(LOG_TAG, "Failed to parse RGB values from: %s", newData.begin());
    }
    return resultCode;
  }
  default:
    ESP_LOGE(LOG_TAG, "Received unknown GATT operation: %d", ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
}

int NimBleDriver::GattAccessLedOnOff([[maybe_unused]] uint16_t conn_handle,
                                     [[maybe_unused]] uint16_t attr_handle,
                                     struct ble_gatt_access_ctxt *ctxt,
                                     void *arg) {
  switch (ctxt->op) {
  case BLE_GATT_ACCESS_OP_WRITE_CHR: {
    std::array<char, 1> newData{};
    uint16_t newDataLength = 0;
    const int resultCode = GattSvrChrWrite(ctxt->om, 0, newData.max_size(),
                                           &newData[0], &newDataLength);
    ESP_LOGI(LOG_TAG, "Set Leds: %s", newData[0] == '1' ? "ON" : "OFF");
    auto *nimBLEDriver = static_cast<NimBleDriver *>(arg);
    nimBLEDriver->m_newLedPowerModeCallback(newData[0] == '1' ? true : false);
    return resultCode;
  }
  default:
    ESP_LOGE(LOG_TAG, "Received unknown GATT operation: %d", ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
  }
}

int NimBleDriver::GattSvrChrWrite(struct os_mbuf *data, uint16_t min_len,
                                  uint16_t max_len, char *dst, uint16_t *len) {
  if (!len || !dst || data->om_len < min_len || data->om_len > max_len) {
    return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
  }
  const auto resultCode = ble_hs_mbuf_to_flat(data, dst, max_len, len);
  ESP_LOGI(LOG_TAG, "BLE received %d bytes", *len);
  return resultCode;
}