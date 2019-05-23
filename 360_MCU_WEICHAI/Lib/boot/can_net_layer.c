/******************************************************************************

                  版权所有 (C), 2017-2018, 赛格导航科技股份有限公司

 ******************************************************************************
  文 件 名   : can_net_layer.c
  版 本 号   : 初稿
  作    者   : xz
  生成日期   : 2018年7月17日
  最近修改   :
  功能描述   : 
  函数列表   :
*
*       1.                can_link_receive_data
*       2.                can_net_layer_send_flow_control
*       3.                can_net_layer_send_send
*       4.                can_net_send
*       5.                check_frame_sequence_number
*       6.                diag_receive_data
*       7.                flow_control_frame_dealwith
*       8.                init_net_layer_parameer
*       9.                join_can_send_queue
*       10.                judge_rev_less_mintime
*       11.                judge_rev_over_maxtime
*       12.                send_consecutive_frame_first_frame
*       13.                send_consecutive_frame_second_frame
*
#define CAN_NEY_LAYER_GLOBALS
  修改历史   :
  1.日    期   : 2018年7月17日
    作    者   : xz
    修改内容   : 创建文件

******************************************************************************/
#include  "can_net_layer.h"
#include  "can_diag_protocol.h" 
#include  "diag_upgrade.h"
#include  "iflash.h"
#include  "can.h"
#include "config.h"
#include  "api.h"
#include  "usart.h"
#include  "protocolapi.h"
#include "timer.h"

/*can报文分类：diag 和 其它*/
static DIAG_REC_TYPE         diag_rev;
static FLOW_FRAME_PARA_TYPE  rev_flow_frame_para;//接收到流控制桢    服务端用
static FLOW_FRAME_PARA_TYPE  send_flow_frame_para;//发送流控制桢     客户端 


//定义网络层发送 结构体
CAN_NET_LAYER_SEND_TYPE    can_net_layer_send;


#define DIAG_MODE 1


/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：void init_diag_parameter(void)
* 函数功能：初始化diag相关参数
  参数说明：
  
* 输入参数：无
* 返 回 值：无
  作者：    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/

void init_net_layer_parameer(void)
{
	can_net_layer_send.valid   = 0;
	can_net_layer_send.step    = STOP_STEP;
	send_flow_frame_para.FS    = 0;
	send_flow_frame_para.BS    = 0;
	send_flow_frame_para.STmin = 100;//100ms
	//can_send_queue.num=0;//shield by lsr 20170912
	//rev_flow_frame_para.FS=
	//rev_flow_frame_para.BS=
	//rev_flow_frame_para.STmin=
}



//******************************************************************************
// DESCRIPTION:网络层发给应用层的错误源码指示
// ARUGMENT   :
// RETURN     :
// AUTHOR     :
//******************************************************************************
void dealwith_linklayer_error(device_info_t *dev,u8  N_Result)
{
  /*here,pad the function of dealwith any error of the link layer*/
    switch(N_Result)
    {
	 	case N_WAIT_NEXT_FC:     //针对发送器对上层的指示
	      dbg_msg(dev,"reset timer,wait for next FC\r\n");
	      break;
	    case N_BUFFER_OVFLW:     //针对发送器对上层的指示
	      dbg_msg(dev,"ERR: buff of receiver over flow\r\n");
	      break;
	    case N_INVALID_FS:
	      dbg_msg(dev,"ERR: FS is invalid\r\n");
	      break;
	    case N_CF_TIMEOUT:
	      dbg_msg(dev,"ERR: CF is timeout\r\n");
	      break;
	    case N_FLOW_OVER:
	      dbg_msg(dev,"ERR: the data is too long\r\n");    //针对接收器对上层的指示
	      break;
	    case N_WRONG_SN:
			{
				dbg_msg(dev, "ERR: SN is wrong\r\n");  
			}
	      break;  
			default:
			   break;
    }
}



/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：void send_consecutive_frame_first_frame(void)
* 函数功能：发送多帧(只发多帧)
  参数说明：Data 为首帧出去PCI 部分
  
* 输入参数：无  
* 返 回 值：无
  作者：    
*----------------------------------------------------------------------------------------------------------------------------------------
 diag发送应考虑 多级缓存 发送，用队列  避免多个任务需同时发送*/
void diag_send_data(device_info_t *dev, u8 *Data, u16 len )
{
	u8 buff[8]={0};
    //多桢发送    
 	can_net_layer_send.had_send_len=0x00;
	can_net_layer_send.consecutive_frame_serial=0x00; //SN  最后
	//can_net_layer_send.len = len ;
	buff[0] = CONSECUTIVE_FRAME_FIRST_HEAD_0X10|((u8)(can_net_layer_send.len&0xff00)>>8);
 	buff[1] = can_net_layer_send.len&0xFF;
	memcpy(&buff[2],Data,6);//对应诊断服务的SID、响应回复
	//dbg_msgv(dev, "发送首帧数据DAT: %x %x %x %x %x %x %x %x···\r\n",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],buff[7]);
	can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)buff);//发送首帧  等待流控帧，发送第二帧在中断内进行
	can_net_layer_send.had_send_len = 6;
	//启动计时
	start_timer(TIMER_DEV3,WAINT_FLOW_CONTROL_FRAME_TIM);//等待诊断仪流控制返回   
    can_net_layer_send.step = CONSECUTIVE_FRAME_SECOND_FRAME_STEP;        //发第二桢状态，连续帧第一帧
}

/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：void send_consecutive_frame_second_frame(void)
* 函数功能：发大于含8字数据数据，第二桢
  参数说明：  
* 输入参数：无
* 返 回 值：无
  作者：    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
