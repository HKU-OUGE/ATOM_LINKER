
#ifndef TOOL_H
#define TOOL_H

#include <stdio.h>
#include "../ros_inc/macro.h"
#include "../ros_inc/serial.h"
#include "../ros_inc/protocol.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float data;
}Data_TEMP_HandleType;

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

}Data_RAW_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float accX;
    float accY;
    float accZ;
}Data_CAL_ACC_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float gyroX;
    float gyroY;
    float gyroZ;
}Data_CAL_GYRO_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float magX;
    float magY;
    float magZ;
}Data_CAL_MAG_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float accX;
    float accY;
    float accZ;
}Data_KAL_ACC_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float gyroX;
    float gyroY;
    float gyroZ;
}Data_KAL_GYRO_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float magX;
    float magY;
    float magZ;
}Data_KAL_MAG_HandleType;

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
}Data_Quaternion_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float roll;
    float pitch;
    float yaw;
}Data_Euler_HandleType;

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
}Data_RoMax_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    u32 packerCounter;
}Data_PacketCounter_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float DT;
}Data_DT_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float ErrorAll;
}Data_MAG_EA_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 percent;

}Data_MAG_Strength_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u16 OS_Time_us;
    u32 OS_Time_ms;
}Data_OS_Time_ms_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    u32 status;
}Data_Status_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    double longlatX;
    double longlatY;
}Data_Position_HandleType;

typedef struct
{
    u16 dataID;
    u8 dataLen;
    u8 reserve;
    float longlatZ;
}Data_Altitude_HandleType;

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
}Data_UTCTime_HandleType;





typedef struct
{
    Data_TEMP_HandleType temperature;
    Data_RAW_HandleType  accRawData;
    Data_RAW_HandleType  gyroRawData;
    Data_RAW_HandleType  magRawData;
    Data_CAL_ACC_HandleType accCal;
    Data_CAL_GYRO_HandleType gyroCal;
    Data_CAL_MAG_HandleType magCal;
    Data_KAL_ACC_HandleType accKal;
    Data_KAL_GYRO_HandleType gyroKal;
    Data_KAL_MAG_HandleType magKal;
    Data_Quaternion_HandleType quat;
    Data_Euler_HandleType euler;
    Data_RoMax_HandleType romatix;
    Data_PacketCounter_HandleType packetCounter;
    Data_OS_Time_ms_HandleType tick;
    Data_Status_HandleType status;
    Data_UTCTime_HandleType UTC_time;
    //Data_CpuUsage_HandleTypeDef CpuUsage;
    Data_KAL_ACC_HandleType accLinear;
    Data_DT_HandleType dt;
    Data_MAG_Strength_HandleType magStrength;
    Data_MAG_EA_HandleType magEA;
    Data_Position_HandleType position;
    Data_Altitude_HandleType altitude;

}Data;

extern int Align(unsigned char nFD);
extern int GetFrame(unsigned char nFD, unsigned char * tmpBuf, int frameLen);
extern void ParserDataPacket(Data *DataHandle, u8 *pBuffer, u16 dataLen, FILE *fpLog);
extern bool ValidFrame(unsigned char * tmpBuf,int frameLen);
extern int  FillFrameHead(unsigned char * tmpBuf);
#endif //_TOOL_H
