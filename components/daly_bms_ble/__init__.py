import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PASSWORD

CODEOWNERS = ["@syssi"]
DEPENDENCIES = ["ble_client"]
AUTO_LOAD = ["binary_sensor", "button", "number", "sensor", "text_sensor", "switch"]
MULTI_CONF = True

CONF_DALY_BMS_BLE_ID = "daly_bms_ble_id"
CONF_STATUS_REGISTERS = "status_registers"

daly_bms_ble_ns = cg.esphome_ns.namespace("daly_bms_ble")
DalyBmsBle = daly_bms_ble_ns.class_(
    "DalyBmsBle", ble_client.BLEClientNode, cg.PollingComponent
)

DALY_BMS_BLE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_DALY_BMS_BLE_ID): cv.use_id(DalyBmsBle),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.require_esphome_version(2024, 12, 0),
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DalyBmsBle),
            cv.Optional(CONF_PASSWORD, default="12345678"): cv.uint32_t,
            cv.Optional(CONF_STATUS_REGISTERS, default=62): cv.one_of(62, 80, int=True),
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("10s")),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)

    cg.add(var.set_password(config[CONF_PASSWORD]))
    cg.add(var.set_status_registers(config[CONF_STATUS_REGISTERS]))
