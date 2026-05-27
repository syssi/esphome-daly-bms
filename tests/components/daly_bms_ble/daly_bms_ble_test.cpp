#include <gtest/gtest.h>
#include "common.h"

namespace esphome::daly_bms_ble::testing {

// ── Version frame (data_len=0x40) ────────────────────────────────────────────

TEST(DalyBmsBleVersionTest, NullSensorsDoNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_version_data_(VERSION_FRAME_1);
}

TEST(DalyBmsBleVersionTest, SoftwareVersion) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor sw_version;
  bms.set_software_version_text_sensor(&sw_version);

  bms.decode_version_data_(VERSION_FRAME_1);

  EXPECT_EQ(sw_version.state, "401012");
}

TEST(DalyBmsBleVersionTest, HardwareVersion) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor hw_version;
  bms.set_hardware_version_text_sensor(&hw_version);

  bms.decode_version_data_(VERSION_FRAME_1);

  EXPECT_EQ(hw_version.state, "BMS");
}

TEST(DalyBmsBleVersionTest, SoftwareVersionFrame2) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor sw_version;
  bms.set_software_version_text_sensor(&sw_version);

  bms.decode_version_data_(VERSION_FRAME_2);

  EXPECT_EQ(sw_version.state, "204012");
}

TEST(DalyBmsBleVersionTest, HardwareVersionFrame2) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor hw_version;
  bms.set_hardware_version_text_sensor(&hw_version);

  bms.decode_version_data_(VERSION_FRAME_2);

  EXPECT_EQ(hw_version.state, "SH39F003");
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
  bms.register_settings_number(0x00A7, &soc_setting, 10.0f, 0.0f);

  bms.decode_settings_data_(SETTINGS_FRAME_1);

  EXPECT_NEAR(soc_setting.state, 68.0f, 0.01f);
}

TEST(DalyBmsBleSettingsTest, RatedCapacity) {
  TestableDalyBmsBle bms;
  TestNumber rated_capacity;
  bms.register_settings_number(0x0080, &rated_capacity, 10.0f, 0.0f);

  bms.decode_settings_data_(SETTINGS_FRAME_1);

  EXPECT_NEAR(rated_capacity.state, 105.0f, 0.01f);  // raw=1050, 1050/10=105.0
}

TEST(DalyBmsBleSettingsTest, MosfetOvertemperatureAlarm) {
  TestableDalyBmsBle bms;
  TestNumber mos_temp;
  bms.register_settings_number(0x00A8, &mos_temp, 1.0f, 40.0f);

  bms.decode_settings_data_(SETTINGS_FRAME_1);

  EXPECT_NEAR(mos_temp.state, 47.0f, 0.01f);  // raw=87, (87-40)/1=47
}

TEST(DalyBmsBleSettingsTest, ChargingOvercurrentWarning) {
  TestableDalyBmsBle bms;
  TestNumber current_warning;
  bms.register_settings_number(0x0093, &current_warning, 10.0f, 30000.0f);

  bms.decode_settings_data_(SETTINGS_FRAME_1);

  EXPECT_NEAR(current_warning.state, -10.0f, 0.01f);  // raw=29900, (29900-30000)/10=-10.0
}

TEST(DalyBmsBleSettingsTest, CellVoltageReference) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0081, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 3200.0f, 0.01f);  // raw=3200, 3200/1=3200
}

TEST(DalyBmsBleSettingsTest, AcquisitionBoardCount) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0082, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 1.0f, 0.01f);  // raw=1
}

TEST(DalyBmsBleSettingsTest, Board1CellCount) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0083, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 4.0f, 0.01f);  // raw=4
}

TEST(DalyBmsBleSettingsTest, Board2CellCount) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0084, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 0.0f, 0.01f);  // raw=0
}

TEST(DalyBmsBleSettingsTest, Board3CellCount) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0085, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 0.0f, 0.01f);  // raw=0
}

TEST(DalyBmsBleSettingsTest, Board1TemperatureSensorCount) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0086, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 1.0f, 0.01f);  // raw=1
}

TEST(DalyBmsBleSettingsTest, Board2TemperatureSensorCount) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0087, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 0.0f, 0.01f);  // raw=0
}

TEST(DalyBmsBleSettingsTest, Board3TemperatureSensorCount) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0088, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 0.0f, 0.01f);  // raw=0
}

TEST(DalyBmsBleSettingsTest, BatteryType) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0089, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 0.0f, 0.01f);  // raw=0
}

TEST(DalyBmsBleSettingsTest, SleepWaitTime) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x008A, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 7200.0f, 0.01f);  // raw=7200
}

