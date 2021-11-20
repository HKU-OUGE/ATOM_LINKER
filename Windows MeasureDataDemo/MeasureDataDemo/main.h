/******************** WINDOWS ATOM_LINKER *********************
* File Name          : ATOM_PROTOCOL.h
* Date First Issued  : 2021/2/15
* Description        : Header file 
********************************************************************************
* History:

* 2021/2/15: V1.0

*******************************************************************************/
#ifndef __MAIN_H
#define __MAIN_H
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define DEVICE_TASK_NUMBER                                  5

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float data;
}TtyData_TEMP_HandleType;

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

}TtyData_RAW_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float accX;
	float accY;
	float accZ;
}TtyData_CAL_ACC_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float gyroX;
	float gyroY;
	float gyroZ;
}TtyData_CAL_GYRO_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float magX;
	float magY;
	float magZ;
}TtyData_CAL_MAG_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float accX;
	float accY;
	float accZ;
}TtyData_KAL_ACC_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float gyroX;
	float gyroY;
	float gyroZ;
}TtyData_KAL_GYRO_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float magX;
	float magY;
	float magZ;
}TtyData_KAL_MAG_HandleType;

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
}TtyData_Quaternion_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float roll;
	float pitch;
	float yaw;
}TtyData_Euler_HandleType;

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
}TtyData_RoMax_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	u32 packerCounter;
}TtyData_PacketCounter_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float DT;
}TtyData_DT_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	float ErrorAll;
}TtyData_MAG_EA_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 percent;

}TtyData_MAG_Strength_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u16 OS_Time_us;
	u32 OS_Time_ms;
}TtyData_OS_Time_ms_HandleType;

typedef struct
{
	u16 dataID;
	u8 dataLen;
	u8 reserve;
	u32 status;
}TtyData_Status_HandleType;

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
}TtyData_UTCTime_HandleType;





typedef struct
{
	TtyData_TEMP_HandleType temperature;
	TtyData_RAW_HandleType  accRawData;
	TtyData_RAW_HandleType  gyroRawData;
	TtyData_RAW_HandleType  magRawData;
	TtyData_CAL_ACC_HandleType accCal;
	TtyData_CAL_GYRO_HandleType gyroCal;
	TtyData_CAL_MAG_HandleType magCal;
	TtyData_KAL_ACC_HandleType accKal;
	TtyData_KAL_GYRO_HandleType gyroKal;
	TtyData_KAL_MAG_HandleType magKal;
	TtyData_Quaternion_HandleType quat;
	TtyData_Euler_HandleType euler;
	TtyData_RoMax_HandleType romatix;
	TtyData_PacketCounter_HandleType packetCounter;
	TtyData_OS_Time_ms_HandleType tick;
	TtyData_Status_HandleType status;
	TtyData_UTCTime_HandleType UTC_time;
	//TtyData_CpuUsage_HandleTypeDef CpuUsage;
	TtyData_KAL_ACC_HandleType accLinear;
	TtyData_DT_HandleType dt;
	TtyData_MAG_Strength_HandleType magStrength;
	TtyData_MAG_EA_HandleType magEA;


}TtyData_HandleType;


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
#define		RATE_1000   1000   //swift mode only


#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))


extern void SelectPackets(char enable);
extern void SetHostUpdateRate(char rate);
extern void commandAckHandle(unsigned char classID, unsigned char mID);
extern void ReceiveData(u8 cid, u8 mid);
extern void clearBuf(u8 cid, u8 mid);
#endif