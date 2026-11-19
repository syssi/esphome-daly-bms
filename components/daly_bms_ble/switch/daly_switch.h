#pragma once

#include "../daly_bms_ble.h"
#include "esphome/components/switch/switch.h"

namespace esphome::daly_bms_ble {

class DalyBmsBle;
class DalySwitch : public switch_::Switch {
 public:
  void set_parent(DalyBmsBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint16_t holding_register) { this->holding_register_ = holding_register; };

 protected:
  void write_state(bool state) override;
  DalyBmsBle *parent_;
  uint16_t holding_register_;
};

}  // namespace esphome::daly_bms_ble
