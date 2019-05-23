/*
********************************************************************************
**									CHINA-SZ 
**
** Project	360pro
**
** File 	CAN_DIAG_PROTOCOL.c 
** 
** Terms:
**	  
** Brief: diag protocol for app program
**
** Version	   Author	  Date		 changed	 Description
** ========   =======	 ========	 ========	 ===========   
**	V1.0	   xz        2018523  
**	 
**
********************************************************************************
*/
#define CAN_DIAG_GLOBALS
#include "stdlib.h"
#include "can_did.h"
#include "can_diag_protocol.h"
#include "protocolapi.h"
#include "can.h"
#include "can_net_layer.h"
#include "usart.h"
#include "api.h"
#include "hw_config.h"
#include "usart.h"
#include "config.h"
#include "iflash.h"
#include "can_dtc.h"
#include "timer.h"
#include "diag_upgrade.h"
CAN_DIAG_TYPE diag; //diag 相关项结构体
SERVER_APPLICATION_LAYER_TIM_TYPE   server_application_layer_tim;//应用层时间参数
CLIENT_APPLICATION_LAYER_TIM_TYPE   client_application_layer_tim;//应用层会话管理计时器

/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：void init_diag_protocol_parameter(void)
* 函数功能：初始化diag相关参数
  参数说明：
* 输入参数：无
* 返 回 值：无
  作者：    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
void init_diag_protocol_parameer(void)
{
	//device_info_t *dev=get_device_info();
 	diag.address_type=DIAG_P_ADDRESS;//物理寻址模式
	diag.sesion_mode=DIAG_DE_MODE;//上电即保持在默认会话模式
	diag.security_level1_state=DIAG_SECURITY_LEVEL_STATE_LOCK; //安全状态
	diag.security_level3_state=DIAG_SECURITY_LEVEL_STATE_LOCK;//安全状态
	diag.dtc_function_enable=DIAG_DTC_FUNCTION_ON;
	diag.communication_enable=DIAG_COMM_RX_AND_TX_ENABLE;
	diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;//取1位
	diag.SAFC_1=0x00;
	diag.SAFC_3=0x00;
	diag.time_SAFC_3=0x00;
	diag.time_SAFC_1=0x00;//一般安全访问失败技术3次时间点
	diag.security_level1_access_step=DIAG_NOT_REQ_SEED;//未请求Seed
	diag.security_level3_access_step=DIAG_NOT_REQ_SEED;//未请求Seed
	//SAFC_1  SAFC_3  从flash读出来
	//应用层时间参数
	server_application_layer_tim.P2can=50*OS_1MS;
	server_application_layer_tim.P2_asterisk_can=5000*OS_1MS;
	server_application_layer_tim.S3_sever=5000*OS_1MS;
	client_application_layer_tim.P2can=50*OS_1MS;//ECU收到诊断发出回应时间要求
	client_application_layer_tim.P2_asterisk_can=5000*OS_1MS;
	client_application_layer_tim.P3_can_phy=100*OS_1MS;
	client_application_layer_tim.P3_can_func=100*OS_1MS;
	client_application_layer_tim.S3_client=5000;
	client_application_layer_tim.S3_Tester=4000;
 	init_did_data( );
	init_dtc_flash();
}
/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：void init_diag_protocol_parameter(void)
* 函数功能：初始化diag相关参数
  参数说明：
* 输入参数：无
* 返 回 值：无
  作者：    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
void init_diag_protocol_parameer_boot(void)
{
 	diag.address_type=DIAG_P_ADDRESS;//物理寻址模式
	diag.sesion_mode=DIAG_PR_MODE;//上电即保持在默认会话模式
	diag.security_level1_state=DIAG_SECURITY_LEVEL_STATE_LOCK; //安全状态
	diag.security_level3_state=DIAG_SECURITY_LEVEL_STATE_LOCK;//安全状态
	diag.dtc_function_enable=DIAG_DTC_FUNCTION_ON;
	diag.communication_enable=DIAG_COMM_RX_AND_TX_DISABLE;
	diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;//取1位
	diag.SAFC_1=0x00;
	diag.SAFC_3=0x00;
	diag.time_SAFC_3=0x00;
	diag.time_SAFC_1=0x00;//一般安全访问失败技术3次时间点
	diag.security_level1_access_step=DIAG_NOT_REQ_SEED;//未请求Seed
	diag.security_level3_access_step=DIAG_NOT_REQ_SEED;//未请求Seed
	//SAFC_1  SAFC_3  从flash读出来
	//应用层时间参数
	server_application_layer_tim.P2can=50*OS_1MS;
	server_application_layer_tim.P2_asterisk_can=5000*OS_1MS;
	server_application_layer_tim.S3_sever=5000*OS_1MS;
	client_application_layer_tim.P2can=50*OS_1MS;//ECU收到诊断发出回应时间要求
	client_application_layer_tim.P2_asterisk_can=5000*OS_1MS;
	client_application_layer_tim.P3_can_phy=100*OS_1MS;
	client_application_layer_tim.P3_can_func=100*OS_1MS;
	client_application_layer_tim.S3_client=2000*OS_1MS;
	client_application_layer_tim.S3_Tester=4000;
}


//******************************************************************************
// DESCRIPTION:
// ARUGMENT   :获取communication_enable的状态
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8  get_diag_communication_state(void)
{
	return diag.communication_enable;
}


u8  get_diag_dtc_function_state(void)
{
	return diag.dtc_function_enable;
}

//******************************************************************************
// DESCRIPTION:
// ARUGMENT   :sesion_mode  状态
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8  get_diag_sesion_mode(void)
{
	return diag.sesion_mode;
}

//******************************************************************************
// DESCRIPTION:
// ARUGMENT   :address_type  当前寻址模式
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8  get_diag_address_type(void)
{
	return diag.address_type;
}

//******************************************************************************
// DESCRIPTION:设置寻址模式
// ARUGMENT   :
// RETURN     :
// AUTHOR     :
//******************************************************************************
void set_diag_address_type(uint8_t type)
{
	diag.address_type=type;
}

void set_diag_session(uint8_t type)
{
	diag.sesion_mode=type;
}

/*****************************************************************************
 函 数 名  : SecuriyAlgorithmFBL
 功能描述  : boot安全算法
 输入参数  : _int32 wSeed  
             _int32 MASK   
 输出参数  : 无
 返 回 值  : u32
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年8月14日
    作    者   : xz
    修改内容   : 新生成函数

*****************************************************************************/
//u32 SecuriyAlgorithmFBL(u32 wSeed,u32 MASK)

/*
u32 SecuriyAlgorithmFBL(u32 ipSeedArray,u32 ECU_boot_mask)
{
	u8 iopKeyArray[4],seed[4],Mask[4];
	u32  key;
	unsigned char Cal[4] = { 0x00, 0x00, 0x00, 0x00 };
	Mask[0]=(ECU_boot_mask>>24u)&0xffu;
	Mask[1]=(u8)(ECU_boot_mask>>16u)&0xffu;
	Mask[2]=(u8)(ECU_boot_mask>>8u)&0xffu;
	Mask[3]=(u8)(ECU_boot_mask)&0xffu;
	
	seed[0] = (ipSeedArray>>24u)&0xffu;
	seed[1] = (ipSeedArray>>16u)&0xffu;
	seed[2] = (ipSeedArray>>8u)&0xffu;
	seed[3] = (ipSeedArray)&0xffu;
	
	Cal[0] = (seed[0] ^ Mask[0]);
	Cal[1] = (seed[1] ^ Mask[1]);
	Cal[2] = (seed[2] ^ Mask[2]);
	Cal[3] = (seed[3] ^ Mask[3]);
	
	iopKeyArray[3] = ((Cal[2] & 0x03) << 5) | ((Cal[3] & 0xED) >> 3);
	iopKeyArray[2] = ((Cal[3] & 0xFC) | (Cal[0] & 0xC9) >> 7);
	iopKeyArray[1] = (Cal[0] & 0xFC) | (Cal[1] & 0x3A);
	iopKeyArray[0] = ((Cal[1] & 0x03) << 6) | (Cal[2] & 0x3E);

	key =(u32)iopKeyArray[3];
	key |= ((u32)iopKeyArray[2])<<8;
	key |= ((u32)iopKeyArray[1])<<16;
	key |= ((u32)iopKeyArray[0])<<24;
	return	key ;
}*/


