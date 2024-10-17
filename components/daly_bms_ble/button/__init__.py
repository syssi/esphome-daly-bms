import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_FACTORY_RESET, CONF_ICON, CONF_ID

from .. import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA, daly_bms_ble_ns

DEPENDENCIES = ["daly_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_RETRIEVE_SETTINGS = "retrieve_settings"
# CONF_FACTORY_RESET = "factory_reset"
CONF_RESET_CHARGING_CYCLES = "reset_charging_cycles"
CONF_RESET_TOTAL_CHARGED_CAPACITY = "reset_total_charged_capacity"
CONF_RESET_TOTAL_DISCHARGED_CAPACITY = "reset_total_discharged_capacity"

ICON_RETRIEVE_SETTINGS = "mdi:cog"
ICON_FACTORY_RESET = "mdi:factory"
ICON_RESET_CHARGING_CYCLES = "mdi:counter"
ICON_RESET_TOTAL_CHARGED_CAPACITY = "mdi:counter"
ICON_RESET_TOTAL_DISCHARGED_CAPACITY = "mdi:counter"

BUTTONS = {
    CONF_RETRIEVE_SETTINGS: 0x56,
    CONF_FACTORY_RESET: 0xCC,
    CONF_RESET_CHARGING_CYCLES: 0xAA,
    CONF_RESET_TOTAL_CHARGED_CAPACITY: 0xCB,
    CONF_RESET_TOTAL_DISCHARGED_CAPACITY: 0xCA,
}

DalyButton = daly_bms_ble_ns.class_("DalyButton", button.Button, cg.Component)

CONFIG_SCHEMA = DALY_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RETRIEVE_SETTINGS): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(DalyButton),
                cv.Optional(CONF_ICON, default=ICON_RETRIEVE_SETTINGS): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_FACTORY_RESET): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(DalyButton),
                cv.Optional(CONF_ICON, default=ICON_FACTORY_RESET): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESET_CHARGING_CYCLES): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(DalyButton),
                cv.Optional(CONF_ICON, default=ICON_RESET_CHARGING_CYCLES): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESET_TOTAL_CHARGED_CAPACITY): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(DalyButton),
                cv.Optional(
                    CONF_ICON, default=ICON_RESET_TOTAL_CHARGED_CAPACITY
                ): cv.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_RESET_TOTAL_DISCHARGED_CAPACITY): button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(DalyButton),
                cv.Optional(
                    CONF_ICON, default=ICON_RESET_TOTAL_DISCHARGED_CAPACITY
                ): cv.icon,
            }
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
