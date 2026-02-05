#include "transceiver.h"
#include "adi_adf7030-1_reg.h"
#include "gpio.h"


static ADF7030_s transceiver;
static ADF7030_STATE_e transceiver_state;

static TRANSCEIVER_ERR_e SPI_txrx(ADF7030_s *target, uint8_t *tx, uint8_t *rx, uint16_t size);
static TRANSCEIVER_ERR_e SPI_tx(ADF7030_s *target, uint8_t *tx, uint16_t size);
static TRANSCEIVER_ERR_e SPI_host_initialization(ADF7030_s *target, uint32_t timeout_ms);

static TRANSCEIVER_ERR_e SPI_txrx(ADF7030_s *target, uint8_t *tx, uint8_t *rx, uint16_t size) {

    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(target->hspi, tx, rx, size, target->spi_timeout);
    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_SET);

    return TRANSCEIVER_ERR_OK;

}

static TRANSCEIVER_ERR_e SPI_tx(ADF7030_s *target, uint8_t *tx, uint16_t size) {

    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(target->hspi, tx, size, target->spi_timeout);
    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_SET);

    return TRANSCEIVER_ERR_OK;
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

TRANSCEIVER_ERR_e ADF7030_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, GPIO_TypeDef *rst_port, uint16_t rst_pin, GPIO_TypeDef *miso_port, uint16_t miso_pin) {

    transceiver.hspi = hspi;
    transceiver.cs_port = cs_port;
    transceiver.cs_pin = cs_pin;
    transceiver.rst_port = rst_port;
    transceiver.rst_pin = rst_pin;
    transceiver.miso_port = miso_port;
    transceiver.miso_pin = miso_pin;

    transceiver_state = ADF7030_PHY_OFF;

    if(SPI_host_initialization(&transceiver, 50) == TRANSCEIVER_ERR_ERROR) {
        return TRANSCEIVER_ERR_ERROR;
    }

#ifdef LOAD_CONFIG
    ADF7030_1_loadConfig();
#endif

    // Set GPIO 4 as an output interrupt

    union {
        uint32_t word;
        uint8_t arr[4];
    } data;

    //Set GPIO 4 to IRQ0 output
    data.word = 0x00000006;
    ADF7030_memoryWrite(PROFILE_GPCON4_7_Addr, data.arr, 4);

    //Set GPIO 1 to output to set the RF switch
    data.word = 0x00001900;
    ADF7030_memoryWrite(PROFILE_GPCON0_3_Addr, data.arr, 4);
    return TRANSCEIVER_ERR_OK;

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

    union {
        uint8_t arr[4];
        uint32_t word;
    } reg_data;

    reg_data.word = 0x20002971;

    ADF7030_1_loadConfiguration(Calibration, cal_len);
    ADF7030_memoryWrite(SM_DATA_CALIBRATION_Addr, reg_data.arr, 4);

    ADF7030_transitionState(ADF7030_CFG_DEV);
    
    while(ADF7030_getTransitionStatus() != ADF7030_IDLE_IN_STATE);

    if(ADF7030_getState() != ADF7030_PHY_OFF) {
        return TRANSCEIVER_ERR_ERROR;
    }

    ADF7030_transitionState(ADF7030_PHY_ON);

    ADF7030_transitionState(ADF7030_DO_CAL);

    HAL_Delay(630);



    ADF7030_memoryRead(PROFILE_RADIO_CAL_CFG1_Addr, reg_data.arr);

    if(ADF7030_getState() != ADF7030_PHY_ON || (reg_data.word & 0x20000000) != 0x20000000) {
        return TRANSCEIVER_ERR_ERROR;
    }

    reg_data.word = 0x20002A21;

    ADF7030_memoryWrite(SM_DATA_CALIBRATION_Addr, reg_data.arr, 4);

    return TRANSCEIVER_ERR_OK;
}
#endif

#ifdef LOAD_CALIBRATION

const uint32_t Calibration[] = {
    #include "Calibration.cfg"
}

const uint32_t cal_len = sizeof(Calibration);

TRANSCEIVER_ERR_e ADF7030_1_loadCalibration(void) {

    ADF7030_transmitionState(ADF7030_PHY_OFF);

    //May need to change how this is written depending on how it is stored
    for(uint8_t i = 0; i < cal_len / 2; i++) {
        union {
            uint32_t word;
            uint8_t arr[4];
        } reg_data;

        reg_data.word = Calibration[i + 1];
        ADF7030_memoryWrite(Calibration[i], reg_data.arr, 4);
    }

    ADF7030_transitionState(ADF7030_CFG_DEV);

    return TRANSCEIVER_ERR_OK;
}
#endif

