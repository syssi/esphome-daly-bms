import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_PERCENT,
    UNIT_SECOND,
    UNIT_VOLT,
)

from .. import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA, daly_bms_ble_ns

DEPENDENCIES = ["daly_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_ACQUISITION_BOARD_COUNT = "acquisition_board_count"
CONF_BALANCING_ACTIVATION_VOLTAGE = "balancing_activation_voltage"
CONF_BALANCING_ACTIVATION_VOLTAGE_DIFFERENCE = "balancing_activation_voltage_difference"
CONF_BATTERY_TYPE = "battery_type"
CONF_BOARD_1_CELL_COUNT = "board_1_cell_count"
CONF_BOARD_1_TEMPERATURE_SENSOR_COUNT = "board_1_temperature_sensor_count"
CONF_BOARD_2_CELL_COUNT = "board_2_cell_count"
CONF_BOARD_2_TEMPERATURE_SENSOR_COUNT = "board_2_temperature_sensor_count"
CONF_BOARD_3_CELL_COUNT = "board_3_cell_count"
CONF_BOARD_3_TEMPERATURE_SENSOR_COUNT = "board_3_temperature_sensor_count"
CONF_CELL_OVERVOLTAGE_ALARM = "cell_overvoltage_alarm"
CONF_CELL_OVERVOLTAGE_WARNING = "cell_overvoltage_warning"
CONF_CELL_UNDERVOLTAGE_ALARM = "cell_undervoltage_alarm"
CONF_CELL_UNDERVOLTAGE_WARNING = "cell_undervoltage_warning"
CONF_CELL_VOLTAGE_DIFFERENCE_ALARM = "cell_voltage_difference_alarm"
CONF_CELL_VOLTAGE_DIFFERENCE_WARNING = "cell_voltage_difference_warning"
CONF_CELL_VOLTAGE_REFERENCE = "cell_voltage_reference"
CONF_CHARGING_OVERCURRENT_ALARM = "charging_overcurrent_alarm"
CONF_CHARGING_OVERCURRENT_WARNING = "charging_overcurrent_warning"
CONF_CHARGING_OVERTEMPERATURE_ALARM = "charging_overtemperature_alarm"
CONF_CHARGING_OVERTEMPERATURE_WARNING = "charging_overtemperature_warning"
CONF_CHARGING_UNDERTEMPERATURE_ALARM = "charging_undertemperature_alarm"
CONF_CHARGING_UNDERTEMPERATURE_WARNING = "charging_undertemperature_warning"
CONF_DISCHARGING_OVERCURRENT_ALARM = "discharging_overcurrent_alarm"
CONF_DISCHARGING_OVERCURRENT_WARNING = "discharging_overcurrent_warning"
CONF_DISCHARGING_OVERTEMPERATURE_ALARM = "discharging_overtemperature_alarm"
CONF_DISCHARGING_OVERTEMPERATURE_WARNING = "discharging_overtemperature_warning"
CONF_DISCHARGING_UNDERTEMPERATURE_ALARM = "discharging_undertemperature_alarm"
CONF_DISCHARGING_UNDERTEMPERATURE_WARNING = "discharging_undertemperature_warning"
CONF_MOSFET_OVERTEMPERATURE_ALARM = "mosfet_overtemperature_alarm"
CONF_RATED_CAPACITY = "rated_capacity"
CONF_SLEEP_WAIT_TIME = "sleep_wait_time"
CONF_STATE_OF_CHARGE_SETTING = "state_of_charge_setting"
CONF_TEMPERATURE_DIFFERENCE_ALARM = "temperature_difference_alarm"
CONF_TEMPERATURE_DIFFERENCE_WARNING = "temperature_difference_warning"
CONF_TOTAL_OVERVOLTAGE_ALARM = "total_overvoltage_alarm"
CONF_TOTAL_OVERVOLTAGE_WARNING = "total_overvoltage_warning"
CONF_TOTAL_UNDERVOLTAGE_ALARM = "total_undervoltage_alarm"
CONF_TOTAL_UNDERVOLTAGE_WARNING = "total_undervoltage_warning"

DalyNumber = daly_bms_ble_ns.class_("DalyNumber", number.Number, cg.Component)

# key: (address, factor, offset, min_value, max_value, step)
NUMBERS = {
    CONF_RATED_CAPACITY: (0x0080, 10.0, 0, 0.0, 6553.5, 0.1),
    CONF_CELL_VOLTAGE_REFERENCE: (0x0081, 1.0, 0, 0, 65535, 1),
    CONF_ACQUISITION_BOARD_COUNT: (0x0082, 1.0, 0, 0, 65535, 1),
    CONF_BOARD_1_CELL_COUNT: (0x0083, 1.0, 0, 0, 65535, 1),
    CONF_BOARD_2_CELL_COUNT: (0x0084, 1.0, 0, 0, 65535, 1),
    CONF_BOARD_3_CELL_COUNT: (0x0085, 1.0, 0, 0, 65535, 1),
    CONF_BOARD_1_TEMPERATURE_SENSOR_COUNT: (0x0086, 1.0, 0, 0, 65535, 1),
    CONF_BOARD_2_TEMPERATURE_SENSOR_COUNT: (0x0087, 1.0, 0, 0, 65535, 1),
    CONF_BOARD_3_TEMPERATURE_SENSOR_COUNT: (0x0088, 1.0, 0, 0, 65535, 1),
    CONF_BATTERY_TYPE: (0x0089, 1.0, 0, 0, 2, 1),
    CONF_SLEEP_WAIT_TIME: (0x008A, 1.0, 0, 0, 65535, 1),
    CONF_CELL_OVERVOLTAGE_WARNING: (0x008B, 1.0, 0, 0, 65535, 1),
    CONF_CELL_OVERVOLTAGE_ALARM: (0x008C, 1.0, 0, 0, 65535, 1),
    CONF_CELL_UNDERVOLTAGE_WARNING: (0x008D, 1.0, 0, 0, 65535, 1),
    CONF_CELL_UNDERVOLTAGE_ALARM: (0x008E, 1.0, 0, 0, 65535, 1),
    CONF_TOTAL_OVERVOLTAGE_WARNING: (0x008F, 10.0, 0, 0, 6553.5, 0.1),
    CONF_TOTAL_OVERVOLTAGE_ALARM: (0x0090, 10.0, 0, 0, 6553.5, 0.1),
    CONF_TOTAL_UNDERVOLTAGE_WARNING: (0x0091, 10.0, 0, 0, 6553.5, 0.1),
    CONF_TOTAL_UNDERVOLTAGE_ALARM: (0x0092, 10.0, 0, 0, 6553.5, 0.1),
    CONF_CHARGING_OVERCURRENT_WARNING: (0x0093, 10.0, 30000, -3000, 3553.5, 0.1),
    CONF_CHARGING_OVERCURRENT_ALARM: (0x0094, 10.0, 30000, -3000, 3553.5, 0.1),
    CONF_DISCHARGING_OVERCURRENT_WARNING: (0x0095, 10.0, 30000, -3000, 3553.5, 0.1),
    CONF_DISCHARGING_OVERCURRENT_ALARM: (0x0096, 10.0, 30000, -3000, 3553.5, 0.1),
    CONF_CHARGING_OVERTEMPERATURE_WARNING: (0x0097, 1.0, 40, -40, 100, 1),
    CONF_CHARGING_OVERTEMPERATURE_ALARM: (0x0098, 1.0, 40, -40, 100, 1),
    CONF_CHARGING_UNDERTEMPERATURE_WARNING: (0x0099, 1.0, 40, -40, 100, 1),
    CONF_CHARGING_UNDERTEMPERATURE_ALARM: (0x009A, 1.0, 40, -40, 100, 1),
    CONF_DISCHARGING_OVERTEMPERATURE_WARNING: (0x009B, 1.0, 40, -40, 100, 1),
    CONF_DISCHARGING_OVERTEMPERATURE_ALARM: (0x009C, 1.0, 40, -40, 100, 1),
    CONF_DISCHARGING_UNDERTEMPERATURE_WARNING: (0x009D, 1.0, 40, -40, 100, 1),
    CONF_DISCHARGING_UNDERTEMPERATURE_ALARM: (0x009E, 1.0, 40, -40, 100, 1),
    CONF_CELL_VOLTAGE_DIFFERENCE_WARNING: (0x009F, 1.0, 0, 0, 65535, 1),
    CONF_CELL_VOLTAGE_DIFFERENCE_ALARM: (0x00A0, 1.0, 0, 0, 65535, 1),
    CONF_TEMPERATURE_DIFFERENCE_WARNING: (0x00A1, 1.0, 0, 0, 65535, 1),
    CONF_TEMPERATURE_DIFFERENCE_ALARM: (0x00A2, 1.0, 0, 0, 65535, 1),
    CONF_BALANCING_ACTIVATION_VOLTAGE: (0x00A3, 1.0, 0, 0, 65535, 1),
    CONF_BALANCING_ACTIVATION_VOLTAGE_DIFFERENCE: (0x00A4, 1.0, 0, 0, 65535, 1),
    CONF_STATE_OF_CHARGE_SETTING: (0x00A7, 10.0, 0, 0.0, 100.0, 0.1),
    CONF_MOSFET_OVERTEMPERATURE_ALARM: (0x00A8, 1.0, 40, -40, 100, 1),
}


CONFIG_SCHEMA = DALY_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RATED_CAPACITY): number.number_schema(
            DalyNumber,
            unit_of_measurement="Ah",
            icon="mdi:battery",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_REFERENCE): number.number_schema(
            DalyNumber,
            unit_of_measurement="mV",
            icon="mdi:flash",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_ACQUISITION_BOARD_COUNT): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_EMPTY,
            icon="mdi:circuit-board",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BOARD_1_CELL_COUNT): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_EMPTY,
            icon="mdi:battery-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BOARD_2_CELL_COUNT): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_EMPTY,
            icon="mdi:battery-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BOARD_3_CELL_COUNT): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_EMPTY,
            icon="mdi:battery-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BOARD_1_TEMPERATURE_SENSOR_COUNT): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_EMPTY,
            icon="mdi:thermometer",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BOARD_2_TEMPERATURE_SENSOR_COUNT): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_EMPTY,
            icon="mdi:thermometer",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BOARD_3_TEMPERATURE_SENSOR_COUNT): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_EMPTY,
            icon="mdi:thermometer",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BATTERY_TYPE): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_EMPTY,
            icon="mdi:battery",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_SLEEP_WAIT_TIME): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_SECOND,
            icon="mdi:sleep",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_OVERVOLTAGE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement="mV",
            icon="mdi:alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_OVERVOLTAGE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement="mV",
            icon="mdi:alert-circle",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_UNDERVOLTAGE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement="mV",
            icon="mdi:alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_UNDERVOLTAGE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement="mV",
            icon="mdi:alert-circle",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_TOTAL_OVERVOLTAGE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_VOLT,
            icon="mdi:alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_TOTAL_OVERVOLTAGE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_VOLT,
            icon="mdi:alert-circle",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_TOTAL_UNDERVOLTAGE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_VOLT,
            icon="mdi:alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_TOTAL_UNDERVOLTAGE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_VOLT,
            icon="mdi:alert-circle",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CHARGING_OVERCURRENT_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_AMPERE,
            icon="mdi:alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CHARGING_OVERCURRENT_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_AMPERE,
            icon="mdi:alert-circle",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGING_OVERCURRENT_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_AMPERE,
            icon="mdi:alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGING_OVERCURRENT_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_AMPERE,
            icon="mdi:alert-circle",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CHARGING_OVERTEMPERATURE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CHARGING_OVERTEMPERATURE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CHARGING_UNDERTEMPERATURE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CHARGING_UNDERTEMPERATURE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGING_OVERTEMPERATURE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGING_OVERTEMPERATURE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGING_UNDERTEMPERATURE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISCHARGING_UNDERTEMPERATURE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_DIFFERENCE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement="mV",
            icon="mdi:delta",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_DIFFERENCE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement="mV",
            icon="mdi:delta",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_TEMPERATURE_DIFFERENCE_WARNING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_TEMPERATURE_DIFFERENCE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BALANCING_ACTIVATION_VOLTAGE): number.number_schema(
            DalyNumber,
            unit_of_measurement="mV",
            icon="mdi:scale-balance",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_BALANCING_ACTIVATION_VOLTAGE_DIFFERENCE): number.number_schema(
            DalyNumber,
            unit_of_measurement="mV",
            icon="mdi:scale-balance",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_STATE_OF_CHARGE_SETTING): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_PERCENT,
            icon="mdi:battery-sync",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_MOSFET_OVERTEMPERATURE_ALARM): number.number_schema(
            DalyNumber,
            unit_of_measurement=UNIT_CELSIUS,
            icon="mdi:thermometer-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_DALY_BMS_BLE_ID])
    for key, (address, factor, offset, min_val, max_val, step) in NUMBERS.items():
        if key in config:
            conf = config[key]
            var = await number.new_number(
                conf, min_value=min_val, max_value=max_val, step=step
            )
            await cg.register_component(var, conf)
            cg.add(hub.register_settings_number(address, var, factor, offset))
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
            cg.add(var.set_factor(factor))
            cg.add(var.set_offset(offset))
