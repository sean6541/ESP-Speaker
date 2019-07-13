#ifndef A2DP_SINK_H
#define A2DP_SINK_H

#include <Arduino.h>
#include <esp_a2dp_api.h>

typedef struct {
  esp_a2d_cb_event_t event;
  esp_a2d_cb_param_t *param;
} a2dp_callback_data_t;

class A2DP {
  public:
    A2DP(const char* dev_name);
    void begin();
  private:
    static void _a2dpEventHandler(void *a2dp_callback_data_v, uint32_t);
    static void _a2dpDataHandler(void *data_v, uint32_t len);
    const char* _dev_name;
};

#endif
