//#define  CAN_DTC_GLOBALS
#include "can_did.h"
#include "can_dtc.h"
#include "usart.h"
#include "iflash.h"
#include "can.h"
#include "api.h"
#include "protocolapi.h"

AVM_DTC_TYPE  	avm_dtc[AVM_DTC_MAX]={{0}};
COM_DATA_TYPE   com={0,0,0,0,0,0,0,0,0,};
/*******???dtc*********/
const AVM_SELF_DTC_CONST_TYPE   avm_selt_dtc_const[AVM_SELT_DTC_MAX_NUM] = {	
	{OVER_DIAGNOSTIC_VOLTAGE_ADDR,0xD4,0x00,0x17},  //????
	{UNDER_DIAGNOSTIC_VOLTAGE_ADDR,0xD4,0x00,0x16}, //????
	{BCM_MISSING_ADDR,0xC1,0x40,0x87}, //missing周期性报文丢失
	{Body_CAN_Bus_off_ADDR,0xC0,0x19,0x88}, // 
	{FrontCam_Fault_ADDR,0x99,0x01,0x04},     
	{BackCam_Fault_ADDR,0x99,0x02,0x04},    
	{LeftCam_Fault_ADDR,0x99,0x03,0x04},    	
	{RightCam_Fault_ADDR,0x99,0x04,0x04},   
	{COMM_Fault_ADDR,0x99,0x00,0x05},    
};


uint8_t get_low_mainPwr_state(void)
{
	return  com.main_pwr_low_bit;
}
uint8_t get_hight_mainPwr_state(void)
{
	return  com.main_pwr_hight_bit;
}


//********************************************************************************************
// DESCRIPTION: ??1S???????
//              1. ??9V??2?,?????
// ARGUMENT   :
// RETURN     :
// AUTHOR     :
//********************************************************************************************
void check_main_volt_abnormal_pwr(device_info_t *dev,float mainPwrVol) //1s run 
{
	 //judge low pwr alarm
	if(mainPwrVol <= MAIN_PWR_LOW_ALARM_VALUE)
	{		/*BIT0 存为当前故障*/
		if(0 == com.main_pwr_low_bit)
		{
			writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_l,\
						BIT0,1);//MASK 置1		
			com.main_pwr_low_bit = 1;	
			dbg_msg(dev, "genearte main_pwr_low alarm\r\n");
			comm_message_send(dev, CM_ACC_OFF, 0, NULL, 0);/*通知ARM ACC*/
			mdelay(1);
			IO_SET(GPIOA, 3, LOW);
			dev->ci->host_ready=0;
			dev->ipc->heart_count=0;
			dev->ipc->dtc_arm_com_sate=1;
		}
	}
	else if(mainPwrVol >=MAIN_PWR_LOW_ALARM_RECOVER)
	{
		if(1 == com.main_pwr_low_bit)
		{
		   com.main_pwr_low_bit = 0;
		   writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].save_off_addr,\
				avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_h,\
				avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_m,\
				avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_l,\
				BIT0,0);
			writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].save_off_addr,\
					avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_h,\
					avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_m,\
					avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_l,\
					BIT3,1);/*清除0x01 当前test dtc故障，保留 历史故障mask*/
			dbg_msgv(dev, "cancel main_pwr_low alarm,start ARM\r\n");
		//	save_dtc();
			dev->ci->write_dtc_flag=1;
			mdelay(1);
			IO_SET(GPIOA, 3, HIGH);
			}
		}
      //judge hight pwr alarmf
	if(mainPwrVol >= MAIN_PWR_HIGHT_ALARM_VALUE)
	{
		if(0 == com.main_pwr_hight_bit)
		{
			com.main_pwr_hight_bit = 1;
			writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_l,\
						BIT0,1);	
		//dbg_msg(dev, "record current high pwr dtc\r\n");
		
			dbg_msg(dev, "genearte main_pwr_high alarm\r\n");
			comm_message_send(dev, CM_ACC_OFF, 0, NULL, 0);/*通知ARM ACC*/
			mdelay(1);
			IO_SET(GPIOA, 3, LOW);
			dev->ci->host_ready=0;
			dev->ipc->heart_count=0;
			dev->ipc->dtc_arm_com_sate=1;
			}	
	}
	else if(mainPwrVol <= (MAIN_PWR_HIGHT_ALARM_RECOVER))
	{
		if(1 == com.main_pwr_hight_bit)
		{
			com.main_pwr_hight_bit = 0;
			writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].save_off_addr,\
				avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_h,\
				avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_m,\
				avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_l,\
				BIT0,0);
			writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].save_off_addr,\
					avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_h,\
					avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_m,\
					avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_l,\
					BIT3,1);
	//		save_dtc();
			dev->ipc->dtc_arm_com_sate=1;
			dbg_msg(dev, "cancel main_pwr_high alarm,start ARM\r\n");
			mdelay(1);
			IO_SET(GPIOA, 3, HIGH );
			}
		}
}


