#pragma once

#include <array>
#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include <map>

#ifdef USE_ESP32
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include <esp_gattc_api.h>
#endif

namespace esphome::daly_bms_ble {

#ifdef USE_ESP32
namespace espbt = esphome::esp32_ble_tracker;
#endif

class DalyBmsBle :
#ifdef USE_ESP32
    public esphome::ble_client::BLEClientNode,
#endif
    public PollingComponent {
 public:
  void dump_config() override;
  void update() override;
  void loop() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_online_status_binary_sensor(binary_sensor::BinarySensor *online_status_binary_sensor) {
    online_status_binary_sensor_ = online_status_binary_sensor;
  }
  void set_balancing_binary_sensor(binary_sensor::BinarySensor *balancing_binary_sensor) {
    balancing_binary_sensor_ = balancing_binary_sensor;
  }
  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
    charging_binary_sensor_ = charging_binary_sensor;
  }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
    discharging_binary_sensor_ = discharging_binary_sensor;
  }

  void set_total_voltage_sensor(sensor::Sensor *total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_charging_power_sensor(sensor::Sensor *charging_power_sensor) {
    charging_power_sensor_ = charging_power_sensor;
  }
  void set_discharging_power_sensor(sensor::Sensor *discharging_power_sensor) {
    discharging_power_sensor_ = discharging_power_sensor;
  }
  void set_error_bitmask_sensor(sensor::Sensor *error_bitmask_sensor) { error_bitmask_sensor_ = error_bitmask_sensor; }
  void set_state_of_charge_sensor(sensor::Sensor *state_of_charge_sensor) {
    state_of_charge_sensor_ = state_of_charge_sensor;
  }
  void set_charging_cycles_sensor(sensor::Sensor *charging_cycles_sensor) {
    charging_cycles_sensor_ = charging_cycles_sensor;
  }
  void set_min_cell_voltage_sensor(sensor::Sensor *min_cell_voltage_sensor) {
    min_cell_voltage_sensor_ = min_cell_voltage_sensor;
  }
  void set_max_cell_voltage_sensor(sensor::Sensor *max_cell_voltage_sensor) {
    max_cell_voltage_sensor_ = max_cell_voltage_sensor;
  }
  void set_min_voltage_cell_sensor(sensor::Sensor *min_voltage_cell_sensor) {
    min_voltage_cell_sensor_ = min_voltage_cell_sensor;
  }
  void set_max_voltage_cell_sensor(sensor::Sensor *max_voltage_cell_sensor) {
    max_voltage_cell_sensor_ = max_voltage_cell_sensor;
  }
  void set_delta_cell_voltage_sensor(sensor::Sensor *delta_cell_voltage_sensor) {
    delta_cell_voltage_sensor_ = delta_cell_voltage_sensor;
  }
  void set_average_cell_voltage_sensor(sensor::Sensor *average_cell_voltage_sensor) {
    average_cell_voltage_sensor_ = average_cell_voltage_sensor;
  }
  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage_sensor) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage_sensor;
  }
  void set_temperature_sensor(uint8_t temperature, sensor::Sensor *temperature_sensor) {
    this->temperatures_[temperature].temperature_sensor_ = temperature_sensor;
  }
  void set_cell_count_sensor(sensor::Sensor *cell_count_sensor) { cell_count_sensor_ = cell_count_sensor; }
  void set_temperature_sensors_sensor(sensor::Sensor *temperature_sensors_sensor) {
    temperature_sensors_sensor_ = temperature_sensors_sensor;
  }
  void set_capacity_remaining_sensor(sensor::Sensor *capacity_remaining_sensor) {
    capacity_remaining_sensor_ = capacity_remaining_sensor;
  }
  void set_balance_current_sensor(sensor::Sensor *balance_current_sensor) {
    balance_current_sensor_ = balance_current_sensor;
  }
  void set_mosfet_temperature_sensor(sensor::Sensor *mosfet_temperature_sensor) {
    mosfet_temperature_sensor_ = mosfet_temperature_sensor;
  }
  void set_board_temperature_sensor(sensor::Sensor *board_temperature_sensor) {
    board_temperature_sensor_ = board_temperature_sensor;
  }
  void set_max_battery_temperature_sensor(sensor::Sensor *s) { max_battery_temperature_sensor_ = s; }
  void set_max_battery_temperature_probe_sensor(sensor::Sensor *s) { max_battery_temperature_probe_sensor_ = s; }
  void set_min_battery_temperature_sensor(sensor::Sensor *s) { min_battery_temperature_sensor_ = s; }
  void set_min_battery_temperature_probe_sensor(sensor::Sensor *s) { min_battery_temperature_probe_sensor_ = s; }
  void set_energy_sensor(sensor::Sensor *s) { energy_sensor_ = s; }
  void set_precharging_binary_sensor(binary_sensor::BinarySensor *s) { precharging_binary_sensor_ = s; }

  void set_battery_status_text_sensor(text_sensor::TextSensor *battery_status_text_sensor) {
    battery_status_text_sensor_ = battery_status_text_sensor;
  }
  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }
  void set_software_version_text_sensor(text_sensor::TextSensor *software_version_text_sensor) {
    software_version_text_sensor_ = software_version_text_sensor;
  }
  void set_hardware_version_text_sensor(text_sensor::TextSensor *hardware_version_text_sensor) {
    hardware_version_text_sensor_ = hardware_version_text_sensor;
  }

  void set_balancer_switch(switch_::Switch *balancer_switch) { balancer_switch_ = balancer_switch; }
  void set_charging_switch(switch_::Switch *charging_switch) { charging_switch_ = charging_switch; }
  void set_discharging_switch(switch_::Switch *discharging_switch) { discharging_switch_ = discharging_switch; }
  void set_protocol_version(uint8_t protocol_version) { protocol_version_ = protocol_version; }

  void register_settings_number(uint16_t address, number::Number *number, float factor, float offset) {
    this->settings_numbers_[address] = {number, factor, offset};
  }
  void send_command(uint8_t function, uint16_t address, uint16_t value);
  void set_response_timeout(uint32_t ms) { queue_.set_timeout_ms(ms); }
