///***************************************************************************//**
// * \file APP_modbus_port.h
// *
// * \brief
// * This the header file of application layer of the modbus port.
// *
// *******************************************************************************/
//
//#if !defined( __INC_APP_MODBUS_PORT_H__ )
//#define __INC_APP_MODBUS_PORT_H__
//
//#if defined(__cplusplus)
//extern "C" {
//#endif
//
//
///*****************************************************************************
// * includes
// *****************************************************************************/
//#include <string.h>
//#include <stdbool.h>
//
///******************************************************************************
// * Macros
// *****************************************************************************/
//
///******************************************************************************
//* Typedefs
//*****************************************************************************/
//
///******************************************************************************
// * Public definitions
// ******************************************************************************/
//
///******************************************************************************
// * Structures
// ******************************************************************************/
//
///******************************************************************************
// * Externs
// ******************************************************************************/
//
///*******************************************************************************
// * Function Prototypes
// *******************************************************************************/
//
///**
// * \brief : This function is used when the any packet received on the modbus.
// *
// * \param[in] modbusSlave_t *pMbusSlave   : Pointer of the modbus slave.
// * \param[in] void *pDataBuff             : Received data buffer of the modbus slave.
// * \param[in] uint16_t wDataLen           : Data length.
// * \return[out]                           : None
// *
// */
//void vAPP_ModbusPaketReceived( modbusSlave_t *pMbusSlave, void *pDataBuff, uint16_t wDataLen );
//
///**
// * \brief : This function is used when the any packet transmit on the modbus.
// *
// * \param[in] uint8_t *pTransmitData            : Buffer data that need to transfer on modbus
// * \param[in] uint16_t wDataLength              : Data length.
// * \param[in] enumModbusPktType ePacketType     : Packet type.
// * \return[out]  uint32_t Status                : Status of the packet
// *
// */
//uint32_t ulAPP_ModbusTrasmitPacket( uint8_t *pTransmitData, uint16_t wDataLength, enumModbusPktType ePacketType );
//
//
///**
// * @} end of Utils_API group
// */
//
//
//
//#if defined(__cplusplus)
//}
//#endif
//
//#endif // __INC_APP_MODBUS_PORT_H__