TEST(DalyBmsBleSettingsTest, CellOvervoltageWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x008B, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 3500.0f, 0.01f);  // raw=3500
}

TEST(DalyBmsBleSettingsTest, CellOvervoltageAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x008C, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 3500.0f, 0.01f);  // raw=3500
}

TEST(DalyBmsBleSettingsTest, CellUndervoltageWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x008D, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 2600.0f, 0.01f);  // raw=2600
}

TEST(DalyBmsBleSettingsTest, CellUndervoltageAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x008E, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 2600.0f, 0.01f);  // raw=2600
}

TEST(DalyBmsBleSettingsTest, TotalOvervoltageWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x008F, &n, 10.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 14.0f, 0.01f);  // raw=140, 140/10=14.0
}

TEST(DalyBmsBleSettingsTest, TotalOvervoltageAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0090, &n, 10.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 14.0f, 0.01f);  // raw=140, 140/10=14.0
}

TEST(DalyBmsBleSettingsTest, TotalUndervoltageWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0091, &n, 10.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 10.4f, 0.01f);  // raw=104, 104/10=10.4
}

TEST(DalyBmsBleSettingsTest, TotalUndervoltageAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0092, &n, 10.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 10.4f, 0.01f);  // raw=104, 104/10=10.4
}

TEST(DalyBmsBleSettingsTest, ChargingOvercurrentAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0094, &n, 10.0f, 30000.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, -10.0f, 0.01f);  // raw=29900, (29900-30000)/10=-10.0
}

TEST(DalyBmsBleSettingsTest, DischargingOvercurrentWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0095, &n, 10.0f, 30000.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, -16.0f, 0.01f);  // raw=29840, (29840-30000)/10=-16.0
}

TEST(DalyBmsBleSettingsTest, DischargingOvercurrentAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0096, &n, 10.0f, 30000.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 16.0f, 0.01f);  // raw=30160, (30160-30000)/10=16.0
}

TEST(DalyBmsBleSettingsTest, ChargingOvertemperatureWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0097, &n, 1.0f, 40.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 45.0f, 0.01f);  // raw=85, (85-40)/1=45
}

TEST(DalyBmsBleSettingsTest, ChargingOvertemperatureAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0098, &n, 1.0f, 40.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 45.0f, 0.01f);  // raw=85, (85-40)/1=45
}

TEST(DalyBmsBleSettingsTest, ChargingUndertemperatureWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x0099, &n, 1.0f, 40.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 0.0f, 0.01f);  // raw=40, (40-40)/1=0
}

TEST(DalyBmsBleSettingsTest, ChargingUndertemperatureAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x009A, &n, 1.0f, 40.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 0.0f, 0.01f);  // raw=40, (40-40)/1=0
}

TEST(DalyBmsBleSettingsTest, DischargingOvertemperatureWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x009B, &n, 1.0f, 40.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 70.0f, 0.01f);  // raw=110, (110-40)/1=70
}

TEST(DalyBmsBleSettingsTest, DischargingOvertemperatureAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x009C, &n, 1.0f, 40.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 70.0f, 0.01f);  // raw=110, (110-40)/1=70
}

TEST(DalyBmsBleSettingsTest, DischargingUndertemperatureWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x009D, &n, 1.0f, 40.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, -1.0f, 0.01f);  // raw=39, (39-40)/1=-1
}

TEST(DalyBmsBleSettingsTest, DischargingUndertemperatureAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x009E, &n, 1.0f, 40.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, -1.0f, 0.01f);  // raw=39, (39-40)/1=-1
}

TEST(DalyBmsBleSettingsTest, CellVoltageDifferenceWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x009F, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 255.0f, 0.01f);  // raw=255
}

TEST(DalyBmsBleSettingsTest, CellVoltageDifferenceAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x00A0, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 255.0f, 0.01f);  // raw=255
}

TEST(DalyBmsBleSettingsTest, TemperatureDifferenceWarning) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x00A1, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 255.0f, 0.01f);  // raw=255
}

TEST(DalyBmsBleSettingsTest, TemperatureDifferenceAlarm) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x00A2, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 255.0f, 0.01f);  // raw=255
}

TEST(DalyBmsBleSettingsTest, BalancingActivationVoltage) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x00A3, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 3200.0f, 0.01f);  // raw=3200
}