#ifdef USE_ESP32
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
  void write_register(uint16_t address, uint16_t value) { send_command(0x06, address, value); }
#endif

  void on_daly_bms_ble_data(const std::vector<uint8_t> &data);
  void set_password(uint32_t password) { this->password_ = password; }
  void set_status_registers(uint8_t protocol_version) { this->status_registers_ = protocol_version; }

 protected:
  binary_sensor::BinarySensor *online_status_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *balancing_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *charging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *discharging_binary_sensor_{nullptr};

  sensor::Sensor *total_voltage_sensor_{nullptr};
  sensor::Sensor *current_sensor_{nullptr};
  sensor::Sensor *power_sensor_{nullptr};
  sensor::Sensor *charging_power_sensor_{nullptr};
  sensor::Sensor *discharging_power_sensor_{nullptr};
  sensor::Sensor *error_bitmask_sensor_{nullptr};
  sensor::Sensor *state_of_charge_sensor_{nullptr};
  sensor::Sensor *charging_cycles_sensor_{nullptr};
  sensor::Sensor *min_cell_voltage_sensor_{nullptr};
  sensor::Sensor *max_cell_voltage_sensor_{nullptr};
  sensor::Sensor *min_voltage_cell_sensor_{nullptr};
  sensor::Sensor *max_voltage_cell_sensor_{nullptr};
  sensor::Sensor *delta_cell_voltage_sensor_{nullptr};
  sensor::Sensor *average_cell_voltage_sensor_{nullptr};
  sensor::Sensor *cell_count_sensor_{nullptr};
  sensor::Sensor *temperature_sensors_sensor_{nullptr};
  sensor::Sensor *capacity_remaining_sensor_{nullptr};
  sensor::Sensor *balance_current_sensor_{nullptr};
  sensor::Sensor *mosfet_temperature_sensor_{nullptr};
  sensor::Sensor *board_temperature_sensor_{nullptr};
  sensor::Sensor *max_battery_temperature_sensor_{nullptr};
  sensor::Sensor *max_battery_temperature_probe_sensor_{nullptr};
  sensor::Sensor *min_battery_temperature_sensor_{nullptr};
  sensor::Sensor *min_battery_temperature_probe_sensor_{nullptr};
  sensor::Sensor *energy_sensor_{nullptr};
  binary_sensor::BinarySensor *precharging_binary_sensor_{nullptr};

  switch_::Switch *balancer_switch_{nullptr};
  switch_::Switch *charging_switch_{nullptr};
  switch_::Switch *discharging_switch_{nullptr};

  struct SettingsNumber {
    number::Number *number;
    float factor;
    float offset;
  };
  std::map<uint16_t, SettingsNumber> settings_numbers_;

  text_sensor::TextSensor *battery_status_text_sensor_{nullptr};
  text_sensor::TextSensor *errors_text_sensor_{nullptr};
  text_sensor::TextSensor *software_version_text_sensor_{nullptr};
  text_sensor::TextSensor *hardware_version_text_sensor_{nullptr};

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
  } cells_[48];

  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[8];

  struct CommandQueue {
    static const size_t LENGTH = 16;

    struct Command {
      uint8_t function;
      uint16_t address;
      uint16_t value;
    };

    void set_timeout_ms(uint32_t ms) { timeout_ms_ = ms; }

    bool enqueue(uint8_t function, uint16_t address, uint16_t value) {
      uint8_t next = (tail_ + 1) % LENGTH;
      if (next == head_)
        return false;
      commands_[tail_] = {function, address, value};
      tail_ = next;
      return true;
    }
    const Command &front() const { return commands_[head_]; }
    void advance() {
      if (empty())
        return;
      head_ = (head_ + 1) % LENGTH;
      pending_ = false;
    }
    void mark_pending(uint32_t now) {
      pending_ = true;
      start_millis_ = now;
    }
    void reset() {
      head_ = tail_ = 0;
      pending_ = false;
    }
    bool empty() const { return head_ == tail_; }
    bool pending() const { return pending_; }
    bool timed_out(uint32_t now) const { return pending_ && (now - start_millis_ > timeout_ms_); }
    uint8_t size() const { return (tail_ + LENGTH - head_) % LENGTH; }

   private:
    Command commands_[LENGTH];
    uint8_t head_{0};
    uint8_t tail_{0};
    bool pending_{false};
    uint32_t start_millis_{0};
    uint32_t timeout_ms_{3000};
  } queue_;

  void queue_command_(uint8_t function, uint16_t address, uint16_t value);
  void send_next_command_();
  void advance_command_queue_();

