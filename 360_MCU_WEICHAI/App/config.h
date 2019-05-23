
#ifndef __CONFIG_H__
#define __CONFIG_H__

///////////// HW config, fixed /////////////
#include <hw_config.h>


///////////// SW config ////////////////

// USART config
#define ENABLE_DEBUG_UART			CFG_DEBUG_UART
#define ENABLE_COMM_UART			1

// command line support(for debug)
#define ENABLE_CMD_LINE			1

// i2c bus support
//#define ENABLE_I2C_BUS				0
// LED config
//#define ENABLE_LED_CTRL			0
// ADC config, if 0 don't care ENABLE_REG_CHN
//#define ENABLE_REG_CHN				0 /*1: regular channel, 0: injection channel*/
//#define ENABLE_REBOOT_KEY			0


//0:  for ymodem debug(no iflash program)
#define ENABLE_IFLASH_UPDATE		1

// ymodem upgrading msg ctrl
#define ENABLE_UPGRADE_MSG		1

#define ENABLE_ACC_DET				1
#define ENABLE_KEY_ADC				1

#define ENABLE_CAN_WEKAUP			1//切换ACC 唤醒 和 兼容 can 唤醒 

#define ENABLE_WEICHAI				1
#define ENABLE_WULING               0 //五菱演示转向灯检测
#define ENABLE_HAIMA                0 //海马演示 
#define ENABLE_JILI                 0 //吉利演示 
#define ENABLE_JILI2                0 //吉利演示 
#define ENABLE_WULING_TWO           0  // ENABLE_WULING 必须使能 第二版协议
#define ENABLE_DIANDONGCHE          0
#define ENABLE_ZHIDOU              	0

#define ENABLE_autotest             1

#define EXTEND_ID                   0
#define ENABLE_TEST                 0   //不测试需关闭
#define ENABLE_UPDATE_ARM_BY_CAN    1
// Other GPIO support(ILL/REVS/PARK/IPOD/BEEP)
//#define ENABLE_TBOX_WAKE			0 /* SLEEP stage TBOX GPIO wake, same as CAN-INH */


//i.mx6 debug 
#define IMX6_NOT_RESET             0


// sound effect chip config
//#define ENABLE_SND_EFFECT			0

// CAN bus support
// stm32 CAN2 is slave(share filter), CAN1 must be enabled for CAN2
#define ENABLE_CAR_CAN				1

/////////////// post process, don't modify///////////////


#if (0 == ENABLE_DEBUG_UART)
#undef ENABLE_CMD_LINE
#define ENABLE_CMD_LINE			0
#undef ENABLE_UPGRADE_MSG
#define ENABLE_UPGRADE_MSG		0
#define wait_for_interrupt()			__WFI()
#else
#define wait_for_interrupt()
#endif

//#define ENABLE_UART_LP				(ENABLE_DEBUG_UART )

//#define ENABLE_CAN_LP				( ENABLE_CAR_CAN)



#endif

