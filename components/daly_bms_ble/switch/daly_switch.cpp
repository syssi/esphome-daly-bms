#include "daly_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::daly_bms_ble {

ESPHOME_LOG_TAG(TAG, "daly_bms_ble.switch");

static const uint8_t DALY_FUNCTION_WRITE = 0x06;

void DalySwitch::dump_config() { LOG_SWITCH("", "DalyBmsBle Switch", this); }
void DalySwitch::write_state(bool state) {
  this->parent_->send_command(DALY_FUNCTION_WRITE, this->holding_register_, (uint16_t) state);
  this->publish_state(state);
}

}  // namespace esphome::daly_bms_ble
