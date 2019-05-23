
#include <can_diag_protocol.h>
#include <usart.h>
#include <api.h>
#include <protocolapi.h>
#include <can.h>
#include "can_did.h"
#include "iflash.h"
#include "timer.h"
#include "can_net_layer.h"
void  get_mcu_ver(device_info_t *dev)
{	
	u8  send_did_data[10]={0};
	para_read_Nbyte(ADDR_SoftwareVersion,&send_did_data[0],10);	
	dbg_msgv(dev," ---AVM Ver %s--- \r\n",send_did_data);
}

static ctrl_info_t cinfo =
{
	0, // host state, will change
	0, // host not ready at default
	0, // revers state, will change
	0, // upgrade type, will set before call
	0, // canwake_init
	0, // upgrade type, will set before call
	0, // canwake_init
	0,
	0,
	0,
	0,
	0,
	0,
	0,//write_flash_time_flag
	0,
	0,
	0,
};

void ipc_init(device_info_t * dev)
{
	dev->ipc = (ipc_info_t *)get_ipc_base();//����� SRAM boot��APP��ת ֵ��Ч������Ҫ�ֿ���ʼ��
	dev->ipc->in_boot = 0;
	dev->ipc->heart_count = 0;
	dev->ipc->can_wait_count= 0;
	dev->ipc->dtc_camera_state=0;
	dev->ipc->dtc_arm_com_sate=0;
	dev->ipc->work_state=0;
	dev->ipc->avm_disviewsts=0;
	dev->ipc->usart_normal=0;
	dev->ipc->miss_avm1=1;
	dev->ipc->miss_avm2=1;
	dev->ipc->miss_avm3=1;
	dev->ipc->miss_avm4=1;
	}

static void flag_init(device_info_t * dev)
{
	dev->diag_t.cf_time_flag= 0;
	dev->diag_t.cf_last_time= 0;
	dev->diag_t.cf_curret_time= 0;
	dev->did_t.flag= 0;
}

static void judge_acc_sts(device_info_t *dev)
{
	#if ENABLE_ACC_DET  //������B+��ʱ�� ACC״̬���벻ͬ����state
	if(IO_GET(GPIOA, 8)!=0)
	{
		set_flag(dev, FLAG_ACC_LINE_OFF);
	}
	
	dev->ci->state = HOST_BOOTING;
	start_timer(TIMER_DEV1,1000);
	#endif
	}

void device_init(device_info_t *dev, u32 vect_offset)
{
	u32 addr, i;

	SCB->VTOR = vect_offset;

	ipc_init(dev);

	flag_init(dev);
	
	dev->ci = &cinfo;
	
	for(i = 0; i <(u32 )ARRAY_SIZE(dev->flags); i++)
	{
		dev->flags[i] = 0;
	}

	addr = (u32)dev->flags;
	
	dev->bitbang_addr = (u32 *)BITBANG_ADDR(addr);

	stimer_init(dev);

	__disable_irq();
	
	clock_switch(dev, TRUE); // ��ת�������³�ʼ��
	
	//	__watchdog_feed();
	Iwdg_Feed();
	
	app_gpio_init(dev); // һ��Ҫ���ڻ��������ʼ�����棬�ȴ������ȶ���������GPIOB0 ��λPMIC

	usart_device_init(dev);
	
	__enable_irq();
	
	dbg_uart_init(dev);

	comm_uart_init(dev);
		
	acc_line_gpio_init(dev);

	can_line_gpio_init(dev);
	
	car_can_wake_init(dev);

	car_can_init(dev);

	ACC_EXTI_Init();

	batt_adc_init(dev);

	init_net_layer_parameer();

	init_diag_protocol_parameer();//???????
	judge_acc_sts(dev);

}

void confirm_upgrading(device_info_t *dev, u8 type, u8 dir)
{
	u8 buf[2];

	buf[0] = type;
	buf[1] = dir;

	comm_message_send(dev, CM_ARM_UPGRADE, 0, buf, 2);

	usart_on_dma_end(dev->comm_uart); // wait for the first ARM11 sync ACK & upgrade cmdsend out
}

void device_deinit(device_info_t *dev)
{
	comm_uart_deinit(dev);

	dbg_uart_deinit(dev);
		
	batt_adc_deinit(dev);

	internal_disableall(dev);

	dev->ci->host_ready=0;
}
void device_reinit(device_info_t *dev)
{
		u32 addr, i;
	
		dev->ipc = (ipc_info_t *)get_ipc_base();
		dev->ipc->in_boot = 0;
		dev->ipc->heart_count = 0;
		dev->ipc->can_wait_count=0;
			
		dev->ci = &cinfo;

		for(i = 0; i <(u32 )ARRAY_SIZE(dev->flags); i++)
		{
			dev->flags[i] = 0;
		}

		addr = (u32)dev->flags;
	
		dev->bitbang_addr = (u32 *)BITBANG_ADDR(addr);

		clock_switch(dev,TRUE);
		//dev->ipc->app_version = APP_VER_BIN;
		app_gpio_init(dev); // for boot.bin reserve                	                 
}

