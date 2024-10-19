#include "daly_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace daly_bms_ble {

static const char *const TAG = "daly_bms_ble.button";

void DalyButton::dump_config() { LOG_BUTTON("", "DalyBmsBle Button", this); }
void DalyButton::press_action() { this->parent_->send_command(this->holding_register_, 0x0000); }

}  // namespace daly_bms_ble
}  // namespace esphome