//	
void send_consecutive_frame_second_frame(device_info_t *dev)    
{
    u8   send_data[8]={0};
    u16    len = 0; 
    len = can_net_layer_send.len - can_net_layer_send.had_send_len;
    if(len>7)
    {
		len = 7;
    }
    can_net_layer_send.consecutive_frame_serial = (can_net_layer_send.consecutive_frame_serial+1)%0x10;
    send_data[0] = CONSECUTIVE_FRAME_SECOND_HEAD_0X20 | can_net_layer_send.consecutive_frame_serial;
    memcpy(&send_data[1],&can_net_layer_send.buff[can_net_layer_send.had_send_len],len);       
	//启动发送
	can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,(u32*)send_data);
	//dbg_msg(dev, "发送连续帧第一帧\r\n");
	//更新已发的参数   
    can_net_layer_send.had_send_len = can_net_layer_send.had_send_len + len;
}


/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：void flow_control_frame_dealwith(void)
* 函数功能：初始化diag相关参数, 根据流控制回的参数，来配置等到参数 flow_dealwith
  参数说明：处理诊断仪回复的 流控制帧
  
* 输入参数：无
* 返 回 值：无
  作者：    
*----------------------------------------------------------------------------------------------------------------------------------------
*/
void flow_control_frame_dealwith(device_info_t *dev,can_msg_t  *can_msg_temp,u8 mode)
{
	 uint8_t g_N_Result=0; 
	// dbg_msg(dev, "AVM接受流控帧进行处理\r\n");
	  //流控帧  长度不对
	 if( mode == DIAG_MODE )
	 {
		 if(get_diag_address_type() == DIAG_F_ADDRESS) //功能寻址不支持	
			 return;
	 }
	 if(can_msg_temp->dlc!=0x08)
	 {
	 	dbg_msg(dev,"FC CAN.DLC is wrong,return\r\n");
		return ;
	 	}
	if(((can_msg_temp->dat.b[0]>>4) == 0x03)&&(can_net_layer_send.step==CONSECUTIVE_FRAME_SECOND_FRAME_STEP)) //CONSECUTIVE_FRAME_SECOND_FRAME_STEP 发第二桢状态  流控制
  	{
        stop_timer(TIMER_DEV3);//硬件定时器
        can_net_layer_send.step   = STOP_STEP;
        rev_flow_frame_para.FS    = can_msg_temp->dat.b[0]&0x0f;
        rev_flow_frame_para.BS    = can_msg_temp->dat.b[1];
        rev_flow_frame_para.STmin = can_msg_temp->dat.b[2]; 

        if(rev_flow_frame_para.FS>0x02)        return;
        if(rev_flow_frame_para.BS>0xFF)       return;
        if(rev_flow_frame_para.STmin>0xFF)     return;    

			
        //rev_flow_frame_para.STmin 定义把时间转换成us
      //   rev_flow_frame_para.STmin=rev_flow_frame_para.STmin*1000;/*单位是ms*/

		  
        //判断接受流控阵FS状态，即允许ECU发送连续帧     
        if(rev_flow_frame_para.FS == 0)
        {  
         //发
         send_consecutive_frame_second_frame(dev);            
				if(can_net_layer_send.had_send_len>=can_net_layer_send.len)
				{          
				can_net_layer_send.valid  = 0; 
				}
				else
				{
				can_net_layer_send.step = CONSECUTIVE_FRAME_THIRD_AND_AFTER_FRAME_STEP;//连续桢发第三桢（含它）以后的桢状态

				start_timer(TIMER_DEV3,(u16)rev_flow_frame_para.STmin);
		           
				g_N_Result = N_FC_RIGHT;
				}
        }
        //等待下一个流控制(下一个流控)
        else if(rev_flow_frame_para.FS == 1)     //if the FS=1,restart the timer 
        {   
            //re-count time and wait for Another fc_frame

			start_timer(TIMER_DEV3,WAINT_FLOW_CONTROL_FRAME_TIM);
           
            can_net_layer_send.step = CONSECUTIVE_FRAME_SECOND_FRAME_STEP;  //发第二桢状态
            g_N_Result  = N_WAIT_NEXT_FC;
        }     
       //超出缓冲区 
        else if(rev_flow_frame_para.FS >= 2)     //the data would be sent is too long to receive for the tester
        {  
            can_net_layer_send.step = STOP_STEP;           
            can_net_layer_send.valid  = 0; 
            diag_rev.consecutive_frame_state = CONSECUTIVE_FRAME_STATE_FINISH;
            g_N_Result = N_BUFFER_OVFLW;  
            dealwith_linklayer_error(dev,g_N_Result);
            return;
        }
        dealwith_linklayer_error(dev,g_N_Result);
     	}
}



/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：ajust_rev_over_maxtime
* 函数功能： 判断发送方大于N_Cr，即对方发的太慢了 接受连续帧
  			if the ECU is receiving diag data,and it didn't receive
			  any thing in 150ms.do report the error cf-timeout and set
			  the receive state to 0
  参数说明： 
* 输入参数：
* 返 回 值：            
  作者：    *----------------------------------------------------------------------------------------------------------------------------------------
*/
void judge_rev_over_maxtime(device_info_t *dev)
{
//	dbg_msgv(dev,"dev->diag_t.cf_curret_time=%d\r\n",dev->diag_t.cf_curret_time);
	if(( dev->diag_t.cf_curret_time - dev->diag_t.cf_last_time)> (REV_CONSECUTIVE_FRAME_OVER_TIM))//N_Cr 接收方接收连续帧的等待时间
	{
		dev->diag_t.cf_time_flag =0;
		if(diag_rev.consecutive_frame_state == CONSECUTIVE_FRAME_STATE_REVCEVING)
		{                
			diag_rev.consecutive_frame_state = CONSECUTIVE_FRAME_STATE_FINISH;
			dealwith_linklayer_error(dev,N_CF_TIMEOUT);            
		}
	}
	dev->diag_t.cf_curret_time=0;		
}
  

