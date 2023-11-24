#include <iostream>
#include <signal.h>

#include "spdlog/spdlog.h"

#include "modbusmap.h"
#include "modbusdata.h"

#define MODBUS_MAP_DEFAULT "modbusmap.json"
#define DATABASE_FILE_DEFAULT "database.db"

typedef enum {
    RTU,
    TCP
} ConnectionType;

static void usage() {
    std::cout << "Usage: ftp_client [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -d: debug mode" << std::endl;
    std::cout << "  -L <file>: save log to file" << std::endl;
    std::cout << "  -t: set connection type to TCP" << std::endl;
    std::cout << "  -r: set connection type to RTU" << std::endl;
    std::cout << "  -m <file>: set MODBUS map file" << std::endl;
    std::cout << "  -D <file>: set database file" << std::endl;
    std::cout << "  -h: help" << std::endl;
}

static void handle_signal(int sig)
{
	if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT) {
      exit(1);
	} else if (sig == SIGHUP) {
		/* do nothing */
	} else if (sig == SIGPIPE) {
		/* do nothing */
	} else if (sig == SIGUSR1) {
		// _want_snaps = !_want_snaps;
	}
}

static void setupSignal() {
  struct sigaction sigact;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_handler = handle_signal;
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, nullptr);
	sigaction(SIGTERM, &sigact, nullptr);
	sigaction(SIGHUP, &sigact, nullptr);
	sigaction(SIGPIPE, &sigact, nullptr);
	sigaction(SIGQUIT, &sigact, nullptr);
	sigaction(SIGUSR1, &sigact, nullptr);
}

static void setupDatabase(std::string databaseFile, std::string modbusMapFile) {
    spdlog::info("Setting up database");
    spdlog::info("Database file: {}", databaseFile);
    spdlog::info("Modbus map file: {}", modbusMapFile);

    ModbusMap modbusMap(modbusMapFile);
    auto map = modbusMap.getMap();
    auto cmd3Offset = modbusMap.getCmd3Offset();
    auto cmd4Offset = modbusMap.getCmd4Offset();

    auto storage = ModbusData::initStorage(databaseFile);

    for (auto& [slaveId, registers] : map) {
        spdlog::info("Slave id: {}", slaveId);
        for (auto& [registerId, modbusRegister] : registers) {
            spdlog::info("Register id: {}", registerId);
            spdlog::info("Register type: {}", modbusRegister.type);
            // Adding to the database
            ModbusData modbusData;
            modbusData.slaveId = slaveId;
            modbusData.address = registerId;
            modbusData.command = modbusRegister.command;
            if (modbusRegister.command == 3) {
                modbusData.address += cmd3Offset;
            }
            else if (modbusRegister.command == 4) {
                modbusData.address += cmd4Offset;
            }
            else {
                spdlog::error("Invalid command: {}", modbusRegister.command);
                continue;
            }
            modbusData.type = modbusRegister.type;
            if (modbusRegister.type == ModbusRegister::MODBUS_TYPE_FLOAT) {
                modbusData.value = modbusRegister.value.valf[0];
            }
            else if (modbusRegister.type == ModbusRegister::MODBUS_TYPE_UINT32) {
                modbusData.value = modbusRegister.value.val32[0];
            }
            else if (modbusRegister.type == ModbusRegister::MODBUS_TYPE_UINT16) {
                modbusData.value = modbusRegister.value.val16[0];
            }
            else {
                spdlog::error("Invalid register type: {}", modbusRegister.type);
                continue;
            }
            try {
                storage.insert(modbusData);
                spdlog::info("Data inserted to database");
            }
            catch (std::system_error& e) {
                spdlog::error("Error inserting data to database: {}", e.what());
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // Handle the signals to finish correctly the program
    setupSignal();

    // Setup log
    spdlog::set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] [thread %t] %v");

    std::string modbusMapFile = MODBUS_MAP_DEFAULT;
    std::string databaseFile = DATABASE_FILE_DEFAULT;
    ConnectionType connectionMode = TCP;
    bool isSetupDatabase = false;
    int opt;
	while ((opt = getopt(argc, argv, "dL:trm:D:h")) != -1) {
		switch (opt) {
            case 'd':
			    spdlog::set_level(spdlog::level::debug);
                spdlog::debug("Debug mode enabled");
			    break;
            case 'L':
                // Save log to file
                spdlog::info("Saving log to file {}",optarg);
                // Not yet available
                break;
            case 't':
                spdlog::info("Setting connection type to TCP");
                connectionMode = TCP;
                break;
            case 'r':
                spdlog::info("Setting connection type to RTU");
                connectionMode = RTU;
                break;
            case 'm':
                spdlog::info("Setting MODBUS map from file {}", optarg);
                modbusMapFile = optarg;
                isSetupDatabase = true;
                break;
            case 'D':
                spdlog::info("Setting database file to {}", optarg);
                databaseFile = optarg;
                break;
            case 'h':
                usage();
                return 0;
		default:
			break;
		}
	}

    if (isSetupDatabase == true) {
        setupDatabase(databaseFile, modbusMapFile);
    }

}
