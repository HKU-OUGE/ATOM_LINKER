/******************** WINDOWS ATOM_LINKER *********************
* File Name          : ATOM_PROTOCOL.h
* Date First Issued  : 2021/1/26
* Description        : 
********************************************************************************
* History:

*  2021/1/26: V1.0

*******************************************************************************/

#include "stdafx.h"
#include "stdlib.h"
#include "string.h"
#include "Com.h"

#include "atom_macro.h"
#include "atomprotocol.h"

u8 priBuf[PRINT_SIZE] = { '\0' };
extern unsigned long ucComNo;

int packageResponse;
int sendStatus;
int downloadStatus;
int errorRateBase;

u32 resendTry = 0;
u32 hostUpdateRate = 100;
u32 frameStartTime = 0;
u32 frameRateEndTime = 0;
u32 frameRateCounter = 0;

u32 lastUnProcess = 0;
u32 unProcessedLen = 0;
u32 frameLen = 0;
u32 wantedBytes = 0;
u32 gUnvalid = 0;

u32 updateCount = 0;
u32 frameTotal = 0;
u32 frameErrorTotal = 0;
u32 mFrameCount = 0;


extern u32 fileLen;
extern u32  loop;
extern u32 gFrame_number;


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

/*-----------------------send command--------------------------*/
int  AtomCmd_Compose_Send(u8 MADDR, u8 ClassID, u8 msgID, u8* payloadBuffer, u16 PayloadLen)
{
	int index = 0;
	int ret = 0;
	u8 crc;

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

	printf("TX: ");
	for (int i = 0; i < (ATOM_CMDLEN_NO_PAYLOAD + PayloadLen); i++)
		printf("%02X ", pBuf[i]);
	printf("\n");

	SendUARTMessageLength(ucComNo, (const char *)pBuf, index);

	return ret;
}