/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：ajust_rev_less_mintime
* 函数功能： 判断发送方小于最小间隔内，即太快了
  参数说明： 
* 输入参数：
* 返 回 值：            
  作者：    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
void judge_rev_less_mintime(void)
{
	u32 curret_time;
	 //要求对方最快速度，就不判断
	if(send_flow_frame_para.STmin==STMIN_0MS) 
	{
		return ;
	}
	if((curret_time-diag_rev.CF_last_frame_time)<= send_flow_frame_para.STmin)
	{
		if(diag_rev.consecutive_frame_state == CONSECUTIVE_FRAME_STATE_REVCEVING)
	    {                  
	        //diag_rev.consecutive_frame_state = CONSECUTIVE_FRAME_STATE_FINISH; //暂时掉
	        //提示对方发送大快了
	        //dealwith_linklayer_error(N_CF_TIMEOUT);            
	    }
	}

}



/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：void can_net_layer_send_flow_control(uint8_t FS,uint8_t BS,uint8_t STmin)
* 函数功能： 根据网络层的参数，发送流控制桢
  参数说明： 
* 输入参数：三个参数值分别对应为FS  BS STIM
* 返 回 值：
            
  作者：    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/

void can_net_layer_send_flow_control(device_info_t *dev,u32 id,u8 FS,u8 BS,u8 STmin)
{
	u8 send_data[8] = {0};
	u8 send_enable  =  DISABLE;

	if( id >= 0x700 && id <= 0x7ff )
	{   //诊断ID的流控帧处理
		if( id == AVM_Phy_Resp )
		{
			send_enable = ENABLE;
		}
	}
	else
	{   //普通ID的流控帧处理x
		send_enable = ENABLE;
	//	dbg_msg(dev, "read others flow control enable\r\n");
	}

	if( send_enable == 1 )
	{	//流控制格式
		send_data[0] = 0x30|FS;
		send_data[1] = BS;
		send_data[2] = STmin;
		send_data[3] = FIX_NO_USE_DATA;
		send_data[4] = FIX_NO_USE_DATA;
		send_data[5] = FIX_NO_USE_DATA;	 
		send_data[6] = FIX_NO_USE_DATA;
		send_data[7] = FIX_NO_USE_DATA;	 
		//dbg_msg(dev, "AVM返回流控帧\r\n");
		can_msg_Send_SWJ_Up(dev,id,(u32*)send_data);	 
	}
}

/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：check_frame_sequence_number(uint8_t current_frame_sn,uint8_t last_frame_sn) 
* 函数功能： 连续桢的序号为 1X~21~2F 20~2F,//判断连续桢序号是否正确
  参数说明： 
* 输入参数：current_frame_sn    当前桢的序号 last_frame_sn 上一桢的序号
* 返 回 值：error:return 0
            right:return 1
  作者：    qdh
*----------------------------------------------------------------------------------------------------------------------------------------
*/
uint8_t check_frame_sequence_number(uint8_t current_frame_sn,uint8_t last_frame_sn)
{ 
	if((last_frame_sn >=0x10) &&(last_frame_sn < 0x20))	
    {
		if(current_frame_sn != 0x21) //连续帧第一个SN=1  ，最大为F
			return 0;
		else                            
			return 1;
    }
    else if((last_frame_sn >= 0x20)&&(last_frame_sn<0x2f))
    {
        if(current_frame_sn != last_frame_sn + 1)
        	return 0;
        else
        	return 1;
    }
    else if(last_frame_sn == 0x2f)
    {
        if(current_frame_sn != 0x20)
        	return 0;
        else
          return 1;       
    }
    else
      return 0;
}


