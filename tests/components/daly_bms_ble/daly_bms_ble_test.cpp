#include <gtest/gtest.h>
#include "common.h"

namespace esphome::daly_bms_ble::testing {

// ── Version frame (data_len=0x40) ────────────────────────────────────────────

TEST(DalyBmsBleVersionTest, NullSensorsDoNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_version_data_(VERSION_FRAME_1);
}

TEST(DalyBmsBleVersionTest, DispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.on_daly_bms_ble_data(VERSION_FRAME_1);
}

TEST(DalyBmsBleVersionTest, SecondFrameDispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.on_daly_bms_ble_data(VERSION_FRAME_2);
}

// ── Password frame (data_len=0x06) ───────────────────────────────────────────

TEST(DalyBmsBlePasswordTest, NullSensorsDoNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_password_data_(PASSWORD_FRAME_1);
}

TEST(DalyBmsBlePasswordTest, DispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.on_daly_bms_ble_data(PASSWORD_FRAME_1);
}

// ── Settings frame (data_len=0x52) ───────────────────────────────────────────

TEST(DalyBmsBleSettingsTest, ChargingSwitchOn) {
  TestableDalyBmsBle bms;
  TestSwitch charging;
  bms.set_charging_switch(&charging);

  bms.decode_settings_data_(SETTINGS_FRAME_1);

  EXPECT_TRUE(charging.state);
}

TEST(DalyBmsBleSettingsTest, DischargingSwitchOn) {
  TestableDalyBmsBle bms;
  TestSwitch discharging;
  bms.set_discharging_switch(&discharging);

  bms.decode_settings_data_(SETTINGS_FRAME_1);

  EXPECT_TRUE(discharging.state);
}

TEST(DalyBmsBleSettingsTest, StateOfChargeSetting) {
  TestableDalyBmsBle bms;
  TestNumber soc_setting;
  bms.set_state_of_charge_setting_number(&soc_setting);

  bms.decode_settings_data_(SETTINGS_FRAME_1);

  EXPECT_NEAR(soc_setting.state, 68.0f, 0.01f);
}

TEST(DalyBmsBleSettingsTest, NullSensorsDoNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_settings_data_(SETTINGS_FRAME_1);
}

TEST(DalyBmsBleSettingsTest, DispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.on_daly_bms_ble_data(SETTINGS_FRAME_1);
}

// ── Status frame, 80 registers (data_len=0xA0) ───────────────────────────────

TEST(DalyBmsBleStatus80RegTest, TotalVoltage) {
  TestableDalyBmsBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_NEAR(voltage.state, 52.5f, 0.01f);
}

TEST(DalyBmsBleStatus80RegTest, Current) {
  TestableDalyBmsBle bms;
  sensor::Sensor current;
  bms.set_current_sensor(&current);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_NEAR(current.state, -3.1f, 0.01f);
}

TEST(DalyBmsBleStatus80RegTest, StateOfCharge) {
  TestableDalyBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_NEAR(soc.state, 53.0f, 0.01f);
}

TEST(DalyBmsBleStatus80RegTest, BatteryStatusDischarging) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor status;
  bms.set_battery_status_text_sensor(&status);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_EQ(status.state, "Discharging");
}

TEST(DalyBmsBleStatus80RegTest, CapacityRemaining) {
  TestableDalyBmsBle bms;
  sensor::Sensor capacity;
  bms.set_capacity_remaining_sensor(&capacity);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_NEAR(capacity.state, 111.3f, 0.01f);
}

TEST(DalyBmsBleStatus80RegTest, CellCount) {
  TestableDalyBmsBle bms;
  sensor::Sensor cell_count;
  bms.set_cell_count_sensor(&cell_count);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_FLOAT_EQ(cell_count.state, 16.0f);
}

TEST(DalyBmsBleStatus80RegTest, TemperatureSensors) {
  TestableDalyBmsBle bms;
  sensor::Sensor temp_sensors;
  bms.set_temperature_sensors_sensor(&temp_sensors);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_FLOAT_EQ(temp_sensors.state, 4.0f);
}

TEST(DalyBmsBleStatus80RegTest, ChargingCycles) {
  TestableDalyBmsBle bms;
  sensor::Sensor cycles;
  bms.set_charging_cycles_sensor(&cycles);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_FLOAT_EQ(cycles.state, 57.0f);
}

TEST(DalyBmsBleStatus80RegTest, BalancingOff) {
  TestableDalyBmsBle bms;
  binary_sensor::BinarySensor balancing;
  bms.set_balancing_binary_sensor(&balancing);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_FALSE(balancing.state);
}

TEST(DalyBmsBleStatus80RegTest, ChargingAndDischargingOn) {
  TestableDalyBmsBle bms;
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_TRUE(charging.state);
  EXPECT_TRUE(discharging.state);
}

TEST(DalyBmsBleStatus80RegTest, DeltaCellVoltage) {
  TestableDalyBmsBle bms;
  sensor::Sensor delta;
  bms.set_delta_cell_voltage_sensor(&delta);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_NEAR(delta.state, 0.003f, 0.0001f);
}

