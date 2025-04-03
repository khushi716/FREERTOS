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
#include"adc_init.h"
#define MBUS_READ_ADC_VAL_ADDR 0
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
//static bool cbCmdFanOnOff( uint8_t *pData, uint8_t bDataLength, void *pvUserdata );

/** 
 * \brief : This function callback of the control the light sensor.
 *
 * \param [in] *pData           : Pointer of the data
 * \param [in] bDataLength      : Length of the data
 * \param [in] *pvUserdata      : pointer of the passing arguments
 *
 * \return boStatus             : Status of the command
 */
//static bool cbCmdControlLightSensor( uint8_t *pData, uint8_t bDataLength, void *pvUserdata );

/** 
 * \brief : This function callback of the control the mono stable.
 *
 * \param [in] *pData           : Pointer of the data
 * \param [in] bDataLength      : Length of the data
 * \param [in] *pvUserdata      : pointer of the passing arguments
 *
 * \return boStatus             : Status of the command
 */
//static bool cbCmdControlMonoStable( uint8_t *pData, uint8_t bDataLength, void *pvUserdata );

/** 
 * \brief : This function callback of set motor driver direction.
 *
 * \param [in] *pData           : Pointer of the data
 * \param [in] bDataLength      : Length of the data
 * \param [in] *pvUserdata      : pointer of the passing arguments
 *
 * \return boStatus             : Status of the command
 */
//static bool cbCmdSetMotorDriverDirection( uint8_t *pData, uint8_t bDataLength, void *pvUserdata );

/*****************************************************************************
 * Global Variable
 *****************************************************************************/
/** APP modbus slave initialization variable */
modbusSlave_t tModbusSlave;
/** APP modbus  slave all read write parameter structure type of variable */
extern tAPP_All_Mesaurement_Results tMesurementsResult;
/** APP modbus received command data buffer */
char abReceiveCmdData[APP_BUFFER_DATA_LEN];
/** APP modbus transfer response data buffer*/
char abSendResponseData[APP_BUFFER_DATA_LEN];
/** APP modbus salve received data buffer */
static uint8_t abAPPModbusRXBuffer[APP_MODBUS_RX_BUFFER_LENGTH];
/** APP modbus slave transfer data buffer */
static uint8_t abAPPModbusTXBuffer[APP_MODBUS_TX_BUFFER_LENGTH];
static bool cbCmdControlLightIntensity( uint8_t *pData, uint8_t bDataLength, void *pvUserdata );
/** Input register value respective Register index */
static regMap_t tInputRegMap[] = {
    
    { MBUS_READ_ADC_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.adc_v, NULL, NULL},

};

/** Holding register value respective Register index */
static regMap_t tHoldingRegMap[] = {
        
    { MBUS_READ_ADC_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.adc_v, cbCmdControlLightIntensity, &tMesurementsResult.adc_v },

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
 * Function Name: cbCmdControlLightIntensity
 *******************************************************************************
 * Summary:
 * This function is called when Modbus writes to the PWM register.
 * It adjusts the PWM duty cycle to control the LED brightness.
 *
 * Parameters:
 * uint8_t *pData         : Data received from the Modbus write command.
 * uint8_t bDataLength    : Length of the data received.
 * void *pvUserdata       : Pointer to the user data (PWM duty cycle).
 *
 * Return:
 *  boStatus              : Returns true if the operation is successful.
 *
 *******************************************************************************/
static bool cbCmdControlLightIntensity(uint8_t *pData, uint8_t bDataLength, void *pvUserdata)
{
    bool boStatus = false;
    uint8_t *pvData = pvUserdata;
    uint8_t pwm_control_value;
    pwm_control_value = modbusReadBE(pData);  // Read the Modbus value

    if ((APP_TWO_BYTE_VAL_LENGTH == bDataLength) && (pwm_control_value >= 0 && pwm_control_value <= 100))
    {
        // Update PWM duty cycle based on Modbus command
        tMesurementsResult.adc_v = pwm_control_value;

        // Set PWM duty cycle to the corresponding value (0-100)
        float duty_cycle = (float)pwm_control_value;
        cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
        if (result != CY_RSLT_SUCCESS)
        {
            printf("API cyhal_pwm_set_duty_cycle failed with error code: %ld\n", (long unsigned int)result);
        }

        // Print the updated PWM value
        printf("Updated PWM duty cycle: %d%%\r\n", pwm_control_value);
        boStatus = true;
    }
    return boStatus;
}








