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
CAN_DIAG_TYPE diag; //diag �����ṹ��
SERVER_APPLICATION_LAYER_TIM_TYPE   server_application_layer_tim;//Ӧ�ò�ʱ�����
CLIENT_APPLICATION_LAYER_TIM_TYPE   client_application_layer_tim;//Ӧ�ò�Ự�����ʱ��

/*
*----------------------------------------------------------------------------------------------------------------------------------------
* ����ԭ�ͣ�void init_diag_protocol_parameter(void)
* �������ܣ���ʼ��diag��ز���
  ����˵����
* �����������
* �� �� ֵ����
  ���ߣ�    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
void init_diag_protocol_parameer(void)
{
	//device_info_t *dev=get_device_info();
 	diag.address_type=DIAG_P_ADDRESS;//����Ѱַģʽ
	diag.sesion_mode=DIAG_DE_MODE;//�ϵ缴������Ĭ�ϻỰģʽ
	diag.security_level1_state=DIAG_SECURITY_LEVEL_STATE_LOCK; //��ȫ״̬
	diag.security_level3_state=DIAG_SECURITY_LEVEL_STATE_LOCK;//��ȫ״̬
	diag.dtc_function_enable=DIAG_DTC_FUNCTION_ON;
	diag.communication_enable=DIAG_COMM_RX_AND_TX_ENABLE;
	diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;//ȡ1λ
	diag.SAFC_1=0x00;
	diag.SAFC_3=0x00;
	diag.time_SAFC_3=0x00;
	diag.time_SAFC_1=0x00;//һ�㰲ȫ����ʧ�ܼ���3��ʱ���
	diag.security_level1_access_step=DIAG_NOT_REQ_SEED;//δ����Seed
	diag.security_level3_access_step=DIAG_NOT_REQ_SEED;//δ����Seed
	//SAFC_1  SAFC_3  ��flash������
	//Ӧ�ò�ʱ�����
	server_application_layer_tim.P2can=50*OS_1MS;
	server_application_layer_tim.P2_asterisk_can=5000*OS_1MS;
	server_application_layer_tim.S3_sever=5000*OS_1MS;
	client_application_layer_tim.P2can=50*OS_1MS;//ECU�յ���Ϸ�����Ӧʱ��Ҫ��
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
* ����ԭ�ͣ�void init_diag_protocol_parameter(void)
* �������ܣ���ʼ��diag��ز���
  ����˵����
* �����������
* �� �� ֵ����
  ���ߣ�    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
void init_diag_protocol_parameer_boot(void)
{
 	diag.address_type=DIAG_P_ADDRESS;//����Ѱַģʽ
	diag.sesion_mode=DIAG_PR_MODE;//�ϵ缴������Ĭ�ϻỰģʽ
	diag.security_level1_state=DIAG_SECURITY_LEVEL_STATE_LOCK; //��ȫ״̬
	diag.security_level3_state=DIAG_SECURITY_LEVEL_STATE_LOCK;//��ȫ״̬
	diag.dtc_function_enable=DIAG_DTC_FUNCTION_ON;
	diag.communication_enable=DIAG_COMM_RX_AND_TX_DISABLE;
	diag.SAFC_reach_3num_mcu_resetbit = DIAG_ECU_HAD_NOT_RESET;//ȡ1λ
	diag.SAFC_1=0x00;
	diag.SAFC_3=0x00;
	diag.time_SAFC_3=0x00;
	diag.time_SAFC_1=0x00;//һ�㰲ȫ����ʧ�ܼ���3��ʱ���
	diag.security_level1_access_step=DIAG_NOT_REQ_SEED;//δ����Seed
	diag.security_level3_access_step=DIAG_NOT_REQ_SEED;//δ����Seed
	//SAFC_1  SAFC_3  ��flash������
	//Ӧ�ò�ʱ�����
	server_application_layer_tim.P2can=50*OS_1MS;
	server_application_layer_tim.P2_asterisk_can=5000*OS_1MS;
	server_application_layer_tim.S3_sever=5000*OS_1MS;
	client_application_layer_tim.P2can=50*OS_1MS;//ECU�յ���Ϸ�����Ӧʱ��Ҫ��
	client_application_layer_tim.P2_asterisk_can=5000*OS_1MS;
	client_application_layer_tim.P3_can_phy=100*OS_1MS;
	client_application_layer_tim.P3_can_func=100*OS_1MS;
	client_application_layer_tim.S3_client=2000*OS_1MS;
	client_application_layer_tim.S3_Tester=4000;
}


//******************************************************************************
// DESCRIPTION:
// ARUGMENT   :��ȡcommunication_enable��״̬
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
// ARUGMENT   :sesion_mode  ״̬
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8  get_diag_sesion_mode(void)
{
	return diag.sesion_mode;
}

//******************************************************************************
// DESCRIPTION:
// ARUGMENT   :address_type  ��ǰѰַģʽ
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8  get_diag_address_type(void)
{
	return diag.address_type;
}

//******************************************************************************
// DESCRIPTION:����Ѱַģʽ
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
 �� �� ��  : SecuriyAlgorithmFBL
 ��������  : boot��ȫ�㷨
 �������  : _int32 wSeed  
             _int32 MASK   
 �������  : ��
 �� �� ֵ  : u32
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��8��14��
    ��    ��   : xz
    �޸�����   : �����ɺ���

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
*@brief : ֪�����ӡ���Կ�㷨���ο�Z1��Ŀ�������淶9.1
*autor:  xz  
*Time �� 20180524
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
// DESCRIPTION:��Ϸ���$10 �Ựģʽ for app
// ARUGMENT   : //bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ��ȣ�֡�ṹ��
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8  SID_10_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	if(len!=0x01)//�жϳ���
	{
		responce_result=FOR_ERR;
        dbg_msg(dev, "$10 service ����ȷ����Ϣ���ȡ�����\r\n");
		can_send_NRC(DIAG_SESSION_SID,FOR_ERR);
		return  responce_result;
	}
	switch(buffer[0])//��������  ��һ���ֽ�buffer[0]�����ӷ���ѡ��
	{
		case 0x01:				  
		  	if(diag.sesion_mode == DIAG_PR_MODE)
		  	{
		  		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
				can_send_NRC(DIAG_SESSION_SID,CONDITION_ERR);
				return responce_result;	
			}
		    diag.sesion_mode = DIAG_DE_MODE;//���ý���Ĭ�ϻỰģʽ
			diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;//��ȫ״̬����
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;//��ȫ״̬����
		    diag.dtc_function_enable = DIAG_DTC_FUNCTION_ON;//ʹ��DTC  
		    if(diag.communication_enable == DIAG_COMM_RX_AND_TX_DISABLE)//�ж�ͨ�ſ���ʹ��
		    {
		      diag.communication_enable   = DIAG_COMM_RX_AND_TX_ENABLE;
			//	diag_comm_enable( );//init_can(0);  //test
		    }
			
			//dbg_msg(dev, "Enter $10 01 service success,Need positive response\r\n");
		    //responce contect   $10���������Ӧ��ʽ
			send_data[0] = 0x06;//����
			send_data[1] = 0x50;
			send_data[2] = 0x01;//���� ���� 01 �ӷ���diagnostic session type��	    	    
			send_data[3] = (client_application_layer_tim.P2can&0xFF00)>>8; //��Ÿ��ֽ�
			send_data[4] = client_application_layer_tim.P2can&0xFF;//���ֽ�
			send_data[5] =(client_application_layer_tim.P2_asterisk_can&0xFF00)>>8;// 
			send_data[6] =client_application_layer_tim.P2_asterisk_can&0xFF;//���ֽ� 
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
		break;
			
		case 0x02:
			if((diag.address_type == DIAG_F_ADDRESS)||(diag.sesion_mode == DIAG_DE_MODE))//��̻Ự����������Ѱַģʽ		  
			{
				responce_result=NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//DIAG_PR_MODE��֧�ֹ���Ѱַ
				can_send_NRC(DIAG_SESSION_SID,CONDITION_ERR);
				return responce_result;					
			}
			diag.sesion_mode =DIAG_PR_MODE;
			diag.address_type =DIAG_F_ADDRESS;
		  	diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;
			dbg_msg(dev,"Ԥ����������ת��BOOT\r\n");		
			send_data[0] = 0x02;
			send_data[1] = 0x50;
			send_data[2] = 0x02;
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//������ģʽ�Ŀ϶���Ӧ
			//comm_message_send(dev,CM_UPDATE_ARM_READY, 0,NULL , 0);//֪ͨARM����uboot
			dev->ipc->app_jump = 1;//������־λ ��1
			systick_stop();
			device_deinit(dev);
			process_switch(dev->ipc->boot_entry, dev->ipc->boot_stack);//��ת
			break;
			
		case 0x03:
			if(diag.sesion_mode == DIAG_PR_MODE)//	��Ĭ��ģʽת��	  
			{
				responce_result=NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//DIAG_PR_MODE��֧�ֹ���Ѱַ
				return responce_result; 				
			}
			diag.sesion_mode = DIAG_EX_MODE;
		  	diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;	
		   //responce contect
		 	dbg_msg(dev,"$10 03 for PR mode, checkPreconditon sucessed\r\n");
			send_data[0] = 0x06;//����
			send_data[1] = 0x50;
		    send_data[2] = 0x03;//���� ���� 01 �ӷ���diagnostic session type��	    	    
		    send_data[3] = (client_application_layer_tim.P2can&0xFF00)>>8; //��Ÿ��ֽ�
		    send_data[4] = client_application_layer_tim.P2can&0xFF;//���ֽ�
		    send_data[5] =(client_application_layer_tim.P2_asterisk_can&0xFF00)>>8;// 
		    send_data[6] =client_application_layer_tim.P2_asterisk_can&0xFF;//���ֽ� 
		   	can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
		break;

		case 0x81 :
			if(diag.sesion_mode == DIAG_PR_MODE)
		  	{
		  		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
				can_send_NRC(DIAG_SESSION_SID,CONDITION_ERR);
				return responce_result;	
			}
		    diag.sesion_mode = DIAG_DE_MODE;//���ý���Ĭ�ϻỰģʽ
			diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;//��ȫ״̬����
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;//��ȫ״̬����
		    diag.dtc_function_enable = DIAG_DTC_FUNCTION_ON;//ʹ��DTC  
		    if(diag.communication_enable == DIAG_COMM_RX_AND_TX_DISABLE)//�ж�ͨ�ſ���ʹ��
		    {
		      diag.communication_enable   = DIAG_COMM_RX_AND_TX_ENABLE;
			//	diag_comm_enable( );//init_can(0);  //test
		    }
			
			dbg_msg(dev, "Enter $10 81 service success,No Positive Response Required\r\n");
			
		break;

		case 0x82 :
			if(diag.address_type == DIAG_F_ADDRESS)//��̻Ự����������Ѱַģʽ		  
			{
				responce_result=NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//DIAG_PR_MODE��֧�ֹ���Ѱַ
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
//				can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//���ط���Ӧ
//				return NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;
//			}
			dbg_msg(dev,"$10 82 for PR mode, checkPreconditon sucessed\r\n");
			send_data[0] = 0x02;
			send_data[1] = 0x50;
			send_data[2] = 0x02;
	     //	can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//������ģʽ�Ŀ϶���Ӧ
			dev->ipc->app_jump = 1;//������־λ ��1
			process_switch(dev->ipc->boot_entry, dev->ipc->boot_stack);//��ת
		break;

		case 0x83 :
			if(diag.sesion_mode == DIAG_PR_MODE)//	��Ĭ��ģʽת��  ��ȫ״̬�л�������״̬����ͨ�ſ��Ʒ���Ϳ���DTC���÷����״̬����Ӱ��	  
			{
 				diag.security_level1_state=DIAG_SECURITY_LEVEL_STATE_LOCK; //��ȫ״̬����
				diag.security_level3_state=DIAG_SECURITY_LEVEL_STATE_LOCK;//��ȫ״̬����
			}
			diag.sesion_mode = DIAG_EX_MODE;
		  	diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_LOCK;
		    diag.security_level3_state = DIAG_SECURITY_LEVEL_STATE_LOCK;	
		   //responce contect
		  	dbg_msg(dev,"$10 83 for PR mode,No Positive Response Required\r\n");
		break;
			
		default:
				dbg_msg(dev,"��֧���ӷ���\r\n");
				responce_result=SUB_ERR;//buffer[0]ֵ����Ӧ01 02 03�ӷ���֧��
			 	can_send_NRC(DIAG_SESSION_SID,SUB_ERR);
			break;			
	}	
	return responce_result;		
}


//******************************************************************************
// DESCRIPTION: $11  ECUReset  ֧��01 hardReset,03 softReset����Ҫ����Ӧ�ظ�
// ARUGMENT   : bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ���
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_11_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	
	if(len!=0x01)//�жϳ���
	{
		responce_result=FOR_ERR;
		dbg_msg(dev, "$11 service ����ȷ����Ϣ���ȡ�����\r\n");
		can_send_NRC(DIAG_ECU_RESET_SID,FOR_ERR);
		return  responce_result;
	}

	switch(buffer[0])
	{
		case 0x01:  //hardReset
			if((diag.sesion_mode == DIAG_PR_MODE)&&(diag.address_type==DIAG_F_ADDRESS))
		  	{
		  		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
				can_send_NRC(DIAG_ECU_RESET_SID,CONDITION_ERR);
				return responce_result;	
			}
			
			diag.sesion_mode = DIAG_EX_MODE;
			dbg_msg(dev, "HardReset������\r\n");
			send_data[0] = 2;
			send_data[1] = 0x51;//$11���������Ӧ��ʽ
			send_data[2] = 0x01;	
		 	can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
			/*PMIC��*/
			IO_SET(GPIOA, 3, LOW);
			mdelay(10);
			software_reset(); 
			break;
	
		case 0x03: //softReset	   
			if((diag.sesion_mode == DIAG_PR_MODE)&&(diag.address_type==DIAG_F_ADDRESS))
		  	{
		  		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
				can_send_NRC(DIAG_ECU_RESET_SID,CONDITION_ERR);
				return responce_result;	
			}	
			dbg_msg(dev, "SoftReset������\r\n");
			send_data[0] = 2;
			send_data[1] = 0x51;//$11���������Ӧ��ʽ
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
				responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
				can_send_NRC(DIAG_SESSION_SID,CONDITION_ERR);
				return responce_result; 
			}
			dbg_msg(dev, "Enter $11 81 service success,N0 positive response\r\n");
			mdelay(10);
			software_reset(); 
		break;
		
		default:
			dbg_msg(dev,"��֧���ӷ���\r\n");
			responce_result=SUB_ERR;//buffer[0]ֵ����Ӧ01 02 03�ӷ���֧��
			can_send_NRC(DIAG_ECU_RESET_SID,SUB_ERR);
		break;			
	}
  return responce_result;
	
}

