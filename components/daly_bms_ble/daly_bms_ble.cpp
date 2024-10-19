#include "daly_bms_ble.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace daly_bms_ble {

static const char *const TAG = "daly_bms_ble";

static const uint16_t DALY_BMS_SERVICE_UUID = 0xFFF0;
static const uint16_t DALY_BMS_NOTIFY_CHARACTERISTIC_UUID = 0xFFF1;   // handle 0x17?
static const uint16_t DALY_BMS_CONTROL_CHARACTERISTIC_UUID = 0xFFF2;  // handle 0x15?

static const uint8_t DALY_FRAME_START = 0xD2;
static const uint8_t DALY_FRAME_START2 = 0x03;

static const uint8_t DALY_FRAME_LEN_STATUS = 0x7C;
static const uint8_t DALY_FRAME_LEN_VERSIONS = 0x40;
static const uint8_t DALY_FRAME_LEN_PASSWORD = 0x06;

static const uint8_t DALY_COMMAND_REQ_STATUS = 0x00;

static const uint8_t MAX_RESPONSE_SIZE = 129;

static const uint8_t ERRORS_SIZE = 8;
static const char *const ERRORS[ERRORS_SIZE] = {
    "Total voltage overcharge protection",   // 0000 0001
    "Single voltage overcharge protection",  // 0000 0010
    "Charge overcurrent protection",         // 0000 0100
    "Discharge overcurrent protection",      // 0000 1000
    "Total voltage overdischarge",           // 0001 0000
    "Single voltage overdischarge",          // 0010 0000
    "High temperature protection",           // 0100 0000
    "Short circuit protection",              // 1000 0000
};

void DalyBmsBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                     esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      break;
    }
    case ESP_GATTC_DISCONNECT_EVT: {
      this->node_state = espbt::ClientState::IDLE;

      // this->publish_state_(this->voltage_sensor_, NAN);
      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      auto *char_notify = this->parent_->get_characteristic(DALY_BMS_SERVICE_UUID, DALY_BMS_NOTIFY_CHARACTERISTIC_UUID);
      if (char_notify == nullptr) {
        ESP_LOGE(TAG, "[%s] No notify service found at device, not an Daly BMS..?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_notify_handle_ = char_notify->handle;

      auto status = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(),
                                                      char_notify->handle);
      if (status) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
      }

      auto *char_command =
          this->parent_->get_characteristic(DALY_BMS_SERVICE_UUID, DALY_BMS_CONTROL_CHARACTERISTIC_UUID);
      if (char_command == nullptr) {
        ESP_LOGE(TAG, "[%s] No control service found at device, not an Daly BMS..?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_command_handle_ = char_command->handle;
      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      this->node_state = espbt::ClientState::ESTABLISHED;

      this->send_command(DALY_COMMAND_REQ_STATUS, 0x00);
      break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
      ESP_LOGV(TAG, "Notification received (handle 0x%02X): %s", param->notify.handle,
               format_hex_pretty(param->notify.value, param->notify.value_len).c_str());

      std::vector<uint8_t> data(param->notify.value, param->notify.value + param->notify.value_len);

      this->on_daly_bms_ble_data(param->notify.handle, data);
      break;
    }
    default:
      break;
  }
}

void DalyBmsBle::update() {
  if (this->node_state != espbt::ClientState::ESTABLISHED) {
    ESP_LOGW(TAG, "[%s] Not connected", this->parent_->address_str().c_str());
    return;
  }

  this->send_command(DALY_COMMAND_REQ_STATUS, 0x00);
}

void DalyBmsBle::on_daly_bms_ble_data(const uint8_t &handle, const std::vector<uint8_t> &data) {
  if (data[0] != DALY_FRAME_START || data[1] != DALY_FRAME_START2 || data.size() > MAX_RESPONSE_SIZE) {
    ESP_LOGW(TAG, "Invalid response received: %s", format_hex_pretty(&data.front(), data.size()).c_str());
    return;
  }

  uint8_t frame_len = data.size();
  uint16_t computed_crc = crc16(data.data(), frame_len - 2);
  uint16_t remote_crc = uint16_t(data[frame_len - 2]) | (uint16_t(data[frame_len - 1]) << 8);
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "CRC check failed! 0x%04X != 0x%04X", computed_crc, remote_crc);
    return;
  }

  uint8_t frame_type = data[2];  // data length

  switch (frame_type) {
    case DALY_FRAME_LEN_STATUS:
      this->decode_status_data_(data);
      break;
    case DALY_FRAME_LEN_VERSIONS:
      this->decode_version_data_(data);
      break;
    case DALY_FRAME_LEN_PASSWORD:
      this->decode_password_data_(data);
      break;
    case 0x20:  // Run Info Last Battery Value
    case 0x52:  // Set Info
    default:
      ESP_LOGW(TAG, "Unhandled response received (frame_type 0x%02X): %s", frame_type,
               format_hex_pretty(&data.front(), data.size()).c_str());
  }
}