TEST(DalyBmsBleSettingsTest, BalancingActivationVoltageDifference) {
  TestableDalyBmsBle bms;
  TestNumber n;
  bms.register_settings_number(0x00A4, &n, 1.0f, 0.0f);
  bms.decode_settings_data_(SETTINGS_FRAME_1);
  EXPECT_NEAR(n.state, 20.0f, 0.01f);  // raw=20
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

// ── Balancer switch frame (data_len=0x02) ────────────────────────────────────

TEST(DalyBmsBleBalancerSwitchTest, SwitchOn) {
  TestableDalyBmsBle bms;
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);

  bms.decode_balancer_switch_data_(BALANCER_SWITCH_FRAME_ON);

  EXPECT_TRUE(balancer.state);
}

TEST(DalyBmsBleBalancerSwitchTest, SwitchOff) {
  TestableDalyBmsBle bms;
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);

  bms.decode_balancer_switch_data_(BALANCER_SWITCH_FRAME_OFF);

  EXPECT_FALSE(balancer.state);
}

TEST(DalyBmsBleBalancerSwitchTest, NullSwitchDoesNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_balancer_switch_data_(BALANCER_SWITCH_FRAME_ON);
}

TEST(DalyBmsBleBalancerSwitchTest, DispatchedViaOnDataOn) {
  TestableDalyBmsBle bms;
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);

  bms.on_daly_bms_ble_data(BALANCER_SWITCH_FRAME_ON);

  EXPECT_TRUE(balancer.state);
}

TEST(DalyBmsBleBalancerSwitchTest, DispatchedViaOnDataOff) {
  TestableDalyBmsBle bms;
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);

  bms.on_daly_bms_ble_data(BALANCER_SWITCH_FRAME_OFF);

  EXPECT_FALSE(balancer.state);
}

// ── Request frames (outgoing) ────────────────────────────────────────────────
// All golden values from https://github.com/syssi/esphome-daly-bms/issues/9
// Frame layout: D2 <func> <addr_hi> <addr_lo> <val_hi> <val_lo> <crc_lo> <crc_hi>

TEST(DalyBmsBleRequestFrameTest, Status62Registers) {
  TestableDalyBmsBle bms;
  // d2 03 00 00 00 3e d7 b9
  EXPECT_EQ(bms.build_frame_(0x03, 0x0000, 62),
            (std::array<uint8_t, 8>{0xD2, 0x03, 0x00, 0x00, 0x00, 0x3E, 0xD7, 0xB9}));
}

TEST(DalyBmsBleRequestFrameTest, Status80Registers) {
  TestableDalyBmsBle bms;
  // d2 03 00 00 00 50 56 55  (CRC verified by Python/Modbus-CRC-16)
  EXPECT_EQ(bms.build_frame_(0x03, 0x0000, 80),
            (std::array<uint8_t, 8>{0xD2, 0x03, 0x00, 0x00, 0x00, 0x50, 0x56, 0x55}));
}

TEST(DalyBmsBleRequestFrameTest, Settings) {
  TestableDalyBmsBle bms;
  // d2 03 00 80 00 29 96 5f
  EXPECT_EQ(bms.build_frame_(0x03, 0x0080, 0x0029),
            (std::array<uint8_t, 8>{0xD2, 0x03, 0x00, 0x80, 0x00, 0x29, 0x96, 0x5F}));
}

TEST(DalyBmsBleRequestFrameTest, SoftwareVersion) {
  TestableDalyBmsBle bms;
  // d2 03 00 a9 00 20 87 91
  EXPECT_EQ(bms.build_frame_(0x03, 0x00A9, 0x0020),
            (std::array<uint8_t, 8>{0xD2, 0x03, 0x00, 0xA9, 0x00, 0x20, 0x87, 0x91}));
}

TEST(DalyBmsBleRequestFrameTest, Password) {
  TestableDalyBmsBle bms;
  // d2 03 00 c9 00 03 c6 56
  EXPECT_EQ(bms.build_frame_(0x03, 0x00C9, 0x0003),
            (std::array<uint8_t, 8>{0xD2, 0x03, 0x00, 0xC9, 0x00, 0x03, 0xC6, 0x56}));
}

TEST(DalyBmsBleRequestFrameTest, BalancerSwitch) {
  TestableDalyBmsBle bms;
  // d2 03 00 cf 00 01 a7 96
  EXPECT_EQ(bms.build_frame_(0x03, 0x00CF, 0x0001),
            (std::array<uint8_t, 8>{0xD2, 0x03, 0x00, 0xCF, 0x00, 0x01, 0xA7, 0x96}));
}

// ── Command queue ────────────────────────────────────────────────────────────