/*
*@brief : 知豆种子、秘钥算法，参考Z1项目诊断需求规范9.1
*autor:  xz  
*Time ： 20180524
*/
 /* for test
 int Seed_APP[10][4]={
		{0xad,0x21,0xc6,0x3b},//0x0f 0xb4 0x3d 0xb1 	1
		{0xe1,0x4f,0x8f,0x24},//0xeb 0x6c 0x85 0x41		2
		{0x9a,0x07,0x02,0x41},//0x5d 0x4c 0x1d 0x11		3
		{0x26,0xff,0x86,0xaa},//0x75 0x2c 0x3d 0xa1		4
		{0x48,0xbd,0x21,0x9f},//0xb9 0x24 0x15 0xf1		5
		{0x56,0x11,0xec,0x26},//0xd5 0x74 0x6d 0x61		6
		{0x47,0xe3,0xc4,0xf6},//0xb7 0xbc 0x2d 0x61		7
		{0x0e,0x2e,0x43,0x94},//0x45 0xe8 0x25 0x41		8
		{0x27,0x4d,0xee,0xbc},//0x77 0x64 0x7d 0xc1		9
		{0x6f,0x76,0xb0,0x02},//0x07 0x08 0x8d 0x21		10
};

 u32 key[10][4]={
			{0x0f, 0xb4, 0x3d ,0xb1 },	 //  	1
			{0xeb, 0x6c, 0x85 ,0x41	},	//  
			{0x5d, 0x4c, 0x1d ,0x11	},	//  
			{0x75, 0x2c, 0x3d ,0xa1	},	//  
			{0xb9, 0x24, 0x15 ,0xf1	},	//  
			{0xd5, 0x74, 0x6d ,0x61	},	//  
			{0xb7, 0xbc, 0x2d ,0x61	},	//  
			{0x45, 0xe8, 0x25 ,0x41	},	//  
			{0x77, 0x64, 0x7d ,0xc1	},	//  
			{0x07, 0x08, 0x8d ,0x21	},	//0 
};	*/
 
void SecuriyAlgorithmApp(int *seed,int *key, int MASK ,int len)
{	
	int i;
 	int mask[4] = {0x00};	
 	mask[3] = (MASK & 0x000000FF)>>0;
 	mask[2] = (MASK & 0x0000FF00)>>8;
 	mask[1] = (MASK & 0x00FF0000)>>16;
 	mask[0] = (MASK & 0xFF000000)>>24;
	for (i = 0; i < len; i++) 
 	{ 
	//modify by requirement
	 	key[i] = (((mask[i]>>(i+1)) + (seed[i]<<(i+1))) & 0x000000FF); 
	 }
}


//******************************************************************************
// DESCRIPTION:诊断服务$10 会话模式 for app
// ARUGMENT   : //buffer为sid后开始数据内容 len为sid后开始的数据长度（帧结构）
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8  SID_10_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	if(len!=0x01)//判断长度
	{
		responce_result=FOR_ERR;
        dbg_msg(dev, "$10 service 不正确的消息长度···\r\n");
		can_send_NRC(DIAG_SESSION_SID,FOR_ERR);
		return  responce_result;
	}
	switch(buffer[0])//数据内容  第一个字节buffer[0]代表子服务选项
	{
		case 0x01:				  
		  	if(diag.sesion_mode == DIAG_PR_MODE)
		  	{
		  		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
				can_send_NRC(DIAG_SESSION_SID,CONDITION_ERR);
				return responce_result;	
			}
		    diag.sesion_mode = DIAG_DE_MODE;//设置进入默认会话模式
			diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;//安全状态锁定
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;//安全状态锁定
		    diag.dtc_function_enable = DIAG_DTC_FUNCTION_ON;//使能DTC  
		    if(diag.communication_enable == DIAG_COMM_RX_AND_TX_DISABLE)//判断通信控制使能
		    {
		      diag.communication_enable   = DIAG_COMM_RX_AND_TX_ENABLE;
			//	diag_comm_enable( );//init_can(0);  //test
		    }
			
			//dbg_msg(dev, "Enter $10 01 service success,Need positive response\r\n");
		    //responce contect   $10服务的正响应格式
			send_data[0] = 0x06;//长度
			send_data[1] = 0x50;
			send_data[2] = 0x01;//返回 进入 01 子服务（diagnostic session type）	    	    
			send_data[3] = (client_application_layer_tim.P2can&0xFF00)>>8; //存放高字节
			send_data[4] = client_application_layer_tim.P2can&0xFF;//低字节
			send_data[5] =(client_application_layer_tim.P2_asterisk_can&0xFF00)>>8;// 
			send_data[6] =client_application_layer_tim.P2_asterisk_can&0xFF;//低字节 
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
		break;
			
		case 0x02:
			if((diag.address_type == DIAG_F_ADDRESS)||(diag.sesion_mode == DIAG_DE_MODE))//编程会话必须是物理寻址模式		  
			{
				responce_result=NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//DIAG_PR_MODE不支持功能寻址
				can_send_NRC(DIAG_SESSION_SID,CONDITION_ERR);
				return responce_result;					
			}
			diag.sesion_mode =DIAG_PR_MODE;
			diag.address_type =DIAG_F_ADDRESS;
		  	diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;
			dbg_msg(dev,"预备升级，跳转到BOOT\r\n");		
			send_data[0] = 0x02;
			send_data[1] = 0x50;
			send_data[2] = 0x02;
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//进入编程模式的肯定响应
			//comm_message_send(dev,CM_UPDATE_ARM_READY, 0,NULL , 0);//通知ARM进入uboot
			dev->ipc->app_jump = 1;//升级标志位 置1
			systick_stop();
			device_deinit(dev);
			process_switch(dev->ipc->boot_entry, dev->ipc->boot_stack);//跳转
			break;
			
		case 0x03:
			if(diag.sesion_mode == DIAG_PR_MODE)//	非默认模式转换	  
			{
				responce_result=NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//DIAG_PR_MODE不支持功能寻址
				return responce_result; 				
			}
			diag.sesion_mode = DIAG_EX_MODE;
		  	diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;	
		   //responce contect
		 	dbg_msg(dev,"$10 03 for PR mode, checkPreconditon sucessed\r\n");
			send_data[0] = 0x06;//长度
			send_data[1] = 0x50;
		    send_data[2] = 0x03;//返回 进入 01 子服务（diagnostic session type）	    	    
		    send_data[3] = (client_application_layer_tim.P2can&0xFF00)>>8; //存放高字节
		    send_data[4] = client_application_layer_tim.P2can&0xFF;//低字节
		    send_data[5] =(client_application_layer_tim.P2_asterisk_can&0xFF00)>>8;// 
		    send_data[6] =client_application_layer_tim.P2_asterisk_can&0xFF;//低字节 
		   	can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
		break;

		case 0x81 :
			if(diag.sesion_mode == DIAG_PR_MODE)
		  	{
		  		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
				can_send_NRC(DIAG_SESSION_SID,CONDITION_ERR);
				return responce_result;	
			}
		    diag.sesion_mode = DIAG_DE_MODE;//设置进入默认会话模式
			diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;//安全状态锁定
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;//安全状态锁定
		    diag.dtc_function_enable = DIAG_DTC_FUNCTION_ON;//使能DTC  
		    if(diag.communication_enable == DIAG_COMM_RX_AND_TX_DISABLE)//判断通信控制使能
		    {
		      diag.communication_enable   = DIAG_COMM_RX_AND_TX_ENABLE;
			//	diag_comm_enable( );//init_can(0);  //test
		    }
			
			dbg_msg(dev, "Enter $10 81 service success,No Positive Response Required\r\n");
			
		break;

		case 0x82 :
			if(diag.address_type == DIAG_F_ADDRESS)//编程会话必须是物理寻址模式		  
			{
				responce_result=NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//DIAG_PR_MODE不支持功能寻址
				can_send_NRC(DIAG_SESSION_SID,CONDITION_ERR);
				return responce_result;					
			}
//			if(check1_result==0)
//			{
//				dbg_msgv(dev,"Don`t receive $31 01 02 02,\r\n");
//				send_data[0] = 0x04 ;
//				send_data[1] = 0x7f;
//				send_data[2] = 0x10;
//				send_data[3] = 0x02;
//				send_data[4] = 0x22;//condition not correct
//				can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//返回否定响应
//				return NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;
//			}
			dbg_msg(dev,"$10 82 for PR mode, checkPreconditon sucessed\r\n");
			send_data[0] = 0x02;
			send_data[1] = 0x50;
			send_data[2] = 0x02;
	     //	can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//进入编程模式的肯定响应
			dev->ipc->app_jump = 1;//升级标志位 置1
			process_switch(dev->ipc->boot_entry, dev->ipc->boot_stack);//跳转
		break;

		case 0x83 :
			if(diag.sesion_mode == DIAG_PR_MODE)//	非默认模式转换  安全状态切换到锁定状态，但通信控制服务和控制DTC设置服务的状态不受影响	  
			{
 				diag.security_level1_state=DIAG_SECURITY_LEVEL_STATE_LOCK; //安全状态锁定
				diag.security_level3_state=DIAG_SECURITY_LEVEL_STATE_LOCK;//安全状态锁定
			}
			diag.sesion_mode = DIAG_EX_MODE;
		  	diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;	
		   //responce contect
		  	dbg_msg(dev,"$10 83 for PR mode,No Positive Response Required\r\n");
		break;
			
		default:
				dbg_msg(dev,"不支持子服务\r\n");
				responce_result=SUB_ERR;//buffer[0]值不对应01 02 03子服务不支持
			 	can_send_NRC(DIAG_SESSION_SID,SUB_ERR);
			break;			
	}	
	return responce_result;		
}


