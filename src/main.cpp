#include <Arduino.h>
#include "BluetoothA2DPSink.h"

BluetoothA2DPSink A2DPsink;

void setup() {
  static const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = (i2s_bits_per_sample_t)16,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = true
  };
  A2DPsink.set_i2s_config(i2s_config);
  A2DPsink.start("ESP-Speaker");
  vTaskDelete(NULL);
}

void loop() {

}
