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

static const uint8_t DALY_FRAME_TYPE_STATUS = 0x01;     // 40 bytes
static const uint8_t DALY_FRAME_TYPE_CELL_INFO = 0x02;  // 40 bytes
static const uint8_t DALY_FRAME_TYPE_SETTINGS = 0x03;   // 108 bytes

static const uint16_t DALY_COMMAND_REQ_STATUS = 0xc565;     // 0xc501
static const uint16_t DALY_COMMAND_REQ_CELL_INFO = 0x5b65;  // 0x5b02
static const uint16_t DALY_COMMAND_REQ_SETTINGS = 0x5600;   // 0x5656

static const uint8_t MAX_RESPONSE_SIZE = 129;
static const uint8_t MAX_KNOWN_CELL_COUNT = 16;

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

uint16_t crc16(const uint8_t *data, size_t length) {
  uint16_t crc = 0xFFFF;
  for (int i = 0; i < length; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

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

      this->send_command(DALY_COMMAND_REQ_STATUS);
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

  this->send_command(DALY_COMMAND_REQ_STATUS);
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
    case 0x7C:
      this->decode_status_data_(data);
      break;
    case 0x20:  // Run Info Last Battery Value
    case 0x52:  // Set Info
    case 0x40:  // Version Info
    case 0x06:  // Password?
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

  if (data.size() < 40) {
    ESP_LOGW(TAG, "Invalid status frame length: %d", data.size());
    return;
  }

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
  ESP_LOGI(TAG, "Cell voltage 1: %.3f V", daly_get_16bit(3) * 0.001f);
  ESP_LOGI(TAG, "Cell voltage 2: %.3f V", daly_get_16bit(5) * 0.001f);
  ESP_LOGI(TAG, "Cell voltage 3: %.3f V", daly_get_16bit(7) * 0.001f);
  ESP_LOGI(TAG, "Cell voltage 4: %.3f V", daly_get_16bit(9) * 0.001f);

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
  //  67   2  0x00 0x3C            Temperature 1    [-40,100] °C
  //  69   2  0x00 0x3D            Temperature 2
  //  71   2  0x00 0x3E            Temperature 3
  //  73   2  0x00 0x3F            Temperature 4
  ESP_LOGI(TAG, "Temperature 1: %.2f °C", (daly_get_16bit(67) - 40) * 1.0f);
  ESP_LOGI(TAG, "Temperature 2: %.2f °C", (daly_get_16bit(69) - 40) * 1.0f);
  ESP_LOGI(TAG, "Temperature 3: %.2f °C", (daly_get_16bit(71) - 40) * 1.0f);
  ESP_LOGI(TAG, "Temperature 4: %.2f °C", (daly_get_16bit(73) - 40) * 1.0f);
  //  75   2  0x00 0x00            Temperature 5
  //  77   2  0x00 0x00            Temperature 6
  //  79   2  0x00 0x00            Temperature 7
  //  81   2  0x00 0x00            Temperature 8
  //  83   2  0x00 0x8C            Total voltage
  ESP_LOGI(TAG, "Battery voltage: %.1f V", daly_get_16bit(83) * 0.1f);
  //  85   2  0x75 0x4E            Current
  ESP_LOGI(TAG, "Current: %.1f A", (daly_get_16bit(85) - 30000) * 0.1f);
  //  87   2  0x03 0x84            State of charge
  ESP_LOGI(TAG, "State of charge: %.0f %%", daly_get_16bit(87) * 0.001f);
  //  89   2  0x10 0x3D            Max cell voltage
  ESP_LOGI(TAG, "Max cell voltage: %.3f V", daly_get_16bit(89) * 0.001f);
  //  91   2  0x10 0x1F            Min cell voltage
  ESP_LOGI(TAG, "Min cell voltage: %.3f V", daly_get_16bit(91) * 0.001f);
  //  93   2  0x00 0x00            Max cell temperature
  ESP_LOGI(TAG, "Max cell temperature: %.0f °C", (daly_get_16bit(93) - 40) * 1.0f);
  //  95   2  0x00 0x00            Min cell temperature
  ESP_LOGI(TAG, "Min cell temperature: %.0f °C", (daly_get_16bit(95) - 40) * 1.0f);
  //  97   2  0x00 0x00            Charge/discharge status (0=?, 1=charging, 2=discharging)
  //  99   2  0x0D 0x80            Capacity remaining
  ESP_LOGI(TAG, "Capacity remaining: %.1f Ah", daly_get_16bit(99) * 0.1f);
  // 101   2  0x00 0x04            Cell count
  ESP_LOGI(TAG, "Cell count: %.0f", daly_get_16bit(101) * 1.0f);
  // 103   2  0x00 0x04            Number of temperature sensors
  ESP_LOGI(TAG, "Number of temperature sensors: %.0f", daly_get_16bit(103) * 1.0f);
  // 105   2  0x00 0x39            Charging cycles
  ESP_LOGI(TAG, "Charging cycles: %.0f", daly_get_16bit(105) * 1.0f);
  // 107   2  0x00 0x01            Balancer status (0: off, 1: on)
  // 109   2  0x00 0x00            Charging mosfet status (0: off, 1: on)
  // 111   2  0x00 0x01            Discharging mosfet status (0: off, 1: on)
  // 113   2  0x10 0x2E            Average cell voltage
  ESP_LOGI(TAG, "Average cell voltage: %.3f V", daly_get_16bit(113) * 0.001f);
  // 115   2  0x01 0x41            Delta cell voltage
  ESP_LOGI(TAG, "Delta cell voltage: %.3f V", daly_get_16bit(115) * 0.001f);
  // 117   2  0x00 0x2A            Power
  ESP_LOGI(TAG, "Power: %.0f W", daly_get_16bit(117) * 1.0f);
  // 119   2  0x00 0x00            Alarm1
  // 121   2  0x00 0x00            Alarm2
  // 123   2  0x00 0x00            Alarm3
  // 125   2  0x00 0x00            Alarm4
  // 127   2  0xA0 0xDF            CRC
}

void DalyBmsBle::decode_cell_info_data_(const std::vector<uint8_t> &data) {
  auto daly_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };
  auto daly_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(daly_get_16bit(i + 0)) << 16) | (uint32_t(daly_get_16bit(i + 2)) << 0);
  };

  ESP_LOGI(TAG, "Cell info frame received");
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload              Description                      Unit  Precision
  //  0    1  0x02                 Frame type
  //  1    1  0x00
  //  2    1  0x07                 Cell count
  ESP_LOGI(TAG, "  Cell count: %d", data[2]);
  uint8_t cell_count = data[2];

  //  3    1  0x00
  //  4    1  0x00
  //  5    1  0x00
  //  6    1  0x00
  //  7    1  0x00
  //  8    4  0x00 0x00 0x00 0x00  Balancing mask
  ESP_LOGI(TAG, "  Balancing mask: %d", daly_get_32bit(8));

  //  12   1  0x00
  //  13   1  0x00
  //  14   1  0x00
  //  15   1  0x00
  //  16   1  0x00
  //  17   1  0x00
  //  18   1  0x00
  //  19   1  0x00
  //  20   4  0x40 0x56 0x66 0x66  Balancer voltage
  this->publish_state_(this->balancer_voltage_sensor_, ieee_float_(daly_get_32bit(20)));

  //  24   4  0x40 0x53 0x33 0x33  Cell voltage 1
  //  28   4  0x40 0x53 0x3a 0xe1  Cell voltage 2
  //  32   4  0x40 0x53 0x47 0xae  Cell voltage 3
  //  36   4  0x40 0x53 0x3a 0xe1  Cell voltage 4
  //  40   4  0x40 0x53 0x42 0x8f  Cell voltage 5
  //  44   4  0x40 0x53 0x40 0x00  Cell voltage 6
  //  48   4  0x40 0x53 0x3d 0x71  Cell voltage 7
  //  52   4  0x00 0x00 0x00 0x00  Cell voltage 8
  //  56   4  0x00 0x00 0x00 0x00  Cell voltage 9
  //  60   4  0x00 0x00 0x00 0x00  Cell voltage 10
  //  64   4  0x00 0x00 0x00 0x00  Cell voltage 11
  //  68   4  0x00 0x00 0x00 0x00  Cell voltage 12
  //  72   4  0x00 0x00 0x00 0x00  Cell voltage 13
  //  76   4  0x00 0x00 0x00 0x00  Cell voltage 14
  //  80   4  0x00 0x00 0x00 0x00  Cell voltage 15
  //  84   4  0x00 0x00 0x00 0x00  Cell voltage 16
  float min_cell_voltage = 100.0f;
  float max_cell_voltage = -100.0f;
  float average_cell_voltage = 0.0f;
  uint8_t min_voltage_cell = 0;
  uint8_t max_voltage_cell = 0;
  uint8_t cells = std::min(MAX_KNOWN_CELL_COUNT, cell_count);
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = ieee_float_(daly_get_32bit((i * 4) + 24));
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
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);
  this->publish_state_(this->average_cell_voltage_sensor_, average_cell_voltage);
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
  LOG_SENSOR("", "Balancer voltage", this->balancer_voltage_sensor_);
  LOG_SENSOR("", "Total charged capacity", this->total_charged_capacity_sensor_);
  LOG_SENSOR("", "Total discharged capacity", this->total_discharged_capacity_sensor_);

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

bool DalyBmsBle::send_command(uint16_t function) {
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

bool DalyBmsBle::send_factory_reset() { return false; }

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
