#include "daly_button.h"

namespace esphome::daly_bms_ble {

void DalyButton::press_action() { this->parent_->send_command(this->function_, this->holding_register_, this->value_); }

}  // namespace esphome::daly_bms_ble