#ifdef USE_ESP32
  uint16_t char_notify_handle_{0};
  uint16_t char_command_handle_{0};
#endif
  uint8_t no_response_count_{0};
  uint32_t password_ = 12345678;
  uint8_t status_registers_{62};
  uint8_t protocol_version_{0xD2};

  std::array<uint8_t, 8> build_frame_(uint8_t function, uint16_t address, uint16_t value) const;
  void decode_status_data_(const std::vector<uint8_t> &data);
  void decode_settings_data_(const std::vector<uint8_t> &data);
  void decode_balancer_switch_data_(const std::vector<uint8_t> &data);
  void decode_version_data_(const std::vector<uint8_t> &data);
  void decode_password_data_(const std::vector<uint8_t> &data);
  void decode_p81_cells_data_(const std::vector<uint8_t> &data);
  void decode_p81_status_data_(const std::vector<uint8_t> &data);
  void decode_p81_version_data_(const std::vector<uint8_t> &data);
  void publish_device_unavailable_();
  void reset_online_status_tracker_();
  void track_online_status_();
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(number::Number *obj, float value);
  void publish_state_(switch_::Switch *obj, const bool &state);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  std::string bitmask_to_string_(const char *const messages[], const uint8_t &messages_size, const uint64_t &mask);

  bool check_bit_(uint16_t mask, uint16_t flag) { return (mask & flag) == flag; }
};

}  // namespace esphome::daly_bms_ble
