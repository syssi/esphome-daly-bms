#pragma once

#include "../daly_bms_ble.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace daly_bms_ble {

class DalyBmsBle;
class DalyNumber : public number::Number, public Component {
 public:
  void set_parent(DalyBmsBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint16_t holding_register) { this->holding_register_ = holding_register; };
  void set_factor(float factor) { this->factor_ = factor; };
  void dump_config() override;

 protected:
  void control(float value) override;
  DalyBmsBle *parent_;
  uint16_t holding_register_;
  float factor_{1.0f};
};

}  // namespace daly_bms_ble
}  // namespace esphome
