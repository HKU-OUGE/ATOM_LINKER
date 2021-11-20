/******************** (C) COPYRIGHT 2021 SINGER *********************
* File Name          : ATOM_PROTOCOL.h
* Author             : SINGER
* Date First Issued  : 2021
* Description        : 
********************************************************************************
* History:

* 2021: V1.0

********************************************************************************/

#include "time.h"
#include "stdio.h"
#include "atom_macro.h"
#include "atomprotocol.h"
#include "decode.h"
#include "fifo.h"

#define COMPORT_COMMUNICATION
#define DMP_OUTPUT

#define THREADCOUNT 1
extern u8 buff[1024];
extern UART_HandleTypeDef huart1;
char vCombuf[1024];

SaberData_HandleType saberDataHandle;
extern MidProtocol_StructType UartHandle;
extern char temp_data;

unsigned short usLength = 0, usRxLength = 0;
extern u8 receiveAck ;
extern u8 receiveCid ;
extern u8 receiveMid ;
extern u8 errorCode_Ack;
u8 ReDataFlag = 0;
//int index = 0 ;
//int wait = 0;
//int count = 0;
u8 reTx_flag = 0 ;
u8 timeout_count = 0 ;
char timeoutBuf[] = "-----------time out!------------\n";


char waitAck(void (*pFunction)(char x),u8 cid,u8 mid,char parameter)
{
	
	HAL_Delay(100);
	while(1)
	{
           if( ( receiveAck == 1 ) && (receiveCid == cid) && (receiveMid == (mid | 0x80 )) )
           break;
                reTx_flag++;
                 
		/*------------------Timeout processing--------------*/
		if (reTx_flag == 20)					                                                //Retransmission after 2s
		{
			reTx_flag = 0;
			pFunction(parameter);
			timeout_count++;
			if (timeout_count == 4)										//Unable to receive after 4 retransmissions
			{
                              HAL_UART_Transmit_IT(&huart1, timeoutBuf, sizeof(timeoutBuf));
                              HAL_Delay(100);                                                                               
                              return 0;
                         }
		}
		HAL_Delay(100);
	}	
	if (errorCode_Ack != 0)
	{
		/*---------User Code Begin-------------*/
		//errorCode processing
                //return 0 or 1?
                 
		/*---------User Code End-------------*/
	}
        receiveAck = 0;
        receiveCid = 0;
        receiveMid = 0;
	timeout_count = 0;
	return 1;
}
char send()
{
	u8 ret = 0;
	ReDataFlag = 0;
        /****************************** Switch to config mode ********************************************/
	Atom_switchModeReq(CONFIG_MODE);
        ret = waitAck(Atom_switchModeReq,CLASS_ID_OPERATION_CMD , CMD_ID_SWITCH_TO_CFG_MODE,CONFIG_MODE);
        if( ret == 0 )
        {
          return ret;
        }
        /****************************** Switch to config mode ********************************************/
        
        /****************************** Select  packet***************************************************************/
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
        ret = waitAck(SelectPackets,CLASS_ID_HOSTCONTROL_CMD , CMD_ID_SET_DATA_PAC_CFG,bitEnable);
        if( ret == 0 )
        {
          return ret;
        }
	/****************************** Select  packet***************************************************************/
        
	/****************************** Set host update rate***************************************************************/
	SetHostUpdateRate(RATE_50);
        ret = waitAck(SetHostUpdateRate,CLASS_ID_ALGORITHMENGINE , CMD_ID_SET_PACKET_UPDATE_RATE,RATE_50);
        if( ret == 0 )
        {
          return ret;
        }
        /****************************** Set host update rate***************************************************************/
        
	/****************************** Switch To measure mode***************************************************************/
	Atom_switchModeReq(MEASURE_MODE);
	ret = waitAck(Atom_switchModeReq,CLASS_ID_OPERATION_CMD , CMD_ID_SWITCH_TO_MEASURE_MODE/*CMD_ID_SWITCH_TO_CFG_MODE*/,MEASURE_MODE);
	if( ret == 0 )
	{
		return ret;
	}
	/****************************** Switch To measure mode***************************************************************/
	ReDataFlag = 1;
	return ret;

}

