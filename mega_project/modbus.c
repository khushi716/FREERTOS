/****************************************************************************
* File Name        : modbus.c
*
* Description      : This source file contains the modbus slave.
*
* Related Document : See README.md
*
*****************************************************************************/

/*****************************************************************************
 * includes
 *****************************************************************************/
#include "modbus.h"
#include <string.h>

/*****************************************************************************
 * Defines
 *****************************************************************************/

/*****************************************************************************
 * Typedefs
 *****************************************************************************/
/** Modbus protocol data unit */
typedef struct
{
	uint8_t func_code;
	uint8_t *data;
}ModbusPDU_t;

/** Modbus buffer */
typedef struct
{
	uint8_t slave_id;
	ModbusPDU_t modbusPDU;
	uint16_t pktCRC;
	uint16_t trasactionId;
	uint16_t protocolId;
	uint16_t length;
}ModbusBuffer_t;

/** Function pointer to Modbus request register */
typedef enumModbusRTUError (*ModbusRequestParsingFunction)(
		modbusSlave_t *status,
		uint8_t function,
		uint8_t *requestPDU,
		uint8_t requestLength);

/** Modbus slave functions */
typedef struct
{
	uint8_t fun_id;
	ModbusRequestParsingFunction funPtr;
} ModbusSlaveFunctionHandler;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/
/**
 * \brief : Safely reads a little-endian 16-bit word from provided pointer
 *
 * \param [in] pData    : Buffer pointer
 *
 * \retrun [out]        : 16 bit little-endian data word
 */
static uint16_t modbusReadLE(const uint8_t *pData);

/** 
 * \brief : Safely writes a little-endian 16-bit word to provided pointer
 *
 * \param [in] pData    : Buffer pointer
 * \param [in] val      :  Value that writes in little-endian
 *
 * \retrun [out]        : value of the little-endian
 */
static uint16_t modbusWriteLE(uint8_t *p, uint16_t val);

/** 
 * \brief : Retrun salve tx buffer response length
 *
 * \param [in] sSlave   : Modbus slave structure pointer
 *
 * \retrun [out]        : Tx buffer packet length
 */
static uint16_t modbusSlaveGetResponseLength(const modbusSlave_t *sSlave);

/** 
 * \brief : Calculate modbus crc 16 bit
 *
 * \param [in] nData    : Data buffer
 * \param [in] wLength  : Length if data
 *
 * \return [out]        : Calculated CRC of input data
 */
static uint16_t crc16 (const uint8_t *nData, uint16_t wLength);

/** 
 * \brief : Parse Modbus received frame and executes it
 *
 * \param [in] sSlave           : modbus slave structure pointer
 * \param [in] requestPDU       : Modbus pdu data buffer
 * \param [in] requestLenght    : length of pdu packet
 *
 * \return                      : Modbus RTU status error
 */
static enumModbusRTUError parseModbusRequest( modbusSlave_t *sSlave, ModbusPDU_t *requestPDU, uint16_t requestLenght);

/** 
 * \brief : Unpack received frame as per modbus packet structure
 *
 * \param [in] pframe           : Received data frame
 * \param [in] length           : Length of received data frame
 * \param [in] pRecvBuff [out]  : Unpacked modbus buffer structure
 * \param [in] pPduLength [out] : Modbus pdu length
 *
 * \return                      : Modbus RTU status error
 */
static enumModbusRTUError modbusUnpackRTU(uint8_t *pframe, uint16_t length, ModbusBuffer_t *pRecvBuff, uint16_t *pPduLength);


/** 
 * \brief : Unpack received frame as per modbus packet structure
 *
 * \param [in] pframe           : Received data frame
 * \param [in] length           : Length of received data frame
 * \param [in] pRecvBuff [out]  : Unpacked modbus buffer structure
 * \param [in] pPduLength [out] : Modbus pdu length
 *
 * \return                      : Modbus RTU status error
 */
static enumModbusRTUError modbusUnpackTCP(uint8_t *pframe, uint16_t length, ModbusBuffer_t *pRecvBuff, uint16_t *pPduLength);