//******************************************************************************
// DESCRIPTION: $11  ECUReset  支持01 hardReset,03 softReset，需要正响应回复
// ARUGMENT   : buffer为sid后开始数据内容 len为sid后开始的数据长度
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_11_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	
	if(len!=0x01)//判断长度
	{
		responce_result=FOR_ERR;
		dbg_msg(dev, "$11 service 不正确的消息长度···\r\n");
		can_send_NRC(DIAG_ECU_RESET_SID,FOR_ERR);
		return  responce_result;
	}

	switch(buffer[0])
	{
		case 0x01:  //hardReset
			if((diag.sesion_mode == DIAG_PR_MODE)&&(diag.address_type==DIAG_F_ADDRESS))
		  	{
		  		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
				can_send_NRC(DIAG_ECU_RESET_SID,CONDITION_ERR);
				return responce_result;	
			}
			
			diag.sesion_mode = DIAG_EX_MODE;
			dbg_msg(dev, "HardReset···\r\n");
			send_data[0] = 2;
			send_data[1] = 0x51;//$11服务的正响应格式
			send_data[2] = 0x01;	
		 	can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
			/*PMIC断*/
			IO_SET(GPIOA, 3, LOW);
			mdelay(10);
			software_reset(); 
			break;
	
		case 0x03: //softReset	   
			if((diag.sesion_mode == DIAG_PR_MODE)&&(diag.address_type==DIAG_F_ADDRESS))
		  	{
		  		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
				can_send_NRC(DIAG_ECU_RESET_SID,CONDITION_ERR);
				return responce_result;	
			}	
			dbg_msg(dev, "SoftReset···\r\n");
			send_data[0] = 2;
			send_data[1] = 0x51;//$11服务的正响应格式
		    send_data[2] = 0x03;
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);	
			IO_SET(GPIOA, 3, LOW);
			mdelay(10);
			software_reset(); // never return 
			break;
			
		case 0x81:	//hardReset		
			diag.sesion_mode = DIAG_EX_MODE;
			dbg_msg(dev, "Enter $11 81 service success,N0 positive response\r\n");
			mdelay(10);
			IO_SET(GPIOA, 3, LOW);
			mdelay(10);
			software_reset(); 	
			break;
			
		case 0x83: //softReset	   
			if(diag.sesion_mode == DIAG_PR_MODE)
			{
				responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
				can_send_NRC(DIAG_SESSION_SID,CONDITION_ERR);
				return responce_result; 
			}
			dbg_msg(dev, "Enter $11 81 service success,N0 positive response\r\n");
			mdelay(10);
			software_reset(); 
		break;
		
		default:
			dbg_msg(dev,"不支持子服务\r\n");
			responce_result=SUB_ERR;//buffer[0]值不对应01 02 03子服务不支持
			can_send_NRC(DIAG_ECU_RESET_SID,SUB_ERR);
		break;			
	}
  return responce_result;
	
}

//******************************************************************************
// DESCRIPTION:$14服务 ClearDiagnosticInformation 子服务$FFFFFF(All groups)  高中低三位组成 
// RETURN     :

// AUTHOR     :
//******************************************************************************
u8 SID_14_fun(device_info_t *dev,u8 *buffer,u16 len)
{
    u8 send_data[8] = {0};
    u8 responce_result=NO_ERR;
    if(len != 0x03)             
    {
		responce_result=FOR_ERR;
		dbg_msg(dev, "$14 service 不正确的消息长度···\r\n");
		can_send_NRC(DIAG_CLR_DTC_SID,FOR_ERR);
		return  responce_result;
    }
		
    if((buffer[0]==0xff)&&(buffer[1]==0xff)&&(buffer[2]==0xff))//高、中、低位均为FF 激活  
    {
		clear_dtc() ;   // 清除 故障码 ,设置掩码为0x00，即可清零
    }
	else
	{
		dbg_msg(dev,"不支持子服务\r\n");
		responce_result=OUTRANGE_ERR;//buffer[0]值不对应01 02 03子服务不支持
		can_send_NRC(DIAG_CLR_DTC_SID,OUTRANGE_ERR);
		return responce_result;
	}
	dbg_msg(dev, "Enter $14 service success,Need positive response\r\n");
	send_data[0] = 0x01;
    send_data[1] = 0x54;  //$14 正响应回复Value 54 
    can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);	 

//	save_dtc( );
	
    return responce_result;    
}

//******************************************************************************
// DESCRIPTION:$19 read DTC information
// ARUGMENT   : //buffer为sid后开始数据内容 len为sid后开始的数据长度  
// RETURN     :支持01 02 03 04 0A
// AUTHOR     :
//******************************************************************************
u8 SID_19_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 send_data[AVM_DTC_MAX*4+3]={0};  //DTC 码
	u8 responce_result=NO_ERR;
	u8 tester_mask;
	u16 dtc_count=0;
 	if(len != 0x02)             
  	{
		responce_result=FOR_ERR;
		dbg_msg(dev, "$19 不正确的消息长度···\r\n");
		can_send_NRC(DIAG_R_DTC_SID,FOR_ERR);
		return  responce_result;
    }	
	switch(buffer[0])
	{
		case 0x01:		//通过状态掩码报告故障码个数,tester_mask=1时读当前故障，0x08时读历史故障
			tester_mask = buffer[1];
			if(diag.sesion_mode == DIAG_PR_MODE) //编程会话时  
			{
   			responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
				can_send_NRC(DIAG_R_DTC_SID,NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);
				return responce_result;	
			}
		//	dbg_msg(dev, "Enter $19 01 service success,Need positive response\r\n");
			dtc_count = read_dtc_01_02_sub_id(send_data,0x01,tester_mask);
			send_data[0] = 0x06;
			send_data[1] = 0x59;//正响应格式
			send_data[2] = 0x01;//request type
			send_data[3] = ECU_SUPPORT_MSAK;//
			send_data[4] = 0x00;//ISO15031-6DTCformat 为0 DTC_Format_Identifier
			send_data[5] = dtc_count>>8u;//DTC_Count 高字节
			send_data[6] = dtc_count&0x0F;//低字节
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);	
			break;
			
		case 0x02://通过状态掩码报告故障码  多帧发送
			tester_mask = buffer[1];
			if(diag.sesion_mode == DIAG_PR_MODE)  //不支持编程模式
			{
   			responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
				can_send_NRC(DIAG_R_DTC_SID,CONDITION_ERR);
				return responce_result;	
			}
