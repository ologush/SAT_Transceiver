#include "transceiver.h"



#include "adi_adf7030-1_reg.h"
#include "adf7030-1__common.h"
#include "adf7030-1__core.h"
#include "adf7030-1__irq.h"
#include "adf7030-1__spi.h"
#include "adf7030-1__mem.h"
#include "adf7030-1__patch.h"
#include "adf7030-1__cfg.h"
#include "adf7030-1__state.h"
#include "adf7030-1__gpio.h"

#include "adf703x_utils.h"

uint8_t deviceMem[ADI_ADF7030_1_MEMORY_SIZE];

ADI_ADF7030_1_HANDLE hADF7030_1;

static void ADF7030_1_Callback(void *pCBParam, uint32_t Event, void *pArg) {

}

TRANSCEIVER_ERR_e transceiver_init(void) {
    ADI_ADF7030_1_RESULT eResult;

    eResult = adf7030_1__Open(ADF7030_1_INSTANCE_NUM,
                                   deviceMem,
                                   ADI_ADF7030_1_MEMORY_SIZE,
                                   ADF7030_1_Callback,
                                   NULL,
                                   &hADF7030_1));

    if (eResult != ADI_ADF7030_1_SUCCESS) {
        return TRANSCEIVER_ERR_ERROR;
    }

    eResult = adf7030_1__Setup(&hADF7030_1,
                                );

    eResult = adf7030_1__Enable();
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