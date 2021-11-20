
#ifndef MACRO_H
#define MACRO_H

/* - MACRO definition start here -*/
#define COMPORT_COMMUNICATION
#define DMP_OUTPUT
#define THREADCOUNT 1
#define LINEAR_ACC
#define DEG_RAD     0.01745329252
#define ACCG_NMS   9.7803185
#define PACK_LENGTH 79

/*
 ************************************************************************
 * Definition for Protocol
 ************************************************************************
 */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
#define _RELEASE



#define ATOM_HEADER_LEN                         6
#define ATOM_CRC_LEN                            1
#define ATOM_FOOTER_LEN                         1
#define ATOM_CMDLEN_NO_PAYLOAD                  (ATOM_HEADER_LEN + ATOM_CRC_LEN + ATOM_FOOTER_LEN)


#define FRAME_MIN_SIZE                          ATOM_CMDLEN_NO_PAYLOAD
#define FRAME_MAX_PAYLOAD_SIZE                  0xFE
#define FRAME_MAX_SIZE                          (FRAME_MAX_PAYLOAD_SIZE + ATOM_CMDLEN_NO_PAYLOAD)

#define FRAME_LENGTH                            0xc
#define ATOM_PAYLOAD_INDEX                      6
#define ATOM_PAYLOAD_LEN_INDEX                  5
#define ATOM_MID_INDEX                          4
#define ATOM_CLASS_INDEX                        3
#define ATOM_MADDR_INDEX                        2

#define FRAME_PREAMBLE1                         0x41
#define FRAME_PREAMBLE2                         0x78



#define CLASS_ID_OPERATION_CMD                  0x01
#define CLASS_ID_DEVICE_INFO                    0x02
#define CLASS_ID_SENSOR_CONFIG                  0x03
#define CLASS_ID_ALGORITHMENGINE                0x04
#define CLASS_ID_COMMUNICATION_CONFIG           0x05
#define CLASS_ID_HOSTCONTROL_CMD                0x06
#define CLASS_ID_FIRMWARE_UPDATE                0x0A


	//class 0x01 CLASS_ID_OPERATION_CMD command
#define CMD_ID_WAKEUP_HOST                      0x01
#define CMD_ID_SWITCH_TO_CFG_MODE               0x02
#define CMD_ID_SWITCH_TO_MEASURE_MODE           0x03
#define CMD_ID_SYS_RESET                        0x04
#define CMD_ID_CFG_WRITE_TO_FLASH               0x08
#define CMD_ID_CFG_LOAD_FROM_FLASH              0x09
#define CMD_ID_CFG_LOAD_FACTORY                 0x0A


	//class 0x02 CLASS_ID_DEVICE_INFO command
#define CMD_ID_GET_FW_VERSION                   0x01
#define CMD_ID_GET_PRODUCT_VERSION              0x03
#define CMD_ID_GET_VENDER_INFO                  0x04
#define CMD_ID_GET_ALL_STATUS                   0x06
#define CMD_ID_GET_DEVCIE_INFO                  0x07
#define CMD_ID_SET_MODULEID                     0x0C
#define CMD_ID_GET_MODULEID                     0x0D

	//class 0x03 CLASS_ID_SENSOR_CONFIG command
#define CMD_ID_SET_ACC_SCALE                    0x01
#define CMD_ID_GET_ACC_SCALE                    0x02
#define CMD_ID_SET_ACC_COEFF                    0x03
#define CMD_ID_GET_ACC_COEFF                    0x04
#define CMD_ID_SET_ACC_CALIBRATION              0x05
#define CMD_ID_SET_GYRO_SCALE                   0x06
#define CMD_ID_GET_GYRO_SCALE                   0x07
#define CMD_ID_SET_MAG_SCALE                    0x0B
#define CMD_ID_GET_MAG_SCALE                    0x0C
#define CMD_ID_SET_MAG_COEFF                    0x0D
#define CMD_ID_GET_MAG_COEFF                    0x0E
#define CMD_ID_SET_MAG_CALIBRATION              0x0F
#define CMD_ID_SET_MUX2                         0x12
#define CMD_ID_GET_MUX2                         0x13
#define CMD_ID_MAG_CAL_PAC                      0xB0
#define CMD_ID_MAG_CAL_MSG                      0xB1


	//class 0x04 CLASS_ID_ALGORITHMENGINE command
#define CMD_ID_SET_ACC_1DEKF_PARAM              0x03
#define CMD_ID_GET_ACC_1DEKF_PARAM              0x04
#define CMD_ID_SET_GYRO_1DEKF_PARAM             0x05
#define CMD_ID_GET_GYRO_1DEKF_PARAM             0x06
#define CMD_ID_SET_MAG_1DEKF_PARAM              0x07
#define CMD_ID_GET_MAG_1DEKF_PARAM              0x08
#define CMD_ID_SET_ENGINE_FUSION_MODE           0x09
#define CMD_ID_GET_ENGINE_FUSION_MODE           0x0A
#define CMD_ID_SET_PACKET_UPDATE_RATE           0x10
#define CMD_ID_GET_PACKET_UPDATE_RATE           0x11
#define CMD_ID_SET_GYRO_AUTO_CALIBRATION        0x20
#define CMD_ID_GET_GYRO_AUTO_CALIBRATION        0x21
#define CMD_ID_SET_GYRO_THRESHOLD               0x22
#define CMD_ID_GET_GYRO_THRESHOLD               0x23
#define CMD_ID_SET_YAW_OFFSET                   0x30
#define CMD_ID_SET_ATTITUDE_OFFSET              0x31
#define CMD_ID_SET_REFERENCE_OFFSET             0x32
#define CMD_ID_CLEAR_OFFSET                     0x40

	//class 0x05 CLASS_ID_COMMUNICATION_CONFIG command
