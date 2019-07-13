#include <Arduino.h>
#include "a2dp_sink.h"
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>
#include <esp_a2dp_api.h>
#include <driver/i2s.h>

A2DP::A2DP(const char* dev_name) : _dev_name(dev_name) {}

void A2DP::begin() {
  i2s_config_t i2s_config = {
    .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 48000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 6,
    .dma_buf_len = 60,
    .use_apll = true,
    .tx_desc_auto_clear = true,
  };
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

  i2s_pin_config_t pin_config = {
    .bck_io_num = 26,
    .ws_io_num = 22,
    .data_out_num = 25,
    .data_in_num = -1
  };
  i2s_set_pin(I2S_NUM_0, &pin_config);

  btStart();

  esp_bluedroid_init();
  esp_bluedroid_enable();

  esp_bt_dev_set_device_name(_dev_name);

  esp_a2d_register_callback([] (esp_a2d_cb_event_t event, esp_a2d_cb_param_t *param) {
    a2dp_callback_data_t a2dp_callback_data = { .event = event, .param = param };
    xTimerPendFunctionCall(_a2dpEventHandler, &a2dp_callback_data, NULL, 0);
  });
  esp_a2d_sink_register_data_callback([] (const uint8_t *data, uint32_t len) {
    xTimerPendFunctionCall(_a2dpDataHandler, (void *) data, len, 0);
  });
  esp_a2d_sink_init();

  esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
}

void A2DP::_a2dpEventHandler(void *a2dp_callback_data_v, uint32_t) {
  a2dp_callback_data_t a2dp_callback_data = *(a2dp_callback_data_t *) a2dp_callback_data_v;
  esp_a2d_cb_event_t event = a2dp_callback_data.event;
  esp_a2d_cb_param_t *param = a2dp_callback_data.param;
  switch(event) {
    case ESP_A2D_CONNECTION_STATE_EVT: {
      if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);
      } else if (param->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_NONE);
      }
      break;
    }
    case ESP_A2D_AUDIO_CFG_EVT: {
      if (param->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
        int sample_rate = 16000;
        char oct0 = param->audio_cfg.mcc.cie.sbc[0];
        if (oct0 & (0x01 << 6)) {
          sample_rate = 32000;
        } else if (oct0 & (0x01 << 5)) {
          sample_rate = 44100;
        } else if (oct0 & (0x01 << 4)) {
          sample_rate = 48000;
        }
        i2s_set_clk(I2S_NUM_0, sample_rate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
      }
      break;
    }
    default: {
      break;
    }
  }
}

void A2DP::_a2dpDataHandler(void *data_v, uint32_t len) {
  const uint8_t *data = (const uint8_t *) data_v;
  size_t bytes_written;
  i2s_write(I2S_NUM_0, data, len, &bytes_written, portMAX_DELAY);
}
