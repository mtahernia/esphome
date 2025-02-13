import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_ID, CONF_USE_FAHRENHEIT

AUTO_LOAD = ["climate_ir", "coolix"]
CODEOWNERS = ["@ricksanchez"]

carrier_ir_ns = cg.esphome_ns.namespace("carrier_ir")
CarrierIR = carrier_ir_ns.class_("CarrierIR", climate_ir.ClimateIR)


CONFIG_SCHEMA = climate_ir.CLIMATE_IR_WITH_RECEIVER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(CarrierIR),
        cv.Optional(CONF_USE_FAHRENHEIT, default=False): cv.boolean,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate_ir.register_climate_ir(var, config)
    cg.add(var.set_fahrenheit(config[CONF_USE_FAHRENHEIT]))
