#pragma once

#include "../daly_bms_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace daly_bms_ble {

class DalyBmsBle;
class DalySwitch : public switch_::Switch, public Component {
 public:
  void set_parent(DalyBmsBle *parent) { this->parent_ = parent; };
  void set_command_turn_on(uint16_t command_turn_on) { this->command_turn_on_ = command_turn_on; };
  void set_command_turn_off(uint16_t command_turn_off) { this->command_turn_off_ = command_turn_off; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void write_state(bool state) override;
  DalyBmsBle *parent_;
  uint16_t command_turn_on_;
  uint16_t command_turn_off_;
};

}  // namespace daly_bms_ble
}  // namespace esphome