//******************************************************************************
// DESCRIPTION:$14���� ClearDiagnosticInformation �ӷ���$FFFFFF(All groups)  ���е���λ��� 
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
		dbg_msg(dev, "$14 service ����ȷ����Ϣ���ȡ�����\r\n");
		can_send_NRC(DIAG_CLR_DTC_SID,FOR_ERR);
		return  responce_result;
    }
		
    if((buffer[0]==0xff)&&(buffer[1]==0xff)&&(buffer[2]==0xff))//�ߡ��С���λ��ΪFF ����  
    {
		clear_dtc() ;   // ��� ������ ,��������Ϊ0x00����������
    }
	else
	{
		dbg_msg(dev,"��֧���ӷ���\r\n");
		responce_result=OUTRANGE_ERR;//buffer[0]ֵ����Ӧ01 02 03�ӷ���֧��
		can_send_NRC(DIAG_CLR_DTC_SID,OUTRANGE_ERR);
		return responce_result;
	}
	dbg_msg(dev, "Enter $14 service success,Need positive response\r\n");
	send_data[0] = 0x01;
    send_data[1] = 0x54;  //$14 ����Ӧ�ظ�Value 54 
    can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);	 

//	save_dtc( );
	
    return responce_result;    
}

//******************************************************************************
// DESCRIPTION:$19 read DTC information
// ARUGMENT   : //bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ���  
// RETURN     :֧��01 02 03 04 0A
// AUTHOR     :
//******************************************************************************
u8 SID_19_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 send_data[AVM_DTC_MAX*4+3]={0};  //DTC ��
	u8 responce_result=NO_ERR;
	u8 tester_mask;
	u16 dtc_count=0;
 	if(len != 0x02)             
  	{
		responce_result=FOR_ERR;
		dbg_msg(dev, "$19 ����ȷ����Ϣ���ȡ�����\r\n");
		can_send_NRC(DIAG_R_DTC_SID,FOR_ERR);
		return  responce_result;
    }	
	switch(buffer[0])
	{
		case 0x01:		//ͨ��״̬���뱨����������,tester_mask=1ʱ����ǰ���ϣ�0x08ʱ����ʷ����
			tester_mask = buffer[1];
			if(diag.sesion_mode == DIAG_PR_MODE) //��̻Ựʱ  
			{
   			responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
				can_send_NRC(DIAG_R_DTC_SID,NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);
				return responce_result;	
			}
		//	dbg_msg(dev, "Enter $19 01 service success,Need positive response\r\n");
			dtc_count = read_dtc_01_02_sub_id(send_data,0x01,tester_mask);
			send_data[0] = 0x06;
			send_data[1] = 0x59;//����Ӧ��ʽ
			send_data[2] = 0x01;//request type
			send_data[3] = ECU_SUPPORT_MSAK;//
			send_data[4] = 0x00;//ISO15031-6DTCformat Ϊ0 DTC_Format_Identifier
			send_data[5] = dtc_count>>8u;//DTC_Count ���ֽ�
			send_data[6] = dtc_count&0x0F;//���ֽ�
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);	
			break;
			
		case 0x02://ͨ��״̬���뱨�������  ��֡����
			tester_mask = buffer[1];
			if(diag.sesion_mode == DIAG_PR_MODE)  //��֧�ֱ��ģʽ
			{
   			responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
				can_send_NRC(DIAG_R_DTC_SID,CONDITION_ERR);
				return responce_result;	
			}
