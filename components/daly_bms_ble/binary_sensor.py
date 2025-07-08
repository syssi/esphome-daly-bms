import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA, DalyBmsBle

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
            icon="mdi:battery-heart-variant"
        ),
        cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging"
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:power-plug"
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_DALY_BMS_BLE_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
