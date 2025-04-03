/***************************************************************************//**
 * \file modbus.h
 *
 * \brief
 * This the header file of modbus slave driver.
 *
 *******************************************************************************/

#if !defined(___INC_APP_MODBUS_H___)
#define ___INC_APP_MODBUS_H___


#if defined(__cplusplus)
extern "C" {
#endif

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
/******************************************************************************
 * Macros
 *****************************************************************************/
/** modbus slave as tcp ip */
#define MODBUS_TCP_IP
/** Modbus slave as a rtu */
//#define MODBUS_RTU
/** Modbus minimum length of RTU */
#define MIN_RTU_SIZE					( 4 )
/** Modbus maximum length of RTU */
#define MAX_RTU_SIZE 					( 256 )
/** Modbus PDU length */
#define MAX_PDU_SIZE                    ( 253 )
/** Modbus PDU offset of RTU */
#define MODBUS_RTU_PDU_OFFSET			( 1 )
/** Modbus ADU padding of RTU */
#define MODBUS_RTU_ADU_PADDING			( 3 )
/** Modbus slave id */
#define MODBUS_SLAVE_ID					( 1 )
/** Modbus read input register code */
#define READ_INPUT_REGISTER_FUN_CODE    ( 04 )
/** Modbus holding register code*/
#define READ_HOLDING_REGISTER_FUN_CODE  ( 03 )
/** Modbus address offset */
#define REGISTER_ADDRESS_OFFSET         ( 1 )

/******************************************************************************
 * Typedefs
 *****************************************************************************/
typedef enum _enumByteOrder_
{
	LSB,
	MSB
}enumByteOrder;

typedef enum _enumPacketType_
{
	PACKET_RTU = 0,
	PACKET_TCP_IP,
}enumModbusPktType;

// Modbus exception
typedef enum _enumModbusException_
{
    ModbusException_NONE = 0,
    ModbusException_ILLEGAL_FUNCTION = 1, //!< Illegal function code
    ModbusException_ILLEGAL_ADDRESS = 2,  //!< Illegal data address
    ModbusException_ILLEGAL_VALUE = 3,    //!< Illegal data value
    ModbusException_SLAVE_FAILURE = 4,    //!< Slave could not process the request
    ModbusException_ACK = 5,              //!< Acknowledge
    ModbusException_NACK = 7,             //!< Negative acknowledge
    ModbusException_CRC_FAILURE = 0x0C,   //!< Modbus CRC Failure
}enumModbusException;

// Modbus error
typedef enum _modbus_rtu_err_
{
    MODBUS_OK = 0,                  //!< No error. When everything is fine.
    MODBUS_ERROR_LENGTH,            //!< Invalid frame length
    MODBUS_ERROR_FUNCTION,          //!< Invalid function code
    MODBUS_ERROR_COUNT,             //!< Invalid register count
    MODBUS_ERROR_INDEX,             //!< Invalid index value
    MODBUS_ERROR_VALUE,             //!< Invalid register value
    MODBUS_ERROR_RANGE,             //!< Invalid register range
    MODBUS_ERROR_CRC,               //!< CRC invalid
    MODBUS_ERROR_ADDRESS,           //!< Invalid register address
    MODBUS_ERROR_ALLOC,             //!< Memory allocation error
    MODBUS_ERROR_OTHER,             //!< Other error
}enumModbusRTUError;

/**
 * 32 bit modbus register union typedef
 */
typedef union u32Reg
{
    float fVal;
    uint32_t u32;
    uint16_t u16[2];
    uint8_t u8[4];
}u32Reg_def;

/** Callback function. Send Response frame over serial
 *
 * \param[in] pdata[in] : Response buffer pointer
 * \param[in] datalength[in]: length of data in Response buffer
 * \param[in] packetTyp[in]: Modbus packet type TCP/IP or Serial RTU
 *
 * \return Transmit packet status
 */
typedef uint32_t (*cb_transmitPacket)(uint8_t *pdata, uint16_t datalength, enumModbusPktType packetTyp);


/******************************************************************************
 * Public definitions
 ******************************************************************************/

/******************************************************************************
 * Structures
 ******************************************************************************/
/*
 * Register map structure
 */
typedef struct _regMap_
{
    uint16_t startAddress;                                  /* Starting address of register */
    uint8_t  registerCount;                                 /* No of register used */
    void * pVarbleMemory;                                   /* Pointer to register memory */
    bool (*cb_onWrite)(uint8_t *pData, uint8_t length, void *pUserData);     /* Write callback function pointer */
    void *pUserData;

}regMap_t;

/*
 *  Modbus slave structure
 */
typedef struct _modbusSlave_def_
{
    uint8_t *pTxBuffer;                 /* Transmit buffer pointer */
    uint16_t lengthTxBuffer;            /* Length of transmit packet or Buffer length */
    uint8_t *pRxBuffer;                 /* Receive buffer pointer */
    uint16_t lengthRxBuffer;            /* Receive packet length or Buffer length */

    regMap_t *pHoldignRegMap;           /* Holding register map pointer */
    uint16_t holdingRegMapElement;      /* No of element in register map */

    regMap_t *pInputRegMap;             /* Input register map pointer */
    uint16_t inputRegMapElement;        /* No of element in register map */

    bool packetAvailbleForProcessing;   /* Packet available flag. True when available otherwise false */

    enumModbusPktType mbusPacketTyp;	/* Modbus packet type TCP/IP or serial RTU */
    cb_transmitPacket cbTxPacket;       /* Packet transmit callback function */
}modbusSlave_t;

/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/

/**
 * \brief : Initialize salve holding register map
 *
 * \param[in] pMbusSlave        : Slave structure pointer
 * \param[in] pMap              : Pointer to the holding register map
 * \param[in] countsInMap       : length of holding register map
 *
 * \return                      : Modbus error status
 */
enumModbusRTUError modbusSlaveSetMapHoldingReg(modbusSlave_t *pMbusSlave, regMap_t *pMap, uint16_t countsInMap);

/**
 * \brief : Initialize salve input register map
 *
 * \param[in] pMbusSlave        : Slave structure pointer
 * \param[in] pMap              : Pointer to the input register map
 * \param[in] countsInMap       : length of input register map
 *
 * \return                      : Modbus error status
 */
enumModbusRTUError modbusSlaveSetMapInputReg(modbusSlave_t *pMbusSlave, regMap_t *pMap, uint16_t countsInMap);

/** Process Modbus received packet when available and send appropriate response
 *
 * \param[in] pMbusSlave : Slave structure pointer
 *
 * \return Modbus error status
 */
enumModbusRTUError modbusProcess(modbusSlave_t *pMbusSlave);

/**
 * \brief : Copy received packet in slave RX buffer and set packet available flag
 *
 * \param[in] pMbusSlave        : Slave structure pointer
 * \param[in] packetType        : Modbus packet type TCP/IP or Serial RTU
 * \param[in] pData             : Received Rx buffer pointer
 * \param[in] length            : Length of received packet length
 *
 * \return                      : Modbus status
 */
enumModbusRTUError modbusSlaveOnRxPacket(modbusSlave_t *pMbusSlave, enumModbusPktType packetType, uint8_t *pData, uint16_t length);

/**
 * \brief : Initialize buffer in modbus slave structure
 *
 *  \param[in] pMbusSlave       : Slave structure pointer
 *  \param[in] pTxBuff          : Tx buffer array pointer
 *  \param[in] lengthTxBuff     : Length if Tx buffer array
 *  \param[in] pRxBuff          : Rx buffer array pointer
 *  \param[in] lengthBuff       : Length if Rx buffer array
 *
 *  \return                     : Modbus error status
 */
enumModbusRTUError modbusBufferInit(modbusSlave_t *psSlave, uint8_t *pTxBuff, uint16_t lengthTxBuff, uint8_t *pRxBuff, uint16_t lengthBuff);

/**
 * \brief : Register Slave structure transmit callback of Transmit UART function
 *
 * \param[in] pMbusSlave        : Slave structure pointer
 * \param[in] cbTransmit        : function pointer of modbus transmit function
 *
 * @retrun                      : Modbus success status
 */
enumModbusRTUError modbusRegisterTransmitCallback(modbusSlave_t *psSlave, cb_transmitPacket cbTransmit);



/**
 * \brief : Safely reads a Big-endian 16-bit word from provided pointer
 *
 * \param[in] p                 : Buffer data
 *
 * @retrun                      : Big-endian of 2 byte data buffer
 */
uint16_t modbusReadBE(const uint8_t *p);

/**
 * \brief : Safely write a Big-endian 16-bit word from provided pointer
 *
 * \param[in] p [out]           : Buffer in data write
 * \param[in] val               : Word data
 *
 * @retrun val                  : Word data
 */
uint16_t modbusWriteBE(uint8_t *p, uint16_t val);

/**
 * \brief : Convert uint8 4 bytes to float value
 *
 * \param[in] a                 : pointer to bytes
 *
 * \return                      : float value of 4 bytes
 */
float modbusU8ToFloat(const uint8_t *a);

/**
 *  \brief : Convert uint8 4 bytes to uint32 value
 *
 * \param[in] a                 : pointer to bytes
 *
 * \return                      : uint32 value of 4 bytes
 */
uint32_t modbusU8ToU32(const uint8_t *a);

/**
 * \brief : Return uint16  MSB or LSB of float value
 *
 * \param[in] val               : float value
 * \param[in] order             : LSB or MSB
 *
 * \return                      : uint16 byte of float value
 */
uint16_t modbusFloatToU16(float val, enumByteOrder order);



/**
 * @} end of Utils_API group
 */

#endif /* ___INC_APP_MODBUS_H___ */
