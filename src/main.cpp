#include <Arduino.h>
#include "a2dp_sink.h"

A2DP a2dp("ESP32-Speaker");

extern "C" void app_main() {
  initArduino();
  a2dp.begin();
}