//			dbg_msg(dev, "Enter $19 02 service success,Need positive response\r\n");
			send_data[1] = 0x059;//request type
			send_data[2] = 0x02;//子服务
			send_data[3] =ECU_SUPPORT_MSAK ;//DTCstatusavailabilityMask.
			dtc_count = read_dtc_01_02_sub_id(&send_data[4],0x02,tester_mask);
			dbg_msgv(dev, "dtc_count : %d\r\n", dtc_count);
			if(dtc_count<=1)
			{
				send_data[0]=dtc_count*4+3;
				can_msg_Send_SWJ(dev,AVM_Phy_Resp,(u32*)send_data);
				}
			else{
				can_net_layer_send.len=dtc_count*4+3;
				memcpy(&can_net_layer_send.buff[0],&send_data[1],can_net_layer_send.len);
				diag_send_data(dev,can_net_layer_send.buff,can_net_layer_send.len);
				}
			break;
		
		default:
			dbg_msg(dev,"不支持子服务\r\n");
			responce_result=SUB_ERR;//buffer[0]值不对应01 02 03子服务不支持
			can_send_NRC(DIAG_R_DTC_SID,SUB_ERR);
		break;				
	}

	//读取$19  DTC 后续动作
  return responce_result;
}


//******************************************************************************
// DESCRIPTION:$22  通过标识符读某个DID数据服务 for app  
// ARUGMENT   : buffer为sid后开始数据内容 len为sid后开始的数据长度 
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_22_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 responce_result=NO_ERR;
	u8 send_did_data[60]={0};//单个最大长度 上限为60  
	u16 DID = 0;//DID value  2个字节   
	if(len!=0x02)//判断长度
	{
		responce_result=FOR_ERR;
		dbg_msg(dev, "$22 service 不正确的消息长度···\r\n");
		can_send_NRC(DIAG_R_DID_SID,FOR_ERR);
		return  responce_result;
	}
 	if(diag.sesion_mode==DIAG_PR_MODE)//编程会话不支持
	{
		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
		can_send_NRC(DIAG_R_DID_SID,CONDITION_ERR);
		return responce_result;
	}
	DID = buffer[0]<<8;//DID MSB
	DID += buffer[1];//DID LSB
	//dbg_msg(dev, "Enter $22 service success,Need positive response\r\n");
	switch( DID)
	{	//回复均为>8字节
		case DID_ECUConfiguration : 
			para_read_Nbyte( ADDR_ECUConfiguration,send_did_data,LEN_ECUConfiguration);
			can_net_layer_send.len = LEN_ECUConfiguration;//+3 包含22服务和DID两个字节
			responce_result=NO_ERR;
		break;
		
		case DID_ECUSoftwareVersion : 
			para_read_Nbyte( ADDR_SoftwareVersion,send_did_data,LEN_ECUSoftwareVersion);
			can_net_layer_send.len = LEN_ECUSoftwareVersion;//+3 包含22服务和DID两个字节
			responce_result=NO_ERR;
		break;

		case DID_ECUHardwareVersion: //  零部件号 15字节
			para_read_Nbyte( ADDR_HardwareVersion,send_did_data,LEN_ECUHardwareVersion);
			can_net_layer_send.len = LEN_ECUHardwareVersion;
			responce_result=NO_ERR;
		break;

		case DID_Part_Number: // 供应商代码 9个字节
			para_read_Nbyte( ADDR_Part_Number,send_did_data,LEN_Part_Number);
			can_net_layer_send.len = LEN_Part_Number;
			responce_result=NO_ERR;
		break;

		case DID_System_Supplier:	//	电控单元串号		
			para_read_Nbyte( ADDR_System_Supplier,send_did_data,LEN_System_Supplier);
			can_net_layer_send.len = LEN_System_Supplier;
			responce_result=NO_ERR;
		break;
		
		case DID_ECUSerialNumber:  // 供应商电子控制单元硬件版本号  3个字节
			para_read_Nbyte( ADDR_Part_Number,send_did_data,LEN_SerialNumber);
			can_net_layer_send.len = LEN_SerialNumber;
			responce_result=NO_ERR;
		break;
		
		case DID_VINDataIdentifier: // 	
			para_read_Nbyte( ADDR_VINDataIdentifier,send_did_data,LEN_VINDataIdentifier);
			can_net_layer_send.len = LEN_VINDataIdentifier;
			responce_result=NO_ERR;
		break;
		
		default:
			dbg_msg(dev,"不支持子服务\r\n");
			responce_result=OUTRANGE_ERR;//buffer[0]值不对应01 02 03子服务不支持
			//can_send_NRC(DIAG_R_DID_SID,OUTRANGE_ERR);
		break;
		}
		if(responce_result==NO_ERR)
		{		
			can_net_layer_send.buff[0] = 0x62;
			can_net_layer_send.buff[1] = buffer[0];
			can_net_layer_send.buff[2] = buffer[1];
			memcpy(&can_net_layer_send.buff[3],send_did_data,can_net_layer_send.len);
			can_net_layer_send.len +=3;
//			dbg_msgfv(dev, "返回DID多帧长度为：%d\r\n", can_net_layer_send.len);
			diag_send_data(dev,can_net_layer_send.buff, can_net_layer_send.len);//多帧发送
		}
	return  responce_result;
}

//******************************************************************************
// DESCRIPTION:$27 SecurityAccess 01RequestSeed 02Sendkey 和07 08对应
// ARUGMENT   : //buffer为sid后开始数据内容 len为sid后开始的数据长度  
// RETURN     :
// AUTHOR     :
//******************************************************************************