void DalyBmsBle::decode_status_data_(const std::vector<uint8_t> &data) {
  auto daly_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGI(TAG, "Status frame received");
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // See docs/dalyModbusProtocol.xlsx
  //
  // Byte Len Payload              Description                      Unit  Precision
  //   0   1  0xD2                 Start of frame
  //   1   1  0x03                 Start of frame
  //   2   1  0x7C                 Data length
  //   3   2  0x10 0x1F            Cell voltage 1
  //   5   2  0x10 0x29            Cell voltage 2
  //   7   2  0x10 0x33            Cell voltage 3
  //   9   2  0x10 0x3D            Cell voltage 4
  //  11   2  0x00 0x00            Cell voltage 5
  //  13   2  0x00 0x00            Cell voltage 6
  //  15   2  0x00 0x00            Cell voltage 7
  //  17   2  0x00 0x00            Cell voltage 8
  //  19   2  0x00 0x00            Cell voltage 9
  //  21   2  0x00 0x00            Cell voltage 10
  //  23   2  0x00 0x00            Cell voltage 11
  //  25   2  0x00 0x00            Cell voltage 12
  //  27   2  0x00 0x00            Cell voltage 13
  //  29   2  0x00 0x00            Cell voltage 14
  //  31   2  0x00 0x00            Cell voltage 15
  //  33   2  0x00 0x00            Cell voltage 16
  //  35   2  0x00 0x00            Cell voltage 17
  //  37   2  0x00 0x00            Cell voltage 18
  //  39   2  0x00 0x00            Cell voltage 19
  //  41   2  0x00 0x00            Cell voltage 20
  //  43   2  0x00 0x00            Cell voltage 21
  //  45   2  0x00 0x00            Cell voltage 22
  //  47   2  0x00 0x00            Cell voltage 23
  //  49   2  0x00 0x00            Cell voltage 24
  //  51   2  0x00 0x00            Cell voltage 25
  //  53   2  0x00 0x00            Cell voltage 26
  //  55   2  0x00 0x00            Cell voltage 27
  //  57   2  0x00 0x00            Cell voltage 28
  //  59   2  0x00 0x00            Cell voltage 29
  //  61   2  0x00 0x00            Cell voltage 30
  //  63   2  0x00 0x00            Cell voltage 31
  //  65   2  0x00 0x00            Cell voltage 32
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  uint8_t cells = std::min(data[102], (uint8_t) 16);
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = daly_get_16bit(3 + (i * 2)) * 0.001f;
    average_cell_voltage = average_cell_voltage + cell_voltage;
    if (cell_voltage > 0 && cell_voltage < min_cell_voltage) {
      min_cell_voltage = cell_voltage;
      min_voltage_cell = i + 1;
    }
    if (cell_voltage > max_cell_voltage) {
      max_cell_voltage = cell_voltage;
      max_voltage_cell = i + 1;
    }
    this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
  }
  average_cell_voltage = average_cell_voltage / cells;

  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);
  this->publish_state_(this->max_voltage_cell_sensor_, (float) max_voltage_cell);
  this->publish_state_(this->min_voltage_cell_sensor_, (float) min_voltage_cell);
  // this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);
  this->publish_state_(this->average_cell_voltage_sensor_, average_cell_voltage);

  //  67   2  0x00 0x3C            Temperature 1    [-40,100] °C
  //  69   2  0x00 0x3D            Temperature 2
  //  71   2  0x00 0x3E            Temperature 3
  //  73   2  0x00 0x3F            Temperature 4
  //  75   2  0x00 0x00            Temperature 5
  //  77   2  0x00 0x00            Temperature 6
  //  79   2  0x00 0x00            Temperature 7
  //  81   2  0x00 0x00            Temperature 8
  uint8_t temperature_sensors = std::min(data[104], (uint8_t) 8);
  this->publish_state_(this->temperature_sensors_sensor_, temperature_sensors);
  for (uint8_t i = 0; i < temperature_sensors; i++) {
    this->publish_state_(this->temperatures_[i].temperature_sensor_, (daly_get_16bit(67 + (i * 2)) - 40) * 1.0f);
  }

  //  83   2  0x00 0x8C            Total voltage
  this->publish_state_(this->total_voltage_sensor_, daly_get_16bit(83) * 0.1f);

  //  85   2  0x75 0x4E            Current
  this->publish_state_(this->current_sensor_, (daly_get_16bit(85) - 30000) * 0.1f);

  //  87   2  0x03 0x84            State of charge
  this->publish_state_(this->state_of_charge_sensor_, daly_get_16bit(87) * 0.1f);

  //  89   2  0x10 0x3D            Max cell voltage
  ESP_LOGV(TAG, "Max cell voltage: %.3f V", daly_get_16bit(89) * 0.001f);

  //  91   2  0x10 0x1F            Min cell voltage
  ESP_LOGV(TAG, "Min cell voltage: %.3f V", daly_get_16bit(91) * 0.001f);

  //  93   2  0x00 0x00            Max cell temperature
  ESP_LOGV(TAG, "Max cell temperature: %.0f °C", (daly_get_16bit(93) - 40) * 1.0f);

  //  95   2  0x00 0x00            Min cell temperature
  ESP_LOGV(TAG, "Min cell temperature: %.0f °C", (daly_get_16bit(95) - 40) * 1.0f);

  //  97   2  0x00 0x00            Charge/discharge status (0=idle, 1=charging, 2=discharging)
  ESP_LOGI(TAG, "Status: %s",
           data[98] == 0   ? "Idle"
           : data[98] == 1 ? "Charging"
           : data[98] == 2 ? "Discharging"
                           : "Unknown");

  //  99   2  0x0D 0x80            Capacity remaining
  this->publish_state_(this->capacity_remaining_sensor_, daly_get_16bit(99) * 0.1f);

  // 101   2  0x00 0x04            Cell count
  this->publish_state_(this->cell_count_sensor_, daly_get_16bit(101) * 1.0f);

  // 103   2  0x00 0x04            Number of temperature sensors
  this->publish_state_(this->temperature_sensors_sensor_, daly_get_16bit(103) * 1.0f);

  // 105   2  0x00 0x39            Charging cycles
  this->publish_state_(this->charging_cycles_sensor_, daly_get_16bit(105) * 1.0f);

  // 107   2  0x00 0x01            Balancer status (0: off, 1: on)
  ESP_LOGI(TAG, "Balancer status: %s", ONOFF((bool) data[108]));

  // 109   2  0x00 0x00            Charging mosfet status (0: off, 1: on)
  ESP_LOGI(TAG, "Charging mosfet: %s", ONOFF((bool) data[109]));

  // 111   2  0x00 0x01            Discharging mosfet status (0: off, 1: on)
  ESP_LOGI(TAG, "Discharging mosfet: %s", ONOFF((bool) data[112]));

  // 113   2  0x10 0x2E            Average cell voltage
  ESP_LOGV(TAG, "Average cell voltage: %.3f V", daly_get_16bit(113) * 0.001f);

  // 115   2  0x01 0x41            Delta cell voltage
  this->publish_state_(this->delta_cell_voltage_sensor_, daly_get_16bit(115) * 0.0001f);

  // 117   2  0x00 0x2A            Power
  float power = daly_get_16bit(117) * 1.0f;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));               // 500W vs 0W -> 500W
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  // 119   2  0x00 0x00            Alarm1
  // 121   2  0x00 0x00            Alarm2
  // 123   2  0x00 0x00            Alarm3
  // 125   2  0x00 0x00            Alarm4
  // 127   2  0xA0 0xDF            CRC
}

