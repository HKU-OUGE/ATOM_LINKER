/*---------------------------------------------------------------------------------------------
*  Last Modify: 2021.4.20
 *  Version:     1.1
 *  Description: Config using a JSON file on ROS/Linux
 *--------------------------------------------------------------------------------------------*/
#include "../ros_inc/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "../config_tool/cJSON.h"
#include "../ros_inc/macro.h"
#include "../ros_inc/serial.h"
#include "../ros_inc/protocol.h"

/*---------------------------------------------------------------------------------------------
 *  Function:    InitConfig
 *  Last Modify: 2021.4.20
 *  Description: Get config from a JSON file and download the config 
 *  Parameter:   fileName = JSON config file path
 *  Return:      8bit unsigned char
 *               nFD = serialport file desriptor, used by system call ,such as read,write
 *--------------------------------------------------------------------------------------------*/

signed char  InitConfig(const char *fileName){

    int status = 0;
    const cJSON *name = NULL;
    const char * pJSONData = cJSON_
        FileToMem(fileName); //"/home/atom/catkin_ws/devel/lib/imu_publish/cfg.json")
    cJSON *monitor_json = cJSON_Parse(pJSONData);
    int cfgIndex = 0;
    int baudRate = 0;
    int dataPktLen = 0;
    unsigned char nFD = 0;
    u16 pid = 0;

#ifdef ROS_ON
    ConfigSingleDataPacket cfgPacket[DATAPACKET_CNT_ROS];
#else
    ConfigSingleDataPacket cfgPacket[DATAPACKET_CNT];
#endif

    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
#ifndef MCU_ON
            printf("Error before: %s\n", error_ptr);
#endif
        }
        status = -1;
        return status;
    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "BAUDRATE");
    if (cJSON_IsNumber(name)) {
        baudRate = name->valueint;
    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "COM_NAME");
    if (cJSON_IsString(name))
    {
#ifndef MCU_ON
        printf("Connet Serial Port: %s,with baudrate:%d \n", name->valuestring, baudRate);
#endif
        nFD = OpenCom(name->valuestring,baudRate);
        if(nFD<0){
            return -1;
        }
    }

    SwitchModeReq(nFD, CONFIG_MODE);


    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "LINEAR_ACC");
    if (cJSON_IsTrue(name))
    {
#ifndef MCU_ON
        printf("Linear acceleration data enable\n");
#endif
        pid = SESSION_NAME_LINEAR_ACC | 0x8000;
        cfgPacket[cfgIndex].reserve0 = 0xff;
        cfgPacket[cfgIndex].reserve1 = 0xff;
        cfgPacket[cfgIndex].packetID = pid;
        cfgIndex++;
        dataPktLen+= LINEARACC_LEN;
    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "ANGULAR_VELOCITY");
    if (cJSON_IsTrue(name))
    {
#ifndef MCU_ON
        printf("Angular velocity data enable \n");
#endif
        pid = SESSION_NAME_CAL_GYRO | 0x8000;
        cfgPacket[cfgIndex].reserve0 = 0xff;
        cfgPacket[cfgIndex].reserve1 = 0xff;
        cfgPacket[cfgIndex].packetID = pid;
        cfgIndex++;
        dataPktLen+= ANGULAR_VELOCITY_LEN;
    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "ORIENTATION");
    if (cJSON_IsTrue(name))
    {
#ifndef MCU_ON
        printf("Quaternion data enable \n");
#endif
        pid = SESSION_NAME_QUAT | 0x8000;
        cfgPacket[cfgIndex].reserve0 = 0xff;
        cfgPacket[cfgIndex].reserve1 = 0xff;
        cfgPacket[cfgIndex].packetID = pid;
        cfgIndex++;
        dataPktLen+= ORIENTION_LEN;
    }
	
	name = cJSON_GetObjectItemCaseSensitive(monitor_json, "EULER");
    if (cJSON_IsTrue(name))
    {
#ifndef MCU_ON
        printf("EULER data enable \n");
#endif
        pid = SESSION_NAME_EULER | 0x8000;
        cfgPacket[cfgIndex].reserve0 = 0xff;
        cfgPacket[cfgIndex].reserve1 = 0xff;
        cfgPacket[cfgIndex].packetID = pid;
        cfgIndex++;
        dataPktLen+= ORIENTION_LEN;
    }


#ifndef ROS_ON
    //Add other data packet ,such as raw data, temperature, packet cnt, eulur data...
    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "ACC_RAW");
    if (cJSON_IsTrue(name))
    {
#ifndef MCU_ON
        printf("Acceleration rawdata enable \n");
#endif
        pid = SESSION_NAME_RAW_ACC | 0x8000;
        cfgPacket[cfgIndex].reserve0 = 0xff;
        cfgPacket[cfgIndex].reserve1 = 0xff;
        cfgPacket[cfgIndex].packetID = pid;
        cfgIndex++;
        dataPktLen+= ACC_RAW_LEN;

    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "GYRO_RAW");
    if (cJSON_IsTrue(name))
    {
        printf("Gyroscope rawdata enable \n");
        pid = SESSION_NAME_RAW_GYRO | 0x8000;
        cfgPacket[cfgIndex].reserve0 = 0xff;
        cfgPacket[cfgIndex].reserve1 = 0xff;
        cfgPacket[cfgIndex].packetID = pid;
        cfgIndex++;
        dataPktLen+= GYRO_RAW_LEN;
    }
#ifndef MCU_ON
    printf("Packet total length is  %d \n", dataPktLen);
#endif

#endif
    SetDataPacketConfigReq(nFD, (u8 *) &cfgPacket, cfgIndex * 4);
SwitchModeReq(nFD, MEASURE_MODE);
    /*TODO:free pJSONData*/
    if(pJSONData != NULL){
        free((void *)pJSONData);
    }
    return nFD;
}

