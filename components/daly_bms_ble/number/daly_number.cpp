#include "daly_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace daly_bms_ble {

static const char *const TAG = "daly_bms_ble.number";

void DalyNumber::dump_config() { LOG_NUMBER("", "DalyBmsBle Number", this); }
void DalyNumber::control(float value) {
  if (this->parent_->write_register(this->holding_register_, (uint16_t) (value * this->factor_))) {
    this->publish_state(value);
  }
}

}  // namespace daly_bms_ble
}  // namespace esphome
