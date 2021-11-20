/******************** WINDOWS ATOM_LINKER *********************
* File Name          : ATOM_PROTOCOL.h
* Date First Issued  : 2021/1/27
* Description        : 
********************************************************************************
* History:

* 2021/1/27: V1.1

*******************************************************************************/
#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include "Com.h"

#define		TOTAL_PORT_NUM		256
#define		START_PORT_NUM		0

#define		iBufferSize 250
#define     UARTBufferLength 98304
#undef  SYNCHRONOUS_MODE

int dataReady;
static HANDLE		 hComDev[TOTAL_PORT_NUM]         ={NULL};
static unsigned long long ulComMask = 0;
static HANDLE		 hCOMThread[TOTAL_PORT_NUM]      ={NULL};
static OVERLAPPED	 stcWriteStatus[TOTAL_PORT_NUM]  = {0};
static OVERLAPPED	 stcReadStatus[TOTAL_PORT_NUM]   = {0};

#ifdef SYNCHRONOUS_MODE
static HANDLE		 hReceiveEvent[TOTAL_PORT_NUM]   ={NULL};
#endif

static volatile char chrUARTBuffers[TOTAL_PORT_NUM][UARTBufferLength]={0};
static volatile unsigned long ulUARTBufferStart[UARTBufferLength] = { 0 }, ulUARTBufferEnd[UARTBufferLength] = { 0 };
int Receive_count;
unsigned short CollectUARTData(const unsigned long ulCOMNo,char chrUARTBufferOutput[])
{
	unsigned long ulLength=0;
	unsigned long ulEnd ;
	unsigned long ulStart ;

#ifdef SYNCHRONOUS_MODE
	WaitForSingleObject(hReceiveEvent[ulIndexCorrect],INFINITE);
	ResetEvent(hReceiveEvent[ulIndexCorrect]);
#endif
	ulEnd = ulUARTBufferEnd[ulCOMNo];
	ulStart = ulUARTBufferStart[ulCOMNo];
	if (ulEnd == ulStart)
		return(0);
	if (ulEnd > ulStart)
	{
		memcpy((void*)chrUARTBufferOutput,(void*)(chrUARTBuffers[ulCOMNo]+ulStart),ulEnd-ulStart);
		ulLength = ulEnd-ulStart;
	}
	else
	{
		memcpy((void*)chrUARTBufferOutput,(void*)(chrUARTBuffers[ulCOMNo]+ulStart),UARTBufferLength-ulStart);
		if ( ulEnd != 0 )
		{
			memcpy((void*)(chrUARTBufferOutput+(UARTBufferLength-ulStart)),(void*)chrUARTBuffers[ulCOMNo],ulEnd);
		}
		ulLength = UARTBufferLength+ulEnd-ulStart;
	}
	ulUARTBufferStart[ulCOMNo] = ulEnd;
	return (unsigned short) ulLength;
}

signed char SendUARTMessageLength(const unsigned long ulChannelNo, const char chrSendBuffer[],const unsigned short usLen)
{
	DWORD iR;
	DWORD dwRes;
	DCB dcb;
	char chrDataToSend[1000] = {0};
	memcpy(chrDataToSend,chrSendBuffer,usLen);
	memcpy(&chrDataToSend[usLen],chrSendBuffer,usLen);

	GetCommState(hComDev[ulChannelNo] ,&dcb);
	dcb.fDtrControl = 0;//DTR = 1;
	SetCommState(hComDev[ulChannelNo] ,&dcb);

	if ( WriteFile(hComDev[ulChannelNo],chrSendBuffer,usLen,&iR,&(stcWriteStatus[ulChannelNo])) || GetLastError() != ERROR_IO_PENDING  ) 
		return -1;
	dwRes = WaitForSingleObject(stcWriteStatus[ulChannelNo].hEvent,1000);
	Sleep(10);
	dcb.fDtrControl = 1;//DTR = 0;
	SetCommState(hComDev[ulChannelNo] ,&dcb);
	Sleep(10);

	if(dwRes != WAIT_OBJECT_0 || ! GetOverlappedResult(hComDev[ulChannelNo], &stcWriteStatus[ulChannelNo], &iR, FALSE))
		return 0;
	return 0;
}

