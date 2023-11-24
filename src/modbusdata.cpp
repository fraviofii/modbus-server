#include "modbusdata.h"

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