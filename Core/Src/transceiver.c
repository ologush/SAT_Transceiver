#include "transceiver.h"
#include "adi_adf7030-1_reg.h"

static ADF7030_s transceiver;
static ADF7030_MACHINE_e transceiver_state;

static TRANSCEIVER_ERR_e SPI_txrx(ADF7030_s *target, uint8_t *tx, uint8_t *rx, uint16_t size);
static TRANSCEIVER_ERR_e SPI_tx(ADF7030_s *target, uint8_t *tx, uint16_t size);
static TRANSCEIVER_ERR_e SPI_host_initialization(ADF7030_s *target);

static TRANSCEIVER_ERR_e SPI_txrx(ADF7030_s *target, uint8_t *tx, uint8_t *rx, uint16_t size) {

    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(target->hspi, tx, rx, size, target->spi_timeout);
    HAL_GPIO_Write_Pin(target->cs_port, target->cs_pin, GPIO_PIN_SET);

}

static TRANSCEIVER_ERR_e SPI_tx(ADF7030_s *target, uint8_t *tx, uint16_t size) {

    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(target->hspi, tx, size, target->spi_timeout);
    HAL_GPIO_Write_Pin(target->cs_port, target->cs_pin, GPIO_PIN_SET);

}

static TRANSCEIVER_ERR_e SPI_host_initialization(ADF7030_s *target, uint32_t timeout_ms) {

    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_RESET);
    uint32_t start = HAL_GetTick();

    while (HAL_GPIO_ReadPin(target->miso_port, target->miso_pin) == GPIO_PIN_RESET) {

        if ((HAL_GetTick() - start) > timeout_ms) {
            return TRANSCEIVER_ERR_ERROR;
        }
    }

    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_SET);

}

TRANSCEIVER_ERR_e ADF7030_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, GPIO_TypeDef *rst_port, uint16_t rst_pin, GPIO_HandleTypeDef miso_port, uint16_t miso_pin) {

    transceiver.hspi = hspi;
    transceiver.cs_port = cs_port;
    transceiver.cs_pin = cs_pin;
    transceiver.rst_port = rst_port;
    transceiver.rst_pin = rst_pin;

    transceiver_state = ADF7030_PHY_OFF;

    if(SPI_host_initialization(&transceiver, 50) == TRANSCEIVER_ERR_ERROR) {
        return TRANSCEIVER_ERR_ERROR;
    }

#ifdef LOAD_CONFIG
    ADF7030_1_loadConfig();
#endif

}

#ifdef LOAD_CONFIG
const uint8_t Radio_Memory_Configuration[] = {
    #include "Settings_ADF7030-1.cfg"
}
TRANSCEIVER_ERR_e ADF7030_1_loadConfig(uint8_t config[], uint32_t len) {

    uint32_t i = 0;
    while (i <= len) {

        uint32_t block_len;

        block_len = (config[i] << 16) | (config[i + 1] << 8) | (config[i + 2]) - 3;

        if (block_len < 3) {
            return TRANSCEIVER_ERR_ERROR;
        }

        block_len -= 3;
        i += 3;

        if (i + block_len > len) {
            return TRANSCEIVER_ERR_ERROR;
        }

        SPI_tx(transceiver.hspi, &config[i], block_len);

        i += block_len;

    }

    return TRANSCEIVER_ERR_OK;
}
#endif

#ifdef CALIBRATE
const uint8_t Calibration[] = {
    #include "OffLineCalibrations.cfg"
}

const uint32_t cal_len = sizeof(Calibration);

TRANSCEIVER_ERR_e ADF7030_1_calibrate(void) {


    if (transceiver_state != ADF7030_PHY_OFF) {
        return TRANSCEIVER_ERR_ERROR;
    }


    ADF7030_1_loadConfiguration(Calibration, cal_len);
    ADF7030_memoryWrite(SM_DATA_CALIBRATION_Addr, 0x20002971);

    ADF7030_transitionState(ADF7030_CFG_DEV);
    
    //wait until it returns to PHY_OFF

    while(ADF7030_getTransitionStatus() != ADF7030_IDLE_IN_STATE);

    if(ADF7030_getState() != ADF7030_PHY_OFF) {
        return TRANSCEIVER_ERR_ERROR;
    }

    ADF7030_transitionState(ADF7030_PHY_ON);

    ADF7030_transitionState(ADF7030_DO_CAL);

    HAL_Delay(630);

    uint8_t reg_data[4] = {0};

    union {
        uint8_t arr[4];
        uint32_t word;
    } reg_data {.word = 0};

    ADF7030_memoryRead(PROFILE_RADIO_CAL_CFG1_Addr, reg_data);

    if(ADF7030_getState() != ADF7030_PHY_ON || (reg_data.word & 0x20000000) != 0x20000000) {
        return TRANSCEIVER_ERR_ERROR;
    }

    ADF7030_memoryWrite(SM_DATA_CALIBRATION_Addr, 0x20002A21)

    return TRANSCEIVER_ERR_OK;
}
#endif

