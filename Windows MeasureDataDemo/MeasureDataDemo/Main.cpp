/******************** WINDOWS ATOM_LINKER *********************
* File Name          : ATOM_PROTOCOL.h
* Date First Issued  : 2021/1/8
* Description        : 
********************************************************************************
* History:

* 2021/2/28:÷’”⁄∏„∫√¡À ‰∆

*******************************************************************************/
#include "stdafx.h"
#include "Com.h"
#include "windows.h"
#include "time.h"
#include "stdio.h"
#include "atom_macro.h"
#include "atomprotocol.h"
#include "main.h"
#include <Windows.h>

#define COMPORT_COMMUNICATION
#define DMP_OUTPUT

#define THREADCOUNT 1

TtyData_HandleType TtyDataHandle;
HANDLE ghThreads[THREADCOUNT];
HANDLE ghMutex;
DWORD WINAPI DataHandleProc(LPVOID);

unsigned short usLength = 0, usRxLength = 0;
long long gValidCount = 0, gUnvalid = 0;

int resendCounter;
int frameLen = 0;
unsigned char * pBufferStart = NULL, *pCustomer = NULL;
unsigned char *pProducer = NULL, *pCurrent = NULL, *pBufferEnd = NULL;
unsigned char * p;
unsigned char chr[RING_BUFFER_SIZE * 2];
unsigned char chrBuffer[RING_BUFFER_SIZE];
u8  ringBuf[RING_BUFFER_SIZE];
u8 tempbuf[RING_BUFFER_SIZE] = { '\0' };

unsigned char sum = 0;
unsigned long ucComNo = 0;
unsigned long baud = 0;
HANDLE gDoneEvent;
UINT ID = 1;

u32 unProcessBytes, previous_bytes, processedBytes;
DWORD   Start_time, End_time, Process1_time;
DWORD   Startprocess_time, Endprocess_time, Process_time;
int count_L;
int points = 0;
u8 receiveAck;
void CreateEventsAndThreads(void);

extern int dataReady;
extern  int Receive_count;
extern void Process();
extern void DataPacketParser(u8 *pBuffer, u16 dataLen);
int timeout_count = 0;
u8 reTx_flag = 0;
u8 errorCode_ack;
u8 rcid;
u8 rmid;


signed char OpenCom(void)
{
	static unsigned long ulNo = 0;
	signed char cResult = 0;
	printf("WINDOWS Connecting Com%d! ", ucComNo);
	do
	{
		cResult = OpenCOMDevice(ucComNo, baud);
	} while (cResult != 0);
	printf("WINDOWS Com%d Connected\r\n", ucComNo);
	return 0;
}