void DataPacketParser(Buffer_Type *HOST_Buffer, u16 dataLen)
{
	u16 PID = 0;
	u8 *pData;
	
	pData = buff;
	u8 index = 6;
	u8 pl = 0;
	u32 BufferSur = 0;
	u32 DataOver = 0;
	
	//reset saberDataHandle
	memset(buff, 0, 1024);
	BufferSur = 1024 - HOST_Buffer->send_pt_count; 
	DataOver = dataLen - BufferSur;
	
	if( dataLen > BufferSur )
	{
		memcpy(buff,HOST_Buffer->send_pt, BufferSur);
		memcpy(buff + BufferSur,HOST_Buffer->buffer,DataOver);
	}
	else
		memcpy(buff,HOST_Buffer->send_pt,dataLen);
	
	while (index < dataLen-2)
	{
          
                PID = ((pData[index]+(pData[index+1]<<8))& 0x7fff);
		pl = *(pData + index + 2);
                
		if (PID == (SESSION_NAME_TEMPERATURE))
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.temperature.data, pData + index, PL_TEMPERTURE);
			saberDataHandle.temperature.dataID = PID;
			saberDataHandle.temperature.dataLen = pl;
                        
                        /* USER CODE BEGIN 4 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150); 
                        */
                        
                        /* USER CODE END 4 */
                       
                        
                        index += PL_TEMPERTURE;

		}
		else if (PID == (SESSION_NAME_RAW_ACC))
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.accRawData.accX, pData + index, PL_RAW_DATA);
			saberDataHandle.accRawData.dataID = PID;
			saberDataHandle.accRawData.dataLen = pl;
                        
                        /* USER CODE BEGIN 5 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150);
                        */
                        /* USER CODE END 5 */
                        
                        index += PL_RAW_DATA;
              

		}
		else if (PID == SESSION_NAME_RAW_GYRO)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.gyroRawData.gyroX, pData + index, PL_RAW_DATA);
			saberDataHandle.gyroRawData.dataID = PID;
			saberDataHandle.gyroRawData.dataLen = pl;
			
                        /* USER CODE BEGIN 6 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150); 
                        */
                        /* USER CODE END 6 */
                                              
                        index += PL_RAW_DATA;
		}
		else if (PID == SESSION_NAME_RAW_MAG)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.magRawData.magX, pData + index, PL_RAW_DATA);
			saberDataHandle.magRawData.dataID = PID;
			saberDataHandle.magRawData.dataLen = pl;
			
                        /* USER CODE BEGIN 7 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150);
                        */
                        /* USER CODE END 7 */
                       
                        
                        index += PL_RAW_DATA;
		}
		else if (PID == SESSION_NAME_CAL_ACC)
		{
         		//Ignore pid and pl
                        index += 3;
                        memcpy(&saberDataHandle.accCal.accX, pData + index, PL_CAL_DATA);
                        saberDataHandle.accCal.dataID = PID;
			saberDataHandle.accCal.dataLen = pl;
			
	/*		sprintf(vCombuf, "Acc_Cal:\t%-12.3f\t%-12.3f\t%-12.3f\n",saberDataHandle.accCal.accX,saberDataHandle.accCal.accY,saberDataHandle.accCal.accZ);
			HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
			HAL_Delay(100);
			memset(vCombuf,'\0',150);*/
			
			index += PL_CAL_DATA;
		}
		else if (PID == SESSION_NAME_CAL_GYRO)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.gyroCal.gyroX, pData + index, PL_CAL_DATA);
                        saberDataHandle.gyroCal.dataID = PID;
			saberDataHandle.gyroCal.dataLen = pl;
			
	/*		sprintf(vCombuf, "Gyro_Cal:\t%-12.3f\t%-12.3f\t%-12.3f\n",saberDataHandle.gyroCal.gyroX,saberDataHandle.gyroCal.gyroY,saberDataHandle.gyroCal.gyroZ);
			HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
			HAL_Delay(100);
			memset(vCombuf,'\0',150);*/
			
			index += PL_CAL_DATA;
                        
		}	
		else if (PID == SESSION_NAME_CAL_MAG)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.magCal.magX, pData + index, PL_CAL_DATA);
			saberDataHandle.magCal.dataID = PID;
			saberDataHandle.magCal.dataLen = pl;
                        
			/* USER CODE BEGIN 8 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150);
                        */
                        /* USER CODE END 8 */
                        
                        index += PL_CAL_DATA;
		}
		else if (PID == SESSION_NAME_KAL_ACC)
		{
			//Ignore pid and pl
			index += 3;

                        memcpy(&saberDataHandle.accKal.accX, pData + index, PL_KAL_DATA);
			saberDataHandle.accKal.dataID = PID;
			saberDataHandle.accKal.dataLen = pl;
			
      
                       /* USER CODE BEGIN 9 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150);
                        */
                        /* USER CODE END 9 */
                        
                        index += PL_KAL_DATA;
		}
		else if (PID == SESSION_NAME_KAL_GYRO)
		{
			//Ignore pid and pl
			index += 3 ;

			memcpy(&saberDataHandle.gyroKal.gyroX, pData + index, PL_KAL_DATA);
			saberDataHandle.gyroKal.dataID = PID;
			saberDataHandle.gyroKal.dataLen = pl;
                        
			/* USER CODE BEGIN 10 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150);
                        */
                        /* USER CODE END 10 */
                        
                        index += PL_KAL_DATA;
		}
		else if (PID == SESSION_NAME_KAL_MAG)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.magKal.magX, pData + index, PL_KAL_DATA);
			saberDataHandle.magKal.dataID = PID;
			saberDataHandle.magKal.dataLen = pl;
			
                        /* USER CODE BEGIN 11 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150);
                        */
                        /* USER CODE END 11 */
                        
                        index += PL_KAL_DATA;
		}
		else if (PID == SESSION_NAME_QUAT)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.quat.Q0.uint_x, pData + index, PL_QUAT_EULER);
			saberDataHandle.quat.dataID = PID;
			saberDataHandle.quat.dataLen = pl;
			
