//#define  CAN_DTC_GLOBALS

#include "can_dtc.h"
#include "iflash.h"


AVM_DTC_TYPE  	avm_dtc[AVM_DTC_MAX]={{0}};
COM_DATA_TYPE   com={0,0,0,0,0,0,0,0,0,};
/*******???dtc*********/
const AVM_SELF_DTC_CONST_TYPE   avm_selt_dtc_const[AVM_SELT_DTC_MAX_NUM] = {	
	{OVER_DIAGNOSTIC_VOLTAGE_ADDR,0xD4,0x00,0x17},  //????
	{UNDER_DIAGNOSTIC_VOLTAGE_ADDR,0xD4,0x00,0x16}, //????
	{BCM_MISSING_ADDR,0xC1,0x50,0x87}, //missing周期性报文丢失
	{Body_CAN_Bus_off_ADDR,0xC0,0x19,0x88}, // ????
	{FrontCam_Fault_ADDR,0x99,0x01,0x04},     
	{BackCam_Fault_ADDR,0x99,0x02,0x04},    
	{LeftCam_Fault_ADDR,0x99,0x03,0x04},    	
	{RightCam_Fault_ADDR,0x99,0x04,0x04},   
	{COMM_Fault_ADDR,0x99,0x00,0x05},    //Body-CAN????bus-off??	
};


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

void save_dtc(void)
{
	STMFLASH_Write(ADD_DTC_START,(u16 *)&avm_dtc[0],sizeof(AVM_DTC_TYPE)*AVM_DTC_MAX);
//	return 1;
}

void clear_dtc(void)
{
    u8 i;
	for(i=0;i<AVM_DTC_MAX;i++)
	{		 
		//avm_dtc[i].dtc_h=0x00;
		//avm_dtc[i].dtc_m=0x00;
		//avm_dtc[i].dtc_l=0x00;
		avm_dtc[i].mask=0x00; 	
	} 
	//save_dtc( );
}

#if 0
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
		writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_l,\
						BIT0,1);
		//dbg_msg(dev, "record current low pwr dtc\r\n");
		if(0 == com.main_pwr_low_bit)
		{
			dev->adc_t.low_keepCount++;
			if(dev->adc_t.low_keepCount > MAIN_PWR_LOW_ALARM_TIM)//持续2s
			{
				dev->adc_t.low_keepCount=MAIN_PWR_LOW_ALARM_TIM;
				if(com.main_pwr_low_bit==0)
					{
						com.main_pwr_low_bit = 1;
						writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_l,\
							BIT3,1);
						save_dtc();
						dbg_msg(dev, "genearte main_pwr_low alarm\r\n");
						comm_message_send(dev, CM_ACC_OFF, 0, NULL, 0);/*通知ARM ACC*/
						mdelay(1000);
						IO_SET(GPIOA, 3, LOW);
						dev->ci->host_ready=0;
						dev->ipc->heart_count=0;
					}
			}
		}
	}
	else if(mainPwrVol >= (MAIN_PWR_LOW_ALARM_VALUE+5))
	{
		if(dev->adc_t.low_keepCount)
		{
			dev->adc_t.low_keepCount--;
			}
		if(0 == dev->adc_t.low_keepCount)
		{
			if(1 == com.main_pwr_low_bit)
			{
				com.main_pwr_low_bit = 0;
				writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_UNDER_VOLT].dtc_l,\
						BIT0,0);/*清除0x01 当前test dtc故障，保留 历史故障mask*/
				save_dtc();
				dbg_msg(dev, "cancel main_pwr_low alarm,start ARM\r\n");
				mdelay(1);
				IO_SET(GPIOA, 3, HIGH);
			}
		}
	}
	else {
		}
	
      //judge hight pwr alarmf
	if(mainPwrVol >= MAIN_PWR_HIGHT_ALARM_VALUE)
	{

		writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_l,\
						BIT0,1);	
		//dbg_msg(dev, "record current high pwr dtc\r\n");
		if(0 == com.main_pwr_hight_bit)
		{
			dev->adc_t.hight_keepCount++;
			if(dev->adc_t.hight_keepCount >= MAIN_PWR_LOW_ALARM_TIM)
			{
				dev->adc_t.hight_keepCount=MAIN_PWR_LOW_ALARM_TIM;
				if(com.main_pwr_hight_bit==0)
					{
						com.main_pwr_hight_bit = 1;
						writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_l,\
						BIT3,1);
						save_dtc();
						dbg_msg(dev, "genearte main_pwr_high alarm\r\n");
						comm_message_send(dev, CM_ACC_OFF, 0, NULL, 0);/*通知ARM ACC*/
						mdelay(1000);
						IO_SET(GPIOA, 3, LOW);
						dev->ci->host_ready=0;
						dev->ipc->heart_count=0;
					}	
			}
		}
	}
	else if(mainPwrVol <= (MAIN_PWR_HIGHT_ALARM_VALUE-5))
	{
		if(dev->adc_t.hight_keepCount)
		{
			dev->adc_t.hight_keepCount--;
			if(0 == dev->adc_t.hight_keepCount)
			{
				if(1 == com.main_pwr_hight_bit)
				{
					com.main_pwr_hight_bit = 0;
					writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_OVER_VOLT].dtc_l,\
							BIT0,0);
					save_dtc();
					dbg_msg(dev, "cancel main_pwr_high alarm,start ARM\r\n");
					mdelay(1);
					IO_SET(GPIOA, 3, HIGH );
				}
			}
		}	
	}
	else {
		}
}


