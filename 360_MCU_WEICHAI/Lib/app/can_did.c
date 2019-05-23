#define CAN_DID_GLOBALS
#include <stdio.h>
#include <string.h>
#include "iflash.h"
#include "can_did.h"
#include "usart.h"
//#include "usart.h"

const u8 ECU_SoftwareVersion[LEN_ECUSoftwareVersion]="SW:a.00.00"; 
const u8 ECU_HardwareVersion[LEN_ECUHardwareVersion]="HW:A.0.0";														
const u8 ECU_Part_Number[LEN_Part_Number]="12345678911";//Part Number 
const u8 ECU_System_Supplier[LEN_System_Supplier]="1234567";//System Supplier Identifier 
const u8 ECU_SerialNumber[LEN_SerialNumber]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};//ECUSerialNumberDataIdentifier ECU								
const u8 ECU_VINDataIdentifier[LEN_VINDataIdentifier]="10000abc10cd11000";//车辆VIN号
const u8 ECUConfiguration[LEN_ECUConfiguration]={0xFF,0xFF,0x0F,0,0};


void init_did_data(void)
{
	device_info_t *dev;
	u8 w_buffer[1024]={0};
	//读取did是否初始化标志位
	para_read_Nbyte( ADDR_DID_FLAG,w_buffer, 1);
	if(w_buffer[0]!=0x35)
	{		
	//	dbg_msg(dev,"test2\r\n");
		w_buffer[0]=0x35;
		memcpy(&w_buffer[2],&ECU_SoftwareVersion[0],LEN_ECUSoftwareVersion);
		memcpy(&w_buffer[2+10],&ECU_HardwareVersion[0],LEN_ECUHardwareVersion);
		memcpy(&w_buffer[2+10+8],&ECU_Part_Number[0],LEN_Part_Number);
		memcpy(&w_buffer[2+10+8+12],&ECU_System_Supplier[0],LEN_System_Supplier);
		memcpy(&w_buffer[2+30+7+1],&ECU_SerialNumber[0],LEN_SerialNumber);
		memcpy(&w_buffer[2+38+14],&ECU_VINDataIdentifier[0],LEN_VINDataIdentifier);
		memcpy(&w_buffer[2+38+14+18],&ECUConfiguration[0],LEN_ECUConfiguration);
		STMFLASH_Write(ADDR_DID_FLAG,(u16*)w_buffer,512);/*附带把 dtc 初始化 刷写一页*/
	}
	else
	{
		dbg_msg(dev,"DID has been written \r\n");
			}
}	

