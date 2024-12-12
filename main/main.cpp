#include "Application/Controller/Controller.h"

namespace {
Controller controller;
} // namespace

extern "C" {
// Called by ESP-IDF (cpu_start.c)
void app_main(void) { controller.Start(); }
}
