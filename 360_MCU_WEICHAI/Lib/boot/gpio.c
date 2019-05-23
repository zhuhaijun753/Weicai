
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
	temp1=(~(u32)NVIC_Group)&0x07;//��?o����y??
	temp1<<=8;
	temp=SCB->AIRCR;  //?����??��?���?����??
	temp&=0X0000F8FF; //?????��?�㡤?����
	temp|=0X05FA0000; //D�䨨???3��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //����??��?����	    	  				   
}

void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//����??��?����
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//��?�̨�????  
	NVIC->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//��1?��?D????(��a??3y��??��,?������2������?��OK) 
	NVIC->IP[NVIC_Channel]|=temp<<4;		//����???����|��??��??o��?��??��??��??   	    	  				   
}

void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//��?��??D????��??�¡�����?����o?
	EXTOFFSET=(BITx%4)*4; 
	RCC->APB2ENR|=0x01;//��1?��io?�䨮?����?��			 
	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//??3y?-���䨦��??��?��?��?
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITx��3��?��?GPIOx.BITx 
	//��??������??
	EXTI->IMR|=1<<BITx;//  ?a??line BITx��?��??D??
	//EXTI->EMR|=1<<BITx;//2??����?line BITx��?��?��??t (��?1?2??����??a??,?����2?t��?��??����?��?,��?��??������?t��???��?����o��?T����??��??D??!)
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx��?��??t???��??�䣤����
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx��?��??t��?��y?��??�䣤����
} 

void ACC_EXTI_Init(void)
{
	RCC->APB2ENR|=1<<2;     //Aʱ��ʹ��
	IO_INIT_I_PU(GPIOA, 8);
	
	Ex_NVIC_Config(0,8,1); 	
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2);  	//?��??2��?������??��??1��?����2	   
}
void CAN_RX_EXTI_Init(void)
{
	RCC->APB2ENR|=1<<2;     //Aʱ��ʹ��
	IO_INIT_I_PU(GPIOA, 11);
	
	Ex_NVIC_Config(0,11,1); 	
	MY_NVIC_Init(2,0,EXTI15_10_IRQn,2);  	//?��??2��?������??��??1��?����2	   
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
*@brief: ͣ��ģʽ�µȴ�ACC�жϽ��� 
*autor��xz
*DATE��20180628
*/
//ACC ����
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
*@brief: ͣ��ģʽ�µȴ�CAN�жϽ��� 
*autor��xz
*DATE��20180628
*/
//CAN ����
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

//�ⲿ�ж����ú���
//ֻ���GPIOA~G;������PVD,RTC��USB����������
//����:
//GPIOx:0~6,����GPIOA~G
//BITx:��Ҫʹ�ܵ�λ;
//TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
//�ú���һ��ֻ������1��IO��,���IO��,���ε���
//�ú������Զ�������Ӧ�ж�,�Լ�������           
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	AFIO_TypeDef *aio = AFIO;  //��������IO��
	RCC_TypeDef  *rcc = RCC;
	EXTI_TypeDef *exti = EXTI;
    u8 EXTADDR;
    u8 EXTOFFSET;
    EXTADDR=BITx/4;//�õ��жϼĴ�����ı��
    EXTOFFSET=(BITx%4)*4; 
    rcc->APB2ENR|=__BIT(0) +__BIT(GPIOx+2);//ʹ��io����ʱ�� 
    IO_INIT_I_PU(GPIOA, BITx);//ʹ��PA.X
    aio->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//���ԭ�����ã�����
    aio->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITxӳ�䵽GPIOx.BITx 
    //�Զ�����
    exti->IMR|=1<<BITx;//  ����line BITx�ϵ��ж�
    //EXTI->EMR|=1<<BITx;//������line BITx�ϵ��¼� (������������,��Ӳ�����ǿ��Ե�,��������������ʱ���޷������ж�!)
    if(TRIM&0x01)
		exti->FTSR|=1<<BITx;//line BITx���¼��½��ش���
    if(TRIM&0x02)
		exti->RTSR|=1<<BITx;//line BITx���¼��������ش���
}

//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����             
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)     
{    
	SCB_Type *scb =SCB;         
    scb->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//����NVIC��������ƫ�ƼĴ���
    //���ڱ�ʶ����������CODE��������RAM��
}

//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5��            
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)     
{ 
	
	SCB_Type *scb =SCB; 
	u32 temp,temp1;      
    temp1=(~NVIC_Group)&0x07;//ȡ����λ
    temp1<<=8;
    temp=scb->AIRCR;  //��ȡ��ǰ������
    temp&=0X0000F8FF; //�����ǰ����
    temp|=0X05FA0000; //д��Կ��
    temp|=temp1;       
    scb->AIRCR=temp;  //���÷���                                 
}
//����NVIC 
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����       
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)     
{ 
	NVIC_Type *nvic = NVIC;
	u32 temp;    
    MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
    temp=NVIC_PreemptionPriority<<(4-NVIC_Group);      
    temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
    temp&=0xf;//ȡ����λ  
    nvic->ISER[NVIC_Channel/32]|=(1<<NVIC_Channel%32);//ʹ���ж�λ(Ҫ����Ļ�,�෴������OK) 
    nvic->IP[NVIC_Channel]|=temp<<4;//������Ӧ���ȼ����������ȼ�                                    
}



//�ⲿ�жϳ�ʼ������
//��ʼ��PA8Ϊ�ж�����.
void ACC_EXTI_Init(void)
{
    Ex_NVIC_Config(0,8,1);       //ACC_IN�½��ش���  ����
   
    MY_NVIC_Init(2,0,EXTI9_5_IRQn,2);//��ռ2�������ȼ�1����2 
}


//�ⲿ�жϳ�ʼ������
//��ʼ��PA11Ϊ�ж�����.
void CAN_RX_EXTI_Init(void)
{
    Ex_NVIC_Config(0,11,1);       //CAN_RX�½��ش���  ����
   
    MY_NVIC_Init(2,1,EXTI15_10_IRQn,2);//��ռ2�������ȼ�1����2 
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
	// avoid malfunction  IMR�ж����μĴ��� д1��Ч 
	exti->IMR &= ~__BIT(3);
	// input pullup/pulldown
	IO_INIT_I_PU(GPIOA, 8);
	// ILL & PARK DET have no external Pullup, so use internal pullup
	aio->EXTICR[8>>2] |= 0x00;//����ԭ������
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
//	Det_line_msg_pre_init();//����ͷ��

	IO_INIT_I_PU(GPIOA, 8); // ACC in

	IO_INIT_O_PP(GPIOA, 3, HIGH); //

	IO_INIT_O_PP(GPIOB, 0, LOW); //PMIC ��λIMAX6
	
	IO_INIT_O_PP(GPIOB, 8, LOW); //

	//����ͷ��Դʹ�ܽ�
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




