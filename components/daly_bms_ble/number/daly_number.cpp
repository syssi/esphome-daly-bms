#include "daly_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace daly_bms_ble {

static const char *const TAG = "daly_bms_ble.number";

static const uint8_t DALY_FUNCTION_WRITE = 0x06;

void DalyNumber::dump_config() { LOG_NUMBER("", "DalyBmsBle Number", this); }
void DalyNumber::control(float value) {
  // Protocol stores SOC in 0.1 % units (e.g. 68.0 % → 680)
  uint16_t raw_value = (uint16_t) (value * 10);
  if (this->parent_->send_command(DALY_FUNCTION_WRITE, this->holding_register_, raw_value)) {
    this->publish_state(value);
  }
}

}  // namespace daly_bms_ble
}  // namespace esphome
