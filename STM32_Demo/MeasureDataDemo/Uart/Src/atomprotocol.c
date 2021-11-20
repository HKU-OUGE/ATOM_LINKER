/******************** (C) COPYRIGHT 2021 SINGER *********************
* File Name          : ATOM_PROTOCOL.h
* Author             : SINGER
* Date First Issued  : 2021
* Description        : 
********************************************************************************
* History:

* 2021: V1.0

*******************************************************************************/

#include "stdlib.h"
#include "string.h"

#include "atom_macro.h"
#include "atomprotocol.h"
#include "global.h"
extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart1;
u8 priBuf[PRINT_SIZE] = { '\0' };
u8 g=0;


extern void commandAckHandle(unsigned char classID, unsigned char mID, unsigned char result, unsigned char* pData, unsigned short dataLen);

u8  Atom_BCC(u8 *addr, u16 len)
{
	unsigned char *DataPoint;
	DataPoint = addr;
	unsigned char XorData = 0;
	unsigned short DataLength = len;

	while (DataLength--)
	{
		XorData = XorData ^ *DataPoint;
		DataPoint++;
	}

	return XorData;
}

u16  Atom_CRC16(u8 *pdata, u32 length)
{
	u16 wcrc = 0xFFFF;
	u8 temp;
	u32 i = 0, j = 0;
	for (i = 0; i < length; i++)
	{
		temp = *pdata & 0x00FF;
		pdata++;
		wcrc ^= temp;
		for (j = 0; j < 8; j++)
		{
			if (wcrc & 0x0001)
			{
				wcrc >>= 1;
				wcrc ^= 0xA001;
			}
			else
			{
				wcrc >>= 1;
			}
		}
	}
	return wcrc;
}

int  AtomCmd_Compose_Send(u8 MADDR, u8 ClassID, u8 msgID, u8* payloadBuffer, u16 PayloadLen)
{
       // MX_USART6_UART_Init();
	int index = 0;
	int ret = 0;
	u8 crc;
       // u8 ret ;

	unsigned char *pBuf = (unsigned char*)malloc(ATOM_CMDLEN_NO_PAYLOAD + PayloadLen);

	//Header
	pBuf[index++] = 'A';
	pBuf[index++] = 'x';
	pBuf[index++] = MADDR;
	pBuf[index++] = ClassID;
	pBuf[index++] = msgID;
	pBuf[index++] = PayloadLen & 0xff;
	//Payload
	
	for (int i = 0; i < PayloadLen; i++)
		pBuf[index++] = payloadBuffer[i];
	
	
	//BCC
	crc = Atom_BCC((u8*)pBuf, ATOM_HEADER_LEN + PayloadLen);
	pBuf[index++] = crc & 0xff;

	//Footer
	pBuf[index++] = 'm';
        
   
 	HAL_UART_Transmit_IT(&huart1, pBuf, index);
       
        
	return ret;
}




int  checkCIDValidation(u8 classID)
{
	int ret = 0;
	if (!((classID == 0x01) || (classID == 0x02) \
		|| (classID == 0x03) || (classID == 0x04) \
		|| (classID == 0x05) || (classID == 0x06) \
		|| (classID == 0x0A) || (classID == 0x0E)))
	{
		ret = 0;
	}
	else
		ret = 1;

	return ret;
}

void Atom_switchModeReq(char mode)
{
	if (mode == CONFIG_MODE)
	{
		sendPacket(MADDR_OUT, CLASS_ID_OPERATION_CMD, CMD_ID_SWITCH_TO_CFG_MODE, 00, NULL, 0x00);
	}
	else if (mode == MEASURE_MODE)
	{ 
		g=1;
		sendPacket(MADDR_OUT, CLASS_ID_OPERATION_CMD, CMD_ID_SWITCH_TO_MEASURE_MODE, 00, NULL, 0x00);
	}

}

void sendPacket(u8 MADDR, u8 classID, u8 msgID, u8 res, u8* payloadData, u16 payloadLen)
{
	AtomCmd_Compose_Send(MADDR, classID, msgID, (u8*)payloadData, payloadLen);

}

void Atom_setDataPacketConfigReq(u8* pData, u8 dataLen)
{
	sendPacket(MADDR_OUT, CLASS_ID_HOSTCONTROL_CMD, CMD_ID_SET_DATA_PAC_CFG, 00, pData, dataLen);
}

void Atom_setPacktUpdateRateReq(u8 *setRate, u8 dataLen_2)
{
	sendPacket(MADDR_OUT, CLASS_ID_ALGORITHMENGINE, CMD_ID_SET_PACKET_UPDATE_RATE, 00, setRate, dataLen_2);

}
