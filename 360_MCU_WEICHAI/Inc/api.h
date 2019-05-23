
#ifndef __API_H__
#define __API_H__
#include "miscdef.h"
static u32 __INLINE get_flag(device_info_t *dev, u32 flag) // return 0 OR 1
{
	u32 tmp;

	tmp = dev->bitbang_addr[flag];

	if(tmp)
	{
		dev->bitbang_addr[flag] = 0;
	}

	return tmp;
}

static u32 __INLINE chk_flag(device_info_t *dev, u32 flag)
{
	return dev->bitbang_addr[flag];
}

static void __INLINE set_flag(device_info_t *dev, u32 flag)
{
	dev->bitbang_addr[flag] = 1;
}

static void __INLINE clear_flag(device_info_t *dev, u32 flag)
{
	dev->bitbang_addr[flag] = 0;
}

device_info_t *get_device_info(void);


void device_init(device_info_t *dev, u32 vect_offset);
void device_init_boot(device_info_t *dev, u32 vect_offset);
void device_deinit(device_info_t *dev);
void device_reinit(device_info_t *dev);

void upgrading_process(device_info_t *dev, message_t *dmsg);

state_func_t *get_stage_func_array(void);


void app_code_invalidate(device_info_t * dev, u8 *mt, u8 *uuid);
bool app_code_verify(device_info_t * dev);
bool app_code_mark(device_info_t *dev, u8 *mt, u8 *uuid);

void Enter_stopmode(void);
void ACC_EXTI_Init(void);
void CAN_RX_EXTI_Init(void);

void peripheral_power_ctrl(u32 on);
void boot_gpio_init(device_info_t *dev);
void app_gpio_init(device_info_t * dev);


void watchdog_start(device_info_t * dev);
void watchdog_feed(device_info_t * dev);
u32 get_ipc_base(void);
u32 get_boot_base(void);
u32 get_app_base(void);
u32 get_boot_version(void);
u32 get_app_version(void);
void get_mach_type(u8 *buf);
void get_uuid(u8 *buf);
void update_uuid(u8 *uuid);
void mdelay(u32 ms);
void udelay(u32 us);
void software_reset(void);
void clock_switch(device_info_t *dev, bool high_speed);
void process_switch(u32 entry, u32 stack);
void dump_post_message(device_info_t *dev);
void boot_debug_mode(device_info_t *dev);
void Iwdg_Feed(void);
void Iwdg_Init(u8 pre,u16 rlr);//1,4096  819ms

void device_low_power_enter(void);
void device_low_power_leave(void);
void confirm_upgrading(device_info_t * dev,u8 type,u8 dir);
//void upgrading_process(device_info_t *dev, message_t *dmsg);


//void sys_halt(device_info_t *dev, u32 ms);

//const char *get_car_type_string(void);

u32 systick_debug(device_info_t * dev);

void endian_exchg16(union16_t * un);
void endian_exchg32(union32_t * un);
void endian_exchg64(union64_t * un);
void power_on_gpio_ctrl(void);
void item_test_init(void);
u32 item_test_operation(device_info_t *dev, message_t *msg);
void ipc_init(device_info_t * dev);

void Iwdg_Feed(void);
void Iwdg_Init(u8 pre,u16 rlr);//1,4096  819ms
//adc
void Batt_adc_message_process(device_info_t * dev);
void batt_adc_init(device_info_t *dev);
void batt_adc_deinit(device_info_t *dev);
void internal_disableall(device_info_t *dev);
void internal_ableall(device_info_t *dev);
void  get_mcu_ver(device_info_t *dev);
 void Iwdg_Init(u8 pre,u16 rlr);
 void Iwdg_Feed(void);
 void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);
 void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);
 void stop_mode_proc(device_info_t *dev);
#endif/*end of __API_H__ */

