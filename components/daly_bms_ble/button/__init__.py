import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_FACTORY_RESET, CONF_RESTART, DEVICE_CLASS_RESTART

from .. import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA, daly_bms_ble_ns

DEPENDENCIES = ["daly_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"
CONF_RETRIEVE_VERSION = "retrieve_version"
CONF_SHUTDOWN = "shutdown"
CONF_RESET_CURRENT = "reset_current"

DALY_FUNCTION_READ = 0x03
DALY_FUNCTION_WRITE = 0x06

ICON_RETRIEVE_SETTINGS = "mdi:cog"
ICON_RETRIEVE_VERSION = "mdi:information"
ICON_RESTART = "mdi:restart"
ICON_SHUTDOWN = "mdi:power"
ICON_RESET_CURRENT = "mdi:counter"
ICON_FACTORY_RESET = "mdi:factory"

# (function, address, value)
BUTTONS = {
    CONF_RETRIEVE_SETTINGS: (DALY_FUNCTION_READ, 0x0080, 0x0029),
    CONF_RETRIEVE_VERSION: (DALY_FUNCTION_READ, 0x00A9, 0x0020),
    CONF_RESTART: (DALY_FUNCTION_WRITE, 0x00F0, 0x0001),
    CONF_SHUTDOWN: (DALY_FUNCTION_WRITE, 0x00F1, 0x0001),
    CONF_RESET_CURRENT: (DALY_FUNCTION_WRITE, 0x00F2, 0x0001),
    CONF_FACTORY_RESET: (DALY_FUNCTION_WRITE, 0x00F3, 0x0001),
}

DalyButton = daly_bms_ble_ns.class_("DalyButton", button.Button, cg.Component)

CONFIG_SCHEMA = DALY_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RETRIEVE_SETTINGS): button.button_schema(
            DalyButton, icon=ICON_RETRIEVE_SETTINGS
        ),
        cv.Optional(CONF_RETRIEVE_VERSION): button.button_schema(
            DalyButton, icon=ICON_RETRIEVE_VERSION
        ),
        cv.Optional(CONF_RESTART): button.button_schema(
            DalyButton, icon=ICON_RESTART, device_class=DEVICE_CLASS_RESTART
        ),
        cv.Optional(CONF_SHUTDOWN): button.button_schema(
            DalyButton, icon=ICON_SHUTDOWN
        ),
        cv.Optional(CONF_RESET_CURRENT): button.button_schema(
            DalyButton, icon=ICON_RESET_CURRENT
        ),
        cv.Optional(CONF_FACTORY_RESET): button.button_schema(
            DalyButton, icon=ICON_FACTORY_RESET, device_class=DEVICE_CLASS_RESTART
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_DALY_BMS_BLE_ID])
    for key, (function, address, value) in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = await button.new_button(conf)
            await cg.register_component(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_function(function))
            cg.add(var.set_holding_register(address))
            cg.add(var.set_value(value))
