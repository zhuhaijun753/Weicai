#ifndef __CAN_DTC_H__
#define __CAN_DTC_H__
#include "miscdef.h"


#define  MAIN_PWR_HIGHT_ALARM_VALUE    165u  //16V
#define  MAIN_PWR_LOW_ALARM_VALUE   		 80u //9V
#define  MAIN_PWR_TOO_LOW_VALUE   		 65u //9V
#define  MAIN_PWR_LOW_ALARM_TIM        200  //����2s

#define AVM_SELT_DTC_OVER_VOLT   				0 //
#define AVM_SELT_DTC_UNDER_VOLT  				1  //
#define AVM_SELT_DTC_MISS_FLAME  				2  //
#define AVM_SELT_DTC_CAN_OFF 						3//
#define AVM_SELT_DTC_FRONT_CAMERA_FALUT  			4 
#define AVM_SELT_DTC_BACK_CAMERA_FALUT 				5
#define AVM_SELT_DTC_LEFT_CAMERA_FALUT  			6 
#define AVM_SELT_DTC_RIGHT_CAMERA_FALUT  		7
#define AVM_SELT_DTC_ARM_COMM_ERROR		 		8
#define AVM_SELT_DTC_MAX_NUM           			9//

/******************AVM 自身故障　end****************************/

#define BIT0   0
#define BIT1   1
#define BIT2   2
#define BIT3   3
#define BIT4   4
#define BIT5   5
#define BIT6   6
#define BIT7   7



#define		OVER_DIAGNOSTIC_VOLTAGE_ADDR  0  //诊断过压
#define  	UNDER_DIAGNOSTIC_VOLTAGE_ADDR  1 //诊断欠压
#define  	BCM_MISSING_ADDR  			 2
#define		Body_CAN_Bus_off_ADDR		3     //CAN通讯故障
#define		FrontCam_Fault_ADDR				4
#define		BackCam_Fault_ADDR			5
#define		LeftCam_Fault_ADDR			6
#define   	RightCam_Fault_ADDR			7
#define		COMM_Fault_ADDR				8	//ARM 软件错误  mcu30s内未收到ARM心跳  	
#define   	AVM_DTC_MAX				     9



//定义体检故障码结构体
typedef struct  
{
	uint8_t index;
	uint8_t dtc_h;
	uint8_t dtc_m;
	uint8_t dtc_l;
	uint8_t  num;/*��ʷ���ϳ�������*/
	uint8_t mask;	
}AVM_DTC_TYPE;


typedef struct  
{
	uint8_t save_off_addr; //放偏移地址 storeIndex;
	uint8_t dtc_h;
	uint8_t dtc_m;
	uint8_t dtc_l;
}AVM_SELF_DTC_CONST_TYPE;

typedef struct 
{	
	uint8_t main_pwr_hight_bit: 1;
	uint8_t main_pwr_low_bit: 1;
	uint8_t cycle_flame_miss_bit: 1;
	uint8_t body_can_buf_off_bit:1;
	uint8_t Front_camera_error_bit:1;
	uint8_t Back_camera_error_bit:1;
	uint8_t Left_camera_error_bit:1;
	uint8_t Right_camera_error_bit:1;
	uint8_t Arm_comm_err_bit:1;
}COM_DATA_TYPE;


/****DTC��ʼ��ַ*******/		




void  writeDTC(unsigned char DTCIndex,unsigned char DTCByteH,unsigned char DTCByteM,unsigned char DTCByteL,uint8_t num,uint8_t value);  //valueu  =1 set 1 ; =0 set 0; nsigned char DTCByteStateMask) unsigned char UpGradeState;
//读VIN from ems
void clear_dtc(void);
void clr_history_dtc(void);
void check_main_volt_abnormal_pwr(device_info_t *dev,float mainPwrVol);
u16  read_dtc_01_02_sub_id(u8 *r_buffer,u8 sub_sid,u8 mask);
void  camera_front_error_check(device_info_t *dev);
void  camera_back_error_check(device_info_t * dev);
void  camera_left_error_check(device_info_t *dev);
void  camera_right_error_check(device_info_t *dev);
void  arm_comm_fault_proc(device_info_t *dev) ;
void  can_buf_off_check(device_info_t *dev);
void init_dtc_flash(void);
void can_flame_missing_check(device_info_t *dev);
void save_dtc(void); 
#endif /*__CAN_DTC_H__*/

