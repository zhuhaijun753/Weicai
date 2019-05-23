
#include <stdio.h>
#include <string.h>
#include <can.h>
#include <usart.h>
#include <api.h>
#include <can_net_layer.h>
#include <timer.h>
#include <version.h>
static device_info_t device;
int main(void)
{	
	message_t msg, *dmsg;
	state_func_t *state_func;
	device_info_t *dev = &device;
	// vector base, uarts, gpio...
	device_init(dev, get_boot_base());
	dbg_msgv(dev, "%s\r\n",MCU_BOOT_VER); 
		
	if(	dev->ipc->app_jump)
	{
		
		dev->ipc->app_jump = 0;
		dev->ci->host_ready =0;//������ʱ��ǿ����1
		dev->ci->state =HOST_RUNNING;
	}
	else
	{
		if(FALSE == app_code_verify(dev))//APP_FLAG��־λ����
		{
			dbg_msg(dev, "\r\nAPP_FLAG is wrong\r\n");
			dev->ci->state = HOST_RUNNING;//����			

		} 
		else 	
		{
			boot_debug_mode(dev);
			goto exit0; // normal boot up
		}
	}

	dmsg = dbg_msg_alloc(dev);
	
	state_func = get_stage_func_array();

	systick_start(dev);//ע��boot����� ι����ÿ50msι��һ�� 50ms��ʱ������systick �ṩ
	
	while(1)
	{
		(state_func[dev->ci->state])(dev, &msg, dmsg);
	}

exit0:
	dbg_msgv(dev, "\r\nStarting App from %x, stack top %x...\r\n", dev->ipc->app_entry, dev->ipc->app_stack);

	systick_stop();

	device_deinit(dev);

	process_switch(dev->ipc->app_entry, dev->ipc->app_stack);

	while(1);
}

device_info_t *get_device_info(void)
{
	return &device;
}

