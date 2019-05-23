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
    unsigned short CarSpeed_flag;   //�����ٳ���20Km/hʱ���˱�־λΪ1
    short CarSteeringWheel;
    unsigned int CarStat;
    unsigned char Light;//0����1 �� 2��
    unsigned char IsR;	// 0 ���� 1����
    unsigned char alarm; //0���� 1˫��
    unsigned char AvmEnableByLamp;  //���ڿ���ת���ʱ�Ƿ���Ҫ��ʾ360����   1��ʹ��    0���ر�
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
    unsigned char caliFlag;     //�궨�ı�־λ  
    unsigned char caliResult;   //�궨�����0 �궨�ɹ�  33 ǰ�궨ʧ��   34 ��궨ʧ��   35 ��궨ʧ��   36 �ұ궨ʧ��
}AvmCalibrationPara;

typedef enum
{
    CALI_ERR_0  = 0,       /*�ɹ���ƴ�����*/
    CALI_ERR_1  = 1,       /*���������������*/
    CALI_ERR_2  = 2,       /*���󣺴�ԭʼ�ļ�ʧ��*/
    CALI_ERR_17 = 17,     /*����ǰ�ǵ���ȡʧ��*/
    CALI_ERR_18 = 18,     /*���󣺺�ǵ���ȡʧ��*/
    CALI_ERR_19 = 19,     /*������ǵ���ȡʧ��*/
    CALI_ERR_20 = 20,     /*�����ҽǵ���ȡʧ��*/
    CALI_ERR_33 = 33,     /*����ǰ�궨ʧ��*/
    CALI_ERR_34 = 34,     /*���󣺺�궨ʧ��*/
    CALI_ERR_35 = 35,     /*������궨ʧ��*/
    CALI_ERR_36 = 36,     /*�����ұ궨ʧ��*/
    CALI_ERR_49 = 49,     /*����ǰͼ������*/
    CALI_ERR_50 = 50,     /*���󣺺�ͼ������*/
    CALI_ERR_51 = 51,     /*������ͼ������*/
    CALI_ERR_52 = 52,     /*������ͼ������*/
    CALI_ERR_65 = 65,     /*������ǰ����ͼ������*/
    CALI_ERR_66 = 66,     /*������ǰ����ͼ������*/
    CALI_ERR_67 = 67,     /*������󽻽�ͼ������*/
    CALI_ERR_68 = 68,     /*�����Һ󽻽�ͼ������*/
}CaliErro;

typedef enum
{
    CAMERA_CALI_NONE    = 0,             /*����*/
    CAMERA_CALI_F       = 0x00000001,    /*ǰ����ͷ�궨����*/
    CAMERA_CALI_B       = 0x00000002,    /*������ͷ�궨����*/
    CAMERA_CALI_L       = 0x00000004,    /*������ͷ�궨����*/
    CAMERA_CALI_R       = 0x00000008,    /*������ͷ�궨����*/
}CameraCali;




typedef struct cAvmCycleSendInfo{
	unsigned char TriggerDvd;           /*0:close avm output 1:open  */
	unsigned char AvmFault;             /*0: no fault 1:fault*/
	unsigned char AvmWorkstat;          /*0��No Request Display
										  1��AVM + Front View
										  2��AVM + Rear View
										  3:   AVM + Left View
										  4:   AVM + Right View
										  5:  Enlarged  Front View
										  6:  Enlarged  Reart View
										  7:  Enlarged  Left View
										  8:  Enlarged  Right View
										  9~15:Reserved*/
	unsigned char LdwSwitchStat;  		/*0: Switch OFF
									 	  1: Switch On */
	unsigned char LdwErrorStat;  		/*0��No Error
								   		  1��Error*/
	unsigned char AvmCaliStat; 			/*0=Not calibrate
										  1=Claibrating
										  2=Claibrate success
										  3=Calibrate failure*/
	unsigned char SupplyFlag; 			/*0��TTE       
										  1��INVO
										  2��default ->  SEG
										  3��Reserved*/	
	unsigned char LdwAudioWarning;		/*0��Inactive 
										  1��Active*/
	unsigned char LdwWorkStat;      	/*0:  wait
										  1:  Active
										  2:  Left Departure
										  3:  Right Departure
										  4~7:Reserved*/					
}AvmCycleSendInfo;




#endif
