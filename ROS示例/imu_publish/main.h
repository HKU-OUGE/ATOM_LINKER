#ifndef __MAIN_H
#define __MAIN_H


#define ROS_ON
//#define MCU_ON

#define 	RATE_50		50
#define		RATE_100	100
#define		RATE_200	200
#define		RATE_400	400
#define		RATE_500	500    //swift mode only
#define		RATE_800	800    //swift mode only
#define		RATE_1000   1000   //swift mode only
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#endif