DWORD WINAPI ReceiveCOMData(PVOID pParam)
{	
	unsigned long uLen;
	unsigned long ulLen1;
	unsigned long ulLen2;
	DWORD	dwRes;
	COMSTAT Comstat;
	DWORD dwErrorFlags;
	char chrBuffer[iBufferSize]={0};
	unsigned long ulUARTBufferEndTemp=ulUARTBufferEnd[0];

	unsigned long ulComNumber = 0;
	memcpy(&ulComNumber,pParam,4); 
	//////////////////***************Old
	static DWORD Com_start_time, Com_End_time, Com_time, Com_last_time, Com_current_time, Com_1_time;
//	int  Com_receive_count;
	float CoM_speed;
	Com_start_time=GetTickCount();
	static int flag=(-1);
	///////////////////************Old
	while (1)
	{
		if ( ! ReadFile(hComDev[ulComNumber],chrBuffer,iBufferSize-1,&uLen,&(stcReadStatus[ulComNumber])) )
		{			
			dwRes = GetLastError() ;
			if ( dwRes != ERROR_IO_PENDING)
			{			
				ClearCommError(hComDev[ulComNumber],&dwErrorFlags,&Comstat);
				continue;
			}

			WaitForSingleObject(stcReadStatus[ulComNumber].hEvent,INFINITE);
			
			if ( !GetOverlappedResult(hComDev[ulComNumber], &(stcReadStatus[ulComNumber]), &uLen, FALSE))
				continue;
			if(uLen <= 0)
				continue;
			
			if (uLen > 0)
			{
				dataReady = 1;
				Receive_count = uLen + Receive_count;
				if (flag ==(-1))
				{					
					Com_last_time = GetTickCount();					
				}
				if (flag == 1)
				{
					Com_current_time = GetTickCount();
					Com_1_time = Com_current_time - Com_last_time;					
				}

				if (Receive_count >= 19000)
				{
					Com_End_time = GetTickCount();
				Com_time = Com_End_time - Com_start_time;

				CoM_speed = (float)((float)Receive_count / Com_time)*1000.0;			
				
			}

				flag = flag*(-1);
			}
			
			if ( (ulUARTBufferEndTemp + uLen) > UARTBufferLength )
			{		
				ulLen1 = UARTBufferLength - ulUARTBufferEndTemp;
				ulLen2 = uLen - ulLen1;
				if (ulLen1 > 0)
				{
					memcpy((void *)&chrUARTBuffers[ulComNumber][ulUARTBufferEnd[ulComNumber]],(void *)chrBuffer,ulLen1);
				}
				if (ulLen2 > 0)
				{
					memcpy((void *)&chrUARTBuffers[ulComNumber][0],(void *)(chrBuffer+ulLen1),ulLen2);
				}
				ulUARTBufferEndTemp = ulLen2;
			}
			else
			{
				memcpy((void *)&chrUARTBuffers[ulComNumber][ulUARTBufferEnd[ulComNumber]],(void *)chrBuffer,uLen);
				ulUARTBufferEndTemp+=uLen;	
			}

			if (  ulUARTBufferEndTemp == ulUARTBufferStart[ulComNumber])
			{
				printf("Error!");
			}
			else
			{
				ulUARTBufferEnd[ulComNumber] = ulUARTBufferEndTemp;
			}

#ifdef SYNCHRONOUS_MODE
			SetEvent(hReceiveEvent[ucComNumber]);
#endif
			continue;
		}
//
//		if(uLen <= 0)
//			continue;
//		
//		if ( (ulUARTBufferEndTemp + uLen) > (UARTBufferLength) )
//		{
//			ulLen1 = UARTBufferLength - ulUARTBufferEndTemp;
//			ulLen2 = uLen - ulLen1;
//			if (ulLen1 > 0)
//			{
//				memcpy((void *)&chrUARTBuffers[ulComNumber][ulUARTBufferEnd[ulComNumber]],(void *)chrBuffer,ulLen1);
//			}
//			if (ulLen2 > 0)
//			{
//				memcpy((void *)&chrUARTBuffers[ulComNumber][0],(void *)(chrBuffer+ulLen1),ulLen2);
//			}
//			ulUARTBufferEndTemp = ulLen2;
//		}
//		else
//		{
//			memcpy((void *)&chrUARTBuffers[ulComNumber][ulUARTBufferEnd[ulComNumber]],(void *)chrBuffer,uLen);
//			ulUARTBufferEndTemp+=uLen;	
//		}
//
//		if (  ulUARTBufferEndTemp== ulUARTBufferStart[ulComNumber])
//		{
//			printf("Error!");
//		}
//		else
//		{
//			ulUARTBufferEnd[ulComNumber] = ulUARTBufferEndTemp;
//		}	
//
//#ifdef SYNCHRONOUS_MODE
//		SetEvent(hReceiveEvent[ucComNumber]);
//#endif

	}
	return 0;
}

