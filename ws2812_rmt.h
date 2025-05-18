#pragma once

#include "esphome.h"
#include "esphome/components/light/light_output.h"

namespace esphome {
namespace ws2812_rmt {

class WS2812RMT : public light::LightOutput {
 public:
  void setup() override;
  void write_state(light::LightState *state) override;
  light::LightTraits get_traits() override {
    return light::LightTraits(true, true, false, light::ColorMode::RGB);
  }
  void set_pin(uint8_t pin) { pin_ = pin; }
  void set_num_leds(uint16_t num) { num_leds_ = num; }
  void set_channel(uint8_t channel) { rmt_channel_ = (rmt_channel_t)channel; }

 protected:
  uint8_t pin_;
  uint16_t num_leds_;
  rmt_channel_t rmt_channel_ = RMT_CHANNEL_0;
};

}  // namespace ws2812_rmt
}  // namespace esphome
