#include "daly_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace daly_bms_ble {

static const char *const TAG = "daly_bms_ble.button";

static const uint16_t DALY_COMMAND_FACTORY_RESET = 0xCCCC;

void DalyButton::dump_config() { LOG_BUTTON("", "DalyBmsBle Button", this); }
void DalyButton::press_action() {
  if (this->holding_register_ == DALY_COMMAND_FACTORY_RESET) {
    this->parent_->send_factory_reset();
    return;
  }

  this->parent_->send_command(this->holding_register_);
}

}  // namespace daly_bms_ble
}  // namespace esphome