TEST(DalyBmsBleStatus80RegTest, PowerCalculation) {
  TestableDalyBmsBle bms;
  sensor::Sensor power, charging_power, discharging_power;
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  // total_voltage=52.5V * current=-3.1A = -162.75W
  EXPECT_NEAR(power.state, -162.75f, 0.5f);
  EXPECT_FLOAT_EQ(charging_power.state, 0.0f);
  EXPECT_NEAR(discharging_power.state, 162.75f, 0.5f);
}

TEST(DalyBmsBleStatus80RegTest, FourTemperatures) {
  TestableDalyBmsBle bms;
  sensor::Sensor t1, t2, t3, t4;
  bms.set_temperature_sensor(0, &t1);
  bms.set_temperature_sensor(1, &t2);
  bms.set_temperature_sensor(2, &t3);
  bms.set_temperature_sensor(3, &t4);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_NEAR(t1.state, 15.0f, 0.1f);
  EXPECT_NEAR(t2.state, 15.0f, 0.1f);
  EXPECT_NEAR(t3.state, 15.0f, 0.1f);
  EXPECT_NEAR(t4.state, 15.0f, 0.1f);
}

TEST(DalyBmsBleStatus80RegTest, SixteenCellVoltages) {
  TestableDalyBmsBle bms;
  sensor::Sensor cells[16];
  for (int i = 0; i < 16; i++) {
    bms.set_cell_voltage_sensor(i, &cells[i]);
  }
  sensor::Sensor min_v, max_v, min_cell, max_cell, avg;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_NEAR(cells[0].state, 3.279f, 0.001f);   // C1  min
  EXPECT_NEAR(cells[1].state, 3.281f, 0.001f);   // C2
  EXPECT_NEAR(cells[6].state, 3.282f, 0.001f);   // C7  max
  EXPECT_NEAR(cells[10].state, 3.280f, 0.001f);  // C11
  EXPECT_NEAR(cells[15].state, 3.281f, 0.001f);  // C16

  EXPECT_NEAR(min_v.state, 3.279f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.282f, 0.001f);
  EXPECT_NEAR(min_cell.state, 1.0f, 0.1f);
  EXPECT_NEAR(max_cell.state, 7.0f, 0.1f);
  EXPECT_NEAR(avg.state, 3.28125f, 0.0001f);
}

TEST(DalyBmsBleStatus80RegTest, BalanceCurrent) {
  TestableDalyBmsBle bms;
  sensor::Sensor balance_current;
  bms.set_balance_current_sensor(&balance_current);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_NEAR(balance_current.state, 0.0f, 0.001f);
}

TEST(DalyBmsBleStatus80RegTest, MosfetTemperature) {
  TestableDalyBmsBle bms;
  sensor::Sensor mosfet_temp;
  bms.set_mosfet_temperature_sensor(&mosfet_temp);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_NEAR(mosfet_temp.state, 15.0f, 0.1f);
}

TEST(DalyBmsBleStatus80RegTest, NoAlarms) {
  TestableDalyBmsBle bms;
  sensor::Sensor error_bitmask;
  text_sensor::TextSensor errors;
  bms.set_error_bitmask_sensor(&error_bitmask);
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_80_REG_2);

  EXPECT_FLOAT_EQ(error_bitmask.state, 0.0f);
  EXPECT_EQ(errors.state, "");
}

TEST(DalyBmsBleStatus80RegTest, NullSensorsDoNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_status_data_(STATUS_FRAME_80_REG_2);
}

TEST(DalyBmsBleStatus80RegTest, DispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.on_daly_bms_ble_data(STATUS_FRAME_80_REG_2);
}

// ── Status frame, 62 registers (data_len=0x7C) ───────────────────────────────

TEST(DalyBmsBleStatus62RegTest, TotalVoltage) {
  TestableDalyBmsBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);

  bms.decode_status_data_(STATUS_FRAME_62_REG_NO_ALARMS);

  EXPECT_NEAR(voltage.state, 27.1f, 0.01f);
}

TEST(DalyBmsBleStatus62RegTest, CurrentIdle) {
  TestableDalyBmsBle bms;
  sensor::Sensor current;
  bms.set_current_sensor(&current);

  bms.decode_status_data_(STATUS_FRAME_62_REG_NO_ALARMS);

  EXPECT_NEAR(current.state, 0.0f, 0.01f);
}

TEST(DalyBmsBleStatus62RegTest, StateOfChargeFull) {
  TestableDalyBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);

  bms.decode_status_data_(STATUS_FRAME_62_REG_NO_ALARMS);

  EXPECT_NEAR(soc.state, 100.0f, 0.01f);
}

TEST(DalyBmsBleStatus62RegTest, BatteryStatusIdle) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor status;
  bms.set_battery_status_text_sensor(&status);

  bms.decode_status_data_(STATUS_FRAME_62_REG_NO_ALARMS);

  EXPECT_EQ(status.state, "Idle");
}

