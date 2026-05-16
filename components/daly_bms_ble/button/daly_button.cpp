#include "daly_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome::daly_bms_ble {

static const char *const TAG = "daly_bms_ble.button";

void DalyButton::dump_config() { LOG_BUTTON("", "DalyBmsBle Button", this); }
void DalyButton::press_action() { this->parent_->send_command(this->function_, this->holding_register_, this->value_); }

}  // namespace esphome::daly_bms_ble
