import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PASSWORD

CODEOWNERS = ["@syssi"]
DEPENDENCIES = ["ble_client"]
AUTO_LOAD = ["binary_sensor", "button", "number", "sensor", "text_sensor", "switch"]
MULTI_CONF = True

CONF_DALY_BMS_BLE_ID = "daly_bms_ble_id"
CONF_PROTOCOL_VERSION = "protocol_version"
CONF_STATUS_REGISTERS = "status_registers"
CONF_RESPONSE_TIMEOUT = "response_timeout"

daly_bms_ble_ns = cg.esphome_ns.namespace("daly_bms_ble")
DalyBmsBle = daly_bms_ble_ns.class_(
    "DalyBmsBle", ble_client.BLEClientNode, cg.PollingComponent
)

ProtocolVersion = daly_bms_ble_ns.enum("ProtocolVersion")
PROTOCOL_VERSION_OPTIONS = {
    "DALY_0xD2": ProtocolVersion.DALY_0xD2,
    "DALY_0x81": ProtocolVersion.DALY_0x81,
}

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
            cv.Optional(CONF_PROTOCOL_VERSION, default="DALY_0xD2"): cv.enum(
                PROTOCOL_VERSION_OPTIONS, upper=False
            ),
            cv.Optional(CONF_STATUS_REGISTERS, default=62): cv.one_of(62, 80, int=True),
            cv.Optional(
                CONF_RESPONSE_TIMEOUT, default="3s"
            ): cv.positive_time_period_milliseconds,
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
    cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))
    cg.add(var.set_status_registers(config[CONF_STATUS_REGISTERS]))
    cg.add(var.set_response_timeout(config[CONF_RESPONSE_TIMEOUT]))