u8 SID_27_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	int Security_key[4],app_password[4];  //ECU-self calculated key 自己计算的Key  4个字节
//应该考虑到负响应的优先级
	if((len!=0x01)&&(len!=0x05))//判断长度
	{
		diag.SAFC_1++;
		if(diag.SAFC_1>=3)
		{
			dbg_msg(dev,"security failed 3 times\r\n");
			diag.SAFC_1=3;
			//diag.open_securitu_timer=0x01;
			//stimer_start(dev, FLAG_SECURITY_DELAY,TO_CNT_1MIN);
			//diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;
			//responce_result=OVER_TRY_ERR;
			//can_send_NRC(DIAG_SECURITY_ACCESS_SID,OVER_TRY_ERR);
			//return responce_result; 
		}
		responce_result=FOR_ERR;
		dbg_msg(dev, "$27 service 不正确的消息长度···\r\n");
		can_send_NRC(DIAG_SECURITY_ACCESS_SID,FOR_ERR);
		return  responce_result;
		} 

	
	if((diag.sesion_mode ==DIAG_DE_MODE)||(diag.sesion_mode ==DIAG_PR_MODE)||(diag.address_type ==DIAG_F_ADDRESS )	)	 
	{
		diag.SAFC_1++;
		if(diag.SAFC_1>=3)
		{
			dbg_msg(dev,"security failed 3 times\r\n");
			diag.SAFC_1=0;
			diag.open_securitu_timer=0x01;
			stimer_start(dev, FLAG_SECURITY_DELAY,TO_CNT_1MIN);
			diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;
			//responce_result=OVER_TRY_ERR;
			//can_send_NRC(DIAG_SECURITY_ACCESS_SID,OVER_TRY_ERR);
			//return responce_result; 
		}
		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=7f 	
		can_send_NRC(DIAG_SECURITY_ACCESS_SID,NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);
		return responce_result; 
		}

	switch(buffer[0])
	{
	//for App Seed and key
	case 0x01:	//app程序 ：01 02 在扩展会话 物理寻址才 可用			  	   							   						
			
		if(diag.open_securitu_timer==0x01)//开启定时器标志位
		{
			if(get_flag(dev,FLAG_SECURITY_DELAY))
			{
			dbg_msg(dev,"delay 60s arrived，retry\r\n");
			diag.SAFC_1=0;
			diag.open_securitu_timer=0x00;//关闭定时器
			stop_timer(FLAG_SECURITY_DELAY);
			}
		else//定时未到
		{
			dbg_msg(dev,"delay 60s is't arrived，please wait\r\n");
			responce_result =DELAY_NOT_ARRIVAL_ERR; 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,DELAY_NOT_ARRIVAL_ERR);
			return  responce_result;
			}	
		}

			
		if(diag.security_level1_state == DIAG_SECURITY_LEVEL_STATE_UNLOCK)  //if having been unlock,send datas 00 00 00 00，已经解锁安全访问
		{	
			 send_data[0] = 0x06;
     		 send_data[1] = 0x67;//正响应格式
     		 can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);	   	   
		}
		else
		{
            diag.security_ECU_Mask = ECU_Mask_APP;
			dbg_msg(dev, "Enter $27 01 service success,Back seed\r\n");
			send_data[0] = 0x06;//长度
			send_data[1] = 0x67;
            send_data[2] = 0x01;
			//4个字节的种子放入send_data
			srand(get_systick_value());
			diag.security_seed[0]=rand()%0xff;
			diag.security_seed[1]=rand()%0xff;
			diag.security_seed[2]=rand()%0xff;
			diag.security_seed[3]=rand()%0xff;
			diag.security_seed[3]=0x7a;
			diag.security_seed[2]=0xd6;
			diag.security_seed[1]=0xde;
			diag.security_seed[0]=0x09;
			send_data[3]=diag.security_seed[0];// seed高字节放前面
			send_data[4]=diag.security_seed[1];
			send_data[5]=diag.security_seed[2];
			send_data[6]=diag.security_seed[3];
			//	dbg_msgv(dev,"seed = 0x%02x-0x%02x-0x%02x-0x%02x\r\n",send_data[3],send_data[2],send_data[1],send_data[0]);
            can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);//返回seed
            diag.security_level1_access_step = DIAG_HAD_REQ_SEED;//确认安全访问步骤
		}			
		break;
		
	case 0x02://返回 key

		if(diag.security_level1_state == DIAG_SECURITY_LEVEL_STATE_UNLOCK)  //if having been unlock,send datas 00 00 00 00，已经解锁安全访问
		{	
			 send_data[0] = 0x06;
     		 send_data[1] = 0x67;//正响应格式
     		 can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);
			 
		}
		else{	
		if(diag.security_level1_access_step != DIAG_HAD_REQ_SEED) 
		{
			diag.SAFC_1++;
			if(diag.SAFC_1>=3)
			{
				dbg_msg(dev,"security failed 3 times\r\n");
				diag.SAFC_1=3;
				//diag.open_securitu_timer=0x01;
				//stimer_start(dev, FLAG_SECURITY_DELAY,TO_CNT_1MIN);
				//diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;
				//responce_result=OVER_TRY_ERR;
				//can_send_NRC(DIAG_SECURITY_ACCESS_SID,OVER_TRY_ERR);
				//return responce_result; 
			}
			responce_result = SEQUENCE_ERROR;//条件不满足NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,SEQUENCE_ERROR);
			return responce_result; 
			}
		else
		{
			diag.security_level1_access_step = DIAG_NOT_REQ_SEED; 
			}

		
		//obtain the key from tester获取Key from 诊断仪
		app_password[0] = buffer[1];
		app_password[1]=  buffer[2];
		app_password[2] = buffer[3];
		app_password[3] = buffer[4];
		SecuriyAlgorithmApp(diag.security_seed,Security_key,ECU_Mask_APP,4);

		//dbg_msgv(dev,"seed = 0x%02x-0x%02x-0x%02x-0x%02x\r\n",diag.security_seed[0],diag.security_seed[1],diag.security_seed[2],diag.security_seed[3]);
		//dbg_msgv(dev,"Security_key = 0x%02x-0x%02x-0x%02x-0x%02x\r\n",Security_key[0],Security_key[1],Security_key[2],Security_key[3]);
		//dbg_msgv(dev,"receive_key = 0x%02x-0x%02x-0x%02x-0x%02x\r\n",buffer[1],buffer[2],buffer[3],buffer[4]);
		if(memcmp(Security_key,app_password,4)!=0)  
		{ 
			dbg_msg(dev,"key is wrong\r\n");
            diag.SAFC_1++;
            if(diag.SAFC_1>=3)
			{
				dbg_msg(dev,"security failed 3 times\r\n");
				diag.SAFC_1=0;
				diag.open_securitu_timer=0x01;
				stimer_start(dev, FLAG_SECURITY_DELAY,TO_CNT_1MIN);
				diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;
				responce_result=OVER_TRY_ERR;
				can_send_NRC(DIAG_SECURITY_ACCESS_SID,OVER_TRY_ERR);
				return responce_result; 
		 		}
			else
			{
				
			if(diag.open_securitu_timer==0x01)//开启定时器标志位
			{
				if(get_flag(dev,FLAG_SECURITY_DELAY))
				{
					dbg_msg(dev,"delay 60s arrived，retry\r\n");
					diag.SAFC_1=0;
					diag.open_securitu_timer=0x00;//关闭定时器
					stop_timer(FLAG_SECURITY_DELAY);
					}
				else//定时未到
				{
					dbg_msg(dev,"delay 60s is't arrived，please wait\r\n");
					responce_result =DELAY_NOT_ARRIVAL_ERR; 
					can_send_NRC(DIAG_SECURITY_ACCESS_SID,DELAY_NOT_ARRIVAL_ERR);
					return  responce_result;
					}	
				}
			}
			responce_result=INVALID_KEY_ERR;
			can_send_NRC(DIAG_SECURITY_ACCESS_SID , responce_result);
			//dbg_msg(dev,"key wrong, 3\r\n");
			return responce_result;		
		}		
		else
		{
			dbg_msg(dev,"key is right\r\n");
			diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;
			diag.SAFC_1 = 0;
			diag.open_securitu_timer=0x00;//关闭定时器
			stop_timer(FLAG_SECURITY_DELAY);//60s延时定时器
			diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_UNLOCK; //匹配后清零标志位，发送解除标志位
		}
		send_data[0] = 0x02;
		send_data[1] = 0x67;
		send_data[2] = 0x02;
	//	dbg_msg(dev,"秘钥正确返回正响应\r\n");
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);	
		diag.security_level1_access_step = DIAG_NOT_REQ_SEED;//第二步？？？标志位？？
		}
		break;
	#if 0	
	case 0x21:	//app程序 ：01 02 在扩展会话 物理寻址才 可用	
		//长度合法性判断
		if(len!=0x01)//判断长度
			{
			responce_result=FOR_ERR;
			//dbg_msg(dev, "$27 service 不正确的消息长度···\r\n");
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,FOR_ERR);
			return  responce_result;
			}  
		if((diag.sesion_mode !=DIAG_PR_MODE)||(diag.address_type !=DIAG_P_ADDRESS )  )    
			{
			responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,CONDITION_ERR);
			return responce_result; 
			}
		if((diag.SAFC_1>=3)&&(diag.SAFC_reach_3num_mcu_resetbit == DIAG_ECU_HAD_NOT_RESET))                     
			{
			responce_result = DELAY_NOT_ARRIVAL_ERR;//
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,DELAY_NOT_ARRIVAL_ERR);
			return responce_result; 
			}
		if(diag.security_level1_state == DIAG_SECURITY_LEVEL_STATE_UNLOCK)  //if having been unlock,send datas 00 00 00 00，已经解锁安全访问
		{	
			send_data[0] = 0x06;
            send_data[1] = 0x67;//正响应格式
            can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);	   	   
		}
		else
		{
            diag.security_seed = Seed_BOOT;//(uint32_t)(rand());	
            diag.security_ECU_Mask = ECU_Mask_Boot;
			//dbg_msg(dev, "Enter $27 01 service success,Back seed\r\n");
			send_data[0] = 0x06;//长度
			send_data[1] = 0x67;
            send_data[2] = 0x21;
			//4个字节的种子放入send_data
            send_data[3] = diag.security_seed>>24;
            send_data[4] = (diag.security_seed >>16)&0xFF;
            send_data[5] = (diag.security_seed>>8)&0xFF;
            send_data[6] = diag.security_seed &0xFF;
            can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);//返回seed
            diag.security_level1_access_step = DIAG_HAD_REQ_SEED;//确认安全访问步骤
		}			
		break;
		
	case 0x22://返回 key  
		if((diag.sesion_mode !=DIAG_PR_MODE)&&(diag.address_type !=DIAG_F_ADDRESS )  )    	 
			{
			responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//条件不满足NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,CONDITION_ERR);
			return responce_result; 
			}
		if((diag.security_level1_state == DIAG_SECURITY_LEVEL_STATE_UNLOCK)||(diag.security_level1_access_step != DIAG_HAD_REQ_SEED))    
			{
			responce_result = SEQUENCE_ERROR;//条件不满足NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,SEQUENCE_ERROR);
			return responce_result; 
			}
		if(diag.security_level1_access_step != DIAG_HAD_REQ_SEED) 
			{
			responce_result = SEQUENCE_ERROR;//条件不满足NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,SEQUENCE_ERROR);
			return responce_result; 
			}
		else
			diag.security_level1_access_step = DIAG_NOT_REQ_SEED;

		//obtain the key from tester获取Key from 诊断仪
		PassWord = ((u32)buffer[4])<<24;
		PassWord |= ((u32)buffer[3])<<16;
		PassWord |= ((u32)buffer[2])<<8;
		PassWord |=(u32) buffer[1];
		//dbg_msgv(dev,"PassWord  = %08x\r\n",PassWord);
		//ECU-self calculate the Security_key 
		Security_key_boot = SecuriyAlgorithmFBL(Seed_BOOT,ECU_Mask_Boot);
	//	dbg_msgv(dev,"Security_key_boot  = %08x,PassWord=%08x\r\n",Security_key_boot,PassWord);
		//Security_key_boot =PassWord;//for  test，上位机秘钥有问题
		//compare the PassWord with Security_key
		if(PassWord != Security_key_boot)  
		{ 
            diag.SAFC_1++;
            if(diag.SAFC_1 <= 3)
            { 
                if(diag.SAFC_1==3)
                {
                    diag.time_SAFC_1 = 0; //OSTimeGet(&os_err); //test
                    diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;
					 responce_result=OVER_TRY_ERR;
                }
                else
                {
                     responce_result=INVALID_KEY_ERR;
                }
            }
            else
            {  
                diag.SAFC_1 = 3;
                responce_result=OVER_TRY_ERR;
            }
			can_send_NRC(DIAG_SECURITY_ACCESS_SID , responce_result);
			return responce_result;
		}
		else
		{
			diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;
			diag.SAFC_1 = 0;
			diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_UNLOCK; //匹配后清零标志位，发送解除标志位
			send_data[0] = 0x02;
			send_data[1] = 0x67;
			send_data[2] = 0x22;
		//dbg_msg(dev,"秘钥正确\r\n");
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);	
			diag.security_level1_access_step = DIAG_NOT_REQ_SEED;//第二步？？？标志位？？
		}
		break;
	#endif	
	default:
		responce_result=SUB_ERR;
		can_send_NRC(DIAG_SECURITY_ACCESS_SID , responce_result);
		break;			
	}
	
	return responce_result;
}