int  AtomSaveToRingBuffer(u32 usLength, u8* chrBuffer, u8** pProducer, u8* ringBuffer, u32 ringBufferLen)
{
	int chrIndex = 0;
	int ret = 0;
	u8* pBufferStart, *pBufferEnd;
	u32 bufferAvailabe;

	pBufferStart = ringBuffer;
	pBufferEnd = pBufferStart + ringBufferLen;
	
	//Save data from comport to ringbuffer then update producer
	bufferAvailabe = (u32)(pBufferEnd - (*pProducer));
	if (usLength < bufferAvailabe)
	{
		memcpy((void*)(*pProducer), chrBuffer, usLength);
	}
	else //part of chrBuffer should be copied to begin of ringbuffer.
	{
		memcpy((void*)(*pProducer), chrBuffer, pBufferEnd - (*pProducer));
		chrIndex += (u32)(pBufferEnd - (*pProducer));
		memcpy(pBufferStart, (char*)chrBuffer + chrIndex, usLength - (pBufferEnd - (*pProducer)));
	}

	//update producer
	(*pProducer) += usLength;

	//reach the end and rewind to begin
	if ((*pProducer) > pBufferEnd)
		(*pProducer) = pBufferStart + ((*pProducer) - pBufferEnd);

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

//Searching header and checking footer , to make sure it is a complete package
int   AtomCmd_SearchingFrame(u8 **pCurrent, u32 rBufferLen, u32* punProcessedLen, u32 *pPacketLength)
{
	
	u8 temp[56] = { "\0" };
	u32 processInd = 0;
	int ret = 0;
	int len = *punProcessedLen;
	u8 classID = 0;
	u16 payloadlen = 0;
	u16 packetLen = 0;

	
	while ((*punProcessedLen) >= ATOM_CMDLEN_NO_PAYLOAD)
	{

		/*Check Preamble*/
		if (((*pCurrent)[processInd] == FRAME_PREAMBLE1) && (((*pCurrent)[processInd + 1] == FRAME_PREAMBLE2)))
		{
			//Record frame Total
			frameTotal++;
			mFrameCount++;

			if (mFrameCount >= (10 * hostUpdateRate))
			{
				float frameRate = 0;
				frameRate *= 1000;
				mFrameCount = 0;
			}


			/*Check CID*/
			classID = 0x0F & ((*pCurrent)[processInd + ATOM_CLASS_INDEX]);
			ret = checkCIDValidation(classID);
			if (!ret)
			{
				processInd += 2;
				len -= 2;
				*punProcessedLen = len;

				//error frame increase
				frameErrorTotal++;

				if (processInd >= rBufferLen)
					processInd = processInd - rBufferLen;
			}
			else
			{
				/*Check MID, TBD */

				/*Check payload len*/
				payloadlen = (u8)((*pCurrent)[processInd + ATOM_PAYLOAD_LEN_INDEX]);
				if (payloadlen == 0xFF)
				{
					payloadlen = *((u16*)((*pCurrent) + processInd + ATOM_PAYLOAD_LEN_INDEX + 1));
					packetLen = ATOM_CMDLEN_NO_PAYLOAD + 2 + payloadlen;
				}
				else
				{
					packetLen = ATOM_CMDLEN_NO_PAYLOAD + payloadlen;
				}

				if ((*punProcessedLen) >= (u32)packetLen)
				{
					/*Check footer*/
					if ((*pCurrent)[processInd + packetLen - 1] == 'm')
					{
						/*Check BCC*/
						//u8 sum = Atom_CRC8((u8*)((*pCurrent) + processInd), packetLen - 2);
						u8 sum = Atom_BCC((u8*)((*pCurrent) + processInd), packetLen - 2);
						u16 frameLen = packetLen;
						u8 crc = (*pCurrent)[processInd + packetLen - 2];

						if (crc == sum)
						{
							//Print out header for debug
							memset(priBuf, 0, PRINT_SIZE);

							sprintf((char*)temp, "[0x%p] ", (*pCurrent) + processInd);
							strcat((char*)&priBuf, (char*)&temp);
							sprintf((char*)temp, "SERIAL Searched:  ");
							strcat((char*)&priBuf, (char*)&temp);
							for (int i = 0; i < (packetLen); i++)
							{
								sprintf((char*)temp, "%02X ", (*pCurrent)[i + processInd]);
								strcat((char*)&priBuf, (char*)&temp);
							}

							ret = 1;	
							*pPacketLength = packetLen;
							break;
						}
						else
						{
							processInd += frameLen;
							len -= frameLen;
							*punProcessedLen = len;

							//error frame increase
							frameErrorTotal++;

							if (processInd >= rBufferLen)
								processInd = processInd - rBufferLen;
						}
					}
					else
					{
						processInd += packetLen;
						len -= packetLen;
						*punProcessedLen = len;

						//error frame increase
						frameErrorTotal++;

						if (processInd >= rBufferLen)
							processInd = processInd - rBufferLen;
					}
				}
				else
				{
					ret = ERROR_NOT_ENOUGH_LENGTH;
					frameTotal--;
					mFrameCount--;
					break;
				}
			}
		}
		else
		{
			processInd++;
			len--;
			*punProcessedLen = len;
			if (processInd >= rBufferLen)
				processInd = processInd - rBufferLen;
		}

	}

	(*pCurrent) += processInd;

	return ret;
}

int   Atom_DecodeData(u8* pCur, u16 payloadLenth)
{
	
	int ret = 0;
#if 1
	u8 *pData = NULL;
	u8 classID = *(pCur + ATOM_CLASS_INDEX);
	u8 msgID = *(pCur + ATOM_MID_INDEX);
	
	u16 dataLength = (u16)payloadLenth;
	u8 errorCode = ((classID & 0xF0) >> 4);

	if (payloadLenth > 0xFE)
		pData = pCur + ATOM_PAYLOAD_INDEX + 2;
	else
		pData = pCur + ATOM_PAYLOAD_INDEX;

	switch (classID & 0x0F)
	{
	case CLASS_ID_HOSTCONTROL_CMD:

		if (msgID == (CMD_ID_Tty_DATA_PACKET | 0x80))
		{

			if (errorCode != RETURN_CODE_OK)
			{
				qCritical("ERROR: CMD_ID_GET_DATA_PAC");
			}
			else
			{
				//Parsing packets£¬pData is our Tty data packets point
				/*-----------User Code Begin----------------*/
				DataPacketParser(pData, dataLength);

				/*-----------User Code End----------------*/

			}
		}
		break;

	default:

		ret = RETURN_CODE_ERROR_CLASS;
		break;
	}

#endif
	return ret;
}

int  AtomCmd_Parser(u8* pCurrent, u32* unProcessed)
{
	u16 payloadLen = 0;
	int ret = 0;
	u16 packetLength;

	payloadLen = (u8)(*((u8*)(pCurrent + ATOM_PAYLOAD_LEN_INDEX)));
	packetLength = ATOM_CMDLEN_NO_PAYLOAD + payloadLen;

	//Support SLP
	if (payloadLen == 0xFF)
	{
		payloadLen = *((u16*)(pCurrent + ATOM_PAYLOAD_LEN_INDEX + 1));
		packetLength = ATOM_CMDLEN_NO_PAYLOAD + payloadLen + 2;
	}

	//Decode one frame .
	if ((*unProcessed) >= packetLength)
	{
		frameLen = packetLength;

		ret = Atom_DecodeData((unsigned char*)(pCurrent), payloadLen);
		if (ret != 0)
		{
			ret = ERROR_DECODE_FAIL;
		}

		//Move to next frame
		(pCurrent) += frameLen;
		(*unProcessed) -= frameLen;
	}
	else
	{
		ret = ERROR_NOT_ENOUGH_LENGTH;
	}

	return ret;
}

int  AtomCmd_Processer(u8* pHeader, u8** ppCustomer, u8* ringBuffer, u32 ringBufferLen, u32* pBytes)
{
	int ret = 0;
	u8 * pCurrent = pHeader;

	u8* pBufferStart, *pBufferEnd;
	u32 unProcessedLen_pre;

	pBufferStart = ringBuffer;
	pBufferEnd = pBufferStart + ringBufferLen;
	lastUnProcess = 0;
	unProcessedLen_pre = *((u32*)pBytes);
	unProcessedLen = *pBytes;

	//Move pCustomer to next Frame
	(*ppCustomer) += (unProcessedLen_pre);
	if ((*ppCustomer) > pBufferEnd)
		(*ppCustomer) = pBufferStart + ((*ppCustomer) - pBufferEnd);

	ret = AtomCmd_Parser((u8*)pCurrent, (u32*)&unProcessedLen);

	lastUnProcess = unProcessedLen;
	*pBytes = unProcessedLen;

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