#include "daly_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace daly_bms_ble {

static const char *const TAG = "daly_bms_ble.switch";

void DalySwitch::dump_config() { LOG_SWITCH("", "DalyBmsBle Switch", this); }
void DalySwitch::write_state(bool state) {
  if (this->parent_->send_command(state ? this->command_turn_on_ : this->command_turn_off_)) {
    this->publish_state(state);
  }
}

}  // namespace daly_bms_ble
}  // namespace esphome
