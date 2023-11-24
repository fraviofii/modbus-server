#pragma once

#include <cstdint>

#include <string>
#include <vector>

class ModbusRegister
{
public:
    int type;
    int command;
    union value_u {
        uint16_t    val16[4];
        uint32_t    val32[2];
        float       valf[2];
        double      vald;
        uint8_t     bytes[8];
    } value;

    enum REGISTERS_TYPE {
        MODBUS_TYPE_UINT16,
        MODBUS_TYPE_UINT32,
        MODBUS_TYPE_FLOAT,
        MODBUS_TYPE_FLOAT_ABCD,
        MODBUS_TYPE_FLOAT_DCBA,
        MODBUS_TYPE_FLOAT_BADC,
        MODBUS_TYPE_FLOAT_CDAB,
        MODBUS_TYPE_DOUBLE,
        MODBUS_TYPE_UNKNOWN
    };

    static const std::vector<std::string> typeList;

    ModbusRegister();
    virtual ~ModbusRegister() { };

    void exportValue(uint16_t *out);
    static const std::vector<std::string> getTypeList(void);
    static int getType(std::string type);
    static std::string getType(int type);
};