char waitAck(void (*pFunction)(char x),u8 cid,u8 mid,char parameter)
{
	
	Sleep(100);
	ReceiveData(cid, mid);
	while ((cid != rcid) && (mid != (rmid | 0x80))&& (receiveAck != 1)  )
	{
		reTx_flag++;
		/*------------------Timeout processing--------------*/
		if (reTx_flag == 20)											//Retransmission after 2s
		{
			reTx_flag = 0;
			pFunction(parameter);
			timeout_count++;
			if (timeout_count == 4)										//Unable to receive after 6 retransmissions
			{
				return 0;
			}
		}
		Sleep(100);
		ReceiveData(cid, mid);
		
		
	}	
	if (errorCode_ack != 0)
	{
		/*---------User Code Begin-------------*/
		//errorCode processing
		//return 0 or 1?
		/*---------User Code End-------------*/
	}
	receiveAck = 0;
	timeout_count = 0;
	return 1;
}
void main(int argc, char* argv[])
{
	FILE *fp;
	u8 ret = 0;
	pBufferStart = (unsigned char *)ringBuf;
	pBufferEnd = pBufferStart + RING_BUFFER_SIZE;
	pCustomer = pBufferStart;
	pProducer = pBufferStart;

	fp = fopen("Com.ini", "r");
	if (fp == 0)
	{
		printf("missing com.ini file.Redo one please\n");
		return;
	}
	if (EOF == fscanf(fp, "Com = %d\r\nBaudrate = %ld", &ucComNo, &baud))
	{
		printf("error in com.ini£°Redo one please");
		Sleep(5000);
	}
	fclose(fp);


#ifdef COMPORT_COMMUNICATION
	// Create COMthread
	OpenCom();
#endif
	/****************************** Switch to config mode ********************************************/
	Atom_switchModeReq(CONFIG_MODE);
	
	ret = waitAck(Atom_switchModeReq, CLASS_ID_OPERATION_CMD, CMD_ID_SWITCH_TO_CFG_MODE, CONFIG_MODE);
	if (ret == 0)
	{
		return;
	}
	/****************************** Switch to config mode ********************************************/

	
	/****************************** Select packet***************************************************/
	// Enable packets,  use SET_BIT to enable packet and use CLEAR_BIT to disable packet.
	int bitEnable = 0;
	/*-------------------Uesr Code Begin------------------*/
	//Users can set the packet according to their own needs
	SET_BIT(bitEnable, PACKET_CAL_ACC);
	//SET_BIT(bitEnable, PACKET_CAL_ACC);
	SET_BIT(bitEnable, PACKET_CAL_GYRO);
	//SET_BIT(bitEnable, PACKET_CAL_MAG);	
	SET_BIT(bitEnable, PACKET_EULER_DATA);
	//SET_BIT(bitEnable, PACKET_EULER_DATA);
	SET_BIT(bitEnable, PACKET_QUAT_DATA);
	//SET_BIT(bitEnable, PACKET_QUAT_DATA);

	/*-------------------Uesr Code End-------------------*/

	SelectPackets(bitEnable);

	ret = waitAck(SelectPackets, CLASS_ID_HOSTCONTROL_CMD, CMD_ID_SET_DATA_PAC_CFG, bitEnable);
	if (ret == 0)
	{
		return;
	}
	/****************************** Select packet***************************************************************/

	/****************************** Set host update rate***************************************************************/
	SetHostUpdateRate(RATE_100);

	ret = waitAck(SetHostUpdateRate, CLASS_ID_ALGORITHMENGINE, CMD_ID_SET_PACKET_UPDATE_RATE, RATE_100);
	if (ret == 0)
	{
		return;
	}
	/****************************** Set host update rate***************************************************************/

	/****************************** Switch To measure mode***************************************************************/
	Atom_switchModeReq(MEASURE_MODE);

	ret = waitAck(Atom_switchModeReq, CLASS_ID_OPERATION_CMD, CMD_ID_SWITCH_TO_MEASURE_MODE, MEASURE_MODE);
	if (ret == 0)
	{
		return;
	}
	/****************************** Switch To measure mode***************************************************************/

	CreateEventsAndThreads();
	getchar();
}

DWORD WINAPI DataHandleProc(LPVOID lpParam)
{
	//printf("Thread %d entering...\n", GetCurrentThreadId());

	//Process data...
#ifdef COMPORT_COMMUNICATION
	if (dataReady == 1)
	{
		ReceiveData(0x06, 0x81);
	}

#endif

	printf("Thread %d exiting\n", GetCurrentThreadId());
	return 1;
}



/*---------------------Receive data and determine if it is a packet or command Ack--------------------------------------*/
void ReceiveData(u8 cid,u8 mid)
{
	int index = 0;
	int frameStart = 0;
	int ind = 0;
	unsigned char * pStart = NULL;
	int remainBytes = 0;

	signed char cResult[2] = { 0 };

	while (1)
	{
		usLength = CollectUARTData(ucComNo, (char*)chrBuffer);
		int chrIndex = 0;

		if (usLength > 0)
		{
			Start_time = GetTickCount();
			usRxLength += usLength;

			//Save data from comport to ringbuffer then update producer
			if (usLength < (pBufferEnd - pProducer))
			{
				memcpy((void*)pProducer, chrBuffer, usLength);
			}
			else //part of chrBuffer should be copied to begin of ringbuffer.
			{
				memcpy((void*)pProducer, chrBuffer, pBufferEnd - pProducer);
				chrIndex += (int)(pBufferEnd - pProducer);
				memcpy(pBufferStart, (char*)&chrBuffer + chrIndex, usLength - (pBufferEnd - pProducer));
			}
			//update producer 
			pProducer += usLength;

			//reach the end and rewind to begin
			if (pProducer > pBufferEnd)
				pProducer = pBufferStart + (pProducer - pBufferEnd);

			//Processing data
			if (usRxLength >= FRAME_MIN_SIZE)
			{
				if ((cid == CLASS_ID_HOSTCONTROL_CMD) && (mid == (CMD_ID_Tty_DATA_PACKET | 0x80)))
				{
					Process();
				}
				else
				{
					commandAckHandle(cid, mid);
					return;
				}
				
			}

			
		}
	}
	return;
}

