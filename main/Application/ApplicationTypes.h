// ---------------------------------------------------------------------------
//
// Filename:
//   ApplicationTypes.h
//
// Product or product-subsystem:
//   RvIHH-Scanner
//
// Original author:
//   Daphne Annink
//
// Description:
//   Defines all types used by the application
//
// ---------------------------------------------------------------------------

#ifndef BC_APPLICATION_TYPES_H
#define BC_APPLICATION_TYPES_H

#include <cstdint>
constexpr uint8_t LED_COUNT = 52;

struct RGB_t {
  uint8_t red{};
  uint8_t green{};
  uint8_t blue{};
};

#endif // BC_APPLICATION_TYPES_H