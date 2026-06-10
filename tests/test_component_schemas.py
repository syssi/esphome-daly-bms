"""Schema structure tests for daly_bms_ble ESPHome component modules."""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

import components.daly_bms_ble as hub  # noqa: E402
from components.daly_bms_ble import (  # noqa: E402
    binary_sensor,
    button,
    number,
    sensor,
    switch,
    text_sensor,
)


class TestHubConstants:
    def test_conf_ids_defined(self):
        assert hub.CONF_DALY_BMS_BLE_ID == "daly_bms_ble_id"
        assert hub.CONF_STATUS_REGISTERS == "status_registers"


class TestSensorLists:
    def test_cells_count(self):
        assert len(sensor.CELLS) == 48

    def test_cells_naming(self):
        assert sensor.CELLS[0] == "cell_voltage_1"
        assert sensor.CELLS[47] == "cell_voltage_48"
        for i, key in enumerate(sensor.CELLS, 1):
            assert key == f"cell_voltage_{i}"

    def test_temperatures_count(self):
        assert len(sensor.TEMPERATURES) == 8

    def test_temperatures_naming(self):
        assert sensor.TEMPERATURES[0] == "temperature_1"
        assert sensor.TEMPERATURES[7] == "temperature_8"
        for i, key in enumerate(sensor.TEMPERATURES, 1):
            assert key == f"temperature_{i}"

    def test_sensors_list_completeness(self):
        assert "total_voltage" in sensor.SENSOR_DEFS
        assert "state_of_charge" in sensor.SENSOR_DEFS
        assert "error_bitmask" in sensor.SENSOR_DEFS
        assert len(sensor.SENSOR_DEFS) == 25

    def test_no_cell_keys_in_sensors_list(self):
        for key in sensor.SENSOR_DEFS:
            assert key not in sensor.CELLS
            assert key not in sensor.TEMPERATURES


class TestBinarySensorConstants:
    def test_binary_sensor_defs_dict(self):
        assert binary_sensor.CONF_ONLINE_STATUS in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_BALANCING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_CHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_DISCHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_PRECHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert len(binary_sensor.BINARY_SENSOR_DEFS) == 5


class TestSwitchConstants:
    def test_switches_dict(self):
        assert switch.CONF_CHARGING in switch.SWITCHES
        assert switch.CONF_DISCHARGING in switch.SWITCHES
        assert switch.CONF_BALANCER in switch.SWITCHES
        assert len(switch.SWITCHES) == 3


class TestButtonConstants:
    def test_buttons_dict_has_all_entries(self):
        from esphome.const import CONF_FACTORY_RESET, CONF_RESTART

        assert button.CONF_RETRIEVE_SETTINGS in button.BUTTONS
        assert button.CONF_RETRIEVE_VERSION in button.BUTTONS
        assert CONF_RESTART in button.BUTTONS
        assert button.CONF_SHUTDOWN in button.BUTTONS
        assert button.CONF_RESET_CURRENT in button.BUTTONS
        assert CONF_FACTORY_RESET in button.BUTTONS

    def test_button_addresses_are_unique(self):
        addresses = [address for _, address, _ in button.BUTTONS.values()]
        assert len(addresses) == len(set(addresses))


class TestNumberConstants:
    def test_numbers_count(self):
        assert len(number.NUMBERS) == 39

    def test_number_addresses_are_unique(self):
        addresses = [address for address, *_ in number.NUMBERS.values()]
        assert len(addresses) == len(set(addresses))

    def test_number_tuple_structure(self):
        for address, factor, offset, min_val, max_val, step in number.NUMBERS.values():
            assert isinstance(address, int)
            assert factor > 0
            assert min_val < max_val
            assert step > 0

    def test_rated_capacity_address(self):
        assert number.NUMBERS[number.CONF_RATED_CAPACITY][0] == 0x0080

    def test_mosfet_overtemperature_alarm_address(self):
        assert number.NUMBERS[number.CONF_MOSFET_OVERTEMPERATURE_ALARM][0] == 0x00A8

    def test_state_of_charge_setting_address(self):
        assert number.NUMBERS[number.CONF_STATE_OF_CHARGE_SETTING][0] == 0x00A7

    def test_all_number_keys_present(self):
        expected = {
            number.CONF_RATED_CAPACITY,
            number.CONF_CELL_VOLTAGE_REFERENCE,
            number.CONF_ACQUISITION_BOARD_COUNT,
            number.CONF_BOARD_1_CELL_COUNT,
            number.CONF_BOARD_2_CELL_COUNT,
            number.CONF_BOARD_3_CELL_COUNT,
            number.CONF_BOARD_1_TEMPERATURE_SENSOR_COUNT,
            number.CONF_BOARD_2_TEMPERATURE_SENSOR_COUNT,
            number.CONF_BOARD_3_TEMPERATURE_SENSOR_COUNT,
            number.CONF_BATTERY_TYPE,
            number.CONF_SLEEP_WAIT_TIME,
            number.CONF_CELL_OVERVOLTAGE_WARNING,
            number.CONF_CELL_OVERVOLTAGE_ALARM,
            number.CONF_CELL_UNDERVOLTAGE_WARNING,
            number.CONF_CELL_UNDERVOLTAGE_ALARM,
            number.CONF_TOTAL_OVERVOLTAGE_WARNING,
            number.CONF_TOTAL_OVERVOLTAGE_ALARM,
            number.CONF_TOTAL_UNDERVOLTAGE_WARNING,
            number.CONF_TOTAL_UNDERVOLTAGE_ALARM,
            number.CONF_CHARGING_OVERCURRENT_WARNING,
            number.CONF_CHARGING_OVERCURRENT_ALARM,
            number.CONF_DISCHARGING_OVERCURRENT_WARNING,
            number.CONF_DISCHARGING_OVERCURRENT_ALARM,
            number.CONF_CHARGING_OVERTEMPERATURE_WARNING,
            number.CONF_CHARGING_OVERTEMPERATURE_ALARM,
            number.CONF_CHARGING_UNDERTEMPERATURE_WARNING,
            number.CONF_CHARGING_UNDERTEMPERATURE_ALARM,
            number.CONF_DISCHARGING_OVERTEMPERATURE_WARNING,
            number.CONF_DISCHARGING_OVERTEMPERATURE_ALARM,
            number.CONF_DISCHARGING_UNDERTEMPERATURE_WARNING,
            number.CONF_DISCHARGING_UNDERTEMPERATURE_ALARM,
            number.CONF_CELL_VOLTAGE_DIFFERENCE_WARNING,
            number.CONF_CELL_VOLTAGE_DIFFERENCE_ALARM,
            number.CONF_TEMPERATURE_DIFFERENCE_WARNING,
            number.CONF_TEMPERATURE_DIFFERENCE_ALARM,
            number.CONF_BALANCING_ACTIVATION_VOLTAGE,
            number.CONF_BALANCING_ACTIVATION_VOLTAGE_DIFFERENCE,
            number.CONF_STATE_OF_CHARGE_SETTING,
            number.CONF_MOSFET_OVERTEMPERATURE_ALARM,
        }
        assert set(number.NUMBERS.keys()) == expected


class TestTextSensorConstants:
    def test_text_sensors_list(self):
        assert text_sensor.CONF_BATTERY_STATUS in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_ERRORS in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_SOFTWARE_VERSION in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_HARDWARE_VERSION in text_sensor.TEXT_SENSORS