/** 
 * \brief : Create Modbus response by adding slave Id and calculating CRC
 *
 *  \param [in] sSlave          : Modbus slave structure pointer
 *  \param [in] slaveId         : Salve ID
 *
 *  \return                     : None
 */
static void createModbusResponse(modbusSlave_t *sSlave, ModbusBuffer_t ReceBuff);

/** 
 * \brief : Create Modbus exception response with function code and exception code
 *
 *  \param [in] sSlave          : modbus slave structure pointer
 *  \param [in] function        : Function code
 *  \param [in] code            : Exception code
 *
 *  \return [out]               : Modbus RTU status error
 */
static enumModbusRTUError modbusBuildException(modbusSlave_t *sSlave, uint8_t function, enumModbusException code);

/** 
 * \brief : Allocate Response buffer with length of response
 *
 *  \param [in] sSlave          : Modbus slave structure pointer
 *  \param [in] size            : Length of response
 *
 *  \return [out]               : Modbus RTU status error
 */
static enumModbusRTUError allocateRespBuffer(modbusSlave_t *sSlave, uint16_t size);


/** 
 * \brief : Handle function code request Read Holding and Input register
 *
 *  \param [in] sSlave          : Modbus slave structure pointer
 *  \param [in] function        : Request function code
 *  \param [in] requestData     : Request data pointer
 *  \param [in] requestLength   : Request data length
 *
 *  \return [out] :             :Modbus RTU status error
 */
static enumModbusRTUError modbusParseRequest0304(modbusSlave_t *sSlave, uint8_t function, uint8_t *requestData, uint8_t requestLength);

/** 
 * \brief : Handle function code request Write single Holding register
 *
 *  \param [in] sSlave          : Modbus slave structure pointer
 *  \param [in] function        : Request function code
 *  \param [in] requestData     : Request data pointer
 *  \param [in] requestLength   : Request data length
 *
 *  \return [out]               : Modbus RTU status error
 */
static enumModbusRTUError modbusParseRequest06(modbusSlave_t *sSlave, uint8_t function, uint8_t *requestData, uint8_t requestLength);

/** 
 * \brief : Handle function code request Write multiple Holding register
 *
 *  \param [in] sSlave          : Modbus slave structure pointer
 *  \param [in] function        : Request function code
 *  \param [in] requestData     : Request data pointer
 *  \param [in] requestLength   : Request data length
 *
 *  \return [out]               : Modbus RTU status error
 */
static enumModbusRTUError modbusParseRequest16(modbusSlave_t *sSlave, uint8_t function, uint8_t *requestData, uint8_t requestLength);


/*****************************************************************************
 * Static variable
 *****************************************************************************/
/** Modbus slave function handler */
static ModbusSlaveFunctionHandler sModbusRequestfunc[] = {
		{
				.fun_id = 03,
				.funPtr = modbusParseRequest0304,
		},
		{
				.fun_id = 04,
				.funPtr = modbusParseRequest0304,
		},
		{
				.fun_id = 06,
				.funPtr = modbusParseRequest06,
		},
		{
				.fun_id = 16,
				.funPtr = modbusParseRequest16,
		}
};

/** CRC check table */
static const uint16_t wCRCTable[] = {
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };

/** Max transfer buffer data length */
static uint16_t maxTxBufferLen;
/** Max received buffer data length */
static uint16_t maxRxBufferLen;
/** Size of modbus request function array*/
static uint8_t requestFuncCnt = sizeof(sModbusRequestfunc) / sizeof(ModbusSlaveFunctionHandler);  
/*****************************************************************************
 * Global Variable
 *****************************************************************************/


/*******************************************************************************
 * Function Name: modbusReadLE 
 *******************************************************************************
 * Summary:
 * Safely reads a little-endian 16-bit word from provided pointer
 *
 * Parameters:
 *  pData                           : Buffer pointer
 *  uint16_t bUpdatedValue          : update value.
 *
 * Return:
 *  16 bit little-endian data word 
 *
 *******************************************************************************/
