// ---------------------------------------------------------------------------
//
// Filename:
//   Controller.h
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

#ifndef BC_APPLICATION_CONTROLLER_H
#define BC_APPLICATION_CONTROLLER_H

#include "Application/Services/LedService.h"

class Controller {
public:
  Controller();
  void Start();

private:
  LedService m_ledService{};
};

#endif // BC_APPLICATION_CONTROLLER_H