//******************************************************************************
// DESCRIPTION:
// ARUGMENT   : buffer为sid后开始数据内容 len为sid后开始的数据长度  
// RETURN     :
// AUTHOR     :
//******************************************************************************
uint8_t SID_28_fun(device_info_t *dev,uint8_t *buffer,uint16_t len)
{
	uint8_t send_data[8]={0};
	uint8_t responce_result=NO_ERR;
	//长度合法性判断
	if(len!=0x02)
	{
		responce_result=FOR_ERR;  
		can_send_NRC(DIAG_COMM_CONTROL_SID,FOR_ERR);
		return responce_result; 
	}
	if(diag.sesion_mode!=DIAG_EX_MODE)
	{
		can_send_NRC(DIAG_COMM_CONTROL_SID,NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);
		return (NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);      			
			}	
	switch(buffer[0])
	{
		case 0x00:				    
			if(buffer[1]!= 0x03)
			{  
				can_send_NRC(DIAG_COMM_CONTROL_SID,OUTRANGE_ERR);
				return (OUTRANGE_ERR);				
			}	
			send_data[0] = 0x02;
			send_data[1] = 0x68;	
			send_data[2] = DIAG_COMM_RX_AND_TX_ENABLE;
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);		   	 
			diag.communication_enable = DIAG_COMM_RX_AND_TX_ENABLE;					
			//diag_comm_enable( );//test			   
			break;	
		case 0x03:		 
			if(buffer[1]!= 0x03)
			{    
				can_send_NRC(DIAG_COMM_CONTROL_SID,OUTRANGE_ERR);
				return (OUTRANGE_ERR);				
			}										
			send_data[0] = 0x02;
			send_data[1] = 0x68;
			send_data[2] = DIAG_COMM_RX_AND_TX_DISABLE;
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);  
			diag.communication_enable = DIAG_COMM_RX_AND_TX_DISABLE;						
			break;
			
		case 0x80:				     	 
			diag.communication_enable = DIAG_COMM_RX_AND_TX_ENABLE;					
			//diag_comm_enable( );//test			   
			break;	
		case 0x83:	/*Normal Communication Messages(0x01) and Network Management Messages(0x02)  both  0x03*/
			diag.communication_enable = DIAG_COMM_RX_AND_TX_DISABLE;						
			//diag_comm_disable( );	test  禁止无关通信	车身报文 网络报文都禁止
			break;
		default:
			can_send_NRC(DIAG_COMM_CONTROL_SID,SUB_ERR);
			responce_result=SUB_ERR;
			break;			
	}
	return responce_result;
	}