/*
*----------------------------------------------------------------------------------------------------------------------------------------
* 函数原型：void diag_receive_data(void)
* 函数功能： 网络层接收数据 处理,主要功能是 处理连续桢，把序号扔掉，组成真正的数据
  参数说明： 
* 输入参数：无
* 返 回 值：无
  作者：    xz
*----------------------------------------------------------------------------------------------------------------------------------------
*/
void diag_receive_data(device_info_t *dev,can_msg_t  *p_msg)
{  
	//流控多包接收
	if(NULL == p_msg)
	{
		return ;
	}
	
//	if(!dev->ci->host_ready)
//	{
//		dbg_msg(dev, "···can msg recv when host not ready, skip···\r\n");
//		return ;
//	}
	if(p_msg->id == AVM_Phy_Req)//物理寻址
	{		 
		set_diag_address_type(DIAG_P_ADDRESS);
	}
	else if(p_msg->id == FUNCATIONAL_REQ_ADDR)
	{
		set_diag_address_type(DIAG_F_ADDRESS);		
	}
	else
	{
		return ;
	}
	//dbg_msgv(dev, "DIAG-ID: 0x%x, DAT: %08x_%08x···\r\n", p_msg->id, p_msg->dat.w[1], p_msg->dat.w[0]);

	if( p_msg->dlc!=0x08) //所有诊断报文dlc=8，不满8填充0x00
	{
		return;    
		}

	//单桢的
    if(p_msg->dat.b[0] < 0x08)//Data[0]为数据长度？？ 判断为单帧？？？             
    {
        if((can_net_layer_send.step == CONSECUTIVE_FRAME_SECOND_FRAME_STEP)||(can_net_layer_send.step == CONSECUTIVE_FRAME_THIRD_AND_AFTER_FRAME_STEP))
        {
          return;
        }   
        if(p_msg->dat.b[0] == 0)                                
    	{
    		return;
    	}
        if((p_msg->dat.b[0] > 0x07) &&(p_msg->dat.b[0]<0x0f))//单帧Data[0]高四位为0 ，低四位为数据长度
    	{
    		return;
    	}
		diag_rev.consecutive_frame_sum = 0;//清零
        memcpy(diag_rev.buf, &p_msg->dat.b[1], 7);//数据第二个字节的前7位放入buf     
        diag_rev.total_len=p_msg->dat.b[0];  //数据报文第一个字节存储数据长度
        app_diagnose(dev,diag_rev.buf,diag_rev.total_len); //单帧经过网络层处理  给数据链层诊断协议处理    
    }

    //连续桢首帧
    else if((p_msg->dat.b[0] >= 0x10) &&(p_msg->dat.b[0] < 0x20))  //receive first frame ==0x10
    {	
		if((can_net_layer_send.step == CONSECUTIVE_FRAME_SECOND_FRAME_STEP)||(can_net_layer_send.step == CONSECUTIVE_FRAME_THIRD_AND_AFTER_FRAME_STEP))
		{
			return;//在发送连续帧状态不能接受多帧
		}   
		//总长度
		if(get_diag_address_type()==DIAG_F_ADDRESS) //功能寻址退出
		{
			return;
		}
		diag_rev.total_len = (p_msg->dat.b[0]&0x0f)<<8;
		diag_rev.total_len = p_msg->dat.b[1] | diag_rev.total_len;//报文DATA总长度，除去PCI的总长度（包含SID）             
		//dbg_msgv(dev, "接受连续帧长度为LEN=%d\r\n",diag_rev.total_len);
		if(diag_rev.total_len < 0x08)  //len_can_data
		{
			return;
		}
        //判断总长度合理性
        if(diag_rev.total_len > DIAG_REV_BUFFER_MAX)  //流控制放在中断那 
        {           
			//发送流控制 可以接继续接收连续桢                     
			send_flow_frame_para.FS=FS_OVLW;//溢出
			send_flow_frame_para.BS=BS_0;//BS参数为0 用于指示发送者在拆分数据的发送期间流控制帧不再发送流控制帧
			if(get_diag_sesion_mode( ) == DIAG_PR_MODE)//编程会话模式
			{  
			    send_flow_frame_para.STmin=STMIN_0MS;//连续帧的帧间隔  设置为0
			}
			else                             
			{
			    send_flow_frame_para.STmin = STMIN_DEFAULT_20MS;
			}						
			can_net_layer_send_flow_control(dev,AVM_Phy_Resp,send_flow_frame_para.FS,send_flow_frame_para.BS,send_flow_frame_para.STmin); //返回服务器 流控制信息			  
			dealwith_linklayer_error(dev,N_FLOW_OVER);//溢出数据太长
			return;
        }
				
        //更新接收连续桢的参数
        diag_rev.consecutive_frame_state = CONSECUTIVE_FRAME_STATE_REVCEVING;  //means ECU is receiving the diagnose data
        diag_rev.consecutive_frame_sum = (diag_rev.total_len - 6)/7; //  每个报文含1个序号数据 ，首帧占6个字节	
		if((diag_rev.total_len - 6)%7)
        {
			diag_rev.consecutive_frame_sum++;
        }		
		//dbg_msgv(dev, "连续帧总个数为:%d\r\n",diag_rev.consecutive_frame_sum);
        diag_rev.last_frame_sn = p_msg->dat.b[0];                          
        diag_rev.num=0;	//清零
        memcpy(&diag_rev.buf[diag_rev.num], &p_msg->dat.b[2], 6);//Data第三个起的地址后 6个字节给buf首地址           
		diag_rev.num+=6;//首帧数据存储
	
		//功能寻址必须是单帧  在物理寻址条件下发送流控制
		if(get_diag_address_type() == DIAG_P_ADDRESS)             
		{                          
            //发送流控制 overflow表示报文长度超出接收方的网络层缓冲区大小				
			send_flow_frame_para.FS=FS_SEND;
            send_flow_frame_para.BS=BS_0;	//不需要等待流控  直接发送连续帧
            if(get_diag_sesion_mode( ) == DIAG_PR_MODE)
            {  
                send_flow_frame_para.STmin=STMIN_0MS;
            }		
            else                             
            {
            
                send_flow_frame_para.STmin = STMIN_DEFAULT_20MS;
            }
		    can_net_layer_send_flow_control(dev,AVM_Phy_Resp,send_flow_frame_para.FS,send_flow_frame_para.BS,send_flow_frame_para.STmin);   	                        	
		 }
        else
        {
            diag_rev.consecutive_frame_sum = 0;  //mul-frame function address,no response
        } 
    }

	
    //20~2F连续桢的接收
    else if((p_msg->dat.b[0] >= 0x20)&&(p_msg->dat.b[0] < 0x30))/*receive continuous frame  连续帧范围*/ 
    {
		judge_rev_over_maxtime(dev)	;
	
        if((can_net_layer_send.step == CONSECUTIVE_FRAME_SECOND_FRAME_STEP)||(can_net_layer_send.step == CONSECUTIVE_FRAME_THIRD_AND_AFTER_FRAME_STEP))
        {
            return;//在发送连续帧状态不能接受多帧
        }  
		if(diag_rev.consecutive_frame_state == CONSECUTIVE_FRAME_STATE_FINISH)
		{                
			return;           
		}
        /*check the frame_head is right or not*/
        if(check_frame_sequence_number(p_msg->dat.b[0],diag_rev.last_frame_sn) == 0)//通过判断相邻序号是否连续判断数据是否传输正确
        {  
            diag_rev.consecutive_frame_state = CONSECUTIVE_FRAME_STATE_FINISH;     //ECU is not in the receiving mul-frame
            dealwith_linklayer_error(dev,N_WRONG_SN);
            return ;
        }
        //
        diag_rev.consecutive_frame_sum--;//此处正在接受一个连续帧。应该减去1.
        //更新接收缓冲区     
        memcpy(&diag_rev.buf[diag_rev.num],&p_msg->dat.b[1], 7); 
        diag_rev.num+=7;//
        if(diag_rev.consecutive_frame_sum==0)   //最后一桢
        {
         	if(diag_rev.num<diag_rev.total_len)  //   if((diag_rev.num + p_msg->DLC) < diag_rev.total_len)
            {
                return;    
            }
			 dev->diag_t.cf_time_flag =0;/**/
            diag_rev.consecutive_frame_state = CONSECUTIVE_FRAME_STATE_FINISH;     //ECU is not in the receiving mul-frame
            diag_rev.last_frame_sn = 0;
			//dbg_msg(dev, "多帧接受完毕，进入诊断层处理 \r\n");
            app_diagnose(dev,diag_rev.buf,diag_rev.total_len);//连续帧没发送完 不送给app_diag处理
        }
        else
        {            
            diag_rev.last_frame_sn = p_msg->dat.b[0];//不是最后一个连续帧  

					/*接收方接收连续帧的等待时间 N_Cr<150ms，开启计时*/
			dev->diag_t.cf_last_time=0;/*定义接受此CF的时间为0 */
			dev->diag_t.cf_time_flag = 1;/*开启滴答计时 10ms 单位*/
        } 
    }    
	
    //正理流控制桢    发送多帧（DTC、DID等）                  
    else if((p_msg->dat.b[0]>>4) == 0x03)/*receive flow control frame*/
    {
		flow_control_frame_dealwith(dev,p_msg,DIAG_MODE);
    }			
}

