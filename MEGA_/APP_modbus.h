/*****************************************************************************
* File Name        : APP_modbus.h
*
* Description      : Header file for the Modbus slave application layer.
*
*****************************************************************************/

#ifndef APP_MODBUS_H
#define APP_MODBUS_H

#include "modbus.h"
#include "cyhal_adc.h"  // Include for ADC handling

/*****************************************************************************
 * Constants
 *****************************************************************************/
#define ADC_CHANNEL (P10_0)  // ADC pin for input (adjust to your setup)
#define VPLUS_CHANNEL_0  (P10_0) // ADC input pin for potentiometer

#define PWM_FREQUENCY (10000u)  // PWM Frequency (10kHz)
#define PWM_DUTY_CYCLE (30.0f)  // PWM Duty cycle (30%)

#define APP_BUFFER_DATA_LEN 64   // Data buffer length (adjust as needed)
#define APP_MODBUS_RX_BUFFER_LENGTH 128  // Modbus RX buffer size
#define APP_MODBUS_TX_BUFFER_LENGTH 128  // Modbus TX buffer size

/*****************************************************************************
 * Typedefs
 *****************************************************************************/
typedef struct
{
	uint8_t buffer[APP_BUFFER_DATA_LEN];
	uint8_t length;
	uint8_t pendingProcess;
}tAPP_Modbus_RX_TX_Data;
typedef struct {
    uint16_t adc_v;  // ADC value in millivolts
} tAPP_All_Mesaurement_Results;

/*****************************************************************************
 * Global Variables
 *****************************************************************************/
extern tAPP_All_Mesaurement_Results tMesurementsResult;
extern uint8_t abAPPModbusRXBuffer[APP_MODBUS_RX_BUFFER_LENGTH];
extern uint8_t abAPPModbusTXBuffer[APP_MODBUS_TX_BUFFER_LENGTH];

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

/* Modbus Slave Initialization */
void vAPP_ModbusSlaveInit(modbusSlave_t *tModbusSlaveConfig, cb_transmitPacket fpTransmitCallback);

/* ADC Initialization */
void adc_single_channel_init(void);
void adc_single_channel_process(void);

/* PWM Initialization */
void pwm_init(void);

/* Modbus Callback */
bool cbCmdReadadc(uint8_t *pData, uint8_t bDataLength, void *pvUserdata);

#endif /* APP_MODBUS_H */