/*****************************************************************************
 ? ? ?  : writeDTC
 ????  : ?DTC????
 ????  : DTCIndex  ????
 			 DTCByteH-M-L 
 			 num   mask???
 			 value  ????
 ????  : ?
 ? ? ?  : 
 ????  : 
 ????  : 
 
 ????      :
  1.?    ?   : 2018?8?28?
    ?    ?   : xz
    ????   : ?????

*****************************************************************************/

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

#if 1

void can_flame_missing_check(device_info_t *dev)  //
{
	static	uint16_t can_flame_missing_cnt=0;
	dev = get_device_info();
	if((dev->ipc->miss_avm1==1u)||(dev->ipc->miss_avm2==1u)||(dev->ipc->miss_avm3==1u)) 
	{	
		writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].dtc_l,\
							BIT0,1);
		if(com.cycle_flame_miss_bit==0u)
		{
			can_flame_missing_cnt++;
			if(can_flame_missing_cnt>=200u)
			{
				can_flame_missing_cnt=200u;
				if(com.cycle_flame_miss_bit==0u)
				{
					com.cycle_flame_miss_bit=1u;
					writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].dtc_l,\
						BIT3,1);
					save_dtc();
					}
				}		
			}
		}
	else 
	{	
	if(can_flame_missing_cnt)
	{
		can_flame_missing_cnt--;
		 if(can_flame_missing_cnt<=50)
		 {
			  can_flame_missing_cnt=0;
			if(com.cycle_flame_miss_bit==1u)
			{
			  com.cycle_flame_miss_bit=0u;
			  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_MISS_FLAME].dtc_l,\
							BIT0,0);	
			 save_dtc();
				}
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
					 save_dtc();/*此处不能把mask=0x01也写入flash 若断电上电会检测到0x01的dtc即使故障已恢复*/
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
			  if(com.Arm_comm_err_bit==1u)
			  	{
			  	  com.Arm_comm_err_bit=0u;
				  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_ARM_COMM_ERROR].dtc_l,\
							BIT0,0);	
				 save_dtc();
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
				 save_dtc();
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
			  if(com.Front_camera_error_bit==0)
			  {
				  com.Front_camera_error_bit=1u;
				  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_FRONT_CAMERA_FALUT].dtc_l,\
							BIT0,0);	
				  save_dtc();
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
				 save_dtc();
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
			  if(com.Back_camera_error_bit==0)
			  {
				  com.Back_camera_error_bit=1u;
				  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_BACK_CAMERA_FALUT].dtc_l,\
							BIT0,0);	
				  save_dtc();
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
				 save_dtc();
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
			  if(com.Left_camera_error_bit==0)
			  {
				  com.Left_camera_error_bit=1u;
				  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_LEFT_CAMERA_FALUT].dtc_l,\
							BIT0,0);	
				  save_dtc();
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
				 save_dtc();
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
			  if(com.Right_camera_error_bit==0)
			  {
				  com.Right_camera_error_bit=1u;
				  writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].save_off_addr,\
							avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_h,\
							avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_m,\
							avm_selt_dtc_const[AVM_SELT_DTC_RIGHT_CAMERA_FALUT].dtc_l,\
							BIT0,0);	
				  save_dtc();
			  	}
			  }
		 }	  
	} 
}