void DalyBmsBle::decode_version_data_(const std::vector<uint8_t> &data) {
  ESP_LOGI(TAG, "Software/hardware version frame received");
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // See docs/dalyModbusProtocol.xlsx
  //
  // Byte Len Payload              Description                      Unit  Precision
  //   0   1  0xD2                 Start of frame
  //   1   1  0x03                 Start of frame
  //   2   1  0x40                 Data length
  //   3  32  0x34 0x30 0x31 0x30 0x31 0x32 0x00 0x00 0x00 0x00
  //          0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //          0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //          0x00 0x00            Software version
  ESP_LOGI(TAG, "Software version: %s", std::string(data.begin() + 3, data.begin() + 3 + 32).c_str());

  //  35  32  0x42 0x4D 0x53 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //          0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //          0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
  //          0x00 0x00            Hardware version
  ESP_LOGI(TAG, "Hardware version: %s", std::string(data.begin() + 35, data.begin() + 35 + 32).c_str());

  //  67   2  0x65 0x13            CRC
}

void DalyBmsBle::decode_password_data_(const std::vector<uint8_t> &data) {
  ESP_LOGI(TAG, "Password frame received");
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // See docs/dalyModbusProtocol.xlsx
  //
  // Byte Len Payload              Description                      Unit  Precision
  //   0   1  0xD2                 Start of frame
  //   1   1  0x03                 Start of frame
  //   2   1  0x06                 Data length
  //   3   6  0x31 0x32 0x33 0x34 0x35 0x36   Password
  ESP_LOGI(TAG, "Password: %s", std::string(data.begin() + 3, data.begin() + 3 + 6).c_str());

  //   9   2  0x4C 0x69            CRC
}

