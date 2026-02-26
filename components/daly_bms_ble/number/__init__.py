import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import CONF_ID, UNIT_PERCENT

from .. import CONF_DALY_BMS_BLE_ID, DALY_BMS_BLE_COMPONENT_SCHEMA, daly_bms_ble_ns

DEPENDENCIES = ["daly_bms_ble"]

CODEOWNERS = ["@syssi"]

CONF_SET_SOC = "set_soc"

ICON_SET_SOC = "mdi:battery-sync"

DalyNumber = daly_bms_ble_ns.class_("DalyNumber", number.Number, cg.Component)

CONFIG_SCHEMA = DALY_BMS_BLE_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_SET_SOC): number.number_schema(
            DalyNumber,
            icon=ICON_SET_SOC,
            unit_of_measurement=UNIT_PERCENT,
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_DALY_BMS_BLE_ID])
    if CONF_SET_SOC in config:
        conf = config[CONF_SET_SOC]
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await number.register_number(
            var, conf, min_value=0.0, max_value=100.0, step=0.1
        )
        cg.add(hub.set_soc_number(var))
        cg.add(var.set_parent(hub))
        cg.add(var.set_holding_register(0x00A7))