TRANSCEIVER_ERR_e ADF7030_1_loadCalibration(void) {

    uint8_t calibrationSettings[] = {};

    //Command the ADF7030-1 into the PHY_OFF state
    
    //Load the configuration settings into the ADF7030-1 

    //Write the saved calibration results into the calibrations results registers 

    //Issue a CMD_CFG_DEV command

    return TRANSCEIVER_ERR_OK;
}

TRANSCEIVER_ERR_e ADF7030_1_getTemperature(float *temp) {

    //Host must enter the monitoring state

    //Retreive the TEMP_OUTPUT bits in the PROFILE_MONITOR1 register

    //Obtain the twos compliment of TEMP_OUTPUT

    //Multiply the TEMP_OUTPUT bits by .0625 deg C

    //The state machine returns to the PHY_ON state


    return TRANSCEIVER_ERR_OK;
}

TRANSCEIVER_ERR_e ADF7030_1_transmitPacket(uint8_t *packet) {

    //Host writes payload data into the ADF7030-1 TX payload buffer and configures programmable fields in the generic packet memory
    //The ADF7030-1 transmits a packet in response to a CMD_PHY_TX command
    return TRANSCEIVER_ERR_OK;
}

TRANSCEIVER_ERR_e ADF7030_1_receivePacket(uint8_t *packet) {

    //In preparation for receiving a packet, the host configures programmable fields in the generic packet memory region and issues a CMD_PHY_RX command.
    //The ADF7030-1 then enters the receive state, PHY_RX
    //If a preamble is detected, the preamble interrupt is set
    //If the preamble then stops being received, the preamble gone interrupt is set.
    //If subsequently the sync word specified in the packet configuration is detected, the ADF7030-1 proceeds to receive the payload and saves the payload of the packet in the RX payload buffer
    

    return TRANSCEIVER_ERR_OK;
}

TRANSCEIVER_ERR_e ADF7030_transitionState(ADF7030_STATE_MACHINE_e target_state) {

    uint8_t command = (1 << 7) | target_state;
    SPI_tx(&transceiver, &command, 1);

}

ADF7030_STATE_e ADF7030_getState() {
    
}

ADF7030_TRANSITION_STATUS_e ADF7030_getTransitionStatus() {

    uint8_t command = 0xFF;
    uint8_t status_byte = 0;
    SPI_txrx(&transceiver, &command, &status_byte, 1);

    ADF7030_TRANSITION_STATUS_e status = (status >> 1) & 0x03;

    return status;
}

ADF7030_READY_STATE_e ADF7030_getReadyState() {

    uint8_t command = 0xFF;
    uint8_t status_byte = 0;
    SPI_txrx(&transceiver, &command, &status_byte, 1);

    ADF7030_READY_STATE_e state = (status_byte >> 4) & 0x01;

    return state;
}

TRANSCEIVER_ERR_e ADF7030_memoryWrite(uint32_t address, uint32_t data) {
    
    uint8_t command = 0x38;

    uint8_t data_stream[9];
    data_stream[0] = command;
    data_stream[1] = (uint8_t) (address >> 12);
    data_stream[2] = (uint8_t) (address >> 8) & 0xF;
    data_stream[3] = (uint8_t) (address >> 4) & 0xF;
    data_stream[4] = (uint8_t) (address) & 0xF;

    data_stream[5] = (uint8_t) (data >> 12);
    data_stream[6] = (uint8_t) (data >> 8) & 0xF;
    data_stream[7] = (uint8_t) (data >> 4) & 0xF;
    data_stream[8] = (uint8_t) (data) & 0xF;

    SPI_tx(&transceiver, &data_stream, 9);

    return TRANSCEIVER_ERR_OK;
}

TRANSCEIVER_ERR_e ADF7030_memoryRead(uint32_t address, uint8_t data[4]) {

    uint8_t command = 0x78;

    uint8_t data_stream[11];
    data_stream[0] = command;
    data_stream[1] = (uint8_t) (address >> 12);
    data_stream[2] = (uint8_t) (address >> 8) & 0xF;
    data_stream[3] = (uint8_t) (address >> 4) & 0xF;
    data_stream[4] = (uint8_t) (address) & 0xF;
    
    data_stream[5] = 0;
    data_stream[6] = 0;
    data_stream[7] = 0;
    data_stream[8] = 0;
    data_stream[9] = 0;
    data_stream[10] = 0;
    data_stream[11] = 0;
    
    HAL_GPIO_WritePin(transceiver->cs_port, transceiver->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(transceiver->hspi, data_stream, 7, transceiver->spi_timeout);
    HAL_SPI_TransmitReceive(transceiver->hspi, data_stream + 7, data, 4, transceiver->spi_timeout);
    HAL_GPIO_Write_Pin(transceiver->cs_port, transceiver->cs_pin, GPIO_PIN_SET);

    return TRANSCEIVER_ERR_OK;
}