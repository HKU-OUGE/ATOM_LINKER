/******************** WINDOWS ATOM_LINKER *********************
* File Name          : ATOM_PROTOCOL.h
* Date First Issued  : 2021/2/10
* Description        : Header file 
********************************************************************************
* History:

* 2021/2/10: V1.0

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
extern void DataPacketParser(u8 *pBuffer, u16 dataLen);
extern int  AtomCmd_SearchingFrame(u8 **pCurrent, u32 rBufferLen, u32* punProcessedLen, u32 *pPacketLength);
extern int  AtomCmd_Processer(u8* pHeader, u8** ppCustomer, u8* ringBuffer, u32 ringBufferLen, u32* pBytes);
extern void Atom_switchModeReq(char mode);
extern void sendPacket(u8 MADDR, u8 classID, u8 msgID, u8 res, u8* payloadData, u16 payloadLen);
extern void Atom_setDataPacketConfigReq(u8* pData, u8 dataLen);
extern void Atom_setPacktUpdateRateReq(u8 *setRate, u8 dataLen_2);
#endif // ATOMPROTOCOL_H
