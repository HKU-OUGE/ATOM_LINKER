/*---------------------------------------------------------------------------------------------
 *  Last Modify: 2021.4.18
 *  Description:  Demo Project on ROS
 *--------------------------------------------------------------------------------------------*/

#include "main.h"
//std include
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

//ros include
#ifdef ROS_ON
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>
#include <ros/time.h>
#endif

//usr include
#include "_ros_inc/_serial.h"
#include "_ros_inc/_macro.h"
#include "_ros_inc/_protocol.h"
#include "_ros_inc/_config.h"
#include "_ros_inc/_tool.h"

using namespace std;

//global variant
Data DataHandle;
ros::Publisher pub;
sensor_msgs::Imu imuMsg;

//Warning:Don't use malloc to get buffer,Just use a global array;
unsigned char frmBuf[256] = { 0 };

//main
int main(int argc, char **argv){
    unsigned char nFD = 0;
    int packLengthFW = 0;
    int pkgLen = 0;
    int pubCnt = 0;
    FILE *fpLog = NULL;
    u8 ret = 0;
    int seq = 0;
    bool met;
    int cycleCnt = 0;
    int errCnt = 0;

    unsigned char * dataBuf = NULL;
    dataBuf = &frmBuf[0];

#ifndef MCU_ON
    printf("Hello, on ROS!\n");
#endif

#ifdef ROS_ON
    ros::init(argc, argv, "main");
    ros::NodeHandle n;
    pub = n.advertise<sensor_msgs::Imu>("Imu_data", 20);
#endif

    if(dataBuf == NULL){
#ifndef MCU_ON
        printf("Data buffer %d bytes valid fail \n", pkgLen);
#endif
        return -2;
    }

    //step 1: read config,open serialport,get serialport file desriptor
    //be careful to choose file path
    nFD = InitConfig("cfg.json");
    //option: a log file
    fpLog = fopen("imu.log","w");
    
    //step 2: align  data frame from the serial stream
    packLengthFW = Align(nFD);
    pkgLen = packLengthFW + _EMPTY_LEN;
    FillFrameHead(dataBuf);

    //step 3:get a whole frame and valid the frame     
    GetFrame(nFD, dataBuf+_HEAD_LEN,packLengthFW+_TAIL_LEN);
    while(!ValidFrame(dataBuf,pkgLen)){
        packLengthFW = Align(nFD);
        FillFrameHead(dataBuf);
        GetFrame(nFD, dataBuf+_HEAD_LEN,packLengthFW+_TAIL_LEN);
    }

    //get a whole frame and valid the frame by ros::rate,such as 100hz
#ifdef ROS_ON
    ros::Rate r(100);
    while (ros::ok())
#else
    while(1)
#endif
    {
        //FillFrameHead(dataBuf);
        GetFrame(nFD, dataBuf,packLengthFW+_EMPTY_LEN);

        while(!ValidFrame(dataBuf,pkgLen)){
            packLengthFW = Align(nFD);
            FillFrameHead(dataBuf);
            GetFrame(nFD, dataBuf+_HEAD_LEN,packLengthFW+_TAIL_LEN);
            errCnt++;
        }
        //step 4:parser a whole frame to generate ros publish data 
        ParserDataPacket(&DataHandle, &dataBuf[_HEAD_LEN], packLengthFW,fpLog);
#ifdef ROS_ON
        imuMsg.linear_acceleration.x = DataHandle.accLinear.accX;
        imuMsg.linear_acceleration.y = DataHandle.accLinear.accY;
        imuMsg.linear_acceleration.z = DataHandle.accLinear.accZ;

        imuMsg.angular_velocity.x = DataHandle.gyroCal.gyroX*DEG_RAD;
        imuMsg.angular_velocity.y = DataHandle.gyroCal.gyroY*DEG_RAD;
        imuMsg.angular_velocity.z = DataHandle.gyroCal.gyroZ*DEG_RAD;

        imuMsg.orientation.w = DataHandle.quat.Q0.float_x;
        imuMsg.orientation.x = DataHandle.quat.Q1.float_x;
        imuMsg.orientation.y = DataHandle.quat.Q2.float_x;
        imuMsg.orientation.z = DataHandle.quat.Q3.float_x;

        imuMsg.header.stamp= ros::Time::now();
        imuMsg.header.frame_id = "imu_link";
        imuMsg.header.seq = seq;
        seq = seq + 1;
        pub.publish(imuMsg);
        pubCnt++;
        ROS_INFO(" *** publish_count: %d, *** \n", pubCnt);

        ros::spinOnce();
        met = r.sleep();
#endif
        //4000 frame to exit, user should customize the value or bypass for a infinite loop 
        if((++cycleCnt)>4000)
            break;
    }
    if(nFD>0){
        _CloseSerialPort(nFD);
    }
    if(fpLog!=NULL)
    fclose(fpLog);
#ifndef MCU_ON
    printf("%d Frames record,%d  Interrupt Frames\n",cycleCnt-1,errCnt);
#endif
    return 0;
}