static uint16_t modbusReadLE(const uint8_t *p)
{
	uint8_t lo = *p;
	uint8_t hi = *(p + 1);
	return (uint16_t) lo | ((uint16_t) hi << 8);
}

/*******************************************************************************
 * Function Name: modbusWriteLE 
 *******************************************************************************
 * Summary:
 * Safely writes a little-endian 16-bit word to provided pointer
 *
 * Parameters:
 *  pData                           : Buffer pointer
 *  uint16_t bUpdatedValue          : update value.
 *
 * Return:
 *  16 bit little-endian data word 
 *
 *******************************************************************************/
static uint16_t modbusWriteLE(uint8_t *p, uint16_t val)
{
	*p = val & 0xff;
	*(p + 1) = val >> 8;
	return val;
}

/*******************************************************************************
 * Function Name: crc16 
 *******************************************************************************
 * 
 * Summary:
 * Calculate modbus crc 16 bit
 *
 * Parameters:
 *  nData    : Data buffer
 *  wLength  : Length if data
 *
 * Return:
 *  Calculated CRC of input data 
 *
 *******************************************************************************/
static uint16_t crc16 (const uint8_t *nData, uint16_t wLength)
{
	uint8_t  nTemp;
	uint16_t wCRCWord = 0xFFFF;

	while (wLength--)
	{
		nTemp = *nData++ ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord  ^= wCRCTable[(nTemp & 0xFF)];
	}
	return wCRCWord;
}

static enumModbusRTUError modbusUnpackRTU(uint8_t *frame, uint16_t length, ModbusBuffer_t *RecvBuff, uint16_t *pduLength)
{
	enumModbusRTUError err = MODBUS_OK;

	// Check Rx frame is grater then minimum modbus frame length
	if(length < MIN_RTU_SIZE || length > MAX_RTU_SIZE)
	{
		err = MODBUS_ERROR_LENGTH;
	}

	// Extract modbus frame data
	RecvBuff->slave_id = frame[0];
	RecvBuff->modbusPDU.func_code = frame[1];
	RecvBuff->modbusPDU.data = &frame[2];
	RecvBuff->pktCRC = modbusReadLE(frame + length - 2);

	// Modbus PDU frame length
	*pduLength = length - MODBUS_RTU_ADU_PADDING;

	return err;
}



static enumModbusRTUError modbusUnpackTCP(uint8_t *frame, uint16_t length, ModbusBuffer_t *RecvBuff, uint16_t *pduLength)
{
	enumModbusRTUError err = MODBUS_OK;

	// Check Rx frame is grater then minimum modbus frame length
	if(length < MIN_RTU_SIZE || length > MAX_RTU_SIZE)
	{
		err = MODBUS_ERROR_LENGTH;
	}

	// Extract modbus frame data
	RecvBuff->slave_id = frame[6];
	RecvBuff->modbusPDU.func_code = frame[7];
	RecvBuff->modbusPDU.data = &frame[8];

	RecvBuff-> trasactionId =  (frame[0] << 8 | frame[1]) ;
	RecvBuff-> protocolId =  (frame[2] << 8 | frame[3]) ;
	*pduLength = (frame[4] << 8 | frame[5]) - 1 ;

	return err;
}


static void createModbusResponse(modbusSlave_t *sSlave, ModbusBuffer_t ReceBuff)
{

	if( PACKET_TCP_IP == sSlave->mbusPacketTyp)
	{
		uint8_t buff[512];
		sSlave->pTxBuffer[0] = ReceBuff.slave_id;
		memcpy(buff,&sSlave->pTxBuffer[0],sSlave->lengthTxBuffer);

		// add transaction ID in response
		sSlave->pTxBuffer[0] = ReceBuff.trasactionId >> 8;
		sSlave->pTxBuffer[1] = ReceBuff.trasactionId & 0xff;

		// add protocol ID in response
		sSlave->pTxBuffer[2] = ReceBuff.protocolId >> 8;
		sSlave->pTxBuffer[3] =  ReceBuff.protocolId & 0xff;

		// add length in response
		sSlave->pTxBuffer[4] = sSlave->lengthTxBuffer >> 8;
		sSlave->pTxBuffer[5] = sSlave->lengthTxBuffer & 0xff;

		memcpy(&sSlave->pTxBuffer[6],buff, sSlave->lengthTxBuffer);
		sSlave->lengthTxBuffer += 6;
	}
	else if(PACKET_RTU == sSlave->mbusPacketTyp)
	{
		uint16_t claCRC;
		sSlave->pTxBuffer[0] = ReceBuff.slave_id;
		// Calculate CRC
		claCRC = crc16(sSlave->pTxBuffer, sSlave->lengthTxBuffer);

		modbusWriteLE(sSlave->pTxBuffer + sSlave->lengthTxBuffer , claCRC);
		sSlave->lengthTxBuffer += 2;
	}

}

