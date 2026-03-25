import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA

DEPENDENCIES = ["daly_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_BALANCING = "balancing"
CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"

BINARY_SENSORS = [
    CONF_BALANCING,
    CONF_CHARGING,
    CONF_DISCHARGING,
]

CONFIG_SCHEMA = DALY_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BALANCING): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:battery-heart-variant"
        ),
        cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:battery-charging"
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.binary_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:power-plug"
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_DALY_BMS_BLE_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
