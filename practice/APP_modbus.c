/****************************************************************************
* File Name        : APP_modbus.c
*
* Description      : This source file contains of the application layer of the
*                    modbus slave.
*
* Related Document : See README.md
*
*****************************************************************************/

/*****************************************************************************
 * includes
 *****************************************************************************/
#include "modbus.h"
#include "APP_modbus.h"
/*****************************************************************************
 * Defines
 *****************************************************************************/

/*****************************************************************************
 * Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/**
 * \brief : This function callback of the turn on/off fan.
 *
 * \param [in] *pData           : Pointer of the data
 * \param [in] bDataLength      : Length of the data
 * \param [in] *pvUserdata      : pointer of the passing arguments
 *
 * \return boStatus             : Status of the command
 */

/*****************************************************************************
 * Global Variable
 *****************************************************************************/
/** APP modbus slave initialization variable */
modbusSlave_t tModbusSlave;
/** APP modbus  slave all read write parameter structure type of variable */
tAPP_All_Mesaurement_Results tMesurementsResult;
/** APP modbus received command data buffer */
char abReceiveCmdData[APP_BUFFER_DATA_LEN];
/** APP modbus transfer response data buffer*/
char abSendResponseData[APP_BUFFER_DATA_LEN];
/** APP modbus salve received data buffer */
static uint8_t abAPPModbusRXBuffer[APP_MODBUS_RX_BUFFER_LENGTH];
/** APP modbus slave transfer data buffer */
static uint8_t abAPPModbusTXBuffer[APP_MODBUS_TX_BUFFER_LENGTH];


/** Input register value respective Register index */
static regMap_t tInputRegMap[] = {
    // Existing entries...
    { MBUS_ACCEL_X_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.wAccelX, NULL, NULL},
    { MBUS_ACCEL_Y_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.wAccelY,  NULL, NULL},
    { MBUS_ACCEL_Z_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.wAccelZ,  NULL, NULL},
};

/** Holding register value respective Register index */
/** Holding register value respective Register index */
static regMap_t tHoldingRegMap[] = {
    // Existing entries...
		 { MBUS_ACCEL_X_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.wAccelX, cbCmdReadAccelX, tMesurementsResult.wAccelX },
		    { MBUS_ACCEL_Y_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.wAccelY, cbCmdReadAccelY, tMesurementsResult.wAccelY },
		    { MBUS_ACCEL_Z_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.wAccelZ, cbCmdReadAccelZ,tMesurementsResult.wAccelZ },
};

/*******************************************************************************
 * Function Name: vAPP_ModbusSlaveInit
 *******************************************************************************
 * Summary:
 * This function initialize the modbus salve.
 *
 * Parameters:
 * modbusSlave_t *tModbusSlaveConfig		: Pointer of the modbus configuration.
 * cb_transmitPacket fpTransmitCallback     : Function callback of the transmit.
 *
 * Return:
 *  None
 *
 *******************************************************************************/
void vAPP_ModbusSlaveInit(modbusSlave_t *tModbusSlaveConfig, cb_transmitPacket fpTransmitCallback)
{
	uint16_t wHoldingRegMapSize = sizeof(tHoldingRegMap) / sizeof(regMap_t);
	uint16_t wInputRegMapSize = sizeof(tInputRegMap) / sizeof(regMap_t);

    /* Initialization modbus buffer */
	modbusBufferInit(tModbusSlaveConfig, abAPPModbusTXBuffer, APP_MODBUS_TX_BUFFER_LENGTH, abAPPModbusRXBuffer, APP_MODBUS_RX_BUFFER_LENGTH);
    /* Register the modbus holding and input register */
	modbusSlaveSetMapHoldingReg(tModbusSlaveConfig, tHoldingRegMap, wHoldingRegMapSize);
	modbusSlaveSetMapInputReg(tModbusSlaveConfig, tInputRegMap, wInputRegMapSize);
    /* Register modbus transmit callback*/
	modbusRegisterTransmitCallback(tModbusSlaveConfig, fpTransmitCallback);
}

/*******************************************************************************
 * Function Name: cbCmdFanOnOff
 *******************************************************************************
 * Summary:
 * This function callback of the turn on/off fan.
 *
 * Parameters:
 * uint8_t *pData				: Pointer of the data
 * uint8_t bDataLength			: Length of the data
 * void *pvUserdata             : pointer of the passing arguments
 *
 * Return:
 * bool boStatus                : Status of the command
 *
 *******************************************************************************/

static bool cbCmdReadAccelX(uint8_t *pData, uint8_t bDataLength, void *pvUserdata)
{
	 bool boStatus = false;
		uint8_t bFanRPMValue;
		bFanRPMValue = modbusReadBE( pData );

    if(){
    	boStatus = true;
    	        tMesurementsResult.wAccelX = bFanRPMValue;
    }
    return boStatus;

}

// Callback function to read accelerometer's Y-axis data
static bool cbCmdReadAccelY(uint8_t *pData, uint8_t bDataLength, void *pvUserdata)
{
	bool boStatus = false;
			uint8_t bFanRPMValue;
			bFanRPMValue = modbusReadBE( pData );

	    if(){
	    	boStatus = true;
	    	        tMesurementsResult.wAccelY = bFanRPMValue;
	    }
	    return boStatus;
}

// Callback function to read accelerometer's Z-axis data
static bool cbCmdReadAccelZ(uint8_t *pData, uint8_t bDataLength, void *pvUserdata)
{
	bool boStatus = false;
			uint8_t bFanRPMValue;
			bFanRPMValue = modbusReadBE( pData );

	    if(){
	    	boStatus = true;
	    	        tMesurementsResult.wAccelZ = bFanRPMValue;
	    }
	    return boStatus;
}

//// Callback function to set accelerometer's range
//static bool cbCmdSetAccelRange(uint8_t *pData, uint8_t bDataLength, void *pvUserdata)
//{
//    bool boStatus = false;
//    if (bDataLength == 1)
//    {
//        uint8_t range = pData[0];
//        // Set accelerometer range
//        Accelerometer_SetRange(range);
//        boStatus = true;
//    }
//    return boStatus;
//}
//
//// Callback function to enable or disable accelerometer
//static bool cbCmdEnableAccel(uint8_t *pData, uint8_t bDataLength, void *pvUserdata)
//{
//    bool boStatus = false;
//    if (bDataLength == 1)
//    {
//        uint8_t enable = pData[0];
//        if (enable)
//        {
//            // Enable accelerometer
//            Accelerometer_Enable();
//        }
//        else
//        {
//            // Disable accelerometer
//            Accelerometer_Disable();
//        }
//        boStatus = true;
//    }
//    return boStatus;
//}