//			dbg_msg(dev, "Enter $19 02 service success,Need positive response\r\n");
			send_data[1] = 0x059;//request type
			send_data[2] = 0x02;//�ӷ���
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
			dbg_msg(dev,"��֧���ӷ���\r\n");
			responce_result=SUB_ERR;//buffer[0]ֵ����Ӧ01 02 03�ӷ���֧��
			can_send_NRC(DIAG_R_DTC_SID,SUB_ERR);
		break;				
	}

	//��ȡ$19  DTC ��������
  return responce_result;
}


//******************************************************************************
// DESCRIPTION:$22  ͨ����ʶ����ĳ��DID���ݷ��� for app  
// ARUGMENT   : bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ��� 
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_22_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 responce_result=NO_ERR;
	u8 send_did_data[60]={0};//������󳤶� ����Ϊ60  
	u16 DID = 0;//DID value  2���ֽ�   
	if(len!=0x02)//�жϳ���
	{
		responce_result=FOR_ERR;
		dbg_msg(dev, "$22 service ����ȷ����Ϣ���ȡ�����\r\n");
		can_send_NRC(DIAG_R_DID_SID,FOR_ERR);
		return  responce_result;
	}
 	if(diag.sesion_mode==DIAG_PR_MODE)//��̻Ự��֧��
	{
		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
		can_send_NRC(DIAG_R_DID_SID,CONDITION_ERR);
		return responce_result;
	}
	DID = buffer[0]<<8;//DID MSB
	DID += buffer[1];//DID LSB
	//dbg_msg(dev, "Enter $22 service success,Need positive response\r\n");
	switch( DID)
	{	//�ظ���Ϊ>8�ֽ�
		case DID_ECUConfiguration : 
			para_read_Nbyte( ADDR_ECUConfiguration,send_did_data,LEN_ECUConfiguration);
			can_net_layer_send.len = LEN_ECUConfiguration;//+3 ����22�����DID�����ֽ�
			responce_result=NO_ERR;
		break;
		
		case DID_ECUSoftwareVersion : 
			para_read_Nbyte( ADDR_SoftwareVersion,send_did_data,LEN_ECUSoftwareVersion);
			can_net_layer_send.len = LEN_ECUSoftwareVersion;//+3 ����22�����DID�����ֽ�
			responce_result=NO_ERR;
		break;

		case DID_ECUHardwareVersion: //  �㲿���� 15�ֽ�
			para_read_Nbyte( ADDR_HardwareVersion,send_did_data,LEN_ECUHardwareVersion);
			can_net_layer_send.len = LEN_ECUHardwareVersion;
			responce_result=NO_ERR;
		break;

		case DID_Part_Number: // ��Ӧ�̴��� 9���ֽ�
			para_read_Nbyte( ADDR_Part_Number,send_did_data,LEN_Part_Number);
			can_net_layer_send.len = LEN_Part_Number;
			responce_result=NO_ERR;
		break;

		case DID_System_Supplier:	//	��ص�Ԫ����		
			para_read_Nbyte( ADDR_System_Supplier,send_did_data,LEN_System_Supplier);
			can_net_layer_send.len = LEN_System_Supplier;
			responce_result=NO_ERR;
		break;
		
		case DID_ECUSerialNumber:  // ��Ӧ�̵��ӿ��Ƶ�ԪӲ���汾��  3���ֽ�
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
			dbg_msg(dev,"��֧���ӷ���\r\n");
			responce_result=OUTRANGE_ERR;//buffer[0]ֵ����Ӧ01 02 03�ӷ���֧��
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
//			dbg_msgfv(dev, "����DID��֡����Ϊ��%d\r\n", can_net_layer_send.len);
			diag_send_data(dev,can_net_layer_send.buff, can_net_layer_send.len);//��֡����
		}
	return  responce_result;
}