int unProcessedLen = 0;
int lastUnProcess = 0;
int Startframe = 0;
int print_flag = 0;


void Process()
{
	u8 * pCurrentHead = NULL;
	u8 * pCurrent = NULL;
	u8 CompletePackage = 0;
	u8 * pEndRing = NULL;
	u8 * pStartRing = NULL;
	int ret = 0;
	pEndRing = (u8*)(&ringBuf) + RING_BUFFER_SIZE;
	pStartRing = (u8*)&ringBuf;
	u32 needProcessedLen = 0;
	if (print_flag)
	{
		printf("\n======================================================================================================================\n");
		printf("%s(),pStartRing = %p, pEndRing  = %p bufferSize = %d \n", __FUNCTION__, pStartRing, pEndRing, pEndRing - pStartRing);
	}
	//copy ringbuff to tempbuf
	if (pProducer >= pCustomer)
	{

		unProcessBytes = (u32)(pProducer - pCustomer);
		if (print_flag)
			printf("%s(), pCustomer = %p, pProducer = %p, unProcessBytes = %d \n", __FUNCTION__, pCustomer, pProducer, unProcessBytes);
		if (unProcessBytes <= sizeof(tempbuf))
			memcpy((void*)&tempbuf, pCustomer, unProcessBytes);

	}
	else
	{

		unProcessBytes = (u32)(RING_BUFFER_SIZE - (pCustomer - pProducer));
		if (print_flag)
			printf("%s(),pProducer  = %p, pCustomer = %p, unProcessBytes = %d \n", __FUNCTION__, pProducer, pCustomer, unProcessBytes);
		if ((u32)(pEndRing - pCustomer) <= sizeof(tempbuf))
			memcpy((void*)&tempbuf, (void*)pCustomer, pEndRing - pCustomer);

		if ((u32)(pProducer - pStartRing) <= (sizeof(tempbuf) - (pEndRing - pCustomer)))
			memcpy((void*)((u8*)&tempbuf + (pEndRing - pCustomer)), (void*)pStartRing, pProducer - pStartRing);

	}

	
	if (unProcessBytes < FRAME_MIN_SIZE)
	{
		return;
	}

	pCurrentHead = (u8*)&tempbuf;

	while ((unProcessBytes >= FRAME_MIN_SIZE) && (ret != ERROR_NOT_ENOUGH_LENGTH))
	{
		previous_bytes = unProcessBytes;
		if (unProcessBytes >= ATOM_HEADER_LEN)
		{
			pCurrent = pCurrentHead;
			ret = AtomCmd_SearchingFrame((u8**)&pCurrentHead, RING_BUFFER_SIZE, (u32*)&unProcessBytes, &needProcessedLen);
		}

		if (ret == FRAME_COMPLETE)
		{
			CompletePackage = 1;
			printf("RX: ");
			for (int i = 0; i < (needProcessedLen); i++)
				printf("%02X ", pCurrentHead[i]);
			printf("\n");

		}

		else if (ret == FRAME_ERROR)
			CompletePackage = 0;
		else
			CompletePackage = 0;

		//Modify consumer if unProcessBytes had been changed.
		pCustomer += (pCurrentHead - pCurrent);
		if (pCustomer > pEndRing)
			pCustomer = pStartRing + (pCustomer - pEndRing);

		if (print_flag)
		{
			printf("%s(),CompletePackage = %d, needProcessedLen = %d, unProcessBytes = %d\n", __FUNCTION__, CompletePackage, needProcessedLen, unProcessBytes);
			printf("%s(),AtomCmd_SearchingFrame() pProducer  = %p, pCustomer = %p, \n", __FUNCTION__, pProducer, pCustomer);
		}
		if (CompletePackage)
		{
			CompletePackage = 0;

			//Save unprocessBytes before processer.
			previous_bytes = unProcessBytes;
			processedBytes = needProcessedLen;
			ret = AtomCmd_Processer(pCurrentHead, (u8**)&pCustomer, ringBuf, RING_BUFFER_SIZE, (u32*)&needProcessedLen);

			//processedBytes = previous_bytes - unProcessBytes;
			pCurrentHead += processedBytes;
			unProcessBytes -= processedBytes;
			if (print_flag)
				printf("after: pProducer  = %p, pCustomer = %p, pCustomerMove: %d,unProcessBytes= %d \n",
				pProducer, pCustomer, processedBytes, unProcessBytes);

		}
		else
		{
			continue;
		}
	}

	if (print_flag)
		printf("after: pProducer  = %p, pCustomer = %p, pCustomerMove: %d,unProcessBytes= %d \n",
		pProducer, pCustomer, processedBytes, unProcessBytes);
}