#if 0
void  writeDTC(unsigned char DTCIndex,unsigned char DTCByteH,unsigned char DTCByteM,unsigned char DTCByteL,uint8_t num,uint8_t value)  //valueu  =1 set 1 ; =0 set 0; nsigned char DTCByteStateMask)
{
	uint8_t temp=0;
	if(value)/*置 1*/
	{
		temp|=(1<<num);/*temp = num 0x01 或者0x08*/
	}
	else
	{
		temp&=(~(1<<num));
	}
	avm_dtc[DTCIndex].index = DTCIndex;
	avm_dtc[DTCIndex].dtc_h = DTCByteH;
	avm_dtc[DTCIndex].dtc_m = DTCByteM;
	avm_dtc[DTCIndex].dtc_l = DTCByteL;		
	
   // firts history dtc
	if( ((avm_dtc[DTCIndex].mask&0x08)==0)&&(temp&0x08))//comfired = 0   test failed  = 1
	{
			avm_dtc[DTCIndex].num=1;

			avm_dtc[DTCIndex].mask |=0x08;/*此处mask = 0x08 */		
		}
 
	//have dtc  counter num
	else if( ((avm_dtc[DTCIndex].mask&0x08)==0x08)&&(temp&0x08))/*有历史故障并且要写历史故障*/
	{
			//avm_dtc[DTCIndex].mask|=0x08;
			avm_dtc[DTCIndex].num++;/*历史故障自加*/
			if(avm_dtc[DTCIndex].num>=40)//大于40次
				{
					avm_dtc[DTCIndex].mask &=0xf7;//BIT3 置0，清除历史故障
				}
		}
	else
		{				
			if(value)
			{
				avm_dtc[DTCIndex].mask|=(1<<num);
			}
			else
			{
				avm_dtc[DTCIndex].mask&=(~(1<<num));
			}
		}	

}
#endif



void  writeDTC(unsigned char DTCIndex,unsigned char DTCByteH,unsigned char DTCByteM,unsigned char DTCByteL,uint8_t num,uint8_t value)  //valueu  =1 set 1 ; =0 set 0; nsigned char DTCByteStateMask)
{
	//uint8_t temp=0;

	avm_dtc[DTCIndex].index = DTCIndex;
	avm_dtc[DTCIndex].dtc_h = DTCByteH;
	avm_dtc[DTCIndex].dtc_m = DTCByteM;
	avm_dtc[DTCIndex].dtc_l = DTCByteL;		
	
   // firts history dtc
	if((value==1)&&(num==BIT3))//comfired = 0   test failed  = 1
	{
		if(avm_dtc[DTCIndex].num>=40)
		{
			avm_dtc[DTCIndex].num=0;
			avm_dtc[DTCIndex].mask &=0xf7;
			}
		avm_dtc[DTCIndex].num++;
		avm_dtc[DTCIndex].mask |=0x08;
		}

	if((value==1)&&(num==BIT0))//comfired = 0   test failed  = 1
	{
		avm_dtc[DTCIndex].mask |=0x01;
	
		}

	if((value==0)&&(num==BIT0))//comfired = 0   test failed  = 1
	{
		avm_dtc[DTCIndex].mask &=0xfe;
		}
}

#if 1

