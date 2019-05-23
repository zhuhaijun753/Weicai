
#include <stdio.h>
#include <string.h>
#include <config.h>
#include <miscdef.h>
#include <usart.h>
#include <api.h>


void EXTI9_5_IRQHandler(void)
{
	EXTI_TypeDef *exti = EXTI;
	
	if(exti->PR & __BIT(8)) // 
	{
		exti->PR |= __BIT(8);
	}
}

void EXTI15_10_IRQHandler(void)
{
	EXTI_TypeDef *exti = EXTI;

	if(exti->PR & __BIT(11))  
	{
		exti->PR |= __BIT(11);
	}

}

static void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
//	SCB_Type *scb =SCB; 
	u32 temp,temp1;	  
	temp1=(~(u32)NVIC_Group)&0x07;//è?oóèy??
	temp1<<=8;
	temp=SCB->AIRCR;  //?áè??è?°μ?éè??
	temp&=0X0000F8FF; //?????è?°·?×é
	temp|=0X05FA0000; //D′è???3×
	temp|=temp1;	   
	SCB->AIRCR=temp;  //éè??·?×é	    	  				   
}

void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//éè??·?×é
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//è?μí????  
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//ê1?ü?D????(òa??3yμ??°,?à·′2ù×÷?íOK) 
	NVIC->IP[NVIC_Channel]|=temp<<4;		//éè???ìó|ó??è??oí?à??ó??è??   	    	  				   
}

void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//μ?μ??D????′??÷×éμ?±ào?
	EXTOFFSET=(BITx%4)*4; 
	RCC->APB2ENR|=0x01;//ê1?üio?′ó?ê±?ó			 
	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//??3y?-à′éè??￡?￡?￡?
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITxó3é?μ?GPIOx.BITx 
	//×??ˉéè??
	EXTI->IMR|=1<<BITx;//  ?a??line BITxé?μ??D??
	//EXTI->EMR|=1<<BITx;//2??á±?line BITxé?μ?ê??t (è?1?2??á±??a??,?úó2?té?ê??éò?μ?,μ?ê??úèí?t·???μ?ê±oò?T·¨??è??D??!)
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITxé?ê??t???μ??′￥·￠
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITxé?ê??té?éy?μ??′￥·￠
} 

void ACC_EXTI_Init(void)
{
	RCC->APB2ENR|=1<<2;     //A时钟使能
	IO_INIT_I_PU(GPIOA, 8);
	
	Ex_NVIC_Config(0,8,1); 	
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2);  	//?à??2￡?×óó??è??1￡?×é2	   
}
void CAN_RX_EXTI_Init(void)
{
	RCC->APB2ENR|=1<<2;     //A时钟使能
	IO_INIT_I_PU(GPIOA, 11);
	
	Ex_NVIC_Config(0,11,1); 	
	MY_NVIC_Init(2,0,EXTI15_10_IRQn,2);  	//?à??2￡?×óó??è??1￡?×é2	   
}

void boot_gpio_init(device_info_t *dev)
{
	if(dev->ipc->app_jump)
	{
		return;
	}

	// enable GPIOA/B/C/D/E peripherals
	RCC->APB2ENR |= __BIT(0)|__BIT(6) | __BIT(5) | __BIT(4) | __BIT(3) | __BIT(2);
/**/
	IO_INIT_O_PP(GPIOB, 0, HIGH); //

	IO_INIT_O_PP(GPIOA, 3, HIGH); //
	
	IO_INIT_O_PP(GPIOB, 8, HIGH); //

	IO_INIT_O_PP(GPIOB, 12, LOW); //

	IO_INIT_O_PP(GPIOB, 13, LOW); //

	IO_INIT_O_PP(GPIOB, 14, LOW); //

	IO_INIT_O_PP(GPIOB, 15, LOW);
}