void CreateEventsAndThreads(void)
{
	DWORD dwThreadID[2];

	gDoneEvent = CreateEvent(
		NULL,               // default security attributes
		TRUE,               // manual-reset event
		FALSE,              // initial state is nonsignaled
		TEXT("DoneEvent")  // object name
		);

	if (gDoneEvent == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return;
	}

	// Create a mutex with no initial owner

	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (ghMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return;
	}


	//  Create DataHandleProc 
	ghThreads[0] = CreateThread(
		NULL,              // default security
		0,                 // default stack size
		DataHandleProc,        // name of the thread function
		NULL,              // no thread parameters
		0,                 // default startup flags
		&dwThreadID[1]);

	//SetThreadPriority(ghThreads[0], THREAD_PRIORITY_NORMAL);

	if (ghThreads[0] == NULL)
	{
		printf("CreateThread failed (%d)\n", GetLastError());
		return;
	}
}




/*--------------Process received ACk------------------------*/

void commandAckHandle(unsigned char classID, unsigned char mID)
{
	u8 errorCode = ((classID & 0xF0) >> 4);;
	int index = 2 ;
	u8 *p= chrBuffer;
	
	while (1)
	{
		if (index == RING_BUFFER_SIZE)
			return;
		if (((chrBuffer[index] & 0x0F) == classID) && ((chrBuffer[index + 1] ) == (mID |0x80)))
		{
			if (chrBuffer[index + 2 +chrBuffer[index + 2] + 2] == 0x6D)
			{
				rcid = classID;
				rmid = mID;
				p = (u8*)(chrBuffer + index - 3);
				break;
			}
		}					
		index++;
	}
	Sleep(1000);
	errorCode_ack = ((chrBuffer[index] & 0xF0) >> 4);
	
	switch (classID)
	{
		/*---------User Code Begin-------------*/
		/*----------Users can add other command parsing--------------------------*/
	case CLASS_ID_OPERATION_CMD:
		if (mID == (CMD_ID_SWITCH_TO_CFG_MODE))
		{
			receiveAck = 1;
		}
		if (mID == (CMD_ID_SWITCH_TO_MEASURE_MODE))
		{
			receiveAck = 1;
		}
		break;
	case CLASS_ID_HOSTCONTROL_CMD:
		if (mID == (CMD_ID_SET_DATA_PAC_CFG))
		{
			receiveAck = 1;
		}
		break;
	case CLASS_ID_ALGORITHMENGINE:
		if (mID == (CMD_ID_SET_PACKET_UPDATE_RATE))
		{
			receiveAck = 1;
		}
		break;
	/*---------User Code End-------------*/
	default:
		break;
	}

	printf("RX: ");
	for (int i = 0; i < (p[ATOM_PAYLOAD_LEN_INDEX] + 8); i++)
		printf("%02X ", p[i]);
	printf("\n");
}

