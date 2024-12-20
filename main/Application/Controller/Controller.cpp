// ---------------------------------------------------------------------------
//
// Filename:
//   Controller.cpp
//
// Product or product-subsystem:
//   RvIHH Scanner
//
// Original author:
//   Daphne Annink
//
// Description:
//
//
// ---------------------------------------------------------------------------

#include "Controller.h"
#include "Application/ApplicationTypes.h"

#include <esp_log.h>
#include <freertos/idf_additions.h>
#include <freertos/projdefs.h>
#include <portmacro.h>

namespace {
constexpr auto LOG_TAG = "Controller";
} // namespace

Controller::Controller(){};

void Controller::Start() {
  ESP_LOGI(LOG_TAG, "Start Application");
  m_ledService.Start();
}