//******************************************************************************
// DESCRIPTION:$27 SecurityAccess 01RequestSeed 02Sendkey ��07 08��Ӧ
// ARUGMENT   : //bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ���  
// RETURN     :
// AUTHOR     :
//******************************************************************************

u8 SID_27_fun(device_info_t *dev,u8 *buffer,u16 len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	int Security_key[4],app_password[4];  //ECU-self calculated key �Լ������Key  4���ֽ�
//Ӧ�ÿ��ǵ�����Ӧ�����ȼ�
	if((len!=0x01)&&(len!=0x05))//�жϳ���
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
		dbg_msg(dev, "$27 service ����ȷ����Ϣ���ȡ�����\r\n");
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
		responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=7f 	
		can_send_NRC(DIAG_SECURITY_ACCESS_SID,NOT_SUPPERT_SUB_IN_ACTIVE_SESSION);
		return responce_result; 
		}

	switch(buffer[0])
	{
	//for App Seed and key
	case 0x01:	//app���� ��01 02 ����չ�Ự ����Ѱַ�� ����			  	   							   						
			
		if(diag.open_securitu_timer==0x01)//������ʱ����־λ
		{
			if(get_flag(dev,FLAG_SECURITY_DELAY))
			{
			dbg_msg(dev,"delay 60s arrived��retry\r\n");
			diag.SAFC_1=0;
			diag.open_securitu_timer=0x00;//�رն�ʱ��
			stop_timer(FLAG_SECURITY_DELAY);
			}
		else//��ʱδ��
		{
			dbg_msg(dev,"delay 60s is't arrived��please wait\r\n");
			responce_result =DELAY_NOT_ARRIVAL_ERR; 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,DELAY_NOT_ARRIVAL_ERR);
			return  responce_result;
			}	
		}

			
		if(diag.security_level1_state == DIAG_SECURITY_LEVEL_STATE_UNLOCK)  //if having been unlock,send datas 00 00 00 00���Ѿ�������ȫ����
		{	
			 send_data[0] = 0x06;
     		 send_data[1] = 0x67;//����Ӧ��ʽ
     		 can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);	   	   
		}
		else
		{
            diag.security_ECU_Mask = ECU_Mask_APP;
			dbg_msg(dev, "Enter $27 01 service success,Back seed\r\n");
			send_data[0] = 0x06;//����
			send_data[1] = 0x67;
            send_data[2] = 0x01;
			//4���ֽڵ����ӷ���send_data
			srand(get_systick_value());
			diag.security_seed[0]=rand()%0xff;
			diag.security_seed[1]=rand()%0xff;
			diag.security_seed[2]=rand()%0xff;
			diag.security_seed[3]=rand()%0xff;
			diag.security_seed[3]=0x7a;
			diag.security_seed[2]=0xd6;
			diag.security_seed[1]=0xde;
			diag.security_seed[0]=0x09;
			send_data[3]=diag.security_seed[0];// seed���ֽڷ�ǰ��
			send_data[4]=diag.security_seed[1];
			send_data[5]=diag.security_seed[2];
			send_data[6]=diag.security_seed[3];
			//	dbg_msgv(dev,"seed = 0x%02x-0x%02x-0x%02x-0x%02x\r\n",send_data[3],send_data[2],send_data[1],send_data[0]);
            can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);//����seed
            diag.security_level1_access_step = DIAG_HAD_REQ_SEED;//ȷ�ϰ�ȫ���ʲ���
		}			
		break;
		
	case 0x02://���� key

		if(diag.security_level1_state == DIAG_SECURITY_LEVEL_STATE_UNLOCK)  //if having been unlock,send datas 00 00 00 00���Ѿ�������ȫ����
		{	
			 send_data[0] = 0x06;
     		 send_data[1] = 0x67;//����Ӧ��ʽ
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
			responce_result = SEQUENCE_ERROR;//����������NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,SEQUENCE_ERROR);
			return responce_result; 
			}
		else
		{
			diag.security_level1_access_step = DIAG_NOT_REQ_SEED; 
			}

		
		//obtain the key from tester��ȡKey from �����
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
				
			if(diag.open_securitu_timer==0x01)//������ʱ����־λ
			{
				if(get_flag(dev,FLAG_SECURITY_DELAY))
				{
					dbg_msg(dev,"delay 60s arrived��retry\r\n");
					diag.SAFC_1=0;
					diag.open_securitu_timer=0x00;//�رն�ʱ��
					stop_timer(FLAG_SECURITY_DELAY);
					}
				else//��ʱδ��
				{
					dbg_msg(dev,"delay 60s is't arrived��please wait\r\n");
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
			diag.open_securitu_timer=0x00;//�رն�ʱ��
			stop_timer(FLAG_SECURITY_DELAY);//60s��ʱ��ʱ��
			diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_UNLOCK; //ƥ��������־λ�����ͽ����־λ
		}
		send_data[0] = 0x02;
		send_data[1] = 0x67;
		send_data[2] = 0x02;
	//	dbg_msg(dev,"��Կ��ȷ��������Ӧ\r\n");
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);	
		diag.security_level1_access_step = DIAG_NOT_REQ_SEED;//�ڶ�����������־λ����
		}
		break;
	#if 0	
	case 0x21:	//app���� ��01 02 ����չ�Ự ����Ѱַ�� ����	
		//���ȺϷ����ж�
		if(len!=0x01)//�жϳ���
			{
			responce_result=FOR_ERR;
			//dbg_msg(dev, "$27 service ����ȷ����Ϣ���ȡ�����\r\n");
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,FOR_ERR);
			return  responce_result;
			}  
		if((diag.sesion_mode !=DIAG_PR_MODE)||(diag.address_type !=DIAG_P_ADDRESS )  )    
			{
			responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,CONDITION_ERR);
			return responce_result; 
			}
		if((diag.SAFC_1>=3)&&(diag.SAFC_reach_3num_mcu_resetbit == DIAG_ECU_HAD_NOT_RESET))                     
			{
			responce_result = DELAY_NOT_ARRIVAL_ERR;//
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,DELAY_NOT_ARRIVAL_ERR);
			return responce_result; 
			}
		if(diag.security_level1_state == DIAG_SECURITY_LEVEL_STATE_UNLOCK)  //if having been unlock,send datas 00 00 00 00���Ѿ�������ȫ����
		{	
			send_data[0] = 0x06;
            send_data[1] = 0x67;//����Ӧ��ʽ
            can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);	   	   
		}
		else
		{
            diag.security_seed = Seed_BOOT;//(uint32_t)(rand());	
            diag.security_ECU_Mask = ECU_Mask_Boot;
			//dbg_msg(dev, "Enter $27 01 service success,Back seed\r\n");
			send_data[0] = 0x06;//����
			send_data[1] = 0x67;
            send_data[2] = 0x21;
			//4���ֽڵ����ӷ���send_data
            send_data[3] = diag.security_seed>>24;
            send_data[4] = (diag.security_seed >>16)&0xFF;
            send_data[5] = (diag.security_seed>>8)&0xFF;
            send_data[6] = diag.security_seed &0xFF;
            can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);//����seed
            diag.security_level1_access_step = DIAG_HAD_REQ_SEED;//ȷ�ϰ�ȫ���ʲ���
		}			
		break;
		
	case 0x22://���� key  
		if((diag.sesion_mode !=DIAG_PR_MODE)&&(diag.address_type !=DIAG_F_ADDRESS )  )    	 
			{
			responce_result = NOT_SUPPERT_SUB_IN_ACTIVE_SESSION;//����������NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,CONDITION_ERR);
			return responce_result; 
			}
		if((diag.security_level1_state == DIAG_SECURITY_LEVEL_STATE_UNLOCK)||(diag.security_level1_access_step != DIAG_HAD_REQ_SEED))    
			{
			responce_result = SEQUENCE_ERROR;//����������NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,SEQUENCE_ERROR);
			return responce_result; 
			}
		if(diag.security_level1_access_step != DIAG_HAD_REQ_SEED) 
			{
			responce_result = SEQUENCE_ERROR;//����������NRC=22 
			can_send_NRC(DIAG_SECURITY_ACCESS_SID,SEQUENCE_ERROR);
			return responce_result; 
			}
		else
			diag.security_level1_access_step = DIAG_NOT_REQ_SEED;

		//obtain the key from tester��ȡKey from �����
		PassWord = ((u32)buffer[4])<<24;
		PassWord |= ((u32)buffer[3])<<16;
		PassWord |= ((u32)buffer[2])<<8;
		PassWord |=(u32) buffer[1];
		//dbg_msgv(dev,"PassWord  = %08x\r\n",PassWord);
		//ECU-self calculate the Security_key 
		Security_key_boot = SecuriyAlgorithmFBL(Seed_BOOT,ECU_Mask_Boot);
	//	dbg_msgv(dev,"Security_key_boot  = %08x,PassWord=%08x\r\n",Security_key_boot,PassWord);
		//Security_key_boot =PassWord;//for  test����λ����Կ������
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
			diag.security_level1_state = DIAG_SECURITY_LEVEL_STATE_UNLOCK; //ƥ��������־λ�����ͽ����־λ
			send_data[0] = 0x02;
			send_data[1] = 0x67;
			send_data[2] = 0x22;
		//dbg_msg(dev,"��Կ��ȷ\r\n");
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32 *)send_data);	
			diag.security_level1_access_step = DIAG_NOT_REQ_SEED;//�ڶ�����������־λ����
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
// ARUGMENT   : bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ���  
// RETURN     :
// AUTHOR     :
//******************************************************************************
uint8_t SID_28_fun(device_info_t *dev,uint8_t *buffer,uint16_t len)
{
	uint8_t send_data[8]={0};
	uint8_t responce_result=NO_ERR;
	//���ȺϷ����ж�
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
			//diag_comm_disable( );	test  ��ֹ�޹�ͨ��	������ ���籨�Ķ���ֹ
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
// ARUGMENT   : bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ���  
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
	if(diag.security_level1_state != DIAG_SECURITY_LEVEL_STATE_UNLOCK)//�谲ȫ����
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
	/*дָ��0xF184-������*/
	else if(DID==DID_Fingerprint)
	{	
		memcpy(dev->did_t.buf,&buffer[2] ,LEN_Fingerprint);
		dev->did_t.LEN =LEN_Fingerprint/2+1;
		dev->did_t.ADDR = ADDR_Fingerprint;
		//STMFLASH_Write(ADDR_Fingerprint,(u16*)&buffer[2],LEN_CURRENT_DID);	
		}
	else
	{
		responce_result=OUTRANGE_ERR;//buffer[0]ֵ����Ӧ01 02 03�ӷ���֧��
		can_send_NRC(DIAG_W_DID_SID,OUTRANGE_ERR);
	}

	if(dev->ci->write_did_flag)//����дdid
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
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//�ӳٵ���־λΪ1��д���ȱ���DID�ͳ���
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
	u16 routine_id;//�������� ID
	u8 Erase_result,routine_control_type;//routine_control_type=start routine(01�ڶ����ֽ�)
   //���ݳ����ж�  ���ȱ������3����ͬ�ӷ������ݳ��Ȳ�һ��
	if(len<3)
	{
		responce_result=FOR_ERR; 
		can_send_NRC(DIAG_ROUTE_CONTROL_SID,FOR_ERR);
		return responce_result; 
	}
	routine_control_type=buffer[0];	                	
	routine_id=buffer[1];
	routine_id=(routine_id<<8)+buffer[2];
	if((routine_control_type== 0x01)&&(routine_id==0xFF00)&&(buffer[3]==0xff))//31  01�ӷ���
	{
		Erase_result = upgrade_earseMemory(dev);
		//Erase_result = 0x01;//�ظ��ٵĳɹ�for test
		//dbg_msg(dev,"ִ���ڴ����\r\n");
		send_data[0]=0x05;
		send_data[1]=0x71;
		send_data[2]=0x01;//Routine Control Type
		send_data[3]=buffer[2];
		send_data[4]=buffer[3];
		send_data[5]=Erase_result;//�ɹ�01 ʧ��02
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
		}
	else if((routine_control_type== 0x01)&&(routine_id==0xff00)&&(buffer[3]==0x01))//����boot
	{
	/*	dbg_msg(dev,"uds upgrade mcu_boot,jump to app\r\n");
		send_data[0]=0x03;
		send_data[1]=0x7F;
		send_data[2]=0x31;//Routine Control Type
		send_data[3]=0x78;
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);//����78��ʾ����ǵȴ�����Ӧ
	*/	
		dev->ipc->upgrade_mcu_boot = 1;//����mcu_boot 	
		systick_stop();
		device_deinit(dev);
		__disable_irq();
		process_switch(dev->ipc->app_entry, dev->ipc->app_stack);//��ת
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
	diag.file_trans_mode = 0x02; //����ģʽ
	update_type = buffer[10];
	if((update_type==0x10u)||(update_type==0x20u))
	{
		dbg_msg(dev,"\r\n��������MCU��������\r\n");	
		upgrade_requestFileDownload(dev,buffer, len);//����������������������ݳ���
		return NO_ERR;
		}
	else
	{	
		dbg_msg(dev,"��������ARM��������\r\n");
		upgrade_requestFileDownload_ARM(dev,buffer, len);//����������������������ݳ���
		comm_message_send(dev,CM_UPDATE_ARM_READY, 0, NULL , 0);
		/*��ʼ��ʱ���೤ʱ��δ�յ�ARM��Ӧ����ʾ����ʧ��*/
		dev->ipc->usart_normal = 0;
		start_timer(TIMER_DEV1,5000);
		return NO_ERR;
		}
}


//******************************************************************************
// DESCRIPTION:
// ARUGMENT   : //bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ���  
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
		//�Ӳ���02Ϊ����ָ��
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
		dbg_msg(dev,"���ݴ�����ɣ��˳��������\r\n");
		}
	else 
		can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
	return NO_ERR;
}
#endif