static uint16_t modbusSlaveGetResponseLength(const modbusSlave_t *sSlave)
{
	return sSlave->lengthRxBuffer;
}

static enumModbusRTUError modbusBuildException(modbusSlave_t *sSlave, uint8_t function, enumModbusException code)
{
	enumModbusRTUError err = MODBUS_OK;
	// Allocate exception response buffer length
	err = allocateRespBuffer(sSlave, 2);

	if (err == MODBUS_OK)
	{
		sSlave->pTxBuffer[1] = function | 0x80;
		sSlave->pTxBuffer[2] = code;
	}

	return err;
}

static enumModbusRTUError allocateRespBuffer(modbusSlave_t *sSlave, uint16_t size)
{
	if (size != 0)
	{
		// Check size of frame is not more then max PDU size
		if((size < MAX_PDU_SIZE) && (size < (maxTxBufferLen - MODBUS_RTU_ADU_PADDING)))
		{
			memset(sSlave->pTxBuffer, 0, sizeof(sSlave->pTxBuffer[0])*(size + 3) );
			sSlave->lengthTxBuffer = size + 1;
		}
		else
		{
			sSlave->lengthTxBuffer = 0;
			return MODBUS_ERROR_ALLOC;
		}
	}
	else
	{
		sSlave->lengthTxBuffer = 0;
		return MODBUS_OK;
	}

	return MODBUS_OK;
}

/*
 * Parse modbus packet and call respective modbus function code function
 */
static enumModbusRTUError parseModbusRequest( modbusSlave_t *sSlave, ModbusPDU_t *requestPDU, uint16_t requestLenght)
{
	// Get function code
	uint8_t funcCode = requestPDU->func_code;
	enumModbusRTUError error = MODBUS_ERROR_FUNCTION;

	for (int i = 0; i < requestFuncCnt; i++)
	{
		// Call appropriate function request
		if (sModbusRequestfunc[i].fun_id == funcCode)
		{
			error = sModbusRequestfunc[i].funPtr(sSlave, funcCode, requestPDU->data, --requestLenght);
			break;
		}
	}

	// if function code is not implemented the rerun Exception response function error
	if (MODBUS_ERROR_FUNCTION == error)
	{
		error = modbusBuildException(sSlave, funcCode, ModbusException_ILLEGAL_FUNCTION);
	}

	return error;
}

