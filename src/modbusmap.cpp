#include "modbusmap.h"

#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"

#include <fstream>

using json = nlohmann::json;

ModbusMap::ModbusMap(std::string filename) :
    cmd4Offset(30001),
    cmd3Offset(0)
{
    spdlog::debug("Initializing ModbusMap with file {}", filename);
    /* Check if the file exists */
    if (access(filename.c_str(), F_OK) == -1) {
        /* File does not exists */
        spdlog::error("Configuration file not found ... creating one");
        return;
    }

    spdlog::info("Loading configuration file...");
    std::ifstream f(filename);
    json config;
    try {
        config = json::parse(f);
    } catch (json::parse_error& e2) {
        spdlog::error("Error parsing addresses file: {}", e2.what());
        return;
    }

    if (config.contains("cmd4Offset") == true) {
        cmd4Offset = config["cmd4Offset"].get<int>();
        spdlog::debug("Initializing cmd4Offset to {}", cmd4Offset);
    }
    else {
        spdlog::warn("Field 'cmd4Offset' not found");
    }

    if (config.contains("cmd3Offset") == true) {
        cmd3Offset = config["cmd3Offset"].get<int>();
        spdlog::debug("Initializing cmd3Offset to {}", cmd3Offset);
    }
    else {
        spdlog::warn("Field 'cmd3Offset' not found");
    }

    if (config.contains("addresses") == true) {
        for (auto& [id, value] : config["addresses"].items()) {
            int valId = std::stoi(id);
            spdlog::debug("Reading slave id: {}",valId);
            if (value.is_object() == true) {
                for (auto& [reg, cfgs] : value.items()) {
                    ModbusRegister modbusRegister;
                    int valReg = std::stoi(reg);
                    spdlog::debug("Reading register {}",valReg);
                    if (cfgs.contains("type") == true) {
                        modbusRegister.type = cfgs["type"].get<int>();
                    }
                    else {
                        spdlog::warn("Slave id {}, reg {}, field 'type' not found",valId, valReg);
                    }
                    if (cfgs.contains("command") == true) {
                        modbusRegister.command = cfgs["command"].get<int>();
                    }
                    else {
                        spdlog::warn("Slave id {}, reg {}, field 'command' not found. Discarding",valId, valReg);
                        continue;
                    }
                    if (cfgs.contains("initialValue") == true) {
                        if (modbusRegister.type == ModbusRegister::MODBUS_TYPE_FLOAT) {
                            modbusRegister.value.valf[0] = cfgs["initialValue"].get<float>();
                            spdlog::debug("Setting id {}, reg {} float initial value {}", valId, valReg, modbusRegister.value.valf[0]);
                        }
                        else if (modbusRegister.type == ModbusRegister::MODBUS_TYPE_UINT32) {
                            modbusRegister.value.val32[0] = cfgs["initialValue"].get<int>();
                            spdlog::debug("Setting id {}, reg {} float initial value {}", valId, valReg, modbusRegister.value.val32[0]);
                        }
                        else if (modbusRegister.type == ModbusRegister::MODBUS_TYPE_UINT16) {
                            modbusRegister.value.val16[0] = cfgs["initialValue"].get<int>();
                            spdlog::debug("Setting id {}, reg {} float initial value {}", valId, valReg, modbusRegister.value.val16[0]);
                        }
                        /**
                         * TODO: Add remaining items...
                         */
                    }
                    else {
                        if (modbusRegister.type == ModbusRegister::MODBUS_TYPE_FLOAT) {
                            modbusRegister.value.valf[0] = 0.0;
                        }
                        else if (modbusRegister.type == ModbusRegister::MODBUS_TYPE_UINT32) {
                            modbusRegister.value.val32[0] = 0;
                        }
                        else if (modbusRegister.type == ModbusRegister::MODBUS_TYPE_UINT16) {
                            modbusRegister.value.val16[0] = 0;
                        }
                    }
                    /*
                        * Updating the register address with the offset according to the command
                        */
                    if (modbusRegister.command == 3) {
                        valReg -= cmd3Offset;
                    }
                    else if (modbusRegister.command == 4) {
                        valReg -= cmd4Offset;
                    }
                    else {
                        spdlog::error("Invalid configured command: {}. Discarding", modbusRegister.command);
                        continue;
                    }
                    /*
                        * Adding the structure to the map
                        */
                    map[valId][valReg] = modbusRegister;
                    spdlog::debug("Added slaveId {}, register {} info to the map", valId, valReg);
                }
            }
            else {
                spdlog::warn("Field id {} content is not an object", id);
            }
        }
    }
}

void ModbusMap::create(void) {

}

void ModbusMap::update(std::map<int,std::map<int,ModbusRegister>> newMap) {
    map = newMap;

    /*
     * TODO: Implement an update file mechanism
     */

}

std::map<int,std::map<int,ModbusRegister>> ModbusMap::getMap(void) {
    return map;
}

int ModbusMap::getCmd3Offset(void) {
    return cmd3Offset;
}

int ModbusMap::getCmd4Offset(void) {
    return cmd4Offset;
}
