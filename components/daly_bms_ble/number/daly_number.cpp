#include "daly_number.h"
#include "esphome/core/log.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::daly_bms_ble {

ESPHOME_LOG_TAG(TAG, "daly_bms_ble.number");

void DalyNumber::dump_config() { LOG_NUMBER("", "DalyBmsBle Number", this); }
void DalyNumber::control(float value) {
  this->parent_->write_register(this->holding_register_, (uint16_t) (value * this->factor_ + this->offset_));
  this->publish_state(value);
}

}  // namespace esphome::daly_bms_ble