static enumModbusRTUError modbusParseRequest0304(modbusSlave_t *sSlave, uint8_t function, uint8_t *requestData, uint8_t requestLength)
{
	// get address
	uint16_t atualAddress = modbusReadBE(requestData) + REGISTER_ADDRESS_OFFSET;
	// No of register Read
	uint16_t noOfRegCnt = modbusReadBE(&requestData[2]);
	// Response Byte Count
	uint8_t byteCount =  noOfRegCnt << 1;
	uint16_t respPduLength = 2 + byteCount;


	regMap_t *sReadRegister = NULL;
	uint16_t regOffsetCnt;
	uint16_t regCnt;
	uint16_t lengthofReg = 0;
	uint16_t nextAddr;
	uint16_t startAddrIndex = -1;
	enumModbusRTUError error = MODBUS_OK;

	do
	{
//		Check minimum frame length
		if (requestLength != 4)
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_VALUE);
			break;
		}

		// Check no of register count
		if (noOfRegCnt == 0 || noOfRegCnt > 125)
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_VALUE);
			break;
		}

		// Check function code is Holding or Input
		if (READ_INPUT_REGISTER_FUN_CODE == function)
		{
			if(sSlave->pInputRegMap != NULL)
			{
				sReadRegister = sSlave->pInputRegMap;
				lengthofReg = sSlave->inputRegMapElement;
			}
			else
			{
				error = MODBUS_ERROR_ADDRESS;
				break;
			}
		}
		else
		{
			if(sSlave->pHoldignRegMap != NULL)
			{
				sReadRegister = sSlave->pHoldignRegMap;
				lengthofReg = sSlave->holdingRegMapElement;
			}
			else
			{
				error = MODBUS_ERROR_ADDRESS;
				break;
			}
		}

		// Check Address and no of read data is valid or not
		for(int i = 0; i < lengthofReg; i++)
		{
			if( (atualAddress >= sReadRegister[i].startAddress) && (atualAddress < (sReadRegister[i].startAddress + sReadRegister[i].registerCount)))
			{
				startAddrIndex = i;
				regOffsetCnt = atualAddress - sReadRegister[i].startAddress;
				regCnt = sReadRegister[i].registerCount - regOffsetCnt;
				nextAddr = atualAddress + regCnt;

				if (regCnt >= noOfRegCnt)
				{
					break;
				}
				for(int j = ++i; j < lengthofReg; j++)
				{
					if (nextAddr == sReadRegister[j].startAddress)
					{
						regCnt += sReadRegister[j].registerCount;
						nextAddr += sReadRegister[j].registerCount;
					}
					else
					{
						error = MODBUS_ERROR_VALUE;
						break;
					}

					if (regCnt >= noOfRegCnt)
					{
						break;
					}
				}

				if(MODBUS_ERROR_VALUE == error || regCnt < noOfRegCnt)
				{
					modbusBuildException(sSlave, function, ModbusException_ILLEGAL_ADDRESS);
					break;
				}
				else if(regCnt >= noOfRegCnt)
				{
					break;
				}
			}
		}

		// Read address is not found
		if ((uint16_t)(-1) == startAddrIndex)
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_ADDRESS);
			break;
		}

		// if all data is valid then allocate response data PDU length
		if (MODBUS_OK == error)
		{
			// allocate response buffer
			error = allocateRespBuffer(sSlave, respPduLength);

			sSlave->pTxBuffer[1] = function;
			sSlave->pTxBuffer[2] = byteCount;
		}

		// Read Register data and fill the tx buffer
		if (MODBUS_OK == error)
		{
			uint8_t index = 0;
			uint16_t noReg = noOfRegCnt;
			uint8_t *pValue;
			uint16_t regVal;
			uint8_t valLSB, valMSB;
			for (int i = startAddrIndex; i < lengthofReg  && (noReg!= 0); i++)
			{
			    if(READ_INPUT_REGISTER_FUN_CODE == function)
			    {
			        if(sReadRegister[i].cb_onWrite)
			            sReadRegister[i].cb_onWrite(sReadRegister[i].pVarbleMemory, sReadRegister[i].registerCount*2, sReadRegister[i].pUserData);
			    }

			    for (int j = regOffsetCnt; (j < sReadRegister[i].registerCount) && (noReg!= 0); j++, --noReg)
			    {
			        pValue = (uint8_t *)sReadRegister[i].pVarbleMemory;
			        valLSB = pValue[j*2];
			        valMSB = pValue[(j*2) + 1];
			        regVal = valMSB;
			        regVal = (regVal << 8) | valLSB;
			        modbusWriteBE(&sSlave->pTxBuffer[(index++) *2 + 3], regVal);
			    }
			    regOffsetCnt = 0;
			}
		}

	}while(0);

	return error;
}