TEST(DalyBmsBleStatus62RegTest, CellCount) {
  TestableDalyBmsBle bms;
  sensor::Sensor cell_count;
  bms.set_cell_count_sensor(&cell_count);

  bms.decode_status_data_(STATUS_FRAME_62_REG_NO_ALARMS);

  EXPECT_FLOAT_EQ(cell_count.state, 8.0f);
}

TEST(DalyBmsBleStatus62RegTest, EightCellVoltages) {
  TestableDalyBmsBle bms;
  sensor::Sensor cells[8];
  for (int i = 0; i < 8; i++) {
    bms.set_cell_voltage_sensor(i, &cells[i]);
  }
  sensor::Sensor min_v, max_v;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);

  bms.decode_status_data_(STATUS_FRAME_62_REG_NO_ALARMS);

  EXPECT_NEAR(cells[0].state, 3.438f, 0.001f);  // C1
  EXPECT_NEAR(cells[1].state, 3.433f, 0.001f);  // C2
  EXPECT_NEAR(cells[2].state, 3.417f, 0.001f);  // C3
  EXPECT_NEAR(cells[3].state, 3.543f, 0.001f);  // C4  max
  EXPECT_NEAR(cells[4].state, 3.339f, 0.001f);  // C5  min
  EXPECT_NEAR(cells[7].state, 3.339f, 0.001f);  // C8

  EXPECT_NEAR(min_v.state, 3.339f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.543f, 0.001f);
}

TEST(DalyBmsBleStatus62RegTest, OneTemperature) {
  TestableDalyBmsBle bms;
  sensor::Sensor t1;
  bms.set_temperature_sensor(0, &t1);

  bms.decode_status_data_(STATUS_FRAME_62_REG_NO_ALARMS);

  EXPECT_NEAR(t1.state, 21.0f, 0.1f);
}

TEST(DalyBmsBleStatus62RegTest, NoAlarms) {
  TestableDalyBmsBle bms;
  sensor::Sensor error_bitmask;
  text_sensor::TextSensor errors;
  bms.set_error_bitmask_sensor(&error_bitmask);
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_NO_ALARMS);

  EXPECT_FLOAT_EQ(error_bitmask.state, 0.0f);
  EXPECT_EQ(errors.state, "");
}

TEST(DalyBmsBleStatus62RegTest, NullSensorsDoNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_status_data_(STATUS_FRAME_62_REG_NO_ALARMS);
}

TEST(DalyBmsBleStatus62RegTest, DispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.on_daly_bms_ble_data(STATUS_FRAME_62_REG_NO_ALARMS);
}

// ── Alarm decoding ───────────────────────────────────────────────────────────

TEST(DalyBmsBleAlarmTest, WarnChargingTemperatureTooHigh) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor errors;
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_ALARM_WARN_CHARGING_TEMP_HIGH);

  EXPECT_EQ(errors.state, "Warning: Charging temperature too high");
}

TEST(DalyBmsBleAlarmTest, WarnCellVoltageTooHigh) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor errors;
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_ALARM_WARN_CELL_VOLTAGE_HIGH);

  EXPECT_EQ(errors.state, "Warning: Cell voltage too high");
}

TEST(DalyBmsBleAlarmTest, WarnVoltageDifferenceTooHigh) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor errors;
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_ALARM_WARN_VOLTAGE_DIFF_HIGH);

  EXPECT_EQ(errors.state, "Warning: Voltage difference too high");
}

TEST(DalyBmsBleAlarmTest, WarnChargingCurrentTooHigh) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor errors;
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_ALARM_WARN_CHARGING_CURRENT_HIGH);

  EXPECT_EQ(errors.state, "Warning: Charging current too high");
}

TEST(DalyBmsBleAlarmTest, AfeAcquisitionChipFailure) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor errors;
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_ALARM_AFE_FAILURE);

  EXPECT_EQ(errors.state, "AFE acquisition chip failure");
}

TEST(DalyBmsBleAlarmTest, ChargingMosOvertemperatureWarning) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor errors;
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_ALARM_CHARGING_MOS_OVERTEMP);

  EXPECT_EQ(errors.state, "Charging MOS over-temperature warning");
}

TEST(DalyBmsBleAlarmTest, TemperatureDifferenceCritical) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor errors;
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_ALARM_TEMP_DIFF_CRITICAL);

  EXPECT_EQ(errors.state, "Critical: Temperature difference too high");
}

TEST(DalyBmsBleAlarmTest, TemperatureDifferenceBothLevels) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor errors;
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_ALARM_TEMP_DIFF_BOTH);

  EXPECT_EQ(errors.state, "Warning: Temperature difference too high;Critical: Temperature difference too high");
}

TEST(DalyBmsBleAlarmTest, TemperatureDifferenceWarningOnly) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor errors;
  bms.set_errors_text_sensor(&errors);

  bms.decode_status_data_(STATUS_FRAME_62_REG_ALARM_TEMP_DIFF_WARNING);

  EXPECT_EQ(errors.state, "Warning: Temperature difference too high");
}

}  // namespace esphome::daly_bms_ble::testing