#if 0
/*
*@brief: 停机模式下等待ACC中断进入 
*autor：xz
*DATE：20180628
*/
//ACC 唤醒
void EXTI9_5_IRQHandler(void)
{
	EXTI_TypeDef *exti = EXTI;

	device_info_t *dev;
	
	dev=get_device_info();
	
	dev->ipc->stopmode_awake_type_state=0x01;
	
	if(exti->PR & __BIT(8)) // 
	{
		exti->PR |= __BIT(8);
	}
}

/*
*@brief: 停机模式下等待CAN中断进入 
*autor：xz
*DATE：20180628
*/
//CAN 唤醒
void EXTI15_10_IRQHandler(void)
{
	EXTI_TypeDef *exti = EXTI;

	device_info_t *dev;
	
	dev=get_device_info();
	
	dev->ipc->stopmode_awake_type_state=0x02;

	if(exti->PR & __BIT(11))  
	{
		exti->PR |= __BIT(11);
	}

}

//外部中断配置函数
//只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
//参数:
//GPIOx:0~6,代表GPIOA~G
//BITx:需要使能的位;
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线           
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	AFIO_TypeDef *aio = AFIO;  //辅助功能IO口
	RCC_TypeDef  *rcc = RCC;
	EXTI_TypeDef *exti = EXTI;
    u8 EXTADDR;
    u8 EXTOFFSET;
    EXTADDR=BITx/4;//得到中断寄存器组的编号
    EXTOFFSET=(BITx%4)*4; 
    rcc->APB2ENR|=__BIT(0) +__BIT(GPIOx+2);//使能io复用时钟 
    IO_INIT_I_PU(GPIOA, BITx);//使能PA.X
    aio->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
    aio->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITx映射到GPIOx.BITx 
    //自动设置
    exti->IMR|=1<<BITx;//  开启line BITx上的中断
    //EXTI->EMR|=1<<BITx;//不屏蔽line BITx上的事件 (如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
    if(TRIM&0x01)
		exti->FTSR|=1<<BITx;//line BITx上事件下降沿触发
    if(TRIM&0x02)
		exti->RTSR|=1<<BITx;//line BITx上事件上升降沿触发
}

//设置向量表偏移地址
//NVIC_VectTab:基址
//Offset:偏移量             
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)     
{    
	SCB_Type *scb =SCB;         
    scb->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
    //用于标识向量表是在CODE区还是在RAM区
}

//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组            
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)     
{ 
	
	SCB_Type *scb =SCB; 
	u32 temp,temp1;      
    temp1=(~NVIC_Group)&0x07;//取后三位
    temp1<<=8;
    temp=scb->AIRCR;  //读取先前的设置
    temp&=0X0000F8FF; //清空先前分组
    temp|=0X05FA0000; //写入钥匙
    temp|=temp1;       
    scb->AIRCR=temp;  //设置分组                                 
}
//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先       
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)     
{ 
	NVIC_Type *nvic = NVIC;
	u32 temp;    
    MY_NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
    temp=NVIC_PreemptionPriority<<(4-NVIC_Group);      
    temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
    temp&=0xf;//取低四位  
    nvic->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//使能中断位(要清除的话,相反操作就OK) 
    nvic->IP[NVIC_Channel]|=temp<<4;//设置响应优先级和抢断优先级                                    
}



//外部中断初始化程序
//初始化PA8为中断输入.
void ACC_EXTI_Init(void)
{
    Ex_NVIC_Config(0,8,1);       //ACC_IN下降沿触发  唤醒
   
    MY_NVIC_Init(2,0,EXTI9_5_IRQn,2);//抢占2，子优先级1，组2 
}


//外部中断初始化程序
//初始化PA11为中断输入.
void CAN_RX_EXTI_Init(void)
{
    Ex_NVIC_Config(0,11,1);       //CAN_RX下降沿触发  唤醒
   
    MY_NVIC_Init(2,1,EXTI15_10_IRQn,2);//抢占2，子优先级1，组2 
}
#endif
#if 0

