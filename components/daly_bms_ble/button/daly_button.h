#pragma once

#include "../daly_bms_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/button/button.h"

namespace esphome::daly_bms_ble {

class DalyBmsBle;
class DalyButton : public button::Button, public Component {
 public:
  void set_parent(DalyBmsBle *parent) { this->parent_ = parent; };
  void set_function(uint8_t function) { this->function_ = function; };
  void set_holding_register(uint16_t holding_register) { this->holding_register_ = holding_register; };
  void set_value(uint16_t value) { this->value_ = value; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void press_action() override;
  DalyBmsBle *parent_;
  uint8_t function_{0};
  uint16_t holding_register_{0};
  uint16_t value_{0};
};

}  // namespace esphome::daly_bms_ble