/**
**@brief:can消息处理进程，处理can常规性报文  车身信息，设置信息
**
**
**************/
u32 Body_Can_proc(device_info_t *dev, can_msg_t *cmsg)
{	
	static u8 Veh_info_buff[8]={0};//必须定义为静态局部变量，不然每次调用会重新初始化。会保存为上一次修改的值
	static u8 Veh_info_buff_last[8]={0};//
	#if ENABLE_WEICHAI
	static body_car_rec_info_t  body_car_rec_data={0};
//	u32 ver[2]={0};
	u8 buf[8]={0};
	u8 HMI_PresentTime[3]={0};
	u8 SEND_KET_DATA=0;
	u16 SAS_SteeringAngle;
	#endif
//	data_info_t di;
	dev=get_device_info();
	
	if(NULL == cmsg)
	{
		return 1 ;
	}
	
	//dbg_msgv(dev, "[Body] ID: 0x%x, DAT: %08x_%08x···\r\n", cmsg->id, cmsg->dat.w[1], cmsg->dat.w[0]);
	if(!dev->ci->host_ready)
	{
	//	dbg_msg(dev, "···can msg recv when host not ready, skip···\r\n");
		return 1;
	}
	//CANID  处理 
	switch(cmsg->id)
	{
	#if ENABLE_WEICHAI
	case 0x031: // from HMI, event 
		//dbg_msgv(dev, "%x  %x\r\n",body_car_rec_data.HMI_AVM_SW,cmsg->dat.b[6]&0x0f);
		//HMI_AVM_SW
		if(body_car_rec_data.HMI_AVM_SW != (cmsg->dat.b[6]&0x0f))
		{//dbg_msg(dev, "test\r\n");
			body_car_rec_data.HMI_AVM_SW = cmsg->dat.b[6]&0x0f;
			if((body_car_rec_data.HMI_AVM_SW == 0x01)||(body_car_rec_data.HMI_AVM_SW == 0x02))
			{//	dbg_msg(dev, "test2\r\n");
				comm_message_send(dev, CM_SEND_KEY, 0, &body_car_rec_data.HMI_AVM_SW, 1);
				}
			else if((body_car_rec_data.HMI_AVM_SW == 0x03)||(body_car_rec_data.HMI_AVM_SW == 0x04))
			{
				SEND_KET_DATA=body_car_rec_data.HMI_AVM_SW - 0x03;
				comm_message_send(dev, CM_VIDEO_CALIBRATION, 0, &SEND_KET_DATA, 1);
				}
			}
		else 
		{
			if((body_car_rec_data.HMI_AVM_SW == 0x03)||(body_car_rec_data.HMI_AVM_SW == 0x04))
			{
				SEND_KET_DATA=body_car_rec_data.HMI_AVM_SW - 0x03;
				comm_message_send(dev, CM_VIDEO_CALIBRATION, 0, &SEND_KET_DATA, 1);
					}
				}
					
		//HMI_AVMViewTypeSet
		if(body_car_rec_data.HMI_AVMViewTypeSet != (cmsg->dat.b[5]&0x78)>>3)
		{
			body_car_rec_data.HMI_AVMViewTypeSet = (cmsg->dat.b[5]&0x78)>>3;
			comm_message_send(dev, CM_SET_ViewType, 0, &body_car_rec_data.HMI_AVMViewTypeSet, 1);
			}
		
		//HMI_AVMModeSet
		if(body_car_rec_data.HMI_AVMModeSet != (cmsg->dat.b[5]&0x03))
		{
			body_car_rec_data.HMI_AVMModeSet = (cmsg->dat.b[5]&0x03);
			comm_message_send(dev, CM_SET_MODE, 0, &body_car_rec_data.HMI_AVMModeSet, 1);
			}
		
		//HMI_SetLampTurnEnable
		if(body_car_rec_data.HMI_SetLampTurnEnable != (cmsg->dat.b[4]&0x03))
		{
			body_car_rec_data.HMI_SetLampTurnEnable = cmsg->dat.b[4]&0x03;
			comm_message_send(dev, CM_SET_TURN_ENABLE, 0, &body_car_rec_data.HMI_SetLampTurnEnable, 1);
		}
		//HMI_SetCarColor
		if(body_car_rec_data.HMI_SetCarColor != (cmsg->dat.b[2]&0XE0)>>5)
		{
			body_car_rec_data.HMI_SetCarColor = (cmsg->dat.b[2]&0XE0)>>5;
			comm_message_send(dev, CM_SET_CAR_COLOUR, 0, &body_car_rec_data.HMI_SetCarColor, 1);
			}
		
	
		//HMI-TIME
		HMI_PresentTime[0] = cmsg->dat.b[2]&0X1F;//
		HMI_PresentTime[1] = cmsg->dat.b[3]&0X3F;
		HMI_PresentTime[2] = (cmsg->dat.b[4]&0XFC)>>2;
		 	comm_message_send(dev, CM_PRESENT_TIME, 0, &HMI_PresentTime[0], 3); 
			
		//HMI_AVMInfo
		body_car_rec_data.HMI_AVMInfo = cmsg->dat.b[1]&0X07;
		if(body_car_rec_data.HMI_AVMInfo==0x01)
		{
			comm_message_send(dev, CM_GET_MODE, 0, &body_car_rec_data.HMI_AVMInfo, 1);
			}
		else if(body_car_rec_data.HMI_AVMInfo==0x02)//0x2:获取MCU版本号
		{
//			para_read_Nbyte(ADDR_SoftwareVersion,&mcu_ver[0],LEN_ECUSoftwareVersion);		
			buf[0] = 0x02;
			buf[1] = (get_app_version()&0xff0000)>>16;
			buf[2] = (get_app_version()&0xff00)>>8;
			buf[3] =  get_app_version()&0xff;
			buf[4] = (get_boot_version()&0xff0000)>>16;
			buf[5] = (get_boot_version()&0xff00)>>8;
			buf[6] = get_boot_version()&0xff;
			body_can_send_fill(dev, 0x0B3,(u32 *)&buf[0],0,"BC0B3");
			}
		else if(body_car_rec_data.HMI_AVMInfo==0x03)//0x3:获取ARM版本号
		{
	//		dbg_msg(dev, "···ARM版本信息··· \r\n");
			comm_message_send(dev, CM_ARM_APP_VERSION, 0, NULL, 0);
			comm_message_send(dev, CM_ARM_ARI_VERSION, 0, NULL, 0);				
		 	}
		else if(body_car_rec_data.HMI_AVMInfo==0x04)
		{
	 //		dbg_msg(dev, "···车身颜色··· \r\n");
			comm_message_send(dev, CM_GET_AVM_INFO, 0, NULL, 0);	
		  	}
		else if(body_car_rec_data.HMI_AVMInfo==0x05)//0x5:查询转向功能是否开启
		{
		//	dbg_msg(dev, "查询转向功能是否开启 \r\n");
			comm_message_send(dev, CM_GET_TURN_STATUS, 0,  NULL, 0);
		  	}

		 break;

	case 0x316: //from BCM ，cycle(40ms)  转向灯 10*cycle 未收到记录dtc
		//To show the Key Position from BCM.
		if(dev->ipc->miss_avm1==0)
		{
			stimer_stop(dev, FLAG_BODYCAN_MISS1);
			stimer_start(dev, FLAG_BODYCAN_MISS1, TO_CNT_400MS);
			}
		else{
			dev->ipc->miss_avm1=0;
			stimer_start(dev, FLAG_BODYCAN_MISS1, TO_CNT_400MS);
			}
		if(body_car_rec_data.BCM_StatusIgnition != ((cmsg->dat.b[1]>>3)&0X03))
		{
			body_car_rec_data.BCM_StatusIgnition = (cmsg->dat.b[1]>>3)&0X03;
			if(body_car_rec_data.BCM_StatusIgnition !=0x00)
			{
				Veh_info_buff[4] = body_car_rec_data.BCM_StatusIgnition-1;				
				}
//			else
//				 dbg_msg(dev, "BCM_StatusIgnition is invalid\r\n");
			}
		
		if(body_car_rec_data.BCM_StatusLamp !=((cmsg->dat.b[1]>>6)&0X03))
		{
			body_car_rec_data.BCM_StatusLamp =(cmsg->dat.b[1]>>6)&0X03;
			}
		//To show the R Shift Status for the MT Model.
		if(body_car_rec_data.BCM_SWStatusShiftRMT != ((cmsg->dat.b[0]>>6)&0X03))
		{
			body_car_rec_data.BCM_SWStatusShiftRMT = (cmsg->dat.b[0]>>6)&0X03;
			//comm_message_send(dev, CM_BCM_SWStatusShiftRMT , 0,  &body_car_rev_data->BCM1_info_t.BCM_StatusIgnition, 1)
			} 
		Veh_info_buff[4] =(Veh_info_buff[4]&0XCF)|(body_car_rec_data.BCM_StatusLamp<<4) ;
			/*车辆左右转  转向灯为闪烁模式时候的  转向灯解析方法
			if(((cmsg->dat.b[3]>>3) & 0x01) == 0)//左转向灯	
			{
				count1++;
				if(count1 >= 20)
				{
					Veh_info_buff[4] &= ~0x10;
					count1 = 21;
				}
				else
				{
				Veh_info_buff[4] |= 0x10; 
				}
			}
			else
			{
				count1 = 0;
			}
			if(((cmsg->dat.b[1]>>5) & 0x01) == 0)//右转向灯
			{
				count2++;
				if(count2 >= 20)
				{
					Veh_info_buff[4] &= ~0x20;
					count2 = 21;
			}
				else
			{
				Veh_info_buff[4] |= 0x20; 
				}
			}
			else
			{
				count2 = 0;
			}*/		
		break;
	
	case 0x1F0: //from BCMB1 cycle(10ms) 
		//VSO status
	//	dbg_msg(dev, "test\r\n");
		body_car_rec_data.ESP_VehicleSpeedValid = (cmsg->dat.b[2]>>4)&0x01;
		if(body_car_rec_data.ESP_VehicleSpeedValid)
			{
				body_car_rec_data.ESP_VehicleSpeed = (cmsg->dat.b[4]<<8)|(cmsg->dat.b[5]);
				if(body_car_rec_data.ESP_VehicleSpeed > 0x1900)
				{
					break;
				}
				body_car_rec_data.ESP_VehicleSpeed *=0.05625;//km/h
			//	dbg_msgv(dev, "vel-speed=%d\r\n",body_car_rec_data.ESP_VehicleSpeed);
				Veh_info_buff[0] = 0x00;
				Veh_info_buff[0] |=(u8)(body_car_rec_data.ESP_VehicleSpeed&0x0F)<<4;
				Veh_info_buff[1]  =(u8)(body_car_rec_data.ESP_VehicleSpeed>>4);
		}
		else
			dbg_msgv(dev, "ESP_VehicleSpeedValid is invalid\r\n");
		
		body_car_rec_data.SAS_SteeringAngleVD   = (cmsg->dat.b[2]>>6)&0x01;  //SAS_SteeringAngleVD 0 invalid  1 valid	
		if(body_car_rec_data.SAS_SteeringAngleVD)
		{
			SAS_SteeringAngle = (((u16)cmsg->dat.b[6])<<8)|cmsg->dat.b[7];
			if(SAS_SteeringAngle > 0x3CF0)
			{
				break;
			}
			SAS_SteeringAngle =SAS_SteeringAngle/10u;
			body_car_rec_data.SAS_SteeringAngle =(s16)((780-(s16)SAS_SteeringAngle)/17);//无符号转换为有符号
			dbg_msgv(dev, "vel-SteeringAngle=%d\r\n",body_car_rec_data.SAS_SteeringAngle);
			Veh_info_buff[2] = (body_car_rec_data.SAS_SteeringAngle & 0XFFu);
			Veh_info_buff[3] =((body_car_rec_data.SAS_SteeringAngle >> 8)&0XFFu);
		}
		else
			dbg_msgv(dev, "SAS_SteeringAngleVD is invalid\r\n");
		 break;

	case  0x278: //from EMS3   cycle(10ms) EMS_EngineRunningStatue
		//engine running status 
		if(dev->ipc->miss_avm3==0)
		{
			stimer_stop(dev, FLAG_BODYCAN_MISS3);
			stimer_start(dev, FLAG_BODYCAN_MISS3, TO_CNT_100MS);
			}
		else{
			dev->ipc->miss_avm3=0;
			stimer_start(dev, FLAG_BODYCAN_MISS3, TO_CNT_100MS);
			}
		
		body_car_rec_data.EMS_EngineRunningStatue = (cmsg->dat.b[5]&0x0E)>>1;
		body_car_rec_data.EMS3_CheckSum           =  cmsg->dat.b[7];//ID MSB XOR ID LSB XOR Byte0 XOR Byte1 XOR … XOR Byte6
		break;

	case  0x116://from tcu2
	//Transmission gear selector lever position
//		dbg_msg(dev, "test\r\n");
//		body_car_rec_data.TCU_TGLeverValid = (cmsg->dat.b[0]&0x08)>>3;
		if(dev->ipc->miss_avm2==0)
		{
			stimer_stop(dev, FLAG_BODYCAN_MISS2);
			stimer_start(dev, FLAG_BODYCAN_MISS2, TO_CNT_100MS);
			}
		else{
			dev->ipc->miss_avm2=0;
			stimer_start(dev, FLAG_BODYCAN_MISS2, TO_CNT_100MS);
			}
		
			body_car_rec_data.TCU_TGSLever =cmsg->dat.b[0]&0x07;
			if(body_car_rec_data.TCU_TGSLever==0x01)
				Veh_info_buff[5]=0x05;
			else if(body_car_rec_data.TCU_TGSLever==0x02)
				Veh_info_buff[5]=0x01;
			else if(body_car_rec_data.TCU_TGSLever==0x03)
				Veh_info_buff[5]=0x00;
			else if(body_car_rec_data.TCU_TGSLever==0x04)
				Veh_info_buff[5]=0x04;
			else if(body_car_rec_data.TCU_TGSLever==0x05)
				Veh_info_buff[5]=0x06;
			else if(body_car_rec_data.TCU_TGSLever==0x02)
				dbg_msg(dev,"Reserved\r\n");
			else if(body_car_rec_data.TCU_TGSLever==0x02)
				dbg_msg(dev,"TCU_TGSLever is invalid\r\n");
			else 
				dbg_msg(dev,"TCU_TGSLever is undefined\r\n");
	
		break;
	#endif 
	
	#if ENABLE_autotest
	    case 0x652:
		if(dev->ipc->autotest_flag == 1)
		{
			   buf[0] = 0x65;
			   buf[1] = 0x02;
			   buf[2] = 0x01;
			   buf[3] = 0x55;
			   buf[4] = 0x55;
			   buf[5] = 0x55;
			   buf[6] = 0x55;
			   buf[7] = 0x55;
			   can_msg_Send_SWJ(dev, 0x653,(u32*)buf); 
			}
	    break;
	#endif
	
	#if ENABLE_TEST
		case GET_MCU_VER_ID:
			buf[0] = 0;
			buf[1] = get_app_version()  >> 16;
			buf[2] = (get_app_version() >> 8) & 0xFF;
			buf[3] = get_app_version()  & 0xFF;
			buf[4] = 0;
			buf[5] = dev->ipc->boot_version>> 16;
			buf[6] = (dev->ipc->boot_version >> 8) & 0xff;
			break;
		case GET_ARM_VER_ID:
			di.data = buf;
			di.len = 0;
			comm_message_send1(dev, CM_ARM_APP_VERSION, 0, &di, 1);
			for( i= 0;i<2000;i++);
			comm_message_send1(dev, CM_ARM_ARI_VERSION, 0, &di, 1);
			break;
		case SET_VIDEO_VIEW_ID:
			buf[0] = cmsg->dat.b[0];
			di.data = buf;
			di.len = 1;
			comm_message_send1(dev, CM_SET_ViewType, 0, &di, 1);
			break;
	#endif
		default:break;
	}

	//处理车身信息 汇总 到Veh_info_buff 匹配与ARM的通讯协议  added 20180706，memcmp判断有没有更新，如有就发送，最后将此次更新的copy到 last数组
	if(memcmp(Veh_info_buff,Veh_info_buff_last,8) != 0)//不相等为非0
	{	
		comm_message_send(dev, CM_SEND_VEHICLE_INFO, 0, &Veh_info_buff[0], 8);
	 	dbg_msgv(dev, "send to ARM: data0:%x data1:%x data2:%x data3:%x data4:%x data5:%x  \r\n", Veh_info_buff[0],Veh_info_buff[1],Veh_info_buff[2],Veh_info_buff[3],Veh_info_buff[4],Veh_info_buff[5]);
	}
	memcpy(Veh_info_buff_last,Veh_info_buff ,8);
	return 1;
}