signed char OpenCOMDevice(const unsigned long ulPortNo,const unsigned long ulBaundrate)
{
	DWORD dwThreadID,dwThreadParam;
	COMSTAT Comstat;
	DWORD dwErrorFlags;
	DWORD dwRes;
	DCB dcb;
	COMMTIMEOUTS comTimeOut;
	TCHAR PortName[10] = {'\\','\\','.','\\','C','O','M',0,0,0};//"\\\\.\\COM";
	TCHAR chrTemple[5]={0};

	if(ulPortNo >= TOTAL_PORT_NUM)
	{
		printf("\nerror: exceed the max com port num\n");
		return -1;
	}


	_itot(ulPortNo+START_PORT_NUM,chrTemple,10);
	_tcscat(PortName,chrTemple);

	if((hComDev[ulPortNo] = CreateFile(PortName,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED ,NULL))==INVALID_HANDLE_VALUE)
	{
		dwRes=GetLastError();
		return -1;
	}
	ulComMask |= 1<<ulPortNo;

	SetupComm(hComDev[ulPortNo] ,iBufferSize,iBufferSize);
	GetCommState(hComDev[ulPortNo] ,&dcb);
		dcb.BaudRate = ulBaundrate;
	dcb.fParity = NOPARITY;
	dcb.ByteSize=8;
	dcb.fDtrControl = 1;//DTR = 0;
	dcb.fRtsControl = 0;//RTS = 0;
	dcb.StopBits=ONESTOPBIT;

	SetCommState(hComDev[ulPortNo] ,&dcb);
	ClearCommError(hComDev[ulPortNo] ,&dwErrorFlags,&Comstat);
	dwRes = GetLastError();

	comTimeOut.ReadIntervalTimeout = 5;				
	comTimeOut.ReadTotalTimeoutMultiplier = 10;		
	comTimeOut.ReadTotalTimeoutConstant = 100;		
	comTimeOut.WriteTotalTimeoutMultiplier = 5;		
	comTimeOut.WriteTotalTimeoutConstant = 5;		
	SetCommTimeouts(hComDev[ulPortNo] ,&comTimeOut);	

	stcWriteStatus[ulPortNo].hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	stcReadStatus[ulPortNo].hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	stcReadStatus[ulPortNo].Internal = 0;
	stcReadStatus[ulPortNo].InternalHigh = 0;
	stcReadStatus[ulPortNo].Offset = 0;
	stcReadStatus[ulPortNo].OffsetHigh = 0;
	dwThreadParam = ulPortNo;
	hCOMThread[dwThreadParam] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ReceiveCOMData,&dwThreadParam,0,&dwThreadID);
	SetThreadPriority(hCOMThread[ulPortNo],THREAD_PRIORITY_NORMAL);
	Sleep(200);

	return 0;

} 

signed char SetBaundrate(const unsigned long ulPortNo,const unsigned long ulBaundrate)
{

	DCB dcb;	
	GetCommState(hComDev[ulPortNo] ,&dcb);
	dcb.BaudRate = ulBaundrate;
	SetCommState(hComDev[ulPortNo] ,&dcb);
	return 0;

} 
void CloseCOMDevice()
{
	unsigned char i;
	for(i=0 ; i<sizeof(ulComMask)*8 ; i++)
	{
		if((ulComMask & (1<<i))==0)
			continue;
		ulUARTBufferEnd[i] = 0;ulUARTBufferStart[i]=0;
		TerminateThread(hCOMThread[i],0);
		WaitForSingleObject(hCOMThread[i],10000);
		PurgeComm(hComDev[i],PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		CloseHandle(stcReadStatus[i].hEvent);
		CloseHandle(stcWriteStatus[i].hEvent);
		CloseHandle(hComDev[i]);
	}
	ulComMask = 0;
}