TRANSCEIVER_ERR_e ADF7030_getTemperature(float *temp) {

    ADF7030_transitionState(ADF7030_MON);

    union {
        uint8_t arr[4];
        uint32_t u_word;
        int32_t word;
    } temp_data;

    ADF7030_memoryRead(PROFILE_MONITOR1_Addr, temp_data.arr, 4);

    temp_data.word = temp_data.u_word & 0x00000FFF;

    if(temp_data.u_word > 0x800) {
        temp_data.word = (int32_t) temp_data.u_word - 0x1000;
    }

    *temp = 0.0625f * (float) temp_data.word;

    return TRANSCEIVER_ERR_OK;
}

TRANSCEIVER_ERR_e ADF7030_transmitPacket(data_packet_s *packet) {

    union {
        uint32_t word;
        uint8_t arr[4];
    } reg_data;
    reg_data.word = 0x00000002;

    // Set GPIO 1 low to put the RF switch in TX mode
    ADF7030_memoryWrite(0x40000820UL, reg_data.arr, 4);

    union {
        data_packet_s packet;
        uint8_t arr[130];
    } tx_data;

    tx_data.packet = *packet;

    uint8_t command = 0x38;

    HAL_GPIO_WritePin(transceiver.cs_port, transceiver.cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(transceiver.hspi, &command, 1, transceiver.spi_timeout);
    HAL_SPI_Transmit(transceiver.hspi, tx_data.arr, 130, transceiver.spi_timeout);
    HAL_GPIO_WritePin(transceiver.cs_port, transceiver.cs_pin, GPIO_PIN_SET);

    ADF7030_transitionState(ADF7030_PHY_TX);

    // Switch the RF switch back to RX mode

    reg_data.word = 0x00000002;
    ADF7030_memoryWrite(0x4000081CUL, reg_data.arr, 4);

    return TRANSCEIVER_ERR_OK;
}

TRANSCEIVER_ERR_e ADF7030_radioSettings() {

    union {
        uint8_t arr[4];
        uint32_t word;
    } reg_data_u;

    ADF7030_memoryRead(GENERIC_PKT_FRAME_CFG0_Addr, reg_data_u.arr, 4);

    //32 bit sync, CRC len is 8
    reg_data_u.word = (reg_data_u.word & 0xC0C0FFFF) | 0x08200000;
    ADF7030_memoryWrite(GENERIC_PKT_FRAME_CFG0_Addr, reg_data_u.arr, 4);
    reg_data_u.word = 0xF0F0F0F0;
    ADF7030_memoryWrite(GENERIC_PKT_SYNCWORD0_Addr, reg_data_u.arr, 4);


    ADF7030_memoryRead(GENERIC_PKT_FRAME_CFG2_Addr, reg_data_u.arr, 4);
    //Len set to 8 bits, need to add the ENDEC_MODE, CRC_SHIFT_IN_ZEROS is set to 1
    reg_data_u.word = (reg_data_u.word & 0x00FFCFFF) | 0x01001FFF;
    ADF7030_memoryWrite(GENERIC_PKT_FRAME_CFG2_Addr, reg_data_u.arr, 4);


    ADF7030_memoryRead(GENERIC_PKT_FRAME_CFG1_Addr, reg_data_u.arr, 4);
    //Enable IRQ1 when full packet has been rx or tx, no irq0, no length setting
    reg_data_u.word = (reg_data_u.word & 0x0000F000) | 0x80000080;
    ADF7030_memoryWrite(GENERIC_PKT_FRAME_CFG1_Addr, reg_data_u.arr, 4);

    //Set the CRC seed as 0xAA
    reg_data_u.word = 0x000000AA;
    ADF7030_memoryWrite(GENERIC_PKT_CRC_SEED_Addr, reg_data_u.arr, 4);  

    //Set the CRC polynomial as 0xFF
    reg_data_u.word = 0x000000FF;
    ADF7030_memoryWrite(GENERIC_PKT_CRC_POLY_Addr, reg_data_u.arr, 4);

    //Final XOR of the CRC calculation
    reg_data_u.word = 0x000000FF;
    ADF7030_memoryWrite(GENERIC_PKT_CRC_FINAL_XOR_Addr, reg_data_u.arr, 4);

    //TX base offset pointer: 0, RX base offset pointer: 512
    ADF7030_memoryRead(GENERIC_PKT_BUFF_CFG0_Addr, reg_data_u.arr, 4);
    reg_data_u.word = (reg_data_u.word & 0xFE800000) | 0x00000080;
    ADF7030_memoryWrite(GENERIC_PKT_BUFF_CFG0_Addr, reg_data_u.arr, 4);


    ADF7030_memoryRead(GENERIC_PKT_BUFF_CFG1_Addr, reg_data_u.arr, 4);
    //TX_SIZE 256, RX_SIZE 256, autoturnaround TX to RX
    reg_data_u.word = (reg_data_u.word & 0x54000000) | 0xD0020100;
    ADF7030_memoryWrite(GENERIC_PKT_BUFF_CFG1_Addr, reg_data_u.arr, 4);

    return TRANSCEIVER_ERR_OK;

}

TRANSCEIVER_ERR_e ADF7030_receivePacket(data_packet_s *packet) {

    union {
        data_packet_s *rx_packet;
        uint8_t arr[130];
    } rx_data;

    rx_data.rx_packet = packet;
    ADF7030_memoryRead(RX_PACKET_MEMORY, rx_data.arr, 130);

    union {
        uint8_t arr[4];
        uint32_t word;
    } reg_data_u;

    // Clear the IRQ0 flag for packet received
    ADF7030_memoryRead(IRQ_CTRL_STATUS0_Addr, reg_data_u.arr, 4);
    reg_data_u.word = reg_data_u.word | 0x00000080;
    ADF7030_memoryWrite(IRQ_CTRL_STATUS0_Addr, reg_data_u.arr, 4);

    return TRANSCEIVER_ERR_OK;
}

TRANSCEIVER_ERR_e ADF7030_transitionState(ADF7030_STATE_e target_state) {

    uint8_t command = (1 << 7) | target_state;
    SPI_tx(&transceiver, &command, 1);

    return TRANSCEIVER_ERR_OK;

}

ADF7030_STATE_e ADF7030_getState() {
    
}

ADF7030_TRANSITION_STATUS_e ADF7030_getTransitionStatus() {

    uint8_t command = 0xFF;
    uint8_t status_byte = 0;
    SPI_txrx(&transceiver, &command, &status_byte, 1);

    ADF7030_TRANSITION_STATUS_e status = (status_byte >> 1) & 0x03;

    return status;
}

ADF7030_READY_STATE_e ADF7030_getReadyState() {

    uint8_t command = 0xFF;
    uint8_t status_byte = 0;
    SPI_txrx(&transceiver, &command, &status_byte, 1);

    ADF7030_READY_STATE_e state = (status_byte >> 4) & 0x01;

    return state;
}

TRANSCEIVER_ERR_e ADF7030_memoryWrite(uint32_t address, uint8_t *data, uint32_t nbytes) {
    
    uint8_t command = 0x38;

    union {
        uint32_t word;
        uint8_t arr[4];
    } addr;

    addr.word = address;

    HAL_GPIO_WritePin(transceiver.cs_port, transceiver.cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(transceiver.hspi, &command, 1, transceiver.spi_timeout);
    HAL_SPI_Transmit(transceiver.hspi, addr.arr, 4, transceiver.spi_timeout);
    HAL_SPI_Transmit(transceiver.hspi, data, nbytes, transceiver.spi_timeout);
    HAL_GPIO_WritePin(transceiver.cs_port, transceiver.cs_pin, GPIO_PIN_SET);

    return TRANSCEIVER_ERR_OK;

}

TRANSCEIVER_ERR_e ADF7030_memoryRead(uint32_t address, uint8_t *data, uint32_t nbytes) {

    uint8_t command = 0x78;

    union {
        uint32_t word;
        uint8_t arr[4];
    } addr;

    addr.word = address;
    
    HAL_GPIO_WritePin(transceiver.cs_port, transceiver.cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(transceiver.hspi, &command, 1, transceiver.spi_timeout);
    HAL_SPI_Transmit(transceiver.hspi, addr.arr, 6, transceiver.spi_timeout); //Adding the extra two bytes to transmit nonsense so that we can start receiving the data on the TransmitReceive call
    HAL_SPI_Receive(transceiver.hspi, data, nbytes, transceiver.spi_timeout);
    HAL_GPIO_WritePin(transceiver.cs_port, transceiver.cs_pin, GPIO_PIN_SET);

    return TRANSCEIVER_ERR_OK;

}