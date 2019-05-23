
#include <usart.h>
#include <api.h>
#include <protocolapi.h>
#include <can_dtc.h>
#include <can.h>
/////////////////////// Board specific ////////////////////////
#if ENABLE_KEY_ADC
static void adc_pin_init(u32 which)
{
	switch(which)
	{
		case ADC_DEV0:
			
			IO_INIT_I_ANA(GPIOA, 4);
			break;

		default:
			return;
	}
}

#endif

// Channel(s) & buffer used


/////////////////////// End of board specific ////////////////////////


#if ENABLE_KEY_ADC
static void adc_init(device_info_t *dev, u32 which)
{
	ADC_TypeDef *adc;
	RCC_TypeDef *rcc = RCC;

	// config AINx related GPIO
	adc_pin_init(which);

	switch(which)
	{
		case ADC_DEV0:

			adc = ADC1;

			// enable ADC1 peripheral
			rcc->APB2ENR |= __BIT(9);

			// power down ADC1 first
			adc->CR1 = 0;
			adc->CR2 = 0;

			udelay(1);

			// reset the peripheral ADC1
			rcc->APB2RSTR |= __BIT(9);
			rcc->APB2RSTR &= ~__BIT(9);


			dbg_msg(dev, "ADC DEV0 initializing...\r\n");

			break;

		// support ADC1 only
		default:

			return;
	}

	adc->SQR1 = (0 << 20);

	adc->SMPR2 = (0x07u << 12) ;

	//while(!adc->SR);
	adc->SR = 0;

	// set external tirgger source
	adc->CR2 |= __BIT(20); // ext trig enable
	adc->CR2 |= (0x07u << 17); // select SWSTART mode

	// select right alignment
	adc->CR2 &= ~__BIT(11);

	adc->CR2|=1<<0;	   
	adc->CR2|=1<<3;       
	while(adc->CR2&1<<3); 			 
	adc->CR2|=1<<2;        
	while(adc->CR2&1<<2);  

}

 static void adc_deinit(device_info_t *dev, u32 which)
{
	ADC_TypeDef *adc;
	RCC_TypeDef *rcc = RCC;


	switch(which)
	{
		case ADC_DEV0:

			adc = ADC1;

			adc->CR2 = 0; // power down ADC
			// disable ADC1 peripheral
			rcc->APB2ENR &= ~__BIT(9);
			



			break;

		default:
			return;
	}
}

static void adc_start(device_info_t *dev, u32 which)
{
	ADC_TypeDef *adc;

	switch(which)
	{
		case ADC_DEV0:

			adc = ADC1;

			// set bit ADON
			adc->CR2 |= __BIT(0);
			
			udelay(1);
			
			// initial calibration registers
			adc->CR2 |= __BIT(3);
			while(__BIT(3) & adc->CR2);
			
			// start calibration
			adc->CR2 |= __BIT(2);
			while(__BIT(2) & adc->CR2);

			// start ADC conv by ADON
			adc->CR2 |= __BIT(0);

			break;

		default:
			return;
	}
}


/*
void ?Adc_Init(void)
{ ? ?
? //先初始化IO口
RCC->APB2ENR|=1<<2; ? ?//使能PORTA口时钟?
GPIOA->CRL&=0XFFF0FFFF;//PA4 anolog输入?


RCC->APB2ENR|=1<<9; ? ?//ADC1时钟使能?
RCC->APB2RSTR|=1<<9; ? //ADC1复位
RCC->APB2RSTR&=~(1<<9);//复位结束

? ?
RCC->CFGR&=~(3<<14); ? //分频因子清零
//SYSCLK/DIV2=12M ADC时钟设置为12M,ADC最大时钟不能超过14M!
//否则将导致ADC准确度下降!?
RCC->CFGR|=2<<14; ? ? ??//这里预分频设置的是6

ADC1->CR1&=0XF0FFFF; ? //工作模式清零
ADC1->CR1|=0<<16; ? ? ?//独立工作模式 ?
ADC1->CR1&=~(1<<8); ? ?//非扫描模式?
ADC1->CR2&=~(1<<1); ? ?//单次转换模式
ADC1->CR2&=~(7<<17);??
ADC1->CR2|=7<<17;? //规则通道组转换选用SWSTART软件控制转换 ?
ADC1->CR2|=1<<20; ? ? ?//使用用外部触发(SWSTART)!!!必须使用一个事件来触发
ADC1->CR2&=~(1<<11); ? //右对齐?
ADC1->SQR1&=~(0XF<<20);//清零（这里需要定义规则序列需要转换的通道数）
ADC1->SQR1&=0<<20; ? ? ? ? //1个转换在规则序列中 也就是只转换规则序列1??
//设置通道4的采样时间
ADC1->SMPR2&=0XFFF0FFFF; //采样时间清空?
ADC1->SMPR2|=7<<12; ? ? ?// 239.5周期,提高采样时间可以提高精确度?


ADC1->CR2|=1<<0;? ?//开启AD转换器 ?
ADC1->CR2|=1<<3; ? ? ? ?//使能复位校准 ?
while(ADC1->CR2&1<<3); ?//等待校准结束 ?
? ? //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 ?
ADC1->CR2|=1<<2; ? ? ? ?//开启AD校准??
while(ADC1->CR2&1<<2); ?//等待校准结束?
} 
*/


u16 Get_Adc(u8 ch)   
{	
	ADC_TypeDef *adc;
	adc = ADC1;
	adc->DR&=0x00;/*需清零 上次读的值 解决首值采样不正确bug*/
	adc->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	adc->SQR3|=ch;		  			    
	adc->CR2|=1<<22;   //启动规则转换通道     
	while(!(adc->SR&1<<1));//等待转换结束 
	return adc->DR;		
}
#endif

void batt_adc_init(device_info_t *dev)
{
#if ENABLE_KEY_ADC

//	dev->adc_t.adc_times =0;
	dev->adc_t.hight_keepCount=0;
	dev->adc_t.low_keepCount=0;

	adc_init(dev, ADC_DEV0);

	adc_start(dev, ADC_DEV0);

	get_flag(dev, FLAG_TIMEOUT_ENCADC);

#endif
}
 
void batt_adc_deinit(device_info_t *dev)
{
#if ENABLE_KEY_ADC
	adc_deinit(dev, ADC_DEV0);
#endif
}

void Batt_adc_message_process(device_info_t * dev)
{
#if ENABLE_KEY_ADC
	float mainPwrVol=0;
	dev =get_device_info();	

	dev->adc_t.main_pwr_volt = Get_Adc(4)*3.3*10.76/4095;//转换成实际电压值
	mainPwrVol = dev->adc_t.main_pwr_volt*1.0+1.2;//矫正  
	mainPwrVol = 10*mainPwrVol;
		//dbg_msgv(dev, "batt = %6.1f \r\n",mainPwrVol);
	if(mainPwrVol<(MAIN_PWR_TOO_LOW_VALUE-5))
	{//低于6.5V  can sleep
			//dev->ci->can_no_tx=0x01;
		//	can_deinit_simple(dev);
		car_can_sleep(dev);
		mdelay(1);
		}
	else 
	{
		car_can_wake_init(dev);
		}
	check_main_volt_abnormal_pwr(dev,mainPwrVol);
#endif
}
