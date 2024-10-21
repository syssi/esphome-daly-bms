#include "daly_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace daly_bms_ble {

static const char *const TAG = "daly_bms_ble.button";

static const uint8_t DALY_FUNCTION_READ = 0x03;
static const uint8_t DALY_FUNCTION_WRITE = 0x06;

static const uint16_t DALY_COMMAND_REQ_SETTINGS_START = 0x0080;
static const uint16_t DALY_COMMAND_REQ_SETTINGS_QTY = 0x0029;

void DalyButton::dump_config() { LOG_BUTTON("", "DalyBmsBle Button", this); }
void DalyButton::press_action() {
  if (this->holding_register_ == DALY_COMMAND_REQ_SETTINGS_START) {
    this->parent_->send_command(DALY_FUNCTION_READ, this->holding_register_, DALY_COMMAND_REQ_SETTINGS_QTY);
    return;
  }

  this->parent_->send_command(DALY_FUNCTION_WRITE, this->holding_register_, 0x0001);
}

}  // namespace daly_bms_ble
}  // namespace esphome
