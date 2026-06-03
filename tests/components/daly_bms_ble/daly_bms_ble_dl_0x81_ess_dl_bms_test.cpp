#include <gtest/gtest.h>
#include "common.h"

namespace esphome::daly_bms_ble::testing {

// ── Cells frame (reg 0-63) ───────────────────────────────────────────────────

TEST(DalyBmsBleEssDlBmsCellsTest, NullSensorsDoNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_dl_cells_data_(DL_CELLS_FRAME);
}

TEST(DalyBmsBleEssDlBmsCellsTest, WrongFrameSizeIsRejected) {
  TestableDalyBmsBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);

  bms.decode_dl_cells_data_(DL_BALANCER_SWITCH_FRAME_ON);

  EXPECT_FALSE(voltage.has_state());
}

TEST(DalyBmsBleEssDlBmsCellsTest, TotalVoltage) {
  TestableDalyBmsBle bms;
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);

  bms.decode_dl_cells_data_(DL_CELLS_FRAME);

  EXPECT_NEAR(voltage.state, 53.0f, 0.01f);
}

TEST(DalyBmsBleEssDlBmsCellsTest, Current) {
  TestableDalyBmsBle bms;
  sensor::Sensor current;
  bms.set_current_sensor(&current);

  bms.decode_dl_cells_data_(DL_CELLS_FRAME);

  EXPECT_NEAR(current.state, -8.9f, 0.01f);
}

TEST(DalyBmsBleEssDlBmsCellsTest, StateOfCharge) {
  TestableDalyBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);

  bms.decode_dl_cells_data_(DL_CELLS_FRAME);

  EXPECT_NEAR(soc.state, 86.4f, 0.01f);
}

TEST(DalyBmsBleEssDlBmsCellsTest, CellCount) {
  TestableDalyBmsBle bms;
  sensor::Sensor cell_count;
  bms.set_cell_count_sensor(&cell_count);

  bms.decode_dl_cells_data_(DL_CELLS_FRAME);

  EXPECT_FLOAT_EQ(cell_count.state, 16.0f);
}

TEST(DalyBmsBleEssDlBmsCellsTest, TemperatureSensors) {
  TestableDalyBmsBle bms;
  sensor::Sensor temp_sensors;
  bms.set_temperature_sensors_sensor(&temp_sensors);

  bms.decode_dl_cells_data_(DL_CELLS_FRAME);

  EXPECT_FLOAT_EQ(temp_sensors.state, 4.0f);
}

TEST(DalyBmsBleEssDlBmsCellsTest, SixteenCellVoltages) {
  TestableDalyBmsBle bms;
  sensor::Sensor cells[16];
  for (int i = 0; i < 16; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);

  bms.decode_dl_cells_data_(DL_CELLS_FRAME);

  EXPECT_NEAR(cells[0].state, 3.316f, 0.001f);   // C1
  EXPECT_NEAR(cells[1].state, 3.312f, 0.001f);   // C2  min
  EXPECT_NEAR(cells[2].state, 3.315f, 0.001f);   // C3
  EXPECT_NEAR(cells[8].state, 3.312f, 0.001f);   // C9  also min
  EXPECT_NEAR(cells[15].state, 3.317f, 0.001f);  // C16 max
}

TEST(DalyBmsBleEssDlBmsCellsTest, MinMaxAvgDeltaCellVoltage) {
  TestableDalyBmsBle bms;
  sensor::Sensor min_v, max_v, avg_v, delta_v, min_cell, max_cell;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_average_cell_voltage_sensor(&avg_v);
  bms.set_delta_cell_voltage_sensor(&delta_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);

  bms.decode_dl_cells_data_(DL_CELLS_FRAME);

  EXPECT_NEAR(min_v.state, 3.312f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.317f, 0.001f);
  EXPECT_NEAR(avg_v.state, 3.3146875f, 0.0001f);
  EXPECT_NEAR(delta_v.state, 0.005f, 0.001f);
  EXPECT_FLOAT_EQ(min_cell.state, 2.0f);   // C2 is first minimum
  EXPECT_FLOAT_EQ(max_cell.state, 16.0f);
}

TEST(DalyBmsBleEssDlBmsCellsTest, FourTemperaturesAt22Celsius) {
  TestableDalyBmsBle bms;
  sensor::Sensor t1, t2, t3, t4;
  bms.set_temperature_sensor(0, &t1);
  bms.set_temperature_sensor(1, &t2);
  bms.set_temperature_sensor(2, &t3);
  bms.set_temperature_sensor(3, &t4);

  bms.decode_dl_cells_data_(DL_CELLS_FRAME);

  EXPECT_NEAR(t1.state, 22.0f, 0.1f);
  EXPECT_NEAR(t2.state, 22.0f, 0.1f);
  EXPECT_NEAR(t3.state, 22.0f, 0.1f);
  EXPECT_NEAR(t4.state, 22.0f, 0.1f);
}

