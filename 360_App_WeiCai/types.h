#ifndef __types_H__
#define __types_H__

typedef unsigned int   uint;
typedef unsigned char  uchar;
typedef unsigned short u16;

#ifndef BOOL
typedef int BOOL;
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


//#define LIUH_DBG
#ifdef LIUH_DBG
    #define DBG(fmt, args...)  printf("Dbg: " fmt, ## args)
#else
    #define DBG(fmt, args...) /* empty debug slot */
#endif



typedef enum cKeyType{
	TNot = 0,
	TEnter =1,
    TBack =2,
    TFront =3,
    TRear =4,
    TLeft = 5,
    TRight = 6,   
}KeyType;


typedef  enum {
    T_START,
    T_CMD,
    T_HID,
    T_LEN,
    T_DATA,
    T_LRC,
}RecType;
	


typedef enum cCarColour{
	TRed,
    Tblue,
    TGreen,
}CarColour;

typedef struct cRecCarInfo{
    /* float CarSpeed; */
    unsigned short CarSpeed;
    unsigned short CarSpeed_flag;   //当车速超过20Km/h时，此标志位为1
    short CarSteeringWheel;
    unsigned int CarStat;
    unsigned char Light;//0正常1 左 2右
    unsigned char IsR;	// 0 正常 1倒车
    unsigned char alarm; //0正常 1双闪
    unsigned char AvmEnableByLamp;  //用于开启转向灯时是否需要显示360画面   1：使能    0：关闭
}RecCarInfo;


typedef struct cAvmParaSetInfo{
	unsigned char allSwitch;//0:open  1:close
	CarColour carColour;
	unsigned char carSubLineStat;
	unsigned char AdasStat;
	unsigned char AdasSpeed;
}AvmParaSetInfo;

typedef struct cAvmCalibrationPara
{
    unsigned char caliFlag;     //标定的标志位  
    unsigned char caliResult;   //标定结果：0 标定成功  33 前标定失败   34 后标定失败   35 左标定失败   36 右标定失败
}AvmCalibrationPara;

typedef enum
{
    CALI_ERR_0  = 0,       /*成功：拼接完成*/
    CALI_ERR_1  = 1,       /*错误：输入参数错误*/
    CALI_ERR_2  = 2,       /*错误：打开原始文件失败*/
    CALI_ERR_17 = 17,     /*错误：前角点提取失败*/
    CALI_ERR_18 = 18,     /*错误：后角点提取失败*/
    CALI_ERR_19 = 19,     /*错误：左角点提取失败*/
    CALI_ERR_20 = 20,     /*错误：右角点提取失败*/
    CALI_ERR_33 = 33,     /*错误：前标定失败*/
    CALI_ERR_34 = 34,     /*错误：后标定失败*/
    CALI_ERR_35 = 35,     /*错误：左标定失败*/
    CALI_ERR_36 = 36,     /*错误：右标定失败*/
    CALI_ERR_49 = 49,     /*错误：前图像不完整*/
    CALI_ERR_50 = 50,     /*错误：后图像不完整*/
    CALI_ERR_51 = 51,     /*错误：左图像不完整*/
    CALI_ERR_52 = 52,     /*错误：右图像不完整*/
    CALI_ERR_65 = 65,     /*错误：左前交界图像不完整*/
    CALI_ERR_66 = 66,     /*错误：右前交界图像不完整*/
    CALI_ERR_67 = 67,     /*错误：左后交界图像不完整*/
    CALI_ERR_68 = 68,     /*错误：右后交界图像不完整*/
}CaliErro;

typedef enum
{
    CAMERA_CALI_NONE    = 0,             /*正常*/
    CAMERA_CALI_F       = 0x00000001,    /*前摄像头标定故障*/
    CAMERA_CALI_B       = 0x00000002,    /*后摄像头标定故障*/
    CAMERA_CALI_L       = 0x00000004,    /*左摄像头标定故障*/
    CAMERA_CALI_R       = 0x00000008,    /*右摄像头标定故障*/
}CameraCali;




typedef struct cAvmCycleSendInfo{
	unsigned char TriggerDvd;           /*0:close avm output 1:open  */
	unsigned char AvmFault;             /*0: no fault 1:fault*/
	unsigned char AvmWorkstat;          /*0：No Request Display
										  1：AVM + Front View
										  2：AVM + Rear View
										  3:   AVM + Left View
										  4:   AVM + Right View
										  5:  Enlarged  Front View
										  6:  Enlarged  Reart View
										  7:  Enlarged  Left View
										  8:  Enlarged  Right View
										  9~15:Reserved*/
	unsigned char LdwSwitchStat;  		/*0: Switch OFF
									 	  1: Switch On */
	unsigned char LdwErrorStat;  		/*0：No Error
								   		  1：Error*/
	unsigned char AvmCaliStat; 			/*0=Not calibrate
										  1=Claibrating
										  2=Claibrate success
										  3=Calibrate failure*/
	unsigned char SupplyFlag; 			/*0：TTE       
										  1：INVO
										  2：default ->  SEG
										  3：Reserved*/	
	unsigned char LdwAudioWarning;		/*0：Inactive 
										  1：Active*/
	unsigned char LdwWorkStat;      	/*0:  wait
										  1:  Active
										  2:  Left Departure
										  3:  Right Departure
										  4~7:Reserved*/					
}AvmCycleSendInfo;




#endif
