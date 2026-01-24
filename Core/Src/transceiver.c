#include "transceiver.h"
#include "adi_adf7030-1_reg.h"

ADF7030_s transceiver;
ADF7030_STATE_MACHINE_e transceiver_state;

static TRANSCEIVER_ERR_e SPI_txrx();
static TRANSCEIVER_ERR_e SPI_rx();

static TRANSCEIVER_ERR_e SPI_txrx(ADF7030_s *target, uint8_t *tx, uint8_t *rx, uint16_t size) {

    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(target->hspi, tx, rx, size, target->spi_timeout);
    HAL_GPIO_Write_Pin(target->cs_port, target->cs_pin, GPIO_PIN_SET);

}

static TRANSCEIVER_ERR_e SPI_tx(ADF7030_s *target, uint8_t *tx, uint8_t *rx, uint16_t size) {

    HAL_GPIO_WritePin(target->cs_port, target->cs_pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(target->hspi, tx, size, target->spi_timeout);
    HAL_GPIO_Write_Pin(target->cs_port, target->cs_pin, GPIO_PIN_SET);

}

TRANSCEIVER_ERR_e ADF7030_init(void) {
    ADI_ADF7030_1_RESULT eResult;

}

#ifdef LOAD_CONFIG
const uint8_t Radio_Memory_Configuration[] = {
    #include "Settings_ADF7030-1.cfg"
}
TRANSCEIVER_ERR_e ADF7030_1_loadConfig(void) {
    
    return TRANSCEIVER_ERR_OK;
}
#endif

#ifdef CALIBRATE
const uint8_t Calibration[] = {
    #include "OffLineCalibrations.cfg"
}
TRANSCEIVER_ERR_e ADF7030_1_calibrate(void) {

    //Write CAL_ENABLE (0x20002971) key to the SM_DATA_CALIBRATION register to enable calibration

    //Issue CMD_CFG_DEV configuration command

    //When the radio returns to the PHY_OFF state from the configuring state, issue the CMD_PHY_ON command to place the ADF7030-1 in the PHY_ON state

    //In the PHY_ON state issue the CMD_DO_CAL command. Typically takes 630 ms

    //On complettion, the radio autonomously returns to the PHY_ON state and the CAL_SUCCESS bit in the PROFILE_RADIO_CAL_CFG1 register is set to 1

    //Write the CAL_DISABLE (0x20002A21) key to SM_DATA_CALIBRATION

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