void can_flame_missing_check(device_info_t *dev)  //期望值为100ms的几个周期性报文  
{
	static	uint16_t  BCM_missing_recovery_cnt=0;
	dev = get_device_info();
	if((dev->ipc->miss_avm1==1u)||(dev->ipc->miss_avm2==1u)||(dev->ipc->miss_avm3==1u)||(dev->ipc->miss_avm4==1u)) 
	{	

			writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].dtc_l,\
							BIT0,1);
			com.cycle_flame_miss_bit=1u;
		}
	else 
	{
		if(com.cycle_flame_miss_bit==1u)
		{
			BCM_missing_recovery_cnt++;
			if(BCM_missing_recovery_cnt>=10)
			{
   			    com.cycle_flame_miss_bit=0u;
				BCM_missing_recovery_cnt=0;
				writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].save_off_addr,\
								avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].dtc_h,\
								avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].dtc_m,\
								avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].dtc_l,\
								BIT0,0);
				writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_BCM].dtc_l,\
							BIT3,1);
			//	  save_dtc();
			dev->ci->write_dtc_flag=1;
				}
			 }
		}
}

void arm_comm_fault_proc(device_info_t *dev)  //
{
	static	uint16_t arm_comm_fault_cnt=0;
	dev = get_device_info();
	if(dev->ipc->dtc_arm_com_sate == 1u)  
	{	
		writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_l,\
							BIT0,1);
		if(com.Arm_comm_err_bit==0u)
		{
			arm_comm_fault_cnt++;
			if(arm_comm_fault_cnt>=20)
			{
				arm_comm_fault_cnt=20;
				if(com.Arm_comm_err_bit==0u)
				{
					com.Arm_comm_err_bit=1u;
					writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_l,\
						BIT3,1);
				//	 save_dtc();/*此处不能把mask=0x01也写入flash 若断电上电会检测到0x01的dtc即使故障已恢复*/
				dev->ci->write_dtc_flag=1;
					}
				}
			}
		}
	else 
	{/*恢复50次  清除当前故障码，如果未恢复就断电 就需要把*/
		 if(arm_comm_fault_cnt)
		 {
			arm_comm_fault_cnt--;
			if(arm_comm_fault_cnt<=10)
	        {
			  arm_comm_fault_cnt=0;	
			  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_l,\
							BIT0,0);	
			  if(com.Arm_comm_err_bit==1u)
			 	{
			  	 com.Arm_comm_err_bit=0u;
		//			 save_dtc();
		dev->ci->write_dtc_flag=1;
			  	}
				}
		 }
	}
}


void  camera_front_error_check(device_info_t *dev)  //
{
	static	uint16_t camera_front_fault_cnt=0;	
	if(dev->ipc->dtc_camera_state&0x01)  //
	{	
		writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_l,\
							BIT0,1);
		if(com.Front_camera_error_bit==0)
		{
		camera_front_fault_cnt++;
		if(camera_front_fault_cnt>=20)
		{
			camera_front_fault_cnt=20;
			if(com.Front_camera_error_bit==0)
			{
				com.Front_camera_error_bit=1;
				writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_l,\
						BIT3,1);
			//	 save_dtc();
			dev->ci->write_dtc_flag=1;
				}
				}	
			}
		}
	else
	{
		 if(camera_front_fault_cnt)
		 {
			camera_front_fault_cnt--;
			if(camera_front_fault_cnt<=10)
	        {
			  camera_front_fault_cnt=0;	
			  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_l,\
							BIT0,0);	
			  if(com.Front_camera_error_bit==0)
			  {
				  com.Front_camera_error_bit=1u;
		//		  save_dtc();
		dev->ci->write_dtc_flag=1;
			  	}
			  }
		 }	  
	} 
}



void  camera_back_error_check(device_info_t *dev)  //
{
	static	uint16_t camera_back_fault_cnt=0;	
	if((dev->ipc->dtc_camera_state>>1)&0x01)  //
	{	
		writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_l,\
							BIT0,1);
		if(com.Back_camera_error_bit==0)
		{
		camera_back_fault_cnt++;
		if(camera_back_fault_cnt>=20)
		{
			camera_back_fault_cnt=20;
			if(com.Back_camera_error_bit==0)
			{
				com.Back_camera_error_bit=1;
				writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_l,\
						BIT3,1);
			//	 save_dtc();
			dev->ci->write_dtc_flag=1;
				}
				}	
			}
		}
	else
	{
		 if(camera_back_fault_cnt)
		 {
			camera_back_fault_cnt--;
			if(camera_back_fault_cnt<=10)
	        {
			  camera_back_fault_cnt=0;
				writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_l,\
							BIT0,0);							
			  if(com.Back_camera_error_bit==0)
			  {
				  com.Back_camera_error_bit=1u;
			//	  save_dtc();
			dev->ci->write_dtc_flag=1;
			  	}
			  }
		 }	  
	} 
}