//extern const ECU_READ_DTC_CMD_CONST_TYPE   ecu_read_dtc_cmd_const[ECU_READ_DTC_MAX];


/*******************************************************************************
* Description: can_link_receive_data
* Argument   :  根据CANID 范围处理接受CAN报文信息 1、车身 2、DIAG 3、NM
* Return     :     -1 数据长度错误，不是12的整数倍
*                   0 无数据取出
*                   1 取得有效数据
* Author     :
********************************************************************************/
void can_link_receive_data(device_info_t * dev)
{	
	can_msg_t  *can_msg_tmp=NULL;	
	can_msg_tmp  = car_can_recv(dev);
	dev=get_device_info();
	if(NULL == can_msg_tmp)
	{
		return ;
	}
	// 根据ID来分配处理途径。1、车身 2、NM报文(备选) 3、DIAG   小端发送，morotola lsb 编码
 	if( can_msg_tmp->id >= 0x700 && can_msg_tmp->id <= 0x7ff )//diag 报文
  	{
			dev->diag_t.diag_rec_sts = 1;
			diag_receive_data(dev,can_msg_tmp);		
    	}
	else if((can_msg_tmp->id > 0x00) && (can_msg_tmp->id <= 0x700))
	{
		if(get_diag_communication_state()==DIAG_COMM_RX_AND_TX_ENABLE)
			{
				Body_Can_proc(dev,can_msg_tmp);
		}
		else {
			dbg_msg(dev,"禁止车身报文通讯\r\n");
			return;
		}
	}
}


