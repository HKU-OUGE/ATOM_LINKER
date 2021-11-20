/*---------------------------------------------------------------------------------------------
 *  Last Modify: 2021/4/21
 *  Description: Measure Mode get data stream tools  on ROS/Linux
 *--------------------------------------------------------------------------------------------*/
#include "../ros_inc/tool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../ros_inc/macro.h"
#include "../ros_inc/serial.h"
#include "../ros_inc/protocol.h"

/*---------------------------------------------------------------------------------------------
 *  Function:    Align
 *  Last Modify: 2021/4/19
 *  Description:  measure data packet align on the serial stream
 *  Parameter:   nFD = serialport file desriptor, used by system call ,such as read,write
 *  Return:      32bit int = data packet length(bytes)           
 *--------------------------------------------------------------------------------------------*/
int Align(unsigned char nFD)
{
    unsigned char tmp = 0;
    //unsigned char tmp1 = 0;
    unsigned int uLen = 0;
    int packLength = 0;
    unsigned int tmp_i = 0;

    while(1)
    {
        uLen=read(nFD,&tmp,1);
        if(tmp == 0x6D)
        {
            uLen=read(nFD,&tmp,1);
            if(tmp == 0x41)
            {

                uLen=read(nFD,&tmp_i,4);
#ifndef MCU_ON
                //printf("Head :%x\n",tmp_i);
#endif
                if(tmp_i == 0x8106FF78)
                {
                    uLen=read(nFD,&tmp,1);
                    packLength = tmp;
#ifndef MCU_ON
                    //printf("Len :%d\n",packLength);
#endif
                    break;
                }
            }
        }
    }
#ifndef MCU_ON
    //printf(" find packet head\n");
#endif
    uLen = 0;
    return  packLength;
}

/*---------------------------------------------------------------------------------------------
 *  Function:    GetFrame
 *  Last Modify: 2021/4/21
 *  Description: Get a frame of  measure data packet
 *  Parameter:   nFD = serialport file desriptor, used by system call ,such as read,write
 *               tmpBuf = buffer store the frame of  measure data packet 
 *               frameLen = data packet length(bytes)
 *  Return:      32bit int =  data packet length(bytes) equal frameLen usually          
 *--------------------------------------------------------------------------------------------*/
int GetFrame(unsigned char nFD, unsigned char * tmpBuf, int frameLen){
    int pkgLen = frameLen;
    int uLen = 0;
    int tLen = 0;
    memset(tmpBuf,0,pkgLen);
    while(uLen < pkgLen) {
        tLen = read(nFD, tmpBuf+uLen, pkgLen-uLen);
        uLen += tLen;
    }
    return uLen;
}
/*---------------------------------------------------------------------------------------------
 *  Last Modify: 2021/4/27
 *  Description: Valid a frame of  measure data packet
 *  Parameter:   tmpBuf = buffer store the frame of  measure data packet 
 *               frameLen = data packet length(bytes)
 *  Return:      32bit int =  data packet length(bytes) equal frameLen usually          
 *--------------------------------------------------------------------------------------------*/
bool ValidFrame(unsigned char * tmpBuf,int frameLen)
{
    return  (tmpBuf[0] == 0x41) &
            (tmpBuf[1] == 0x78) &
            (tmpBuf[2] == 0xFF) &
            (tmpBuf[frameLen-1] == 0x6D);
}

/*---------------------------------------------------------------------------------------------
 *  Function:    FillFrameHead
 *  Last Modify: 2021.4.21
 *  Description: Fill the frame head of  measure data packet losted by Align, using couple of  
 *               Align(unsigned char nFD),such as:
 *               
 *               Align(nFD);
 *               FillFrameHead(dataBuf);
 *                               
 *  Parameter:   tmpBuf = buffer store the frame of  measure data packet 
 *               frameLen = data packet length(bytes)
 *  Return:      32bit int =  data packet length(bytes) equal frameLen usually          
 *--------------------------------------------------------------------------------------------*/
int  FillFrameHead(unsigned char * tmpBuf)
{
    tmpBuf[0] = 0x41;
    tmpBuf[1] = 0x78;
    tmpBuf[2] = 0xFF;
    tmpBuf[3] = 0x06;
    tmpBuf[4] = 0x81;
    return 0;
}

