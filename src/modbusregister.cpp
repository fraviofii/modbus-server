#include "modbusregister.h"
#include "modbus/modbus.h"

#include <cstring>

const std::vector<std::string> ModbusRegister::typeList = {
    "uint16",
    "uint32",
    "float",
    "double",
    "unknown"
};

ModbusRegister::ModbusRegister() : type(0), command(0) {
    memset(value.bytes,0,sizeof(value));
}

void ModbusRegister::exportValue(uint16_t *out) {
    switch (type) {
    case MODBUS_TYPE_UINT16:
        out[0] = value.val16[0];
        break;
    case MODBUS_TYPE_UINT32:
        out[0] = value.val16[0];
        out[1] = value.val16[1];
        break;
    case MODBUS_TYPE_FLOAT:
        modbus_set_float(value.valf[0],out);
        break;
    case MODBUS_TYPE_DOUBLE:
        out[0] = value.val16[0];
        out[1] = value.val16[1];
        out[2] = value.val16[2];
        out[3] = value.val16[3];
        break;
    }
}

std::vector<std::string> const ModbusRegister::getTypeList(void) {
    /*
     * The description must be in the same order of the enum
     */
    return typeList;
}

int ModbusRegister::getType(std::string type) {
    int pos = 0;
    for (auto& _type : typeList) {
        if (type == _type) {
            return pos;
        }
        else {
            pos++;
        }
    }
    return pos;
}

std::string ModbusRegister::getType(int type) {
    return typeList[type];
}
