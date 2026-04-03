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
        assert len(sensor.CELLS) == 32

    def test_cells_naming(self):
        assert sensor.CELLS[0] == "cell_voltage_1"
        assert sensor.CELLS[31] == "cell_voltage_32"
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
        assert "total_voltage" in sensor.SENSORS
        assert "state_of_charge" in sensor.SENSORS
        assert "error_bitmask" in sensor.SENSORS
        assert len(sensor.SENSORS) == 20

    def test_no_cell_keys_in_sensors_list(self):
        for key in sensor.SENSORS:
            assert key not in sensor.CELLS
            assert key not in sensor.TEMPERATURES


class TestBinarySensorConstants:
    def test_binary_sensor_defs_dict(self):
        assert binary_sensor.CONF_BALANCING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_CHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_DISCHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert len(binary_sensor.BINARY_SENSOR_DEFS) == 3


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
        assert CONF_RESTART in button.BUTTONS
        assert button.CONF_SHUTDOWN in button.BUTTONS
        assert button.CONF_RESET_CURRENT in button.BUTTONS
        assert CONF_FACTORY_RESET in button.BUTTONS

    def test_button_addresses_are_unique(self):
        addresses = list(button.BUTTONS.values())
        assert len(addresses) == len(set(addresses))


class TestNumberConstants:
    def test_numbers_dict(self):
        assert number.CONF_STATE_OF_CHARGE_SETTING in number.NUMBERS

    def test_number_tuple_structure(self):
        for key, (address, factor, min_val, max_val, step) in number.NUMBERS.items():
            assert isinstance(address, int)
            assert factor > 0
            assert min_val < max_val
            assert step > 0


class TestTextSensorConstants:
    def test_text_sensors_list(self):
        assert text_sensor.CONF_BATTERY_STATUS in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_ERRORS in text_sensor.TEXT_SENSORS
