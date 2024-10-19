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
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void write_state(bool state) override;
  DalyBmsBle *parent_;
  uint8_t holding_register_;
};

}  // namespace daly_bms_ble
}  // namespace esphome
