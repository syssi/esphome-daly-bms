import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv

from . import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA
from .const import CONF_CHARGING, CONF_DISCHARGING, ICON_CHARGING, ICON_DISCHARGING

DEPENDENCIES = ["daly_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_BALANCING = "balancing"

ICON_BALANCING = "mdi:battery-heart-variant"

# key: binary_sensor_schema kwargs
BINARY_SENSOR_DEFS = {
    CONF_BALANCING: {
        "icon": ICON_BALANCING,
    },
    CONF_CHARGING: {
        "icon": ICON_CHARGING,
    },
    CONF_DISCHARGING: {
        "icon": ICON_DISCHARGING,
    },
}

CONFIG_SCHEMA = DALY_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): binary_sensor.binary_sensor_schema(**kwargs)
        for key, kwargs in BINARY_SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_DALY_BMS_BLE_ID])
    for key in BINARY_SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