/*****************************************************************************
 函 数 名  : TIM4_TRQHandler
 功能描述  : 等待流控制定时器
 输入参数  : i
             i
             i
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2018年7月20日
    作    者   : xz
    修改内容   : 新生成函数

*****************************************************************************/
void TIM4_IRQHandler( void )
{
	u16 len;
	u8 send_data[8]={0};
	// clear pending flag
	TIM4->SR = 0;//状态寄存器，硬件置1 软件清零 
	//等待流控制桢超时  
	 if(can_net_layer_send.step == CONSECUTIVE_FRAME_SECOND_FRAME_STEP)  //发第二桢状态
  	{   
		stop_timer(TIMER_DEV3);
		can_net_layer_send.step = STOP_STEP;
		can_net_layer_send.valid  = 0;
		dbg_msg(get_device_info(),"等待流控超时\r\n");
    } 
	 else if(can_net_layer_send.step == CONSECUTIVE_FRAME_THIRD_AND_AFTER_FRAME_STEP) //连续桢发第三桢（含它）以后的桢状态
    {
		//判断有没有发完 the data had been sent over
		if(can_net_layer_send.had_send_len>=can_net_layer_send.len)
		{
			stop_timer(TIMER_DEV3);
			can_net_layer_send.step = STOP_STEP;           
			can_net_layer_send.valid  = 0; 
		} 
		//发数据
		else
		{
			len = can_net_layer_send.len - can_net_layer_send.had_send_len;
			if(len >= 7u)
			{
				len = 7;
			}
			can_net_layer_send.consecutive_frame_serial = (can_net_layer_send.consecutive_frame_serial+1u)%0x10u;//1 .2 3 ~15 .0
			send_data[0]= CONSECUTIVE_FRAME_SECOND_HEAD_0X20 | can_net_layer_send.consecutive_frame_serial;
			memcpy(&send_data[1],&can_net_layer_send.buff[can_net_layer_send.had_send_len],len); //len=7         
			//启动发送
			can_msg_Send_SWJ_Up(get_device_info(), AVM_Phy_Resp,(u32*)send_data);         
			//更新参数
			can_net_layer_send.had_send_len = can_net_layer_send.had_send_len + len;
			if(can_net_layer_send.had_send_len>=can_net_layer_send.len)
			{
				stop_timer(TIMER_DEV3);
				can_net_layer_send.step = STOP_STEP;           
				can_net_layer_send.valid  = 0;
				/*发送完成 需清零发送buff*/
				memset(&can_net_layer_send.buff[0],0,CAN_NET_LAYER_SEND_BUFF_MAX);
//				can_net_layer_send.buff[CAN_NET_LAYER_SEND_BUFF_MAX]={0};
				can_net_layer_send.len  = 0;
			}
		} 
    } 	
}
