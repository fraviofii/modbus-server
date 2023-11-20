#include "modbus.h"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[])
{
    modbus_t *mbCtx;
    modbus_mapping_t *mb_mapping;
    uint8_t *query;
    int header_length;
    int slaveList[5] = { 1, 3, 4, 5, 6 };
    float f = 1.0;
    int i;
    int rc;
    int requested_address;
    const uint16_t UT_REGISTERS_ADDRESS = 0;
    const uint16_t UT_REGISTERS_NB_MAX = 15;
    const uint16_t UT_INPUT_REGISTERS_ADDRESS = 30000;
    const uint16_t UT_INPUT_REGISTERS_NB = 100;
    uint16_t UT_INPUT_REGISTERS_TAB[300] = { 0 };

    union double_v {
        double d;
        uint16_t v[4];
    };
    union double_v val_d;

    union float_v {
        float d;
        uint16_t v[2];
    };
    union float_v val_f;

    union int_v {
        uint32_t i;
        uint16_t v[2];
    };
    union int_v val_i;

    spdlog::debug("Starting stand-alone testing");

    mbCtx = modbus_new_rtu("COM1",9600,'N',8,2);
    query = (uint8_t*)malloc(MODBUS_RTU_MAX_ADU_LENGTH);
    header_length = modbus_get_header_length(mbCtx);

    // modbus_set_slave_list(mbCtx,slaveList, (sizeof(slaveList)/sizeof(int)));
    modbus_set_slave(mbCtx,1);
    modbus_set_debug(mbCtx, true);

    spdlog::debug("Address mapping");

    UT_INPUT_REGISTERS_TAB[0] = 0x5678;
    UT_INPUT_REGISTERS_TAB[1] = 0x3210;
    f = 1.1;
    for (i=2;i<102;i+=2) {
        modbus_set_float_abcd(f,&UT_INPUT_REGISTERS_TAB[i]);
        f *= 2;
    }

    for(i=0;i<119;i++) {
        UT_INPUT_REGISTERS_TAB[102+i] = i;
    }

    mb_mapping = modbus_mapping_new_start_address(
                /* UT_BITS_ADDRESS */0, /* UT_BITS_NB */0,
                /* UT_INPUT_BITS_ADDRESS */0, /* UT_INPUT_BITS_NB */0,
                UT_REGISTERS_ADDRESS, UT_REGISTERS_NB_MAX,
                UT_INPUT_REGISTERS_ADDRESS, UT_INPUT_REGISTERS_NB);
    if (mb_mapping == NULL) {
        spdlog::error("Failed to allocate the mapping: {}\n",
                modbus_strerror(errno));
        modbus_free(mbCtx);
        return -1;
    }

    /* Initialize values of INPUT REGISTERS */
    for (i=0; i < UT_INPUT_REGISTERS_NB; i++) {
        mb_mapping->tab_input_registers[i] = UT_INPUT_REGISTERS_TAB[i];
    }

    spdlog::debug("Connecting");

    rc = modbus_connect(mbCtx);
    if (rc == -1) {
        spdlog::error("Unable to connect {}\n", modbus_strerror(errno));
        modbus_free(mbCtx);
        return -1;
    }

    spdlog::debug("Waiting for messages");

    header_length = modbus_get_header_length(mbCtx);
    for(;;) {
        do {
            rc = modbus_receive(mbCtx, query);
            /* Filtered queries return 0 */
        } while (rc == 0);

        /* The connection is not closed on errors which require on reply such as
                   bad CRC in RTU. */
        if (rc == -1 && errno != EMBBADCRC) {
            /* Quit */
            break;
        }

        if (query[header_length] == 0x03) {
            /* Read Registers - o caso configurado */
            requested_address = MODBUS_GET_INT16_FROM_INT8(query,header_length+1);
            printf("CMD3 (Holding registers): requested_address = %d\n",requested_address);

            val_d.d = 0.2345;
            mb_mapping->tab_registers[0] = val_d.v[0];
            mb_mapping->tab_registers[1] = val_d.v[1];
            mb_mapping->tab_registers[2] = val_d.v[2];
            mb_mapping->tab_registers[3] = val_d.v[3];

            val_d.d = 15.4521;
            mb_mapping->tab_registers[4] = val_d.v[0];
            mb_mapping->tab_registers[5] = val_d.v[1];
            mb_mapping->tab_registers[6] = val_d.v[2];
            mb_mapping->tab_registers[7] = val_d.v[3];

            val_d.d = 7.1234;
            mb_mapping->tab_registers[8] = val_d.v[0];
            mb_mapping->tab_registers[9] = val_d.v[1];
            mb_mapping->tab_registers[10] = val_d.v[2];
            mb_mapping->tab_registers[11] = val_d.v[3];

            val_i.i = 128900;
            mb_mapping->tab_registers[12] = val_i.v[0];
            mb_mapping->tab_registers[13] = val_i.v[1];
        }

        if (query[header_length] == 0x04) {
            /* Input Registers - o caso configurado */
            requested_address = MODBUS_GET_INT16_FROM_INT8(query,header_length+1);
            printf("CMD4 (Input registers): requested_address = %d\n",requested_address);

            /* NS */
            val_i.i = 123456;
            mb_mapping->tab_input_registers[1] = val_i.v[0];
            mb_mapping->tab_input_registers[2] = val_i.v[1];

            /* U0 */
            modbus_set_float_abcd(15.89,&mb_mapping->tab_input_registers[3]);
            //val_f.d = 15.89;
            //mb_mapping->tab_input_registers[3] = val_f.v[0];
            //mb_mapping->tab_input_registers[4] = val_f.v[1];

            /* I0 */
            val_f.d = 25.89;
            mb_mapping->tab_input_registers[5] = val_f.v[0];
            mb_mapping->tab_input_registers[6] = val_f.v[1];

            /* FP */
            val_f.d = 150.89;
            mb_mapping->tab_input_registers[7] = val_f.v[0];
            mb_mapping->tab_input_registers[8] = val_f.v[1];

            /* S0 */
            val_f.d = 1115.89;
            mb_mapping->tab_input_registers[9] = val_f.v[0];
            mb_mapping->tab_input_registers[10] = val_f.v[1];

            /* Q0 */
            val_f.d = 190.89;
            mb_mapping->tab_input_registers[11] = val_f.v[0];
            mb_mapping->tab_input_registers[12] = val_f.v[1];

            /* P0 */
            val_f.d = 999.89;
            mb_mapping->tab_input_registers[13] = val_f.v[0];
            mb_mapping->tab_input_registers[14] = val_f.v[1];

            /* F */
            val_f.d = 1024.89;
            mb_mapping->tab_input_registers[15] = val_f.v[0];
            mb_mapping->tab_input_registers[16] = val_f.v[1];

            /* EA+ */
            val_f.d = 0.8999;
            mb_mapping->tab_input_registers[53] = val_f.v[0];
            mb_mapping->tab_input_registers[54] = val_f.v[1];

            /* ER+ */
            val_f.d = 1.8999;
            mb_mapping->tab_input_registers[55] = val_f.v[0];
            mb_mapping->tab_input_registers[56] = val_f.v[1];

            /* DA */
            val_f.d = 450.8999;
            mb_mapping->tab_input_registers[63] = val_f.v[0];
            mb_mapping->tab_input_registers[64] = val_f.v[1];
        }

        rc = modbus_reply(mbCtx, query, rc, mb_mapping);
        if (rc == -1) {
            break;
        }
    }

    spdlog::info("End the loop");

    modbus_mapping_free(mb_mapping);
    free(query);
    /* For RTU */
    modbus_close(mbCtx);
    modbus_free(mbCtx);

    spdlog::info("Ending stand-alone testing");

    return 0;
}
