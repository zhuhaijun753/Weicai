#ifndef __CAN_DID_H__
#define __CAN_DID_H__
#ifdef CAN_DID_GLOBALS
#define CAN_DID_EXT 
#else
#define CAN_DID_EXT extern
#endif

#include "stm32f10x.h"
//潍柴  added 20180723 xz
#define LEN_DID_FLAG   			 1
#define LEN_ECUSoftwareVersion   10
#define LEN_ECUHardwareVersion   8
#define LEN_Part_Number          11
#define	LEN_System_Supplier      7
#define LEN_SerialNumber     	 13
#define LEN_VINDataIdentifier    17
#define LEN_ECUConfiguration     5
#define LEN_Fingerprint          7


#define  DID_ECUConfiguration							0xF1A1
#define  DID_ECUSoftwareVersion         				0XF189 
#define  DID_ECUHardwareVersion        					0XF089 
#define  DID_Part_Number         					 	0XF187 
#define  DID_System_Supplier        					0XF18A 
#define  DID_ECUSerialNumber        					0XF18C 
#define  DID_VINDataIdentifier         					0XF190
#define  DID_Fingerprint     						    0XF184

// 基准地址
#define DID_FlASH_ADDR_BASE    				(FLASH_BASE+0xFC00ul)//
//-------------------------- 参数查询,保持DID码地址为偶数起�?--------------------------------
#define ADDR_DID_FLAG					  		 (DID_FlASH_ADDR_BASE)
#define ADDR_SoftwareVersion          (DID_FlASH_ADDR_BASE+LEN_DID_FLAG+1)
#define ADDR_HardwareVersion           (ADDR_SoftwareVersion +LEN_ECUSoftwareVersion)
#define ADDR_Part_Number       			   (ADDR_HardwareVersion+LEN_ECUHardwareVersion)
#define ADDR_System_Supplier        	   (ADDR_Part_Number+LEN_Part_Number+1)
#define ADDR_SerialNumber       		   (ADDR_System_Supplier+LEN_System_Supplier+1)
#define ADDR_VINDataIdentifier    		   (ADDR_SerialNumber+LEN_SerialNumber+1)
#define ADDR_ECUConfiguration    		   (ADDR_VINDataIdentifier+LEN_VINDataIdentifier+1)
#define ADDR_Fingerprint    		       (ADDR_ECUConfiguration+LEN_ECUConfiguration+1)
CAN_DID_EXT void init_did_data(void);
#endif



