#pragma once

#include "modbusregister.h"

#include <map>
#include <string>

class ModbusMap
{
public:
    ModbusMap(std::string filename);

    void update(std::map<int,std::map<int,ModbusRegister>> newMap);
    std::map<int,std::map<int,ModbusRegister>> getMap(void);
    int getCmd3Offset(void);
    int getCmd4Offset(void);
private:
    std::map<int,std::map<int,ModbusRegister>> map;

    int cmd4Offset;
    int cmd3Offset;

    void create(void);
};