void EXTI0_IRQHandler(void)
{
	
	EXTI_TypeDef *exti = EXTI;

	// clear EXTI interrupt pending state
	if(exti->PR & __BIT(0)) // 
	{
		exti->PR |= __BIT(0);
		FCP_S_init();
	}
}
void EXTI1_IRQHandler(void)
{
	
	EXTI_TypeDef *exti = EXTI;

	// clear EXTI interrupt pending state
	if(exti->PR & __BIT(1)) //
	{
		exti->PR |= __BIT(1);

		BCP_S_init();

	}
}
void EXTI2_IRQHandler(void)
{
	
	EXTI_TypeDef *exti = EXTI;

	// clear EXTI interrupt pending state
	if(exti->PR & __BIT(2)) // 
	{
		exti->PR |= __BIT(2);

		LCP_S_init();

	}
}

void EXTI3_IRQHandler(void)
{
	EXTI_TypeDef *exti = EXTI;

	// clear EXTI interrupt pending state
	if(exti->PR & __BIT(3)) // 
	{
		exti->PR |= __BIT(3);

		RCP_S_init();
	}
}



static void exti_init(void)
{
	u32 prio;
	IRQn_Type irq;
	AFIO_TypeDef *aio = AFIO;
	RCC_TypeDef *rcc = RCC;
	EXTI_TypeDef *exti = EXTI;

	// enable AFIO clock
	rcc->APB2ENR |= __BIT(0) +__BIT(2);
	prio = 5;
	irq = EXTI9_5_IRQn;
	NVIC_DisableIRQ(irq);
	// avoid malfunction  IMR中断屏蔽寄存器 写1有效 
	exti->IMR &= ~__BIT(3);
	// input pullup/pulldown
	IO_INIT_I_PU(GPIOA, 8);
	// ILL & PARK DET have no external Pullup, so use internal pullup
	aio->EXTICR[8>>2] |= 0x00;//清零原来设置
	aio->EXTICR[8>>2] |= 0;
	// both Falling & Rising edge trigger a interrupt
	exti->FTSR |= __BIT(8);
	NVIC_SetPriority(irq, prio);
	NVIC_EnableIRQ(irq);
	
}

static void exti_enable(u32 which)
{
	IRQn_Type irq;
	EXTI_TypeDef *exti = EXTI;

	switch(which)
	{

		case EXTI_DEV0:
			irq = EXTI0_IRQn;
			exti->PR |= __BIT(0);
			exti->IMR |= __BIT(0);
			break;

		case EXTI_DEV1:
			irq = EXTI1_IRQn;
			exti->PR |= __BIT(1);
			exti->IMR |= __BIT(1);
			break;


		case EXTI_DEV2:
			irq = EXTI2_IRQn;
			exti->PR |= __BIT(2);
			exti->IMR |= __BIT(2);
			break;

		case EXTI_DEV3:
			irq = EXTI3_IRQn;
			exti->PR |= __BIT(3);
			exti->IMR |= __BIT(3);
			break;


		default:

			return;
	}

	NVIC_EnableIRQ(irq);
}





static void exti_disable(u32 which)
{
	EXTI_TypeDef *exti = EXTI;

	switch(which)
	{
		case EXTI_DEV0:
			exti->IMR &= ~ __BIT(0);
			return;

		case EXTI_DEV1:
			exti->IMR &= ~ __BIT(1);
			return;
		case EXTI_DEV2:
			exti->IMR &= ~ __BIT(2);
			return;

		case EXTI_DEV3:
			exti->IMR &= ~ __BIT(3);
			return;
		default:
			return;
	}
}








static void exti_deinit(u32 which)
{
	exti_disable(which);
}

// separate from acc, for Power source is different, MCUIO3V3
void other_line_gpio_init(device_info_t *dev)
{
	
	//return;
	exti_init(EXTI_DEV0);
	exti_init(EXTI_DEV1);
	exti_init(EXTI_DEV2);
	exti_init(EXTI_DEV3);
	other_line_msg_pre_init(dev);

}