static enumModbusRTUError modbusParseRequest06(modbusSlave_t *sSlave, uint8_t function, uint8_t *requestData, uint8_t requestLength)
{
	// get address
	uint16_t Address;
	uint16_t respPduLength = 5;

	regMap_t *HoldRegInfo;
	uint16_t addrIndex = -1;
	enumModbusRTUError error = MODBUS_OK;

	do
	{
		// Check minimum frame length
		if (requestLength != 4)
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_VALUE);
			break;
		}

		// Check holding register map not null
		if(sSlave->pHoldignRegMap == NULL)
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_ADDRESS);
			break;
		}
		else
		{
			HoldRegInfo = sSlave->pHoldignRegMap;
		}

		// Starting address
		Address = modbusReadBE(requestData);

		// Check Address is available in register map and Value is writable in that address
		for(int i = 0; i < sSlave->holdingRegMapElement; i++)
		{
			if((REGISTER_ADDRESS_OFFSET + Address) == HoldRegInfo[i].startAddress)
			{
				addrIndex = i;
				if (HoldRegInfo[i].registerCount > 1)
				{
					error = MODBUS_ERROR_ADDRESS;
				}
				break;
			}
		}

		// Address is invalid
		if ((MODBUS_ERROR_ADDRESS == error) || ((uint16_t)(-1) == addrIndex))
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_ADDRESS);
			break;
		}

		bool writeStatus = false;

		// Check write callback is available and write register
		if(HoldRegInfo[addrIndex].cb_onWrite != NULL)
		{
			writeStatus = HoldRegInfo[addrIndex].cb_onWrite(&requestData[2], HoldRegInfo[addrIndex].registerCount * 2, HoldRegInfo[addrIndex].pUserData);
			if ( !writeStatus )
			{
				modbusBuildException(sSlave, function, ModbusException_ILLEGAL_VALUE);
				break;
			}
		}

		// allocate response buffer
		if (writeStatus)
		{
			error = allocateRespBuffer(sSlave, respPduLength);
			if (MODBUS_OK == error)
			{
				sSlave->pTxBuffer[1] = function;
				modbusWriteBE(&sSlave->pTxBuffer[2], Address);
				sSlave->pTxBuffer[4] =  requestData[2];
				sSlave->pTxBuffer[5] =  requestData[3];
			}
			else
			{
				modbusBuildException(sSlave, function, ModbusException_SLAVE_FAILURE);
				break;
			}
		}

	}while(0);

	return error;
}

static enumModbusRTUError modbusParseRequest16(modbusSlave_t *sSlave, uint8_t function, uint8_t *requestData, uint8_t requestLength)
{
	// get address
	uint16_t Address;
	// No of register Read
	uint16_t noOfRegCnt;
	uint8_t byteCount;
	uint16_t respPduLength = 5;

	regMap_t *HoldRegInfo;
	uint16_t RegAddress;
	uint16_t startAddrIndex = -1;
	enumModbusRTUError error = MODBUS_OK;

	do
	{
		// Check minimum frame length
		if (requestLength < 7)
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_VALUE);
			break;
		}

		// Check holding register map not null
		if(sSlave->pHoldignRegMap == NULL)
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_ADDRESS);
			break;
		}
		else
		{
			HoldRegInfo = sSlave->pHoldignRegMap;
		}

		// Starting address, Register count and data byte count
		Address = modbusReadBE(requestData);
		noOfRegCnt = modbusReadBE(&requestData[2]);
		byteCount = requestData[4];

		// Check for Register count and data byte count is match
		if (noOfRegCnt != (byteCount >> 1))
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_VALUE);
			break;
		}

		// Check Address is available in register map and values is writable in that address
		for(int i = 0; i < sSlave->holdingRegMapElement; i++)
		{
			if((REGISTER_ADDRESS_OFFSET + Address) == HoldRegInfo[i].startAddress)
			{
				RegAddress = HoldRegInfo[i].startAddress;
				startAddrIndex = i;
				for(int j = 0; (j < noOfRegCnt) && (i < sSlave->holdingRegMapElement); i++)
				{
					if((RegAddress + j) != HoldRegInfo[i].startAddress)
					{
						error = MODBUS_ERROR_ADDRESS;
						break;
					}
					else
					{
						j += HoldRegInfo[i].registerCount;
						byteCount -= HoldRegInfo[i].registerCount * 2;
					}
				}
				break;
			}
		}

		// Address is invalid
		if ((MODBUS_ERROR_ADDRESS == error) || ((uint16_t)(-1) == startAddrIndex))
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_ADDRESS);
			break;
		}
		else if (byteCount != 0)
		{
			modbusBuildException(sSlave, function, ModbusException_ILLEGAL_VALUE);
			break;
		}

		// Check write callback is available and write register
		bool writeStatus = false;
		for (int i = startAddrIndex, j = 0; j < noOfRegCnt; i++)
		{
			if(HoldRegInfo[i].cb_onWrite != NULL)
			{
				writeStatus = HoldRegInfo[i].cb_onWrite(&requestData[5 + (j*2)], HoldRegInfo[i].registerCount * 2, HoldRegInfo[i].pUserData);
				j += HoldRegInfo[i].registerCount;
				if ( !writeStatus )
				{
					modbusBuildException(sSlave, function, ModbusException_ILLEGAL_VALUE);
					break;
				}
			}
		}

		// allocate response buffer
		if (writeStatus)
		{
			error = allocateRespBuffer(sSlave, respPduLength);
			if (MODBUS_OK == error)
			{
				sSlave->pTxBuffer[1] = function;
				modbusWriteBE(&sSlave->pTxBuffer[2], Address);
				modbusWriteBE(&sSlave->pTxBuffer[4], noOfRegCnt);

			}
			else
			{
				modbusBuildException(sSlave, function, ModbusException_SLAVE_FAILURE);
				break;
			}
		}

	}while(0);

	return error;
}


