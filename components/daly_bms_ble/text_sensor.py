import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA

DEPENDENCIES = ["daly_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_BATTERY_STATUS = "battery_status"
CONF_ERRORS = "errors"

ICON_BATTERY_STATUS = "mdi:battery-charging"
ICON_ERRORS = "mdi:alert-circle-outline"

TEXT_SENSORS = [
    CONF_BATTERY_STATUS,
    CONF_ERRORS,
]

CONFIG_SCHEMA = DALY_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BATTERY_STATUS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor,
            icon=ICON_BATTERY_STATUS,
        ),
        cv.Optional(CONF_ERRORS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor,
            icon=ICON_ERRORS,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_DALY_BMS_BLE_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
