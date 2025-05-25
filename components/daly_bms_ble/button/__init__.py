import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_FACTORY_RESET, CONF_ID, CONF_RESTART

from .. import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA, daly_bms_ble_ns

DEPENDENCIES = ["daly_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"
# CONF_RESTART = "restart"
CONF_SHUTDOWN = "shutdown"
CONF_RESET_CURRENT = "reset_current"
# CONF_FACTORY_RESET = "factory_reset"

ICON_RETRIEVE_SETTINGS = "mdi:cog"
ICON_RESTART = "mdi:restart"
ICON_SHUTDOWN = "mdi:power"
ICON_RESET_CURRENT = "mdi:counter"
ICON_FACTORY_RESET = "mdi:factory"

BUTTONS = {
    CONF_RETRIEVE_SETTINGS: 0x0080,
    CONF_RESTART: 0x00F0,
    CONF_SHUTDOWN: 0x00F1,
    CONF_RESET_CURRENT: 0x00F2,
    CONF_FACTORY_RESET: 0x00F3,
}

DalyButton = daly_bms_ble_ns.class_("DalyButton", button.Button, cg.Component)

CONFIG_SCHEMA = DALY_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RETRIEVE_SETTINGS): button.button_schema(
            DalyButton, icon=ICON_RETRIEVE_SETTINGS
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESTART): button.button_schema(
            DalyButton, icon=ICON_RESTART
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_SHUTDOWN): button.button_schema(
            DalyButton, icon=ICON_SHUTDOWN
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESET_CURRENT): button.button_schema(
            DalyButton, icon=ICON_RESET_CURRENT
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_FACTORY_RESET): button.button_schema(
            DalyButton, icon=ICON_FACTORY_RESET
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_DALY_BMS_BLE_ID])
    for key, address in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await button.register_button(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