/*---------------------------------------------------------------------------------------------
 *  Function:    ParserDataPacket
 *  Last Modify: 2021.4.21
 *  Description: Parser a frame of  measure data packet
 *  Parameter:   DataHandle = data struct of all  measure data type,such as, linear_acc... 
 *               pBuffer = data buffer
 *               dataLen = data payload length(bytes)
 *               fpLog = a log file ,optional
 *  Return:      nothing        
 *--------------------------------------------------------------------------------------------*/

void ParserDataPacket(Data *DataHandle,u8 *pBuffer, u16 dataLen, FILE *fpLog)
{
    u16 PID = 0;
    u8 *pData = pBuffer;
    u8 index = 0;
    u8 pl = 0;

    //reset DataHandle
    memset(DataHandle, 0, sizeof(DataHandle));
    //printf("\n");
    while (index < dataLen)
    {
        PID = ((*((u16*)(pData + index))) & 0x7fff);
        pl = *(pData + index + 2);

        if (PID == (SESSION_NAME_TEMPERATURE))
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->temperature.data, pData + index, PL_TEMPERTURE);
            DataHandle->temperature.dataID = PID;
            DataHandle->temperature.dataLen = pl;
            //printf(" *** temperature:\t%11.4f *** \n", DataHandle->temperature.data);

            index += PL_TEMPERTURE;

        }
        else if (PID == (SESSION_NAME_RAW_ACC))
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->accRawData.accX, pData + index, PL_RAW_DATA);
            DataHandle->accRawData.dataID = PID;
            DataHandle->accRawData.dataLen = pl;

            index += PL_RAW_DATA;

        }
        else if (PID == SESSION_NAME_RAW_GYRO)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->gyroRawData.gyroX, pData + index, PL_RAW_DATA);
            DataHandle->gyroRawData.dataID = PID;
            DataHandle->gyroRawData.dataLen = pl;
            index += PL_RAW_DATA;
        }
        else if (PID == SESSION_NAME_RAW_MAG)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->magRawData.magX, pData + index, PL_RAW_DATA);
            DataHandle->magRawData.dataID = PID;
            DataHandle->magRawData.dataLen = pl;
            index += PL_RAW_DATA;
        }
        else if (PID == SESSION_NAME_CAL_ACC)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->accCal.accX, pData + index, PL_CAL_DATA);
            DataHandle->accCal.dataID = PID;
            DataHandle->accCal.dataLen = pl;
            index += PL_CAL_DATA;
            if(fpLog!=NULL)
                fprintf(fpLog," *** accCal:     \t%11.4f, %11.4f, %11.4f *** \n", DataHandle->accCal.accX, DataHandle->accCal.accY, DataHandle->accCal.accZ);
        }
        else if (PID == SESSION_NAME_CAL_GYRO)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->gyroCal.gyroX, pData + index, PL_CAL_DATA);

            DataHandle->gyroCal.dataID = PID;
            DataHandle->gyroCal.dataLen = pl;
            index += PL_CAL_DATA;
            if(fpLog!=NULL)
                fprintf(fpLog," *** gyroCal:    \t%11.4f, %11.4f, %11.4f *** \n", DataHandle->gyroCal.gyroX, DataHandle->gyroCal.gyroY, DataHandle->gyroCal.gyroZ);
        }
        else if (PID == SESSION_NAME_CAL_MAG)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->magCal.magX, pData + index, PL_CAL_DATA);
            DataHandle->magCal.dataID = PID;
            DataHandle->magCal.dataLen = pl;
            index += PL_CAL_DATA;

            //printf(" *** magCal:     \t%11.4f, %11.4f, %11.4f *** \n", DataHandle->magCal.magX, DataHandle->magCal.magY, DataHandle->magCal.magZ);
        }
        else if (PID == SESSION_NAME_KAL_ACC)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->accKal.accX, pData + index, PL_KAL_DATA);
            DataHandle->accKal.dataID = PID;
            DataHandle->accKal.dataLen = pl;
            index += PL_KAL_DATA;
            if(fpLog!=NULL)
                fprintf(fpLog," *** accKal:     \t%11.4f, %11.4f, %11.4f *** \n", DataHandle->accKal.accX, DataHandle->accKal.accY, DataHandle->accKal.accZ);
        }
        else if (PID == SESSION_NAME_KAL_GYRO)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->gyroKal.gyroX, pData + index, PL_KAL_DATA);
            DataHandle->gyroKal.dataID = PID;
            DataHandle->gyroKal.dataLen = pl;
            index += PL_KAL_DATA;
        }
        else if (PID == SESSION_NAME_KAL_MAG)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->magKal.magX, pData + index, PL_KAL_DATA);
            DataHandle->magKal.dataID = PID;
            DataHandle->magKal.dataLen = pl;
            index += PL_KAL_DATA;
        }
            //////////////////////////
        else if (PID == SESSION_NAME_QUAT)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->quat.Q0.uint_x, pData + index, PL_QUAT_EULER);
            DataHandle->quat.dataID = PID;
            DataHandle->quat.dataLen = pl;
            index += PL_QUAT_EULER;
            if(fpLog!=NULL)
                fprintf(fpLog," *** quat :      \t%11.4f, %11.4f, %11.4f, %11.4f *** \n", DataHandle->quat.Q0.float_x, DataHandle->quat.Q1.float_x, DataHandle->quat.Q2.float_x, DataHandle->quat.Q3.float_x);

        }
        else if (PID == SESSION_NAME_EULER)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->euler.roll, pData + index, PL_QUAT_EULER);
            DataHandle->euler.dataID = PID;
            DataHandle->euler.dataLen = pl;
            index += PL_QUAT_EULER;

            //printf(" *** euler:      \t%11.4f, %11.4f, %11.4f *** \n", DataHandle->euler.roll, DataHandle->euler.pitch, DataHandle->euler.yaw);
        }

        else if (PID == SESSION_NAME_ROTATION_M)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->romatix.a, pData + index, PL_MATERIX);
            DataHandle->romatix.dataID = PID;
            DataHandle->romatix.dataLen = pl;
            index += PL_MATERIX;

        }

        else if (PID == SESSION_NAME_LINEAR_ACC)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->accLinear.accX, pData + index, PL_LINEAR_ACC_DATA);
            DataHandle->accLinear.dataID = PID;
            DataHandle->accLinear.dataLen = pl;
            index += PL_LINEAR_ACC_DATA;
            if(fpLog!=NULL)
                fprintf(fpLog," *** lin_acc:     \t%11.4f, %11.4f, %11.4f *** \n", DataHandle->accLinear.accX, DataHandle->accLinear.accY, DataHandle->accLinear.accZ);
        }

        else if (PID == SESSION_NAME_DELTA_T)
        {
            //Ignore pid and pl
            index += 3;
            memcpy(&DataHandle->dt.DT, pData + index, PL_DT_DATA);

            DataHandle->dt.dataID = PID;
            DataHandle->dt.dataLen = pl;
            index += PL_DT_DATA;
        }

        else if (PID == SESSION_NAME_OS_TIME)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->tick.OS_Time_ms, pData+index, PL_OS_REFERENCE_TIME-2); //first 4 bytes are miliseconds
            DataHandle->tick.OS_Time_ms = *((u32*)(pData + index));
            DataHandle->tick.OS_Time_us = *((u16*)(pData + index + 4));

            DataHandle->tick.dataID = PID;
            DataHandle->tick.dataLen = pl;
            index += PL_OS_REFERENCE_TIME;
        }
        else if (PID == SESSION_NAME_STATUS_WORD)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->status.status, pData + index, PL_STATUS);
            DataHandle->status.dataID = PID;
            DataHandle->status.dataLen = pl;
            index += PL_STATUS;
        }
        else if (PID == SESSION_NAME_PACKET_COUNTER)
        {
            //Ignore pid and pl
            index += 3;

            memcpy(&DataHandle->packetCounter.packerCounter, pData + index, PL_PACKET_NUMBER);
            DataHandle->packetCounter.dataID = PID;
            DataHandle->packetCounter.dataLen = pl;
            index += PL_PACKET_NUMBER;

            if(fpLog!=NULL)
                fprintf(fpLog," *** packet_count:  %d, *** \n", DataHandle->packetCounter.packerCounter);
        }
    }
}