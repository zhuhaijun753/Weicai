
#include <stdio.h>
#include <string.h>
#include <Version.h>
#include <miscdef.h>
#include <usart.h>
#include <api.h>
#include <iflash.h>
#include "timer.h"
#include "diag_upgrade.h"
static device_info_t device;
u32  GetLockCode(void)
{
	u32 CpuID[3];
	CpuID[0]=*(vu32*)(0x1ffff7e8);//小到大端
	CpuID[1]=*(vu32*)(0x1ffff7ec);
	CpuID[2]=*(vu32*)(0x1ffff7f0);
	return (CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
}


int main(void)
{ 
	message_t msg, *dmsg;
	state_func_t *state_func;/*定义指向函数指针的指针*/
	device_info_t *dev = &device;
	
	// vector base, uarts, gpio...
	device_init(dev, get_app_base());

	if(dev->ipc->upgrade_mcu_boot)
	{		
	//	dbg_msg(dev,"hererrrrr\r\n");
		upgrade_mcu_boot_init(dev);

		}

	dbg_msg(dev, "\r\n------------------------------------------------------\r\n");

	dbg_msg(dev, "       Welcome to MCU Command Line Interface \r\n");

//	dbg_msgv(dev, "     %s \r\n",MCU_VER);
	

//	dbg_msgv(dev, " ---MCU_BOOT_Ver %d.%d.%d, ", dev->ipc->boot_version >> 16,
//		(dev->ipc->boot_version >> 8) & 0xff, dev->ipc->boot_version & 0xff);

		get_mcu_ver(dev);
//	dbg_msgv(dev, "APP %d.%d.%d,\r\n", get_app_version() >> 16,
//		(get_app_version() >> 8) & 0xff, get_app_version() & 0xff);

	//dbg_msgv(dev, "RO DATA @ 0x%08x\r\n", dev->ipc->rodata_base);

	dbg_msg(dev, "------------------------------------------------------\r\n");
	//dbg_msgv(dev, "LockCode=%x\r\n",GetLockCode());

	dbg_prompt_msg(dev);

	dmsg = dbg_msg_alloc(dev);

	state_func = get_stage_func_array();/*初始化指向函数指针数组的指针等于预先定义的函数指针数组sfunc[]*/

	systick_start(dev);

	while(1)
	{
		(state_func[dev->ci->state])(dev, &msg, dmsg);
	}
}

device_info_t *get_device_info(void)
{
	return &device;
}

