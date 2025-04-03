/***************************************************************************//**
 * \file APP_modbus.h
 *
 * \brief
 * This the header file of modbus application.
 *
 *******************************************************************************/

#if !defined( __INC_APP_MODBUS_H__ )
#define __INC_APP_MODBUS_H__

#if defined(__cplusplus)
extern "C" {
#endif
    
/*****************************************************************************
* includes
*****************************************************************************/
#include "modbus.h"
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/******************************************************************************
 * Macros
 *****************************************************************************/
/** APP Modbus data buffer length */    
#define APP_BUFFER_DATA_LEN                         ( 32 )
/** APP Modbus RX data buffer length */
#define APP_MODBUS_RX_BUFFER_LENGTH       			( APP_BUFFER_DATA_LEN )
/** APP Modbus TX data buffer length */    
#define APP_MODBUS_TX_BUFFER_LENGTH       			( APP_BUFFER_DATA_LEN )
/** APP Modbus command data length */    
#define APP_MODBUS_DATA_LEN                         ( 128 )
/** APP Modbus one byte data length */    
#define	APP_ONE_BYTE_VAL_LENGTH                     ( 0x01 )
/** APP Modbus two bytes data length */    
#define APP_TWO_BYTE_VAL_LENGTH                     ( 0x02 )
/** APP Modbus read data length */
#define APP_CMD_MODBUS_READ_LEN_VALUE               ( 0x08 )
/** APP Modbus reset value */    
#define APP_CMD_RESET_VALUE                         ( 0x00 )
/** APP Modbus read register starting address */
#define	APP_MODBUS_READ_NTC_REGISTER_ADD			( 01 )
/** APP Modbus holding register starting address */
#define APP_MODBUS_FAN_ON_WITH_LOW_RPM_REG_ADD		( 01 )
#define PWM_FREQUENCY (10000u)
/******************************************************************************
 * Typedefs
 *****************************************************************************/    
typedef  enum 
{
    APP_FAN_OFF,
    APP_FAN_ON,
    APP_FAN_MAX,
}tAPP_FAN_Mode;

typedef enum
{
    APP_LIGHT_SENSOR_OFF,
    APP_LIGHT_SENSOR_ON,
    APP_LIGHT_SENSOR_MAX,
}tAPP_Light_Sensor_mode;

typedef enum
{
    APP_MONO_STABLE_OFF,
    APP_MONO_STABLE_ON,
    APP_MONO_STABLE_MAX,
}tAPP_Mono_Stable_mode;

// Input Register index enum
typedef enum
{
	MBUS_READ_ADC_TEMP_VAL_ADDR = APP_MODBUS_READ_NTC_REGISTER_ADD,
    MBUS_FAN_STATUS_VAL_ADDR,
    MBUS_LIGTH_SENSOR_STATUS_VAL_ADDR,     
    MBUS_READ_LUX_RANGE_VAL_ADDR,

}tAPP_Modbus_Input_Reg_Address;

typedef enum
{
	MBUS_FAN_ON_OFF_VAL_ADDR = APP_MODBUS_FAN_ON_WITH_LOW_RPM_REG_ADD,
    MBUS_CONTROL_LIGH_SENSOR_VAL_ADDR,
    MBUS_MOTOR_DRIVER_TRIGGER_A_VAL_ADDR,
    MBUS_MOTOR_DRIVER_SET_DIR_VAL_ADDR,

}tAPP_Modbus_Holding_Reg_Address;

/******************************************************************************
 * Public definitions
 ******************************************************************************/

/******************************************************************************
 * Structures
 ******************************************************************************/
typedef struct
{
	uint8_t buffer[APP_MODBUS_DATA_LEN];
	uint8_t length;
	uint8_t pendingProcess;
}tAPP_Modbus_RX_TX_Data;

typedef struct
{
    uint16_t wFanStatus;
    uint16_t wLightSensorStatus;
    uint16_t wMonoStableControl;
    uint16_t wMotorDriverDirectionValue;
    uint16_t wLightSensorLuxRangevalue;
    int16_t lReadNTCTemperature;
    int16_t adc_v;
}tAPP_All_Mesaurement_Results;

typedef struct
{
    uint16_t wPrevFanStatus;
    uint16_t wPrevLightSensorStatus;
    uint16_t wPrevMonoStableControl;
    uint16_t wPrevMotorDriverDirectionValue;
}tAPP_Previous_Values;

/******************************************************************************
 * Externs
 ******************************************************************************/
extern modbusSlave_t tModbusSlave;
extern char abReceiveCmdData[APP_BUFFER_DATA_LEN];
extern char abSendResponseData[APP_BUFFER_DATA_LEN];
extern tAPP_All_Mesaurement_Results tMesurementsResult;

/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/

/**
 * \brief : This function is used to initialize the modbus salve.
 *
 * \param[in] *tModbusSlaveConfig		: Pointer of the modbus configuration.
 * \param[in] fpTransmitCallback		: Function callback of the transmit.
 * \return[out]                         : None
 *
 */
void vAPP_ModbusSlaveInit(modbusSlave_t *sSlave, cb_transmitPacket fpTransmitCallback);

/**
 * @} end of Utils_API group
 */



#if defined(__cplusplus)
}
#endif

#endif // __INC_APP_MODBUS_H__
