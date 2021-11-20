/******************** (C) COPYRIGHT 2021 SINGER *********************
* File Name          : ATOM_PROTOCOL.h
* Author             : Stephen
* Date First Issued  : 2021
* Description        : Header file 
********************************************************************************
* History:

* 2021: V1.0

*******************************************************************************/
#ifndef ATOMPROTOCOL_H
#define ATOMPROTOCOL_H


/*
 ------------------------------------------------------------------------------
 ------------------------------------------------------------------------------
 - MACRO definition start here
 ------------------------------------------------------------------------------
 ------------------------------------------------------------------------------
 */
/*
 ************************************************************************
 * Definition for Protocol
 ************************************************************************
 */



#define MADDR_OUT                               0xFF

#define RING_BUFFER_SIZE                        (1024 * 192)
#define FILE_BUFFER_SIZE                        (1024 * 192)


#define MAX_RECV_BYTES                          (2048*4)
#define PRINT_SIZE                              (2*MAX_RECV_BYTES)

#define PACKET_PL_INDEX                         2
#define PACKET_DATA_INDEX                       3

enum mode
{
	NOT_CONNNECT_MODE,
	CONNECTED_MODE,
	WAKEUP_MODE,
	CONFIG_MODE,
	MEASURE_MODE,
	DEBUG_MODE,
	MAG_CALIBRATE_MODE,
};

enum parserCode
{
	ERROR_NOT_ENOUGH_LENGTH = 10,
	ERROR_CRC_FAIL,
	ERROR_DECODE_FAIL,
	ERROR_DUPLICATED_FRAME,
};

enum frameType
{
	FRAME_COMPLETE = 1,
	FRAME_ERROR,
	NOT_FRAME,
};

extern void Atom_switchModeReq(char mode);
extern void sendPacket(u8 MADDR, u8 classID, u8 msgID, u8 res, u8* payloadData, u16 payloadLen);
extern void Atom_setDataPacketConfigReq(u8* pData, u8 dataLen);
extern void Atom_setPacktUpdateRateReq(u8 *setRate, u8 dataLen_2);

#endif // ATOMPROTOCOL_H