TEST(DalyBmsBleEssDlBmsCellsTest, PowerCalculation) {
  TestableDalyBmsBle bms;
  sensor::Sensor power, charging_power, discharging_power;
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);

  bms.decode_dl_cells_data_(DL_CELLS_FRAME);

  // 53.0V * -8.9A = -471.7W
  EXPECT_NEAR(power.state, -471.7f, 1.0f);
  EXPECT_FLOAT_EQ(charging_power.state, 0.0f);
  EXPECT_NEAR(discharging_power.state, 471.7f, 1.0f);
}

TEST(DalyBmsBleEssDlBmsCellsTest, DispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.set_protocol_version(DALY_0X81);
  sensor::Sensor voltage;
  bms.set_total_voltage_sensor(&voltage);

  bms.queue_command_(0x03, 0x0000, 64);  // reg 0-63
  bms.on_daly_bms_ble_data(DL_CELLS_FRAME);

  EXPECT_NEAR(voltage.state, 53.0f, 0.01f);
}

// ── Status frame (reg 65-126) ────────────────────────────────────────────────

TEST(DalyBmsBleEssDlBmsStatusTest, NullSensorsDoNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_dl_status_data_(DL_STATUS_FRAME);
}

TEST(DalyBmsBleEssDlBmsStatusTest, WrongFrameSizeIsRejected) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor status;
  bms.set_battery_status_text_sensor(&status);

  bms.decode_dl_status_data_(DL_BALANCER_SWITCH_FRAME_ON);

  EXPECT_EQ(status.state, "");
}

TEST(DalyBmsBleEssDlBmsStatusTest, BatteryStatusDischarging) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor status;
  bms.set_battery_status_text_sensor(&status);

  bms.decode_dl_status_data_(DL_STATUS_FRAME);

  EXPECT_EQ(status.state, "Discharging");
}

TEST(DalyBmsBleEssDlBmsStatusTest, CapacityRemaining) {
  TestableDalyBmsBle bms;
  sensor::Sensor capacity;
  bms.set_capacity_remaining_sensor(&capacity);

  bms.decode_dl_status_data_(DL_STATUS_FRAME);

  EXPECT_NEAR(capacity.state, 271.2f, 0.01f);
}

TEST(DalyBmsBleEssDlBmsStatusTest, ChargingCycles) {
  TestableDalyBmsBle bms;
  sensor::Sensor cycles;
  bms.set_charging_cycles_sensor(&cycles);

  bms.decode_dl_status_data_(DL_STATUS_FRAME);

  EXPECT_FLOAT_EQ(cycles.state, 7.0f);
}

TEST(DalyBmsBleEssDlBmsStatusTest, BalancingActive) {
  TestableDalyBmsBle bms;
  binary_sensor::BinarySensor balancing;
  bms.set_balancing_binary_sensor(&balancing);

  bms.decode_dl_status_data_(DL_STATUS_FRAME);

  EXPECT_TRUE(balancing.state);  // reg 0x4D = 7 (non-zero)
}

TEST(DalyBmsBleEssDlBmsStatusTest, ChargingAndDischargingMosfetOn) {
  TestableDalyBmsBle bms;
  binary_sensor::BinarySensor charging, discharging;
  bms.set_charging_binary_sensor(&charging);
  bms.set_discharging_binary_sensor(&discharging);

  bms.decode_dl_status_data_(DL_STATUS_FRAME);

  EXPECT_TRUE(charging.state);
  EXPECT_TRUE(discharging.state);
}

TEST(DalyBmsBleEssDlBmsStatusTest, MosfetTemperature) {
  TestableDalyBmsBle bms;
  sensor::Sensor mosfet_temp;
  bms.set_mosfet_temperature_sensor(&mosfet_temp);

  bms.decode_dl_status_data_(DL_STATUS_FRAME);

  EXPECT_NEAR(mosfet_temp.state, 24.0f, 0.1f);
}

TEST(DalyBmsBleEssDlBmsStatusTest, BoardTemperature) {
  TestableDalyBmsBle bms;
  sensor::Sensor board_temp;
  bms.set_board_temperature_sensor(&board_temp);

  bms.decode_dl_status_data_(DL_STATUS_FRAME);

  EXPECT_NEAR(board_temp.state, 30.0f, 0.1f);
}

TEST(DalyBmsBleEssDlBmsStatusTest, DispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.set_protocol_version(DALY_0X81);
  sensor::Sensor capacity;
  bms.set_capacity_remaining_sensor(&capacity);

  bms.queue_command_(0x03, 0x0041, 62);  // reg 65-126
  bms.on_daly_bms_ble_data(DL_STATUS_FRAME);

  EXPECT_NEAR(capacity.state, 271.2f, 0.01f);
}

// ── Balancer switch frame (reg 0x00CF) ───────────────────────────────────────

TEST(DalyBmsBleEssDlBmsBalancerSwitchTest, SwitchOn) {
  TestableDalyBmsBle bms;
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);

  bms.decode_balancer_switch_data_(DL_BALANCER_SWITCH_FRAME_ON);

  EXPECT_TRUE(balancer.state);
}

TEST(DalyBmsBleEssDlBmsBalancerSwitchTest, NullSwitchDoesNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_balancer_switch_data_(DL_BALANCER_SWITCH_FRAME_ON);
}