void DalyBmsBle::dump_config() {  // NOLINT(google-readability-function-size,readability-function-size)
  ESP_LOGCONFIG(TAG, "DalyBmsBle:");

  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);

  LOG_SENSOR("", "Total voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Charging power", this->charging_power_sensor_);
  LOG_SENSOR("", "Discharging power", this->discharging_power_sensor_);
  LOG_SENSOR("", "Error bitmask", this->error_bitmask_sensor_);
  LOG_SENSOR("", "State of charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Charging cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Min cell voltage", this->min_cell_voltage_sensor_);
  LOG_SENSOR("", "Max cell voltage", this->max_cell_voltage_sensor_);
  LOG_SENSOR("", "Min voltage cell", this->min_voltage_cell_sensor_);
  LOG_SENSOR("", "Max voltage cell", this->max_voltage_cell_sensor_);
  LOG_SENSOR("", "Delta cell voltage", this->delta_cell_voltage_sensor_);
  LOG_SENSOR("", "Temperature 1", this->temperatures_[0].temperature_sensor_);
  LOG_SENSOR("", "Temperature 2", this->temperatures_[1].temperature_sensor_);
  LOG_SENSOR("", "Temperature 3", this->temperatures_[2].temperature_sensor_);
  LOG_SENSOR("", "Temperature 4", this->temperatures_[3].temperature_sensor_);
  LOG_SENSOR("", "Temperature 5", this->temperatures_[4].temperature_sensor_);
  LOG_SENSOR("", "Temperature 6", this->temperatures_[5].temperature_sensor_);
  LOG_SENSOR("", "Temperature 7", this->temperatures_[6].temperature_sensor_);
  LOG_SENSOR("", "Temperature 8", this->temperatures_[7].temperature_sensor_);
  LOG_SENSOR("", "Cell Voltage 1", this->cells_[0].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 2", this->cells_[1].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 3", this->cells_[2].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 4", this->cells_[3].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 5", this->cells_[4].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 6", this->cells_[5].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 7", this->cells_[6].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 8", this->cells_[7].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 9", this->cells_[8].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 10", this->cells_[9].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 11", this->cells_[10].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 12", this->cells_[11].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 13", this->cells_[12].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 14", this->cells_[13].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 15", this->cells_[14].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 16", this->cells_[15].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell count", this->cell_count_sensor_);
  LOG_SENSOR("", "Temperature sensors", this->temperature_sensors_sensor_);
  LOG_SENSOR("", "Capacity remaining", this->capacity_remaining_sensor_);

  LOG_TEXT_SENSOR("", "Errors", this->errors_text_sensor_);
}

void DalyBmsBle::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void DalyBmsBle::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void DalyBmsBle::publish_state_(switch_::Switch *obj, const bool &state) {
  if (obj == nullptr)
    return;

  obj->publish_state(state);
}

void DalyBmsBle::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

bool DalyBmsBle::send_command(uint8_t function, uint8_t value) {
  uint8_t frame[8];

  frame[0] = 0xD2;  // Modbus device address
  frame[1] = 0x03;  // Function (0x03: Read register, 0x06: Write register, 0x10: Write multiple registers)
  frame[2] = 0x00;  // Starting Address Hi
  frame[3] = 0x00;  // Starting Address Lo
  frame[4] = 0x00;  // Quantity of inputs Hi
  frame[5] = 0x3E;  // Quantity of inputs Lo
  frame[6] = 0xD7;  // CRC
  frame[7] = 0xB9;  // CRC

  ESP_LOGD(TAG, "Send command (handle 0x%02X): %s", this->char_command_handle_,
           format_hex_pretty(frame, sizeof(frame)).c_str());

  auto status =
      esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->char_command_handle_,
                               sizeof(frame), frame, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);

  if (status) {
    ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);
  }

  return (status == 0);
}

std::string DalyBmsBle::bitmask_to_string_(const char *const messages[], const uint8_t &messages_size,
                                           const uint8_t &mask) {
  std::string values = "";
  if (mask) {
    for (int i = 0; i < messages_size; i++) {
      if (mask & (1 << i)) {
        values.append(messages[i]);
        values.append(";");
      }
    }
    if (!values.empty()) {
      values.pop_back();
    }
  }
  return values;
}

}  // namespace daly_bms_ble
}  // namespace esphome