// separate from acc, for Power source is different, MCUIO3V3
void other_line_gpio_int_enable(device_info_t *dev)
{
	exti_enable(EXTI_DEV0);
	exti_enable(EXTI_DEV1);
	exti_enable(EXTI_DEV2);
	exti_enable(EXTI_DEV3);

}

void other_line_gpio_deinit(void)
{

	exti_deinit(EXTI_DEV0);
	exti_deinit(EXTI_DEV1);
	exti_deinit(EXTI_DEV2);
	exti_deinit(EXTI_DEV3);

}
#endif

#if 0
void app_gpio_init(device_info_t *dev)
{
//	Det_line_msg_pre_init();//摄像头电

	IO_INIT_I_PU(GPIOA, 8); // ACC in

	IO_INIT_O_PP(GPIOA, 3, HIGH); //

	IO_INIT_O_PP(GPIOB, 0, LOW); //PMIC 复位IMAX6
	
	IO_INIT_O_PP(GPIOB, 8, LOW); //

	//摄像头电源使能脚
	IO_INIT_O_PP(GPIOB, 12, HIGH); //

	IO_INIT_O_PP(GPIOB, 13, HIGH); //

	IO_INIT_O_PP(GPIOB, 14, HIGH); //

	IO_INIT_O_PP(GPIOB, 15, HIGH);
}


//for  T3  added 20180611

void USB_Master_Slave(u32 on)	//1 master  0 slave
{
	if (on)
	{
		IO_INIT_O_PP(GPIOA, 5, LOW);
	}
		IO_INIT_O_PP(GPIOA, 5, HIGH);
}

void gpio_reset_host_enter(void)
{
		IO_SET(GPIOB, 0, HIGH); // reset  on
		
		IO_SET(GPIOB, 1, LOW); //  PMIC power off
 
}
void gpio_reset_host_leave(void)
{
		IO_SET(GPIOB, 1, HIGH); //  PMIC POWER ON
		
		IO_SET(GPIOB, 0, LOW); //   reset off
}




void peripheral_power_ctrl(u32 on)
{
	IO_SET(GPIOB, 5, on); // M_PWREN, peripheral power ON
}


static void CAM_Control(BYTE num,BYTE onoff)
{
	static u8 buf[4]={0};
	device_info_t *dev;
	dev = get_device_info();

	if(buf[num-RCP_S]==onoff)
	{
		dbg_msgfv(dev, "GIPOB %d is already: %s\r\n", num,onoff ? "ON" : "OFF");
		return;
	}
	buf[num-RCP_S]= onoff;
	IO_SET(GPIOB,num,onoff);
	if(onoff)
	{
		set_flag(dev, FLAG_FCP_POWERON+num-RCP_S);
	}
	else
	{
		set_flag(dev, FLAG_FCP_POWERON+num-RCP_S);
	}
	
}


void power_on_gpio_ctrl(u32 on)
{
	//device_info_t *dev;

	if(on)
	{
		//dev = get_device_info();

		IO_SET(GPIOB, 5, HIGH); //
		mdelay(10);
		IO_SET(GPIOB, 0, LOW); //
		IO_SET(GPIOB, 1, HIGH); //
		IO_SET(GPIOB, 8, LOW); //
		IO_SET(GPIOA, 3, HIGH); //

		CAM_Control(FCP_S,HIGH);
		CAM_Control(BCP_S,HIGH);
		CAM_Control(LCP_S,HIGH);
		CAM_Control(RCP_S,HIGH);

	}
	else
	{
		IO_SET(GPIOB, 12, LOW); //
		mdelay(20);
		IO_SET(GPIOB, 13, LOW); //
		mdelay(20);
		IO_SET(GPIOB, 14, LOW); //
		mdelay(20);
		IO_SET(GPIOB, 15, LOW); //
		mdelay(20);
		IO_SET(GPIOB, 5, LOW); //
		IO_SET(GPIOA, 3, LOW); //

	}
}
#endif




