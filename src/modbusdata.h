#pragma once

#include "sqlite_orm/sqlite_orm.h"

#include <string>

using namespace sqlite_orm;

struct ModbusData {
    int id;
    int slaveId;
    int address;
    int command;
    int type;
    int value;

    static auto initStorage(const std::string& filename) {        
        auto storage = make_storage(
            filename,
            make_table("modbus_data",
                make_column("id", &ModbusData::id, autoincrement(), primary_key()),
                make_column("slaveId", &ModbusData::slaveId),
                make_column("address", &ModbusData::address),
                make_column("command", &ModbusData::command),
                make_column("type", &ModbusData::type),
                make_column("value", &ModbusData::value)
            )
        );
        storage.sync_schema();
        return storage;
    }
};
