
#ifndef __VERSION_H__
#define __VERSION_H__

#include <version.h>

// software version base define
#define __VERSION(ma, md, mi)	(u32)(((mi) << 16) | ((md) << 8) | (ma))


/***************************版本号内部记录********************************************/
/*
ver1.0.0   日期 ：201801116 
		 记录初始版本-潍柴试产定版 
ver1.0.1   日期 ：201801212 
		 说明：潍柴通信测试未通过整改版本。具体修改如下：
			1、更新USART通信转义逻辑，增强通讯可靠性
			2、修改CAN和USART中断优先级至USART中断优先
			3、更新busoff处理、noack处理
			4、更新电源9-16V正常工作，低于或高于关闭ARM，低于6.5V 时 停止报文发送。
			5、修改520报文由MCU定时发送
			6、更新dtc在正常情况下检测到arm通信不正常dtc
			7、工程文件定义为STM32F103CB，使用隐藏的64KB（扩展flash为128KB）
ver1.0.2   日期 ：20190102 
说明：潍柴送测试部测试及DVP实验软件版本
			1、隐藏取消，优化代码结构和细节，减少code-size
			2、更新上电或者acc唤醒时有几率启动arm不成功bug
			3、更新usart发送机制，更新发送为：等待dma传输完成才完成串口数据发送，增加通讯可靠性及实时性
			4、更新串口接受中断处理，允许接受数据覆盖
			5、增加独立看门狗			
ver1.0.3   日期 ：20190128 
说明：潍柴送车厂，第二轮通信测试整改，第一轮 DTC和诊断测试整改
			1、整改通信测试，修改NOACK和BUSOFF策略。同时更新上电PMIC导致ARM偶尔启动不了bug
			2、整改DTC测试，优化dtc记录策略和存储时间超长（影响周期发送报文520）--FLASH模拟EEPROM
			3、整改优化诊断测试open项
*/
// software version
#define APP_VER_BIN				__VERSION(3, 0, 1)
#define BOOT_VER_BIN			__VERSION(3, 0, 1)
#define MCU_BOOT_VER        "AVM-MCU-VER-0.1.0.3"   
#endif /* end of __VERSION_H__ */

