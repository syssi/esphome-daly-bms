#include "daly_switch.h"

namespace esphome::daly_bms_ble {

static const uint8_t DALY_FUNCTION_WRITE = 0x06;

void DalySwitch::write_state(bool state) {
  this->parent_->send_command(DALY_FUNCTION_WRITE, this->holding_register_, (uint16_t) state);
  this->publish_state(state);
}

}  // namespace esphome::daly_bms_ble