uint8_t get_can_bus_state(device_info_t *dev)
{
	if(CAN_ErrorStatusRegister(dev)==CAN_BUS_OFF_REG)
	{
		com.body_can_buf_off_bit = 1u;
		}
	else
		com.body_can_buf_off_bit = 0u;
	
	return  com.body_can_buf_off_bit;
}

/*必须确定检测周期*/
void  can_buf_off_check(device_info_t *dev) 
{	
	static  uint16_t pt_can_buf_off_cnt=0;
	static  uint16_t pt_can_buf_off_tim_cnt=0;
	get_can_bus_state(dev);

    if(com.body_can_buf_off_bit==1)  //??body-can_bus-off
   	{		
		writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_l,\
						BIT0,1);
		pt_can_buf_off_cnt++;
		if(pt_can_buf_off_cnt>=3)/*出现过两次就存一次历史故障*/
		{
			pt_can_buf_off_cnt=3;
		    writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].save_off_addr,\
					avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_h,\
					avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_m,\
					avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_l,\
					BIT3,1);
		    save_dtc();	
			 pt_can_buf_off_tim_cnt++;
			 if(pt_can_buf_off_tim_cnt>100)
			 	pt_can_buf_off_tim_cnt=100;
			}
   	}	
	else
	{	/*恢复*/
		 if(pt_can_buf_off_tim_cnt)
		 {
			pt_can_buf_off_tim_cnt--;
			if(pt_can_buf_off_tim_cnt<=50)
	        {
			  pt_can_buf_off_tim_cnt=0;	
				writeDTC(avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].save_off_addr,\
						avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_h,\
						avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_m,\
						avm_selt_dtc_const[AVM_SELT_DTC_CAN_OFF].dtc_l,\
						BIT0,0);

				save_dtc();	
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
		if(mask == 0x01)
		{
			for(i=0;i<AVM_DTC_MAX;i++)
			{
				if(avm_dtc[i].mask&mask)
		  		{
					r_buffer[n++]=avm_dtc[i].dtc_h;
					r_buffer[n++]=avm_dtc[i].dtc_m;
					r_buffer[n++]=avm_dtc[i].dtc_l;
					r_buffer[n++]=avm_dtc[i].mask;	  			
		  		} 		
			} 
		}
		else if(mask ==0x08)
		{
		for(i=0;i<AVM_DTC_MAX;i++)
		{
				if(avm_dtc[i].mask&mask)
				{
					r_buffer[n++]=avm_dtc[i].dtc_h;
					r_buffer[n++]=avm_dtc[i].dtc_m;
					r_buffer[n++]=avm_dtc[i].dtc_l;
					r_buffer[n++]=avm_dtc[i].mask;
					} 		
				}  		
			}
		else if(mask ==0x09)
		{
			for(i=0;i<AVM_DTC_MAX;i++)
			{
				if(avm_dtc[i].mask==mask)
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


void clr_history_dtc(void)
{
	uint8_t i = 0;	
	for(i=0;i<AVM_DTC_MAX;i++)
	{		 
		//avm_dtc[i].dtc_h=0x00;
		//avm_dtc[i].dtc_m=0x00;
		//avm_dtc[i].dtc_l=0x00;
		avm_dtc[i].mask&= 0xf7;
	} 	
	save_dtc( );
}	


void init_dtc_init(void)
{
	device_info_t *dev;
	dev->ipc->dtc_arm_com_sate=0;
	dev->ipc->work_state=0;
	}


/*把上次掉电之前误存0x09的dtc  变成0x08,解决测试周期有test dtc 时刻断电，0x01当前dtc会存入flash*/
void init_dtc_flash(void)
{ 
	u8 i=0;
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
#endif