TEST(DalyBmsBleEssDlBmsBalancerSwitchTest, DispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.set_protocol_version(DALY_0X81);
  TestSwitch balancer;
  bms.set_balancer_switch(&balancer);

  bms.queue_command_(0x03, 0x00CF, 1);
  bms.on_daly_bms_ble_data(DL_BALANCER_SWITCH_FRAME_ON);

  EXPECT_TRUE(balancer.state);
}

// ── Version frame (reg 0x0178-0x01C1) ────────────────────────────────────────

TEST(DalyBmsBleEssDlBmsVersionTest, NullSensorsDoNotCrash) {
  TestableDalyBmsBle bms;
  bms.decode_dl_version_data_(DL_VERSION_FRAME);
}

TEST(DalyBmsBleEssDlBmsVersionTest, WrongFrameSizeIsRejected) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor sw_version;
  bms.set_software_version_text_sensor(&sw_version);

  bms.decode_dl_version_data_(DL_BALANCER_SWITCH_FRAME_ON);

  EXPECT_EQ(sw_version.state, "");
}

TEST(DalyBmsBleEssDlBmsVersionTest, SoftwareVersion) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor sw_version;
  bms.set_software_version_text_sensor(&sw_version);

  bms.decode_dl_version_data_(DL_VERSION_FRAME);

  EXPECT_EQ(sw_version.state, "41_260321_0323ESS-DL-BMS");
}

TEST(DalyBmsBleEssDlBmsVersionTest, HardwareVersion) {
  TestableDalyBmsBle bms;
  text_sensor::TextSensor hw_version;
  bms.set_hardware_version_text_sensor(&hw_version);

  bms.decode_dl_version_data_(DL_VERSION_FRAME);

  EXPECT_EQ(hw_version.state, "ESS41_0323");
}

TEST(DalyBmsBleEssDlBmsVersionTest, DispatchedViaOnData) {
  TestableDalyBmsBle bms;
  bms.set_protocol_version(DALY_0X81);
  text_sensor::TextSensor sw_version;
  bms.set_software_version_text_sensor(&sw_version);

  bms.queue_command_(0x03, 0x0178, 74);  // reg 0x0178-0x01C1
  bms.on_daly_bms_ble_data(DL_VERSION_FRAME);

  EXPECT_EQ(sw_version.state, "41_260321_0323ESS-DL-BMS");
}

// ── Request frames (TX) ──────────────────────────────────────────────────────
// CRCs verified against docs/pdus/ess-dl-bms-41_260321_0323.txt

TEST(DalyBmsBleEssDlBmsRequestFrameTest, DlCells) {
  TestableDalyBmsBle bms;
  bms.set_protocol_version(DALY_0X81);
  // 81 03 00 00 00 40 5B FA
  EXPECT_EQ(bms.build_frame_(0x03, 0x0000, 0x0040),
            (std::array<uint8_t, 8>{0x81, 0x03, 0x00, 0x00, 0x00, 0x40, 0x5B, 0xFA}));
}

TEST(DalyBmsBleEssDlBmsRequestFrameTest, DlStatus) {
  TestableDalyBmsBle bms;
  bms.set_protocol_version(DALY_0X81);
  // 81 03 00 41 00 3E 8B CE
  EXPECT_EQ(bms.build_frame_(0x03, 0x0041, 0x003E),
            (std::array<uint8_t, 8>{0x81, 0x03, 0x00, 0x41, 0x00, 0x3E, 0x8B, 0xCE}));
}

TEST(DalyBmsBleEssDlBmsRequestFrameTest, DlVersion) {
  TestableDalyBmsBle bms;
  bms.set_protocol_version(DALY_0X81);
  // 81 03 01 78 00 4A 5A 18
  EXPECT_EQ(bms.build_frame_(0x03, 0x0178, 0x004A),
            (std::array<uint8_t, 8>{0x81, 0x03, 0x01, 0x78, 0x00, 0x4A, 0x5A, 0x18}));
}

TEST(DalyBmsBleEssDlBmsRequestFrameTest, DlSettings1) {
  TestableDalyBmsBle bms;
  bms.set_protocol_version(DALY_0X81);
  // 81 03 01 00 00 51 9A 0A
  EXPECT_EQ(bms.build_frame_(0x03, 0x0100, 0x0051),
            (std::array<uint8_t, 8>{0x81, 0x03, 0x01, 0x00, 0x00, 0x51, 0x9A, 0x0A}));
}

TEST(DalyBmsBleEssDlBmsRequestFrameTest, DlBalancerSwitch) {
  TestableDalyBmsBle bms;
  bms.set_protocol_version(DALY_0X81);
  // 81 03 00 CF 00 01 AB F5
  EXPECT_EQ(bms.build_frame_(0x03, 0x00CF, 0x0001),
            (std::array<uint8_t, 8>{0x81, 0x03, 0x00, 0xCF, 0x00, 0x01, 0xAB, 0xF5}));
}

}  // namespace esphome::daly_bms_ble::testing