TEST(DalyBmsBleQueueTest, InitiallyEmpty) {
  TestableDalyBmsBle bms;
  EXPECT_EQ(bms.queue_size(), 0);
  EXPECT_FALSE(bms.command_pending());
}

TEST(DalyBmsBleQueueTest, Enqueue) {
  TestableDalyBmsBle bms;
  bms.queue_command_(0x03, 0x0000, 62);
  EXPECT_EQ(bms.queue_size(), 1);
}

TEST(DalyBmsBleQueueTest, MultipleEnqueue) {
  TestableDalyBmsBle bms;
  for (uint8_t i = 0; i < 5; i++)
    bms.queue_command_(0x03, i, 0);
  EXPECT_EQ(bms.queue_size(), 5);
}

TEST(DalyBmsBleQueueTest, QueueFullDropsCommand) {
  TestableDalyBmsBle bms;
  const uint8_t max_size = TestableDalyBmsBle::CommandQueue::LENGTH - 1;
  for (uint8_t i = 0; i < max_size; i++)
    bms.queue_command_(0x03, i, 0);
  EXPECT_EQ(bms.queue_size(), max_size);

  bms.queue_command_(0x03, 0xAA, 0);
  EXPECT_EQ(bms.queue_size(), max_size);
}

TEST(DalyBmsBleQueueTest, AdvanceDecrementsSize) {
  TestableDalyBmsBle bms;
  bms.queue_command_(0x03, 0x0000, 62);
  bms.queue_command_(0x03, 0x0080, 41);

  bms.advance_command_queue_();

  EXPECT_EQ(bms.queue_size(), 1);
  EXPECT_FALSE(bms.command_pending());
}

TEST(DalyBmsBleQueueTest, AdvanceDrainsFully) {
  TestableDalyBmsBle bms;
  bms.queue_command_(0x03, 0x0000, 62);
  bms.queue_command_(0x03, 0x0080, 41);

  bms.advance_command_queue_();
  bms.advance_command_queue_();

  EXPECT_EQ(bms.queue_size(), 0);
}

TEST(DalyBmsBleQueueTest, ResetClearsQueue) {
  TestableDalyBmsBle bms;
  bms.queue_command_(0x03, 0x0000, 62);
  bms.queue_command_(0x03, 0x0080, 41);

  bms.reset_queue();

  EXPECT_EQ(bms.queue_size(), 0);
  EXPECT_FALSE(bms.command_pending());
}

TEST(DalyBmsBleQueueTest, WrapAround) {
  TestableDalyBmsBle bms;
  for (uint8_t i = 0; i < 9; i++)
    bms.queue_command_(0x03, i, 0);
  for (int i = 0; i < 5; i++)
    bms.advance_command_queue_();

  EXPECT_EQ(bms.queue_size(), 4);

  for (uint8_t i = 0; i < 5; i++)
    bms.queue_command_(0x03, 0x10 + i, 0);

  EXPECT_EQ(bms.queue_size(), 9);
}

TEST(DalyBmsBleQueueTest, AdvanceOnEmptyQueueIsNoop) {
  TestableDalyBmsBle bms;
  bms.advance_command_queue_();
  EXPECT_EQ(bms.queue_size(), 0);
  EXPECT_FALSE(bms.command_pending());
}

TEST(DalyBmsBleQueueTest, ValidResponseAdvancesQueue) {
  TestableDalyBmsBle bms;
  bms.queue_command_(0x03, 0x0000, 62);
  bms.queue_command_(0x03, 0x0080, 41);

  bms.on_daly_bms_ble_data(STATUS_FRAME_80_REG_2);

  EXPECT_EQ(bms.queue_size(), 1);
  EXPECT_FALSE(bms.command_pending());
}

TEST(DalyBmsBleQueueTest, InvalidStartByteDoesNotAdvanceQueue) {
  TestableDalyBmsBle bms;
  bms.queue_command_(0x03, 0x0000, 62);

  bms.on_daly_bms_ble_data({0xFF, 0x03, 0x00, 0x00, 0x00});

  EXPECT_EQ(bms.queue_size(), 1);
}

TEST(DalyBmsBleQueueTest, BadCrcDoesNotAdvanceQueue) {
  TestableDalyBmsBle bms;
  bms.queue_command_(0x03, 0x0000, 62);

  auto bad_crc = STATUS_FRAME_80_REG_2;
  bad_crc.back() ^= 0xFF;
  bms.on_daly_bms_ble_data(bad_crc);

  EXPECT_EQ(bms.queue_size(), 1);
}

}  // namespace esphome::daly_bms_ble::testing
