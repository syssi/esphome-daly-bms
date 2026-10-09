#include "daly_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::daly_bms_ble {

ESPHOME_LOG_TAG(TAG, "daly_bms_ble.button");

void DalyButton::dump_config() { LOG_BUTTON("", "DalyBmsBle Button", this); }
void DalyButton::press_action() { this->parent_->send_command(this->function_, this->holding_register_, this->value_); }

}  // namespace esphome::daly_bms_ble
