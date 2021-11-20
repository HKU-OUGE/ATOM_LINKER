/*---------------------------------------------------------------------------------------------
 *  Last Modify: 2021/4/27
 *  Version:     1.1
 *  Description:protocol implement on ROS/Linux
 *--------------------------------------------------------------------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "../ros_inc/macro.h"
#include "../ros_inc/protocol.h"
#include "../ros_inc/serial.h"


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

/*-----------------------send command--------------------------*/
/*-----------------------命令合成发送函数 先合成再通过UART接口发送 部署时记得去掉printf相关内容来降低时空占用--------------------------*/
int  AtomCmd_Compose_Send(signed char  nFD,u8 MADDR, u8 ClassID, u8 msgID, u8* payloadBuffer, u16 PayloadLen)
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
	int i = 0;
	for (i = 0; i < PayloadLen; i++)
		pBuf[index++] = payloadBuffer[i];

	//BCC
	crc = Atom_BCC((u8*)pBuf, ATOM_HEADER_LEN + PayloadLen);
	pBuf[index++] = crc & 0xff;

	//Footer
	pBuf[index++] = 'm';

	printf("TX: ");
	int j = 0;
	for (j = 0; j < (ATOM_CMDLEN_NO_PAYLOAD + PayloadLen); j++)
		printf("%02X ", pBuf[j]);

	printf("\n");
		
	SendUARTMessageLength(nFD,(const char *)pBuf, index);

	if(!pBuf)
		free(pBuf);
	return ret;
}
/*-----------------------CID预检查函数，只有注册过的CID才会进入处理环节--------------------------*/
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
/*-----------------------握手成功后的一个回调函数，用于切换系统状态--------------------------*/

void SwitchModeReq(signed char  nFD,char mode)
{
	if (mode == CONFIG_MODE)
	{
		sendPacket(nFD, MADDR_OUT, CLASS_ID_OPERATION_CMD, CMD_ID_SWITCH_TO_CFG_MODE, 00, NULL, 0x00);
	}
	else if (mode == MEASURE_MODE)
	{
		sendPacket(nFD, MADDR_OUT, CLASS_ID_OPERATION_CMD, CMD_ID_SWITCH_TO_MEASURE_MODE, 00, NULL, 0x00);
	}

}
/*-----------------------上层调用的数据包发送函数--------------------------*/
void sendPacket(signed char  nFD,u8 MADDR, u8 classID, u8 msgID, u8 res, u8* payloadData, u16 payloadLen)
{
	AtomCmd_Compose_Send(nFD,MADDR, classID, msgID, (u8*)payloadData, payloadLen);

}
/*-----------------------请求返回配置--------------------------*/
void SetDataPacketConfigReq(signed char  nFD,u8* pData, u8 dataLen)
{
	sendPacket(nFD, MADDR_OUT, CLASS_ID_HOSTCONTROL_CMD, CMD_ID_SET_DATA_PAC_CFG, 00, pData, dataLen);
}
/*-----------------------规定返回速率--------------------------*/
void setPacktUpdateRateReq(signed char  nFD, u8 *setRate, u8 dataLen_2)
{
	sendPacket(nFD, MADDR_OUT, CLASS_ID_ALGORITHMENGINE, CMD_ID_SET_PACKET_UPDATE_RATE, 00, setRate, dataLen_2);

}