/************************************************************************
 * GLOBAL FUNCTIONS
 ************************************************************************/
enumModbusRTUError modbusBufferInit(modbusSlave_t *psSlave, uint8_t *pTxBuff, uint16_t lengthTxBuff, uint8_t *pRxBuff, uint16_t lengthRxBuff)
{
	enumModbusRTUError err = MODBUS_OK;

	if( 0 == lengthRxBuff || 0 == lengthTxBuff)
	{
		err = MODBUS_ERROR_ALLOC;
	}
	else
	{
		psSlave->pTxBuffer = pTxBuff;
		psSlave->pRxBuffer = pRxBuff;
		psSlave->lengthTxBuffer = 0;
		psSlave->lengthRxBuffer = 0;
		psSlave->packetAvailbleForProcessing = false;

		maxTxBufferLen = lengthTxBuff;
		maxRxBufferLen = lengthRxBuff;
	}

	return err;
}

enumModbusRTUError modbusSlaveSetMapHoldingReg(modbusSlave_t *pMbusSlave, regMap_t *pMap, uint16_t countsInMap)
{
	pMbusSlave->pHoldignRegMap = pMap;
	pMbusSlave->holdingRegMapElement = countsInMap;

	return MODBUS_OK;
}

enumModbusRTUError modbusSlaveSetMapInputReg(modbusSlave_t *pMbusSlave, regMap_t *pMap, uint16_t countsInMap)
{
	pMbusSlave->pInputRegMap = pMap;
	pMbusSlave->inputRegMapElement = countsInMap;

	return MODBUS_OK;
}

enumModbusRTUError modbusRegisterTransmitCallback(modbusSlave_t *psSlave, cb_transmitPacket cbTransmit)
{
	enumModbusRTUError err = MODBUS_OK;

	if (NULL == cbTransmit)
	{
		psSlave->cbTxPacket = NULL;
		err = MODBUS_ERROR_OTHER;
	}
	else
	{
		psSlave->cbTxPacket = cbTransmit;
	}

	return err;
}