//******************************************************************************
// DESCRIPTION:
// ARUGMENT   : buffer为sid后开始数据内容 len为sid后开始的数据长度  
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_2E_fun(device_info_t *dev,uint8_t *buffer,uint16_t len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;	
	u16 DID;	
	DID=buffer[0];
	DID=(DID<<8)+buffer[1]; //  
	if((len!=19)&&(len!=7)&&(len!=9))
	{
		responce_result=FOR_ERR;  
		can_send_NRC(DIAG_W_DID_SID,FOR_ERR);
		return responce_result; 
		}

	if((diag.address_type  == DIAG_F_ADDRESS)||(diag.sesion_mode  != DIAG_EX_MODE) )
	{
		can_send_NRC(DIAG_W_DID_SID, NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);
		return (NOT_SUPPERT_SUB_IN_ACTIVE_SESSION); 
		}
	if(diag.security_level1_state != DIAG_SECURITY_LEVEL_STATE_UNLOCK)//需安全访问
	{	
		can_send_NRC(DIAG_W_DID_SID,SECURITY_ERR );
		return (SECURITY_ERR);    
		}
		
	if (DID==DID_VINDataIdentifier)
	{
		memcpy(dev->did_t.buf,&buffer[2],LEN_VINDataIdentifier);
		dev->did_t.LEN =LEN_VINDataIdentifier/2+1;
		dev->did_t.ADDR = ADDR_VINDataIdentifier;
		//STMFLASH_Write(ADDR_VINDataIdentifier,(u16*)&buffer[2],LEN_CURRENT_DID);

	}
	else if(DID==DID_ECUConfiguration)
	{	
		memcpy(dev->did_t.buf,&buffer[2] ,LEN_ECUConfiguration);
		dev->did_t.LEN =LEN_ECUConfiguration/2+1;
		dev->did_t.ADDR = ADDR_ECUConfiguration;
		//STMFLASH_Write(ADDR_ECUConfiguration,(u16*)&buffer[2],LEN_CURRENT_DID);
		}
	/*写指纹0xF184-年月日*/
	else if(DID==DID_Fingerprint)
	{	
		memcpy(dev->did_t.buf,&buffer[2] ,LEN_Fingerprint);
		dev->did_t.LEN =LEN_Fingerprint/2+1;
		dev->did_t.ADDR = ADDR_Fingerprint;
		//STMFLASH_Write(ADDR_Fingerprint,(u16*)&buffer[2],LEN_CURRENT_DID);	
		}
	else
	{
		responce_result=OUTRANGE_ERR;//buffer[0]值不对应01 02 03子服务不支持
		can_send_NRC(DIAG_W_DID_SID,OUTRANGE_ERR);
	}

	if(dev->ci->write_did_flag)//可以写did
	{
	//	dbg_msg(dev, "time is right \r\n");
		STMFLASH_Write(dev->did_t.ADDR,(u16*)&buffer[2],dev->did_t.LEN);
		send_data[0] = 0x03;
		send_data[1] = 0x6E;
		send_data[2] = buffer[0];
		send_data[3] = buffer[1];
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
		}
	else {	
		send_data[0] = 0x03;
		send_data[1] = 0x6E;
		send_data[2] = buffer[0];
		send_data[3] = buffer[1];
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//延迟到标志位为1在写，先保存DID和长度
	//	dbg_msg(dev, "time is wrong \r\n");
		dev->did_t.flag = 1;
	//	dbg_msgv(dev, "did_data:%x\r\n",dev->did_t.ADDR);
	//	dbg_msgv(dev, "did_data:%x\r\n",dev->did_t.LEN);
	//	dbg_msgv(dev, "did_data:%s\r\n",dev->did_t.buf);
		}


	return responce_result;
}


/*****************************************************************************
* Description: 
* Argument   :  for ARM  upgrade  from  can 
* Return     :
* Author     :  20180706
*****************************************************************************/
//uint8_t RequestDownload(uint8_t *buffer, uint16_t len)
#if 1

u8 SID_31_fun(device_info_t *dev,u8 *buffer,u16 len)  
{
	u8 send_data[7]={0};
	u8 responce_result=NO_ERR;	
	u16 routine_id;//请求例程 ID
	u8 Erase_result,routine_control_type;//routine_control_type=start routine(01第二个字节)
   //数据长度判断  长度必须大于3，不同子服务数据长度不一样
	if(len<3)
	{
		responce_result=FOR_ERR; 
		can_send_NRC(DIAG_ROUTE_CONTROL_SID,FOR_ERR);
		return responce_result; 
	}
	routine_control_type=buffer[0];	                	
	routine_id=buffer[1];
	routine_id=(routine_id<<8)+buffer[2];
	if((routine_control_type== 0x01)&&(routine_id==0xFF00)&&(buffer[3]==0xff))//31  01子服务
	{
		Erase_result = upgrade_earseMemory(dev);
		//Erase_result = 0x01;//回复假的成功for test
		//dbg_msg(dev,"执行内存擦除\r\n");
		send_data[0]=0x05;
		send_data[1]=0x71;
		send_data[2]=0x01;//Routine Control Type
		send_data[3]=buffer[2];
		send_data[4]=buffer[3];
		send_data[5]=Erase_result;//成功01 失败02
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
		}
	else if((routine_control_type== 0x01)&&(routine_id==0xff00)&&(buffer[3]==0x01))//升级boot
	{
	/*	dbg_msg(dev,"uds upgrade mcu_boot,jump to app\r\n");
		send_data[0]=0x03;
		send_data[1]=0x7F;
		send_data[2]=0x31;//Routine Control Type
		send_data[3]=0x78;
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//返回78提示诊断仪等待正响应
	*/	
		dev->ipc->upgrade_mcu_boot = 1;//升级mcu_boot 	
		systick_stop();
		device_deinit(dev);
		__disable_irq();
		process_switch(dev->ipc->app_entry, dev->ipc->app_stack);//跳转
		return 1;
		}
	else if((routine_control_type== 0x01)&&(routine_id==0x0202)&&(buffer[3]==0xff))
	{
		upgrade_loadFile_check1(dev,&buffer[0],len);
		}
	else 
	{
		can_send_NRC(DIAG_ROUTE_CONTROL_SID,CONDITION_ERR);
		return responce_result=CONDITION_ERR;
		}
	return responce_result;
}

u8 SID_34_fun(device_info_t *dev,uint8_t *buffer, uint16_t len)
{
	u8 update_type;
	if((diag.sesion_mode != DIAG_PR_MODE)||(diag.address_type != DIAG_P_ADDRESS))
	{
		can_send_NRC(DIAG_REQUEST_DOWNLOAD_SID,NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);
		return  0;
	}
	
	if ( diag.security_level1_state != DIAG_SECURITY_LEVEL_STATE_UNLOCK )
	{
	    can_send_NRC(DIAG_REQUEST_DOWNLOAD_SID,SECURITY_ERR);
		return  0;
	}
	diag.file_trans_mode = 0x02; //下载模式
	update_type = buffer[10];
	if((update_type==0x10u)||(update_type==0x20u))
	{
		dbg_msg(dev,"\r\n请求下载MCU升级数据\r\n");	
		upgrade_requestFileDownload(dev,buffer, len);//解析请求下载命令，包含数据长度
		return NO_ERR;
		}
	else
	{	
		dbg_msg(dev,"请求下载ARM升级数据\r\n");
		upgrade_requestFileDownload_ARM(dev,buffer, len);//解析请求下载命令，包含数据长度
		comm_message_send(dev,CM_UPDATE_ARM_READY, 0, NULL , 0);
		/*开始计时，多长时间未收到ARM回应，表示升级失败*/
		dev->ipc->usart_normal = 0;
		start_timer(TIMER_DEV1,5000);
		return NO_ERR;
		}
}


//******************************************************************************
// DESCRIPTION:
// ARUGMENT   : //buffer为sid后开始数据内容 len为sid后开始的数据长度  
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_36_fun(device_info_t *dev,u8 *buffer,u16 len)
{
    u8 responce_result=NO_ERR;
	if((diag.sesion_mode != DIAG_PR_MODE)||(diag.address_type != DIAG_P_ADDRESS))
	{
		can_send_NRC(DIAG_TRANSFER_DATA_SID,NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);
		return  0;
	}
	
	if ( diag.security_level1_state != DIAG_SECURITY_LEVEL_STATE_UNLOCK )
	{
	    can_send_NRC(DIAG_TRANSFER_DATA_SID,SECURITY_ERR);
		return  0;
	}

 if(diag.file_trans_mode==0x02)
    {
		//子参数02为升级指令
		upgrade_transferData(dev,buffer,len);
	}
 else
	{
		can_send_NRC(DIAG_TRANSFER_DATA_SID,SUB_ERR);
		responce_result=SUB_ERR;
	}
	return responce_result;
}

