/****************************************************************************
* File Name        : APP_modbus_port.c
*
* Description      : This source file contains of the Application layer function
*                    of the modbus port
*
* Related Document : See README.md
*
*****************************************************************************/

/*****************************************************************************
 * includes
 *****************************************************************************/

#include <stdint.h>
#include <string.h>
#include "modbus.h"
#include "APP_modbus.h"

/*****************************************************************************
 * Defines
 *****************************************************************************/

/*****************************************************************************
 * Typedefs
 *****************************************************************************/

/*****************************************************************************
 * Global Variable
 *****************************************************************************/
/** APP modbus port transmit packet */
static tAPP_Modbus_RX_TX_Data tModbusTxPkt;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/


/*******************************************************************************
 * Function Name: vAPP_ModbusPaketReceived
 *******************************************************************************
 * Summary:
 * This function is used when the any packet received on the modbus.
 *
 * Parameters:
 * modbusSlave_t *pMbusSlave	: Pointer of the modbus slave.
 * void *pDataBuff				: Received data buffer of the modbus slave.
 * uint16_t wDataLen			: Data length.
 *
 * Return:
 *  None
 *
 *******************************************************************************/

void vAPP_ModbusPaketReceived( modbusSlave_t *pMbusSlave, void *pDataBuff, uint8_t wDataLen )
{
	modbusSlaveOnRxPacket(pMbusSlave, PACKET_RTU, (uint8_t*)pDataBuff, wDataLen);
}

/*******************************************************************************
 * Function Name: ulAPP_ModbusTrasmitPacket
 *******************************************************************************
 * Summary:
 * This function is used when the any packet transmit on the modbus.
 *
 * Parameters:
 * uint8_t *pTransmitData			: Buffer data that need to transfer on modbus
 * uint16_t wDataLength             : Data length
 * enumModbusPktType ePacketType	: Packet type
 *
 * Return:
 *  uint32_t status
 *
 *******************************************************************************/



uint32_t ulAPP_ModbusTrasmitPacket( uint8_t *pTransmitData, uint8_t wDataLength, enumModbusPktType ePacketType )
{
	uint8_t bLenToWrite;
	// clear the transfer buffer
	memset(&tModbusTxPkt, 0x00, sizeof(tAPP_Modbus_RX_TX_Data));
	// Copy data into the transfer packet
	memcpy(tModbusTxPkt.buffer, pTransmitData, wDataLength);
	tModbusTxPkt.length = wDataLength;
	memcpy(abSendResponseData, tModbusTxPkt.buffer, tModbusTxPkt.length);
//    if( uart_is_tx_ready() )
//    {
        for( bLenToWrite = APP_CMD_RESET_VALUE; bLenToWrite < tModbusTxPkt.length; bLenToWrite ++ )
        {
        	( abSendResponseData[bLenToWrite]);
        }

        memset( abSendResponseData, APP_CMD_RESET_VALUE, APP_BUFFER_DATA_LEN);
//    }

	return 0;
}

/* [] END OF FILE */