/*			sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
			HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
			HAL_Delay(100);
			memset(vCombuf,'\0',150);*/
			
			index += PL_QUAT_EULER;                        
		}
		else if (PID == SESSION_NAME_EULER)
		{
			//Ignore pid and pl
			index += 3;
                        memcpy(&saberDataHandle.euler.roll, pData + index, PL_QUAT_EULER);
                        saberDataHandle.euler.dataID = PID;
			saberDataHandle.euler.dataLen = pl;
                        
		                       
/*			sprintf(vCombuf, "Euler:\t%-12.3f\t%-12.3f\t%-12.3f\n",saberDataHandle.euler.roll,saberDataHandle.euler.pitch,saberDataHandle.euler.yaw);
			HAL_UART_Transmit_IT(&huart1, vCombuf, 150);                        
			HAL_Delay(100);
			memset(vCombuf,'\0',150);*/
                        
                        index += PL_QUAT_EULER;
		}

		else if (PID == SESSION_NAME_ROTATION_M)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.romatix.a, pData + index, PL_MATERIX);
			saberDataHandle.romatix.dataID = PID;
			saberDataHandle.romatix.dataLen = pl;
			                        
                       /* USER CODE BEGIN 12 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Quat:\t%-12.3f \t%-12.3f\t %-12.3f\t%-12.3f\n",saberDataHandle.quat.Q0.float_x,saberDataHandle.quat.Q1.float_x,saberDataHandle.quat.Q2.float_x,saberDataHandle.quat.Q3.float_x);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150);
                        */
                        /* USER CODE END 12 */
                        
                        index += PL_MATERIX;
		}

		else if (PID == SESSION_NAME_LINEAR_ACC)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.accLinear.accX, pData + index, PL_LINEAR_ACC_DATA);
			saberDataHandle.accLinear.dataID = PID;
			saberDataHandle.accLinear.dataLen = pl;
			
                        /* USER CODE BEGIN 13 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Acc_Cal_X : %-8.3f\tAcc_Cal_Y  : %-8.3f\tAcc_Cal_Z : %-8.3f\n",saberDataHandle.accCal.accX,saberDataHandle.accCal.accY,saberDataHandle.accCal.accZ);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150); 
                        */
                        /* USER CODE END 13 */ 
                        
                        
                        index += PL_LINEAR_ACC_DATA;
		}
		else if (PID == SESSION_NAME_PACKET_COUNTER)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.packetCounter.packerCounter, pData + index, PL_PACKET_NUMBER);
			saberDataHandle.packetCounter.dataID = PID;
			saberDataHandle.packetCounter.dataLen = pl;
			
                        /* USER CODE BEGIN 14 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Acc_Cal_X : %-8.3f\tAcc_Cal_Y  : %-8.3f\tAcc_Cal_Z : %-8.3f\n",saberDataHandle.accCal.accX,saberDataHandle.accCal.accY,saberDataHandle.accCal.accZ);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150); 
                        */
                        /* USER CODE END 14 */
                       
                        
                        index += PL_PACKET_NUMBER;
		}
		else if (PID == SESSION_NAME_DELTA_T)
		{
			//Ignore pid and pl
			index += 3;
                        
			memcpy(&saberDataHandle.dt.DT, pData + index, PL_DT_DATA);
			saberDataHandle.dt.dataID = PID;
			saberDataHandle.dt.dataLen = pl;
			index += PL_DT_DATA;
                         
                        /* USER CODE BEGIN 15 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Acc_Cal: %-8.3f\t %-8.3f\t %-8.3f\n",saberDataHandle.accCal.accX,saberDataHandle.accCal.accY,saberDataHandle.accCal.accZ);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150); 
                        */
                        /* USER CODE END 15 */
                        
                        index += PL_DT_DATA;
                }
		else if (PID == SESSION_NAME_OS_TIME)
		{
			//Ignore pid and pl
			index += 3;
                        
			memcpy(&saberDataHandle.tick.OS_Time_ms, pData+index, PL_OS_REFERENCE_TIME-2); //first 4 bytes are miliseconds
			saberDataHandle.tick.OS_Time_ms = *((u32*)(pData + index));
			saberDataHandle.tick.OS_Time_us = *((u16*)(pData + index + 4));

			saberDataHandle.tick.dataID = PID;
			saberDataHandle.tick.dataLen = pl;
			
                        /* USER CODE BEGIN 16 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Acc_Cal : %-8.3f\t %-8.3f\t : %-8.3f\n",saberDataHandle.accCal.accX,saberDataHandle.accCal.accY,saberDataHandle.accCal.accZ);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150); 
                        */
                        /* USER CODE END 16 */
                        
                        
                        index += PL_OS_REFERENCE_TIME;
		}
		else if (PID == SESSION_NAME_STATUS_WORD)
		{
			//Ignore pid and pl
			index += 3;

			memcpy(&saberDataHandle.status.status, pData + index, PL_STATUS);
			saberDataHandle.status.dataID = PID;
			saberDataHandle.status.dataLen = pl;
			
                        /* USER CODE BEGIN 17 */
                        
                        /* User can add code according to their own needs,
                          ex: 
                            sprintf(vCombuf, "Acc_Cal_X : %-8.3f\tAcc_Cal_Y  : %-8.3f\tAcc_Cal_Z : %-8.3f\n",saberDataHandle.accCal.accX,saberDataHandle.accCal.accY,saberDataHandle.accCal.accZ);
                            HAL_UART_Transmit_IT(&huart1, vCombuf, 150);
                            HAL_Delay(100);
                            memset(vCombuf,'\0',150); 
                        */
                        /* USER CODE END 17 */
                       
                        
                        index += PL_STATUS;
		}
                else
                  index++;
              
	}

}

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


void SetHostUpdateRate(char rate)
{
	u8 data[2];
	data[0] = rate & 0xff;
	data[1] = (rate & 0xff00) >> 8;
	Atom_setPacktUpdateRateReq((u8*)&data, 2);


}