enumModbusRTUError modbusSlaveOnRxPacket(modbusSlave_t *pMbusSlave, enumModbusPktType packetType, uint8_t *pData, uint16_t length)
{
	enumModbusRTUError err = MODBUS_OK;

	if(NULL == pMbusSlave->pRxBuffer && NULL == pData)
	{
		err = MODBUS_ERROR_OTHER;
	}
	else if(length > maxRxBufferLen)
	{
		err = MODBUS_ERROR_LENGTH;
	}
	else
	{
		memcpy(pMbusSlave->pRxBuffer, pData, length);
		pMbusSlave->mbusPacketTyp = packetType;
		pMbusSlave->lengthRxBuffer = length;
		pMbusSlave->packetAvailbleForProcessing = true;
	}

	return err;
}

enumModbusRTUError modbusProcess(modbusSlave_t *psSlave)
{
	if(psSlave->packetAvailbleForProcessing)
	{
		do
		{
			uint8_t *pRecvData = psSlave->pRxBuffer;
			uint16_t ModbusPduLen;
			enumModbusRTUError err = MODBUS_ERROR_OTHER;
			ModbusBuffer_t ReceBuff;

			// clear packet receive flag
			psSlave->packetAvailbleForProcessing = false;

			if(PACKET_RTU == psSlave->mbusPacketTyp)
			{
				err = modbusUnpackRTU(pRecvData, psSlave->lengthRxBuffer, &ReceBuff, &ModbusPduLen);

			}
			else if(PACKET_TCP_IP == psSlave->mbusPacketTyp)
			{
				err = modbusUnpackTCP(pRecvData, psSlave->lengthRxBuffer, &ReceBuff, &ModbusPduLen);
			}

			if (err != MODBUS_OK)
			{
				break;
			}

			// Check address is match with slave address
			if ((ReceBuff.slave_id != MODBUS_SLAVE_ID) && (ReceBuff.slave_id != 0))
			{
				break;
			}

			if(PACKET_RTU == psSlave->mbusPacketTyp)
			{
				//calculate CRC and check CRC
				if(ReceBuff.pktCRC != crc16(pRecvData, (psSlave->lengthRxBuffer - 2)))
				{
					// Send Invalid CRC response
					modbusBuildException(psSlave, ReceBuff.modbusPDU.func_code, ModbusException_CRC_FAILURE);

				}
				else
				{
					// if CRC match then parse modbus request
					err = parseModbusRequest(psSlave, &ReceBuff.modbusPDU, ModbusPduLen);
				}
			}
			else if(PACKET_TCP_IP == psSlave->mbusPacketTyp)
			{
				err = parseModbusRequest(psSlave, &ReceBuff.modbusPDU, ModbusPduLen);
			}

			// Check response length if greater then zero the send response
			if(modbusSlaveGetResponseLength(psSlave))
			{
				if(ReceBuff.slave_id != 0 && psSlave->cbTxPacket != NULL)
				{
					createModbusResponse(psSlave, ReceBuff);
					psSlave->cbTxPacket(psSlave->pTxBuffer, psSlave->lengthTxBuffer, psSlave->mbusPacketTyp);
				}
			}

		}while(0);
	}

	return MODBUS_OK;
}

uint16_t modbusReadBE(const uint8_t *p)
{
	uint8_t hi = *p;
	uint8_t lo = *(p + 1);
	return (uint16_t) lo | ((uint16_t) hi << 8);
}

uint16_t modbusWriteBE(uint8_t *p, uint16_t val)
{
	*p = val >> 8;
	*(p + 1) = val & 0xff;
	return val;
}

float modbusU8ToFloat(const uint8_t *a)
{
	u32Reg_def value1;

	value1.u8[0] = a[1];
	value1.u8[1] = a[0];
	value1.u8[2] = a[3];
	value1.u8[3] = a[2];

	return  value1.fVal;
}

uint32_t modbusU8ToU32(const uint8_t *a)
{
	u32Reg_def value1;

	value1.u8[0] = a[1];
	value1.u8[1] = a[0];
	value1.u8[2] = a[3];
	value1.u8[3] = a[2];

	return  value1.u32;
}
uint16_t modbusFloatToU16(float val, enumByteOrder order)
{
	u32Reg_def MB_Value;

	MB_Value.fVal = val;

	if(order == LSB)
		return MB_Value.u16[0];
	else
		return MB_Value.u16[1];
}