//******************************************************************************
// DESCRIPTION:
// ARUGMENT   : //bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ���  
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_3E_fun(device_info_t *dev,uint8_t *buffer,uint16_t len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	//���ȺϷ����ж�
	if(len!=0x01)//����Ϊ1
	{
		responce_result=FOR_ERR; 
		can_send_NRC(DIAG_TESTER_PRESENT_SID,FOR_ERR);
		return responce_result; 
	}
	dbg_msg(dev, "Enter $3E service success\r\n");

	switch(buffer[0])
	{
		case 0x00:	//���������
			send_data[0] = 0x02;
			send_data[1] = 0x7E;
			send_data[2] = 0x00;
			can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);	  				
			break;
		case 0x80://ʲô�����÷���
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
// DESCRIPTION: bufferΪsid��ʼ�������� lenΪsid��ʼ�����ݳ���  
// ARUGMENT   :DTC���� EX�Ựģʽ
// RETURN     :
// AUTHOR     :
//******************************************************************************
u8 SID_85_fun(device_info_t *dev,u8 *buffer,u8 len)
{
	u8 send_data[8]={0};
	u8 responce_result=NO_ERR;
	//���ȺϷ����ж�
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
* ����ԭ�ͣ�void app_diagnose(void)
* �������ܣ���ʼ��diag��ز���
  �������˵����  buffer:sid(��)��ʼ��������һ�����ݣ�len:sid(��)�������ܳ���
* �� �� ֵ����
  ���ߣ�    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
u32 app_diagnose(device_info_t *dev,uint8_t *buffer,uint16_t len)
{	
	uint8_t sid=buffer[0];              //sid  ����ID
	uint8_t response_result=NO_ERR;     //��ʼ ��ȷ���
//	uint8_t send_data[7];	
	switch(sid)  //buffer[1]Ϊsid��ʼ��������
    {
	    case DIAG_SESSION_SID:  //�Ựģʽ  10
	        response_result=SID_10_fun(dev,&buffer[1],len-1);                         
	        break;
	    case DIAG_ECU_RESET_SID: //��λ  11
	    	response_result=SID_11_fun(dev,&buffer[1],len-1);                                          
	        break;
	    case DIAG_CLR_DTC_SID:   //�������  14
	        response_result=SID_14_fun(dev,&buffer[1],len-1);                                                           
	        break;
	    case DIAG_R_DTC_SID:  //��DTC
	        response_result=SID_19_fun(dev,&buffer[1],len-1);            	            	
	        break;
	    case DIAG_R_DID_SID:   //����ʶ�� 
	        response_result=SID_22_fun(dev,&buffer[1],len-1);       
	        break;
	    case DIAG_SECURITY_ACCESS_SID:  //��ȫ����  
	        response_result=SID_27_fun(dev,&buffer[1],len-1);                                  	               	   
	        break;
	    case DIAG_COMM_CONTROL_SID: //ͨ�ſ���
	        response_result=SID_28_fun(dev,&buffer[1],len-1);                                  	               	               	            	 
	        break;
		 case DIAG_ROUTE_CONTROL_SID: //ͨ�ſ���
	        response_result=SID_31_fun(dev,&buffer[1],len-1);                                  	               	               	            	 
	        break;
	   case DIAG_REQUEST_DOWNLOAD_SID: //
	        response_result=SID_34_fun(dev,&buffer[1],len-1);                                  	               	               	            	 
	        break;
	   case DIAG_TRANSFER_DATA_SID: //ͨ�ſ���
	           response_result = SID_36_fun(dev,&buffer[0], len);                                 	               	               	            	 
	        break;
	    case DIAG_REQUEST_TRANSFER_EXIT_SIG: //ͨ�ſ���
	        response_result=SID_37_fun(dev,&buffer[1],len-1);                                  	               	               	            	 
	        break;
	    case  DIAG_W_DID_SID://д��ʶ��:  
			response_result=SID_2E_fun(dev,&buffer[1],len-1);      
	        break;               
	    case DIAG_TESTER_PRESENT_SID:
	        response_result = SID_3E_fun(dev,&buffer[1],len-1);            	
	        break;
	    case DIAG_DTC_CONTTOL_SID:
	        response_result = SID_85_fun(dev,&buffer[1],len-1);            	            	            
	        break;			
	    default:
	   		dbg_msg(dev, "SID is not support������\r\n");
	        response_result = SID_ERR;//����֧��
			can_send_NRC( sid, SID_ERR); 
	        break;
    }                	
	return response_result;
}

/*
 *@brief :   Ӧ�ó��������ر���ִ��Ԥ��̻������
*autor:  xz  
*Time �� 20180524

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
	if((get_diag_sesion_mode()!=DIAG_DE_MODE)&&(current_sts==0))//����Ĭ�ϻỰ���򿪶�ʱ��5s
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