void  camera_left_error_check(device_info_t *dev)  //
{
	static	uint16_t camera_left_fault_cnt=0;	
	if((dev->ipc->dtc_camera_state>>2)&0x01)  //
	{	
		writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_l,\
							BIT0,1);
		if(com.Left_camera_error_bit==0)
		{
		camera_left_fault_cnt++;
		if(camera_left_fault_cnt>=20)
		{
			camera_left_fault_cnt=20;
			if(com.Left_camera_error_bit==0)
			{
				com.Left_camera_error_bit=1;
				writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_l,\
						BIT3,1);
			//	 save_dtc();
			dev->ci->write_dtc_flag=1;
				}
				}	
			}
		}
	else
	{
		 if(camera_left_fault_cnt)
		 {
			camera_left_fault_cnt--;
			if(camera_left_fault_cnt<=10)
	    {
			  camera_left_fault_cnt=0;	
			  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_l,\
							BIT0,0);
			  if(com.Left_camera_error_bit==0)
			  {
				  com.Left_camera_error_bit=1u;	
			//	  save_dtc();
			dev->ci->write_dtc_flag=1;
			  	}
			  }
		 }	  
	} 
}



void  camera_right_error_check(device_info_t *dev)  //
{
	static	uint16_t camera_right_fault_cnt=0;	
	if((dev->ipc->dtc_camera_state>>3)&0x01)  //
	{	
		writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_l,\
							BIT0,1);
		if(com.Right_camera_error_bit==0)
		{
		camera_right_fault_cnt++;
		if(camera_right_fault_cnt>=20)
		{
			camera_right_fault_cnt=20;
			if(com.Right_camera_error_bit==0)
			{
				com.Right_camera_error_bit=1;
				writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_l,\
						BIT3,1);
			//	 save_dtc();
			dev->ci->write_dtc_flag=1;
				}
				}	
			}
		}
	else
	{
		 if(camera_right_fault_cnt)
		 {
			camera_right_fault_cnt--;
			if(camera_right_fault_cnt<=10)
	    {
			  camera_right_fault_cnt=0;	
			  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_l,\
							BIT0,0);	
			  if(com.Right_camera_error_bit==0)
			  {
				  com.Right_camera_error_bit=1u;
		//		  save_dtc();
		dev->ci->write_dtc_flag=1;
			  	}
			  }
		 }	  
	} 
}




/*必须确定检测周期*/
void  can_buf_off_check(device_info_t *dev) 
{	
  static  uint16_t pt_can_buf_off_recovery_cnt=0;
  if(dev->dtc_t.busoff_dtc_flag)  //
  {	
  	if(com.body_can_buf_off_bit==0)
	{
  	  dbg_msg(dev,"record busoff dtc\r\n");
   	  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].save_off_addr,\
		avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_h,\
		avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_m,\
		avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_l,\
		BIT0,1);

	   com.body_can_buf_off_bit=1;
		}
	  }		
	else
	{	/*恢复5s都能收到=20ms*250*/
		if(com.body_can_buf_off_bit==1)
	 	{
	 	  pt_can_buf_off_recovery_cnt++;
		  if(pt_can_buf_off_recovery_cnt>=250)
		  {
		 	 dbg_msg(dev,"recovery busoff dtc\r\n");
		  	 pt_can_buf_off_recovery_cnt=0;//恢复busoff 并消除test dtc
	 	 	 com.body_can_buf_off_bit=0;
		  	 writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].save_off_addr,\
				avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_h,\
				avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_m,\
				avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_l,\
				BIT0,0);
			 
			 writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_l,\
						BIT3,1);
		//	 save_dtc();	
		dev->ci->write_dtc_flag=1;
				}
			}
		}
}
	 



#endif