/*-------------------Packet parsing---------------------------------*/
void DataPacketParser(u8 *pBuffer, u16 dataLen)
{
	u16 PID = 0;
	u8 *pData = pBuffer;
	u8 index = 0;
	u8 pl = 0;

	//reset TtyDataHandle
	memset(&TtyDataHandle, 0, sizeof(TtyDataHandle));
	printf("\n");

	while (index < dataLen)
	{
		PID = ((*((u16*)(pData + index))) & 0x7fff);
		pl = *(pData + index + 2);
		if (PID == (SESSION_NAME_TEMPERATURE))
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.temperature.data, pData + index, PL_TEMPERTURE);
			TtyDataHandle.temperature.dataID = PID;
			TtyDataHandle.temperature.dataLen = pl;
			printf(" *** temperature:\t%11.4f *** \n", TtyDataHandle.temperature.data);

			index += PL_TEMPERTURE;

		}
		else if (PID == (SESSION_NAME_RAW_ACC))
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.accRawData.accX, pData + index, PL_RAW_DATA);
			TtyDataHandle.accRawData.dataID = PID;
			TtyDataHandle.accRawData.dataLen = pl;

			index += PL_RAW_DATA;

		}
		else if (PID == SESSION_NAME_RAW_GYRO)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.gyroRawData.gyroX, pData + index, PL_RAW_DATA);
			TtyDataHandle.gyroRawData.dataID = PID;
			TtyDataHandle.gyroRawData.dataLen = pl;
			index += PL_RAW_DATA;
		}
		else if (PID == SESSION_NAME_RAW_MAG)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.magRawData.magX, pData + index, PL_RAW_DATA);
			TtyDataHandle.magRawData.dataID = PID;
			TtyDataHandle.magRawData.dataLen = pl;
			index += PL_RAW_DATA;
		}
		else if (PID == SESSION_NAME_CAL_ACC)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.accCal.accX, pData + index, PL_CAL_DATA);
			TtyDataHandle.accCal.dataID = PID;
			TtyDataHandle.accCal.dataLen = pl;
			index += PL_CAL_DATA;

			printf(" *** accCal:     \t%11.4f, %11.4f, %11.4f *** \n", TtyDataHandle.accCal.accX, TtyDataHandle.accCal.accY, TtyDataHandle.accCal.accZ);
		}
		else if (PID == SESSION_NAME_CAL_GYRO)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.gyroCal.gyroX, pData + index, PL_CAL_DATA);

			TtyDataHandle.gyroCal.dataID = PID;
			TtyDataHandle.gyroCal.dataLen = pl;
			index += PL_CAL_DATA;

			printf(" *** gyroCal:    \t%11.4f, %11.4f, %11.4f *** \n", TtyDataHandle.gyroCal.gyroX, TtyDataHandle.gyroCal.gyroY, TtyDataHandle.gyroCal.gyroZ);
		}
		else if (PID == SESSION_NAME_CAL_MAG)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.magCal.magX, pData + index, PL_CAL_DATA);
			TtyDataHandle.magCal.dataID = PID;
			TtyDataHandle.magCal.dataLen = pl;
			index += PL_CAL_DATA;

			printf(" *** magCal:     \t%11.4f, %11.4f, %11.4f *** \n", TtyDataHandle.magCal.magX, TtyDataHandle.magCal.magY, TtyDataHandle.magCal.magZ);
		}
		else if (PID == SESSION_NAME_KAL_ACC)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.accKal.accX, pData + index, PL_KAL_DATA);
			TtyDataHandle.accKal.dataID = PID;
			TtyDataHandle.accKal.dataLen = pl;
			index += PL_KAL_DATA;

		}
		else if (PID == SESSION_NAME_KAL_GYRO)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.gyroKal.gyroX, pData + index, PL_KAL_DATA);
			TtyDataHandle.gyroKal.dataID = PID;
			TtyDataHandle.gyroKal.dataLen = pl;
			index += PL_KAL_DATA;
		}
		else if (PID == SESSION_NAME_KAL_MAG)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.magKal.magX, pData + index, PL_KAL_DATA);
			TtyDataHandle.magKal.dataID = PID;
			TtyDataHandle.magKal.dataLen = pl;
			index += PL_KAL_DATA;
		}
		//////////////////////////
		else if (PID == SESSION_NAME_QUAT)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.quat.Q0.uint_x, pData + index, PL_QUAT_EULER);
			TtyDataHandle.quat.dataID = PID;
			TtyDataHandle.quat.dataLen = pl;
			index += PL_QUAT_EULER;
			printf(" *** quat :      \t%11.4f, %11.4f, %11.4f, %11.4f *** \n", TtyDataHandle.quat.Q0.float_x, TtyDataHandle.quat.Q1.float_x, TtyDataHandle.quat.Q2.float_x, TtyDataHandle.quat.Q3.float_x);

		}
		else if (PID == SESSION_NAME_EULER)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.euler.roll, pData + index, PL_QUAT_EULER);
			TtyDataHandle.euler.dataID = PID;
			TtyDataHandle.euler.dataLen = pl;
			index += PL_QUAT_EULER;
			//temperature:
			printf(" *** euler:      \t%11.4f, %11.4f, %11.4f *** \n", TtyDataHandle.euler.roll, TtyDataHandle.euler.pitch, TtyDataHandle.euler.yaw);
		}

		else if (PID == SESSION_NAME_ROTATION_M)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.romatix.a, pData + index, PL_MATERIX);
			TtyDataHandle.romatix.dataID = PID;
			TtyDataHandle.romatix.dataLen = pl;
			index += PL_MATERIX;

		}

		else if (PID == SESSION_NAME_LINEAR_ACC)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.accLinear.accX, pData + index, PL_LINEAR_ACC_DATA);
			TtyDataHandle.accLinear.dataID = PID;
			TtyDataHandle.accLinear.dataLen = pl;
			index += PL_LINEAR_ACC_DATA;

		}
		else if (PID == SESSION_NAME_PACKET_COUNTER)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.packetCounter.packerCounter, pData + index, PL_PACKET_NUMBER);
			TtyDataHandle.packetCounter.dataID = PID;
			TtyDataHandle.packetCounter.dataLen = pl;
			index += PL_PACKET_NUMBER;
		}
		else if (PID == SESSION_NAME_DELTA_T)
		{
			//Ignore pid and pl
			index += 3;
			memcpy(&TtyDataHandle.dt.DT, pData + index, PL_DT_DATA);

			TtyDataHandle.dt.dataID = PID;
			TtyDataHandle.dt.dataLen = pl;
			index += PL_DT_DATA;
		}

		else if (PID == SESSION_NAME_OS_TIME)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.tick.OS_Time_ms, pData+index, PL_OS_REFERENCE_TIME-2); //first 4 bytes are miliseconds
			TtyDataHandle.tick.OS_Time_ms = *((u32*)(pData + index));
			TtyDataHandle.tick.OS_Time_us = *((u16*)(pData + index + 4));

			TtyDataHandle.tick.dataID = PID;
			TtyDataHandle.tick.dataLen = pl;
			index += PL_OS_REFERENCE_TIME;
		}
		else if (PID == SESSION_NAME_STATUS_WORD)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&TtyDataHandle.status.status, pData + index, PL_STATUS);
			TtyDataHandle.status.dataID = PID;
			TtyDataHandle.status.dataLen = pl;
			index += PL_STATUS;
		}
	}
	//	printf("\n\n");
}
/*-----------------------Packet selection---------------------------*/
void SelectPackets(char enable)
{
	u16 pid = 0;
	ConfigSingleDataPacket_HandleType Packet[6];
	u8 index = 0;
	for (int i = 0; i < 6; i++)
	{
		switch (i)
			{
			case CAL_ACC:
				if (((enable >> i) & 0x1) == 0x1)
					pid = SESSION_NAME_CAL_ACC | 0x8000;
				else
					pid = SESSION_NAME_CAL_ACC;
				break;
			case CAL_GYRO:
				if (((enable >> i) & 0x1) == 0x1)
					pid = SESSION_NAME_CAL_GYRO | 0x8000;
				else
					pid = SESSION_NAME_CAL_GYRO;
				break;
			case CAL_MAG:
				if (((enable >> i) & 0x1) == 0x1)
					pid = SESSION_NAME_CAL_MAG | 0x8000;
				else
					pid = SESSION_NAME_CAL_MAG;
				break;
			case EULER_DATA:
				if (((enable >> i) & 0x1) == 0x1)
					pid = SESSION_NAME_EULER | 0x8000;
				else
					pid = SESSION_NAME_EULER;
				break;
			case QUAT_DATA:
				if (((enable >> i) & 0x1) == 0x1)
					pid = SESSION_NAME_QUAT | 0x8000;
				else
					pid = SESSION_NAME_QUAT;
				break;
			case TEMPERATURE:
				if (((enable >> i) & 0x1) == 0x1)
					pid = SESSION_NAME_TEMPERATURE | 0x8000;
				else
					pid = SESSION_NAME_TEMPERATURE;
				break;
			default:
				break;

			}
			Packet[index].reserve0 = 0xff;
			Packet[index].reserve1 = 0xff;
			Packet[index].packetID = pid;
			index++;
	
	}
	Atom_setDataPacketConfigReq((u8*)&Packet, index * 4);

}
/*------------------------Set Output Data Rate----------------------*/
void SetHostUpdateRate(char rate)
{
	u8 data[2];
	data[0] = rate & 0xff;
	data[1] = (rate & 0xff00) >> 8;
	Atom_setPacktUpdateRateReq((u8*)&data, 2);
}