#define CMD_ID_SET_COMPORT_PARAM                0x01
#define CMD_ID_GET_COMPORT_PARAM                0x02
#define CMD_ID_SET_COMPORT_PARAM_HANDSHAKE          0x10

	//class 0x06 CLASS_ID_HOSTCONTROL_CMD command
#define CMD_ID__DATA_PACKET				0x81
#define CMD_ID_SET_DRDY_CONFIG                  0x06
#define CMD_ID_GET_DRDY_CONFIG                  0x07
#define CMD_ID_SET_GPIO_CONFIG                  0x08
#define CMD_ID_GET_GPIO_CONFIG                  0x09
#define CMD_ID_SET_DATA_PAC_CFG                 0x0A
#define CMD_ID_GET_DATA_PAC_CFG                 0x0B

	//class 0x0A CLASS_ID_FIRMWARE_UPDATE command
#define CMD_ID_FIRMWARE_UPDATE_REQ              0x01
#define CMD_ID_FIRMWARE_UPDATE_PACKET           0x02

#define SESSION_NAME_TEMPERATURE                0x0000
#define SESSION_NAME_RAW_ACC                    0x0400
#define SESSION_NAME_RAW_GYRO                   0x0401
#define SESSION_NAME_RAW_MAG                    0x0402
#define SESSION_NAME_CAL_ACC                    0x0800
#define SESSION_NAME_KAL_ACC                    0x0801
#define SESSION_NAME_LINEAR_ACC                 0x3800
#define SESSION_NAME_HEAVE_MOTION               0x0803
#define SESSION_NAME_DELTA_V                    0x0804
#define SESSION_NAME_CAL_GYRO                   0x0C00
#define SESSION_NAME_KAL_GYRO                   0x0C01
#define SESSION_NAME_DELTA_Q                    0x0C02

#define SESSION_NAME_CAL_MAG                    0x1000
#define SESSION_NAME_KAL_MAG                    0x1001

#define SESSION_NAME_QUAT                       0x3000
#define SESSION_NAME_EULER                      0x3001
#define SESSION_NAME_ROTATION_M                 0x3002

#define SESSION_NAME_PACKET_COUNTER             0x4000
#define SESSION_NAME_UTC_TIME                   0x4001
#define SESSION_NAME_OS_TIME                    0x4002
#define SESSION_NAME_SAMPLE_TIME_FINE           0x4003
#define SESSION_NAME_SAMPLE_TIME_COARSE         0x4004
#define SESSION_NAME_ITOW                       0x4005
#define SESSION_NAME_STATUS_WORD                0x4400
#define SESSION_NAME_RSSI                       0x4401
#define SESSION_NAME_CPU_USAGE                  0x4402

#define SESSION_NAME_MAG_RAD_DEV                0x1002
#define SESSION_NAME_MAG_DIS_PCT                0x1003
#define SESSION_NAME_DELTA_T                    0x4006


#define PL_TEMPERTURE                           4
#define PL_RAW_DATA                             6
#define PL_CAL_DATA                             12
#define PL_KAL_DATA                             12
#define PL_LINEAR_ACC_DATA                      12
#define PL_QUAT_EULER                           16
#define PL_MATERIX                              36
#define PL_PACKET_NUMBER                        4
#define PL_OS_REFERENCE_TIME                    6
#define PL_STATUS                               4
#define PL_CPU_USAGE                            40

#define PL_DT_DATA                              4
#define PL_MAG_ERROR_ALL_DATA                   4
#define PL_MAG_STRENGTH_DATA                    1

#define SESSION_NAME_GPIO_LEDMODE               0x0000

//data command
#define CMD_ID_ACC_RAW_DATA                     0x51
#define CMD_ID_GYRO_RAW_DATA                    0x52
#define CMD_ID_MAG_RAW_DATA                     0x53
#define CMD_ID_QUAT_DATA                        0x54
#define CMD_ID_EULER_DATA                       0x55

#define ACC_RAW_DATA_LEN                        0x06
#define GYRO_RAW_DATA_LEN                       0x06
#define MAG_RAW_DATA_LEN                        0x06

#define QUAT_DATA_LEN                           16
#define EULER_DATA_LEN                          12

//response command

#define RETURN_CODE_OK                          0x00
#define RETURN_CODE_ERROR_CLASS                 0x01
#define RETURN_CODE_ERROR_CMD                   0x02
#define RETURN_CODE_ERROR_CRC                   0x03
#define RETURN_CODE_ERROR_FOOTER                0x04
#define RETURN_CODE_ERROR_PAYLOAD               0x05
#define RETURN_CODE_ERROR_OPERATION             0x06
#define RETURN_CODE_ERROR_UNSUPPORT             0x07

#define RETURN_CODE_ERROR_FIRMWARE_UPDATE_REQ   0x18
#define RETURN_CODE_FIRMWARE_UPDATE_RESEND      0x19
#define RETURN_CODE_ERROR_FU_PACKET             0x1A

#define RETURN_CODE_ERROR_FLASH_WRITE           0x0A
#define RETURN_CODE_ERROR_FLASH_READ            0x0B
#define RETURN_CODE_ERROR_MALLOC_FAIL           0x0C
#define RETURN_CODE_ERROR_TIME_OUT              0x0D
#define RETURN_CODE_ERROR_OUT_RANGE             0xD0

#define CMD_ID_COM1_CFG                         0x01
#define CMD_ID_COM2_CFG                         0x02






typedef struct
{
    u8 reserve0;
    u8 reserve1;
    u16 packetID;

}ConfigSingleDataPacket;

#endif


