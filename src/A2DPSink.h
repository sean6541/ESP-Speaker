#ifndef A2DPSINK_H
#define A2DPSINK_H

#include <Arduino.h>
#include <esp_a2dp_api.h>

typedef struct {
  esp_a2d_cb_event_t event;
  esp_a2d_cb_param_t *param;
} a2dp_callback_data_t;

class A2DPSink {
  public:
    A2DPSink() {}
    void begin(const char* dev_name);
  private:
    static void _a2dpEventHandler(void *a2dp_callback_data_v, uint32_t);
    static void _a2dpDataHandler(void *data_v, uint32_t len);
};

extern A2DPSink A2DPsink;

#endif
