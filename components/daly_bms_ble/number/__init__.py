import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import CONF_ID, UNIT_PERCENT

from .. import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA, daly_bms_ble_ns

DEPENDENCIES = ["daly_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_STATE_OF_CHARGE_SETTING = "state_of_charge_setting"

ICON_STATE_OF_CHARGE_SETTING = "mdi:battery-sync"

DalyNumber = daly_bms_ble_ns.class_("DalyNumber", number.Number, cg.Component)

# key: (register_address, factor, min_value, max_value, step)
NUMBERS = {
    CONF_STATE_OF_CHARGE_SETTING: (0x00A7, 10.0, 0.0, 100.0, 0.1),
}

CONFIG_SCHEMA = DALY_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_STATE_OF_CHARGE_SETTING): number.number_schema(
            DalyNumber,
            icon=ICON_STATE_OF_CHARGE_SETTING,
            unit_of_measurement=UNIT_PERCENT,
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_DALY_BMS_BLE_ID])
    for key, (address, factor, min_val, max_val, step) in NUMBERS.items():
        if key not in config:
            continue
        conf = config[key]
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await number.register_number(
            var, conf, min_value=min_val, max_value=max_val, step=step
        )
        cg.add(getattr(hub, f"set_{key}_number")(var))
        cg.add(var.set_parent(hub))
        cg.add(var.set_holding_register(address))
        cg.add(var.set_factor(factor))
