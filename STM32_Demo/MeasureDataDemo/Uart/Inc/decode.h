/******************** (C) COPYRIGHT 2015 Atom Corporation *********************
* File Name          : ATOM_PROTOCOL.h
* Author             : Stephen
* Date First Issued  : 2016.10.20
* Description        : Header file 
********************************************************************************
* History:

* 2016.10.20: V0.1

********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
#ifndef __MAIN_H
#define __MAIN_H

#define DEVICE_TASK_NUMBER                                  5
#include "usart.h"
typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float data;
}SaberData_TEMP_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve0;
	short accX;
	short accY;
	short accZ;
	short gyroX;
	short gyroY;
	short gyroZ;
	short magX;
	short magY;
	short magZ;
	u16 reserve1;

}SaberData_RAW_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float accX;
	float accY;
	float accZ;
}SaberData_CAL_ACC_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float gyroX;
	float gyroY;
	float gyroZ;
}SaberData_CAL_GYRO_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float magX;
	float magY;
	float magZ;
}SaberData_CAL_MAG_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float accX;
	float accY;
	float accZ;
}SaberData_KAL_ACC_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float gyroX;
	float gyroY;
	float gyroZ;
}SaberData_KAL_GYRO_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float magX;
	float magY;
	float magZ;
}SaberData_KAL_MAG_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	union
	{
		unsigned int uint_x;
		float        float_x;
	} Q0;
	union
	{
		unsigned int uint_x;
		float        float_x;
	} Q1;
	union
	{
		unsigned int uint_x;
		float        float_x;
	} Q2;
	union
	{
		unsigned int uint_x;
		float        float_x;
	} Q3;
}SaberData_Quaternion_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float roll;
	float pitch;
	float yaw;
}SaberData_Euler_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float a;
	float b;
	float c;
	float d;
	float e;
	float f;
	float g;
	float h;
	float i;
}SaberData_RoMax_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	u32 packerCounter;
}SaberData_PacketCounter_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float DT;
}SaberData_DT_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float ErrorAll;
}SaberData_MAG_EA_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 percent;

}SaberData_MAG_Strength_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u16 OS_Time_us;
	u32 OS_Time_ms;
}SaberData_OS_Time_ms_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	u32 status;
}SaberData_Status_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	u32 ms;
	u16 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	u8 flag;
}SaberData_UTCTime_HandleType;





typedef struct
{
	SaberData_TEMP_HandleType temperature;
	SaberData_RAW_HandleType  accRawData;
	SaberData_RAW_HandleType  gyroRawData;
	SaberData_RAW_HandleType  magRawData;
	SaberData_CAL_ACC_HandleType accCal;
	SaberData_CAL_GYRO_HandleType gyroCal;
	SaberData_CAL_MAG_HandleType magCal;
	SaberData_KAL_ACC_HandleType accKal;
	SaberData_KAL_GYRO_HandleType gyroKal;
	SaberData_KAL_MAG_HandleType magKal;
	SaberData_Quaternion_HandleType quat;
	SaberData_Euler_HandleType euler;
	SaberData_RoMax_HandleType romatix;
	SaberData_PacketCounter_HandleType packetCounter;
	SaberData_OS_Time_ms_HandleType tick;
	SaberData_Status_HandleType status;
	SaberData_UTCTime_HandleType UTC_time;
	//SaberData_CpuUsage_HandleTypeDef CpuUsage;
	SaberData_KAL_ACC_HandleType accLinear;
	SaberData_DT_HandleType dt;
	SaberData_MAG_Strength_HandleType magStrength;
	SaberData_MAG_EA_HandleType magEA;


}SaberData_HandleType;


#define		CAL_ACC			0
#define		CAL_GYRO		1	
#define		CAL_MAG			2
#define		EULER_DATA		3
#define		QUAT_DATA		4
#define		TEMPERATURE		5


#define		PACKET_CAL_ACC					(1<<CAL_ACC)    
#define		PACKET_CAL_GYRO					(1<<CAL_GYRO)    
#define		PACKET_CAL_MAG					(1<<CAL_MAG)    
#define		PACKET_EULER_DATA				(1<<EULER_DATA)    
#define		PACKET_QUAT_DATA				(1<<QUAT_DATA)    
#define		PACKET_TEMPERATURE				(1<<TEMPERATURE)    

#define 	RATE_50		50
#define		RATE_100	100
#define		RATE_200	200
#define		RATE_400	400
#define		RATE_500	500    //swift mode only
#define		RATE_800	800    //swift mode only
#define		RATE_1000       1000   //swift mode only


#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

extern MidProtocol_StructType UartRxProtocolHandle;
extern MidProtocol_StructType HostRxProtocolHandle;
extern void SelectPackets(char enable);
extern void SetHostUpdateRate(char rate);
extern void sendPacket(u8 MADDR, u8 classID, u8 msgID, u8 res, u8* payloadData, u16 payloadLen);
void DataPacketParser(Buffer_Type *HOST_Buffer, u16 dataLen);




//extern unsigned char buf[1024] = {0} ;

#endif