u16 read_dtc_01_02_sub_id(u8 *r_buffer,u8 sub_sid,u8 mask)
{
	u8 i;
	u16 n=0;
	if(sub_sid==0x01u)
	{  	/*当前故障 没有存到FLASH  无需从FLASH读出*/	
		n=0;
		for(i=0;i<AVM_DTC_MAX;i++)
		{
			if(avm_dtc[i].mask!=0)
		 	{
				if(avm_dtc[i].mask&mask)
		  		{
		  			n++; 	  			
		  		} 		
		 	}  	
		} 
		r_buffer[0]=n;
		return n;
	}
	else if(sub_sid==0x02)
 	{
 	   n=0;
	  if(mask!=0x09)
	  {
		if(mask&0x01)
		{
			for(i=0;i<AVM_DTC_MAX;i++)
			{
				if(avm_dtc[i].mask&0x01)
		  		{
					r_buffer[n++]=avm_dtc[i].dtc_h;
					r_buffer[n++]=avm_dtc[i].dtc_m;
					r_buffer[n++]=avm_dtc[i].dtc_l;
					r_buffer[n++]=avm_dtc[i].mask;	  			
		  		} 		
			} 
		}
		if(mask&0x08)
		{
			for(i=0;i<AVM_DTC_MAX;i++)
			{
				if(avm_dtc[i].mask&0x08)
				{
					r_buffer[n++]=avm_dtc[i].dtc_h;
					r_buffer[n++]=avm_dtc[i].dtc_m;
					r_buffer[n++]=avm_dtc[i].dtc_l;
					r_buffer[n++]=avm_dtc[i].mask;
					} 		
				}  		
			}
			}
		else if((mask==0x09)||(mask==0xff))
		{
			for(i=0;i<AVM_DTC_MAX;i++)
			{
				if(avm_dtc[i].mask!=0)
				{
					r_buffer[n++]=avm_dtc[i].dtc_h;
					r_buffer[n++]=avm_dtc[i].dtc_m;
					r_buffer[n++]=avm_dtc[i].dtc_l;
					r_buffer[n++]=avm_dtc[i].mask;
					} 		
				} 
			}
			return n/4;
  	}	
	else
  	{
		return 0;	
  	}	
  

}

/*
*----------------------------------------------------------------------------------------------------------------------------------------
* ????:void clear_dtc(void)
* ????:??dtc
  ??????:
  r_buffer???? ??????? 0A ????DTC
* ? ? ?:?
  ??:    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
void clear_dtc(void)
{
    uint8_t i;
	device_info_t *dev=get_device_info();
	for(i=0;i<AVM_DTC_MAX;i++)
	{		 
		//avm_dtc[i].dtc_h=0x00;
		//avm_dtc[i].dtc_m=0x00;
		//avm_dtc[i].dtc_l=0x00;
		avm_dtc[i].mask=0x00; 	
	} 
	//save_dtc( );
	dev->ci->write_dtc_flag=1;
}


void clr_history_dtc(void)
{
	device_info_t *dev=get_device_info();
	uint8_t i = 0;	
	for(i=0;i<AVM_DTC_MAX;i++)
	{		 
		//avm_dtc[i].dtc_h=0x00;
		//avm_dtc[i].dtc_m=0x00;
		//avm_dtc[i].dtc_l=0x00;
		avm_dtc[i].mask&= 0xf7;
	} 	
//	save_dtc( );
	dev->ci->write_dtc_flag=1;
}	

void save_dtc(void)
{	
	//dbg_msg(get_device_info(),"save dtc\n");
	STMFLASH_Write(ADD_DTC_START,(u16 *)&avm_dtc[0],sizeof(AVM_DTC_TYPE)*AVM_DTC_MAX/2);
//	return 1;
}


/*把上次掉电之前误存0x09的dtc  变成0x08,解决测试周期有test dtc 时刻断电，0x01当前dtc会存入flash*/
void init_dtc_flash(void)
{ 
	u8 i=0;
	for(i=0;i<AVM_DTC_MAX;i++)
	{
		avm_dtc[i].mask = 0;
		avm_dtc[i].dtc_h = 0;
		avm_dtc[i].dtc_l = 0;
		avm_dtc[i].dtc_m = 0;
		avm_dtc[i].num = 0;
		avm_dtc[i].index = 0;
		}		
	read_dtc_from_flash(ADD_DTC_START,(u8 *)&avm_dtc[0],sizeof(AVM_DTC_TYPE)*AVM_DTC_MAX);
	//dbg_msgv(get_device_info(), "sizeof(u8) = %d\r\n",sizeof(u8) );
	for(i=0;i<AVM_DTC_MAX;i++)
	{
		if(avm_dtc[i].mask==0x09)
		{
			avm_dtc[i].mask=0x08;
			}	
		} 
	STMFLASH_Write(ADD_DTC_START,(u16 *)&avm_dtc[0],sizeof(AVM_DTC_TYPE)*AVM_DTC_MAX/2);	//sizeof(AVM_DTC_TYPE)=6
}