uint8_t SID_37_fun(device_info_t *dev,uint8_t *buffer, uint16_t len)
{
	u8 send_data[4] = {0x03,0x7F,0x37,0x22};
	if((diag.sesion_mode != DIAG_PR_MODE)|(diag.address_type != DIAG_P_ADDRESS))
	{
		can_send_NRC(DIAG_REQUEST_TRANSFER_EXIT_SIG,NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);
		return  0;
	}
	if ( diag.security_level1_state != DIAG_SECURITY_LEVEL_STATE_UNLOCK )
	{
	    can_send_NRC(DIAG_REQUEST_TRANSFER_EXIT_SIG,SECURITY_ERR);
		return  0;
	}
	
	if(diag.file_trans_mode == 0x02)
	{
		upgrade_requestTransferExit(dev, buffer, len);
		dbg_msg(dev,"数据传输完成，退出传输服务\r\n");
		}
	else 
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
	return NO_ERR;
}
#endif

//******************************************************************************
// DESCRIPTION:
// ARUGMENT   : //buffer为sid后开始数据内容 len为sid后开始的数据长度  
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_3E_fun(device_info_t *dev,uint8_t *buffer,uint16_t len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	//长度合法性判断
	if(len!=0x01)//长度为1
	{
		responce_result=FOR_ERR; 
		can_send_NRC(DIAG_TESTER_PRESENT_SID,FOR_ERR);
		return responce_result; 
	}
	dbg_msg(dev, "Enter $3E service success\r\n");

	switch(buffer[0])
	{
		case 0x00:	//诊断仪在线
			send_data[0] = 0x02;
			send_data[1] = 0x7E;
			send_data[2] = 0x00;
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);	  				
			break;
		case 0x80://什么都不用发，
			  //return (NO_ERR);//no need response to the tester 		   	    	   
			break;		
		default:
			responce_result=SUB_ERR;
			can_send_NRC(DIAG_TESTER_PRESENT_SID,SUB_ERR); 
			break;			
	}
	return responce_result;
}

//******************************************************************************
// DESCRIPTION: buffer为sid后开始数据内容 len为sid后开始的数据长度  
// ARUGMENT   :DTC控制 EX会话模式
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_85_fun(device_info_t *dev,u8 *buffer,u8 len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	//长度合法性判断
	if(len!=0x01)
	{
	    responce_result=FOR_ERR; 
		can_send_NRC(DIAG_DTC_CONTTOL_SID,FOR_ERR);
	    return responce_result; 
		}
	if(diag.sesion_mode==DIAG_PR_MODE)
	{
		can_send_NRC(DIAG_DTC_CONTTOL_SID,CONDITION_ERR);
		return (NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);	  
		}
	dbg_msg(dev, "Enter $85 service success\r\n");
	switch(buffer[0])
	{
		case 0x01:	//on 		
			diag.dtc_function_enable = DIAG_DTC_FUNCTION_ON;	
			send_data[0] = 0x02;
			send_data[1] = 0xC5;
			send_data[2] = 0x01;
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);		  			
			break;
		case 0x02://			
			diag.dtc_function_enable = DIAG_DTC_FUNCTIONOFF;//No DTC recording
			send_data[0] = 0x02;
			send_data[1] = 0xC5;
			send_data[2] = 0x02;
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);		  						
			break;
		case 0x81:	//on 		
			diag.dtc_function_enable = DIAG_DTC_FUNCTION_ON;	
			break;
		case 0x82://			
			diag.dtc_function_enable = DIAG_DTC_FUNCTIONOFF;		  						
			break;					
		default:
			responce_result=SUB_ERR;
			can_send_NRC(0x85,SUB_ERR); 
			break;			
	}
	return responce_result;
}


/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：void app_diagnose(void)
* 函数功能：初始化diag相关参数
  输入参数说明：  buffer:sid(含)开始的完整的一桢数据，len:sid(含)后数据总长度
* 返 回 值：无
  作者：    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
u32 app_diagnose(device_info_t *dev,uint8_t *buffer,uint16_t len)
{	
	uint8_t sid=buffer[0];              //sid  服务ID
	uint8_t response_result=NO_ERR;     //初始 正确结果
//	uint8_t send_data[7];	
	switch(sid)  //buffer[1]为sid后开始数据内容
    {
	    case DIAG_SESSION_SID:  //会话模式  10
	        response_result=SID_10_fun(dev,&buffer[1],len-1);                         
	        break;
	    case DIAG_ECU_RESET_SID: //复位  11
	    	response_result=SID_11_fun(dev,&buffer[1],len-1);                                          
	        break;
	    case DIAG_CLR_DTC_SID:   //清除故障  14
	        response_result=SID_14_fun(dev,&buffer[1],len-1);                                                           
	        break;
	    case DIAG_R_DTC_SID:  //读DTC
	        response_result=SID_19_fun(dev,&buffer[1],len-1);            	            	
	        break;
	    case DIAG_R_DID_SID:   //读标识符 
	        response_result=SID_22_fun(dev,&buffer[1],len-1);       
	        break;
	    case DIAG_SECURITY_ACCESS_SID:  //安全访问  
	        response_result=SID_27_fun(dev,&buffer[1],len-1);                                  	               	   
	        break;
	    case DIAG_COMM_CONTROL_SID: //通信控制
	        response_result=SID_28_fun(dev,&buffer[1],len-1);                                  	               	               	            	 
	        break;
		 case DIAG_ROUTE_CONTROL_SID: //通信控制
	        response_result=SID_31_fun(dev,&buffer[1],len-1);                                  	               	               	            	 
	        break;
	   case DIAG_REQUEST_DOWNLOAD_SID: //
	        response_result=SID_34_fun(dev,&buffer[1],len-1);                                  	               	               	            	 
	        break;
	   case DIAG_TRANSFER_DATA_SID: //通信控制
	           response_result = SID_36_fun(dev,&buffer[0], len);                                 	               	               	            	 
	        break;
	    case DIAG_REQUEST_TRANSFER_EXIT_SIG: //通信控制
	        response_result=SID_37_fun(dev,&buffer[1],len-1);                                  	               	               	            	 
	        break;
	    case  DIAG_W_DID_SID://写标识符:  
			response_result=SID_2E_fun(dev,&buffer[1],len-1);      
	        break;               
	    case DIAG_TESTER_PRESENT_SID:
	        response_result = SID_3E_fun(dev,&buffer[1],len-1);            	
	        break;
	    case DIAG_DTC_CONTTOL_SID:
	        response_result = SID_85_fun(dev,&buffer[1],len-1);            	            	            
	        break;			
	    default:
	   		dbg_msg(dev, "SID is not support···\r\n");
	        response_result = SID_ERR;//服务不支持
			can_send_NRC( sid, SID_ERR); 
	        break;
    }                	
	return response_result;
}

/*
 *@brief :   应用程序检测下载报文执行预编程环境检测
*autor:  xz  
*Time ： 20180524

u8 SID_31_fun_checkProgrammingPreconditions(device_info_t *dev)
{	
	if((dev->ipc->app_jump == 1)&&(dev->ci->state == HOST_UPGRADING))
	{
		return 1;
	}
	else return 0; 

}*/

void check_s3_client(device_info_t *dev)
{
	static u8 current_sts=0; 
	if((get_diag_sesion_mode()!=DIAG_DE_MODE)&&(current_sts==0))//不在默认会话，打开定时器5s
	{
		current_sts=1;
		dev->diag_t.diag_rec_sts=0;
		stimer_start(dev, FLAG_S3_CLIENT,TO_CNT_5S);
		}

	if((dev->diag_t.diag_rec_sts==1)&&(current_sts==1))
	{
		current_sts=0;
		dev->diag_t.diag_rec_sts=0;
	//	dbg_msg(dev,"reset S3_clien timer\r\n");
		stimer_stop(dev, FLAG_S3_CLIENT);
		stimer_start(dev, FLAG_S3_CLIENT,TO_CNT_5S);
		}

	if(get_flag(dev, FLAG_S3_CLIENT))
	{
		stimer_stop(dev, FLAG_S3_CLIENT);
		current_sts=0;
		dev->diag_t.diag_rec_sts=0;
		set_diag_session(DIAG_DE_MODE);
		dbg_msg(dev,"back to de_session\r\n");
		}
	}
