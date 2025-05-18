#include "ws2812_rmt.h"
#include <driver/rmt.h>
#include <esp_log.h>

namespace esphome {
namespace ws2812_rmt {

static const char *TAG = "ws2812_rmt";

static const uint32_t WS2812_T0H_NS = 400;
static const uint32_t WS2812_T0L_NS = 850;
static const uint32_t WS2812_T1H_NS = 800;
static const uint32_t WS2812_T1L_NS = 450;
static const uint32_t WS2812_RESET_US = 50;

void WS2812RMT::setup() {
  ESP_LOGI(TAG, "Initializing WS2812 RMT on GPIO%d with %d LEDs", pin_, num_leds_);

  rmt_config_t config = RMT_DEFAULT_CONFIG_TX((gpio_num_t)pin_, rmt_channel_);
  config.clk_div = 2;
  config.mem_block_num = 1;
  config.tx_config.loop_en = false;
  config.tx_config.carrier_en = false;
  config.tx_config.idle_output_en = true;
  config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

  ESP_ERROR_CHECK(rmt_config(&config));
  ESP_ERROR_CHECK(rmt_driver_install(rmt_channel_, 0, 0));
}

void WS2812RMT::write_state(light::LightState *state) {
  auto call = state->current_values;
  uint8_t red = (uint8_t)(call.get_red() * 255);
  uint8_t green = (uint8_t)(call.get_green() * 255);
  uint8_t blue = (uint8_t)(call.get_blue() * 255);

  rmt_item32_t rmt_items[24 * num_leds_];
  for (size_t led = 0; led < num_leds_; led++) {
    uint32_t bits = (green << 16) | (red << 8) | blue;
    for (int j = 0; j < 24; j++) {
      bool bit = (bits >> (23 - j)) & 1;
      rmt_items[led * 24 + j].duration0 = bit ? WS2812_T1H_NS / 25 : WS2812_T0H_NS / 25;
      rmt_items[led * 24 + j].level0 = 1;
      rmt_items[led * 24 + j].duration1 = bit ? WS2812_T1L_NS / 25 : WS2812_T0L_NS / 25;
      rmt_items[led * 24 + j].level1 = 0;
    }
  }

  ESP_ERROR_CHECK(rmt_write_items(rmt_channel_, rmt_items, 24 * num_leds_, true));
  ESP_ERROR_CHECK(rmt_wait_tx_done(rmt_channel_, pdMS_TO_TICKS(100)));
  vTaskDelay(pdMS_TO_TICKS(WS2812_RESET_US / 1000));
}

}  // namespace ws2812_rmt
}  // namespace esphome
