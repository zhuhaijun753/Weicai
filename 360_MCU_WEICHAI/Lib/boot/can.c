

#include <usart.h>
#include <api.h>
#include <can.h>
#include <protocolapi.h>
#include <can_diag_protocol.h>
#if  ENABLE_CAR_CAN//ʹ�ܳ���canͨѶ

/* TIR���������ʶ���Ĵ���*/
#define TMIDxR_TXRQ	 0x00000001 /*TIR��0λTERQ������1�������䷢��data��Ӳ����0 */

/*FMRΪCAN ���������ؼĴ���,���һλFINIT���ƹ������顣
				0: �������鹤��������ģʽ��
				1: �������鹤���ڳ�ʼ��ģʽ�� */
#define FMR_FINIT    0x00000001 /* Filter init mode */

/* CAN��״̬�Ĵ��� (CAN_MSR)��һλINAK ��ʼ��ȷ�� */
#define INAK_TIMEOUT				0x0000FFFF//��ʼ��ȷ��

/* Mailboxes definition  3���������� */
#define CAN_TXMAILBOX_0				0x00
#define CAN_TXMAILBOX_1				0x01
#define CAN_TXMAILBOX_2		     	0x02

// CAN mode definitions can����ģʽ
#define CAN_Mode_Normal			    0x00  /*!< normal mode */
#define CAN_Mode_LoopBack			0x01  /*!< loopback mode */
#define CAN_Mode_Silent				0x02  /*!< silent mode */
#define CAN_Mode_Silent_LoopBack	0x03  /*!< loopback combined with silent mode */

//CAN_SJW                 //1-4, Reg:0 ~ 3
//CAN_BS1                 //1-16, Reg:0 ~ 15
//CAN_BS2                 //1-8, Reg:0 ~ 7
//CAN_Prescaler         // 1-1024, Reg:0 ~ 1023
//������Ϊ 36/(Prescaler*(SJW+BS1+BS2))
// 36MHz is APB1 bus fixed freq, max 36MHz

//����������
#define __MODE_REG(m, sjw, bs1, bs2, pres)		(((m) << 30) | (((sjw) - 1) << 24) | (((bs1) - 1) << 16) | (((bs2) - 1) << 20) | ((pres) - 1))
#define CAN_FMT_NOR_500K			__MODE_REG(CAN_Mode_Normal, 1, 14, 3, 4) // 36M/(4 * 18)
#define CAN_FMT_NOR_250K			__MODE_REG(CAN_Mode_Normal, 1, 3, 2, 24) // 36M/(24 * 6) /* UNDER TEST */
#define CAN_FMT_NOR_200K			__MODE_REG(CAN_Mode_Normal, 4, 10, 6, 9) // 36M/(9 * 20) /* UNDER TEST */
#define CAN_FMT_NOR_125K			__MODE_REG(CAN_Mode_Normal, 1, 14, 3, 16) // 36M/(16 * 18)
#define CAN_FMT_NOR_100K			__MODE_REG(CAN_Mode_Normal, 1, 3, 2, 60) // 36M/(60 * 6)

// for low speed(LOW POWER) mode, 3MHz PCLK1 ��Ƶ
#define CAN_FMT_NOR_500K_LP		__MODE_REG(CAN_Mode_Normal, 1, 1, 1, 2) // 3M/(2 * 3)

// for test  �ػ�ģʽ��
#define CAN_FMT_LOP_500K			__MODE_REG(CAN_Mode_LoopBack, 1, 14, 3, 4) // 36M/(4 * 18)

/*    ��������ģʽ ��ʶ��������ģʽ       */
#define CAN_FilterMode_IdMask		0x00  /*!< identifier/mask mode */
#define CAN_FilterMode_IdList		0x01  /*!< identifier list mode */

/*�����������ģʽ����*/
#define CAN_FilterScale_16bit			0x00 /*!< Two 16-bit filters */
#define CAN_FilterScale_32bit			0x01 /*!< One 32-bit filter */

/*�������������������FIFO0��FIFO1*/
#define CAN_Filter_FIFO0				0x00  /*!< Filter FIFO 0 assignment for filter x */
#define CAN_Filter_FIFO1				0x01  /*!< Filter FIFO 1 assignment for filter x */

/* Receive FIFO number */
#define CAN_FIFO0					0
#define CAN_FIFO1					1

/* Receive Interrupts �����жϴ����¼� */
#define CAN_IT_FMP0					0x00000002 /*!< FIFO 0 message pending Interrupt*/
#define CAN_IT_FF0					0x00000004 /*!< FIFO 0 full Interrupt*/
#define CAN_IT_FOV0					0x00000008 /*!< FIFO 0 overrun Interrupt*/
#define CAN_IT_FMP1					0x00000010 /*!< FIFO 1 message pending Interrupt*/
#define CAN_IT_FF1					0x00000020 /*!< FIFO 1 full Interrupt*/
#define CAN_IT_FOV1					0x00000040 /*!< FIFO 1 overrun Interrupt*/

#define CAN_IT_ERRIE				0x00008000 /*!< �����ж�ʹ�� Error interrupt*/
#define CAN_IT_LECIE				0x00000800 /*!< �ϴδ�����ж�ʹ�� (Last error code interrupt enable)*/
#define CAN_IT_BOFIE				0x00000400 /*!< �����ж�ʹ�� (Bus-off interrupt enable)*/


/* Operating Mode Interrupts */
#define CAN_IT_WKU					0x00010000 /*!<�����ж�����Wake-up Interrupt*/
#define CAN_IT_SLK					0x00020000 /*!<˯�߱�־λ�ж����� Sleep acknowledge Interrupt*/

/* ���ͱ���״̬ */
#define CAN_TxStatus_Failed			0x00 /*!< CAN transmission failed */
#define CAN_TxStatus_Ok				0x01 /*!< CAN transmission succeeded */
#define CAN_TxStatus_Pending		0x02 /*!< CAN transmission pending */
#define CAN_TxStatus_NoMailBox		0x04 /*!< CAN cell did not provide an empty mailbox */

// IDEXT[17:15 -> 2:0] not included, Standard form
#define CAN_ID_TO_REG16(id)			((id) << 5) | (CAN_Id_Standard << 3) | (CAN_RTR_Data << 4)

// IDEXT[17:0 -> 20:3] not included, Standard id form
#define CAN_ID_TO_REG32H(id)			((((u32)(id) << 21) | (CAN_Id_Standard << 2) | (CAN_RTR_Data << 1))>> 16)
#define CAN_ID_TO_REG32L(id)			(u16)(((id) << 21) | (CAN_Id_Standard << 2) | (CAN_RTR_Data << 1))

//
#define CAN_EID_TO_REG32H(id)			((((u32)(id) << 3) | (CAN_Id_Extended ) | (CAN_RTR_Data << 1))>> 16)
#define CAN_EID_TO_REG32L(id)			(u16)(((id) << 3) | (CAN_Id_Extended ) | CAN_RTR_Data << 1)

//16λ���� ����ģʽ  fltn����������  fifn ���������� 
#define CAN_MASK_FILT16(fltn, fifn, id1, id2, msk1, msk2)				\
	{							\
		fltn,						\
		CAN_FilterMode_IdMask,	\
		CAN_FilterScale_16bit,	\
		CAN_ID_TO_REG16(id1),	\
		CAN_ID_TO_REG16(id2),	\
		CAN_ID_TO_REG16(msk1),	\
		CAN_ID_TO_REG16(msk2),	\
		fifn						\
	}

//16λ���� ��ʶ��ģʽ
#define CAN_LIST_FILT16(fltn, fifn, id1, id2, id3, id4)	\
	{							\
		fltn,						\
		CAN_FilterMode_IdList,	\
		CAN_FilterScale_16bit,	\
		CAN_ID_TO_REG16(id1),	\
		CAN_ID_TO_REG16(id2),	\
		CAN_ID_TO_REG16(id3),	\
		CAN_ID_TO_REG16(id4),	\
		fifn						\
	}
//32λ���� ����ģʽ 
#define CAN_MASK_FILT32(fltn, fifn, id1, msk1)		\
	{							\
		fltn,						\
		CAN_FilterMode_IdMask,	\
		CAN_FilterScale_32bit,	\
		CAN_ID_TO_REG32H(id1),	\
		CAN_ID_TO_REG32L(id1),	\
		CAN_ID_TO_REG32H(msk1),	\
		CAN_ID_TO_REG32L(msk1),	\
		fifn						\
	}

//326λ���� ��ʶ���б�ģʽ
#define CAN_LIST_FILT32(fltn, fifn, id1, id2)			\
	{							\
		fltn,						\
		CAN_FilterMode_IdList,	\
		CAN_FilterScale_32bit,	\
		CAN_ID_TO_REG32H(id1), 	\
		CAN_ID_TO_REG32L(id1),	\
		CAN_ID_TO_REG32H(id2),	\
		CAN_ID_TO_REG32L(id2), 	\
		fifn						\
	}

//��չID  32λ���� ��ʶ���б�
#define CAN_ELIST_FILT32(fltn, fifn, id1, id2)			\
	{							\
		fltn,						\
		CAN_FilterMode_IdList,	\
		CAN_FilterScale_32bit,	\
		CAN_EID_TO_REG32H(id1), 	\
		CAN_EID_TO_REG32L(id1),	\
		CAN_EID_TO_REG32H(id2),	\
		CAN_EID_TO_REG32L(id2), 	\
		fifn						\
	}
#endif


// PRE-DEFINE
#if (ENABLE_CAR_CAN)

#define CAN_RX_MSG_CNT				8
/*can��������ṹ��*/
typedef struct can_filt
{
	u32 filt_num;//���������
	u32 mode;//������ģʽCAN_FilterMode_IdMask or CAN_FilterMode_IdMask
	u32 scale;//��������������CAN_FilterScale_16bit or CAN_FilterScale_32bit
	u32 id_hi;//�б���ʶ��  ��
	u32 id_lo;//�б���ʶ��  ��
	u32 id_hi_mask;//���α�ʶ��  ��
	u32 id_lo_mask;//���α�ʶ��  ��
	u32 fifo_num;//�����
}can_filt_t;  //������config �ṹ��

/* can���ܷ���״̬�ṹ��*/
typedef struct can_rxtx
{
	u32 configured; // 0: not configured, 1: configured
	u32 wake_level;

	u32 w_index;
	u32 r_index;
	can_msg_t *msg;

	const u32 irq_enabled; // depand on hw
	IRQn_Type rx_irq0;//�����ж�����
	CAN_TypeDef *dev;
}can_rxtx_t;//can���ܷ��� ״̬�ṹ��
static can_rxtx_t can_info =
{
		0, // default: not configured
		LOW, // WAKE pin default state

		0,
		0,
		NULL,//can��Ϣ

		1, // interrupt disabled ��ʼ��
 		USB_LP_CAN1_RX0_IRQn,//�����ж���ں���
		CAN1  //can1 ʹ��
};//��ʼ��can  һЩ����

#endif

#if ( ENABLE_CAR_CAN)
//can1�����жϷ�����
static void can_recv_isr(u32 fifo_num)
{
	u32 fmi, index;//fmi ��������ƥ���  index  ����
	can_msg_t *msg;
	can_rxtx_t *can_rtx = &can_info;//��ʼ��can_info���ݸ���ָ�� *can_rtx
	CAN_TypeDef *can = can_rtx->dev;//dev  can1
	CAN_FIFOMailBox_TypeDef *fmb;//�����������ָ�루4���Ĵ��������ã�

	index = can_rtx->w_index;//0
	msg = &can_rtx->msg[index];//
	index++;//1
	index &= (CAN_RX_MSG_CNT - 1);//1
	can_rtx->w_index = index;//1
	fmb = &can->sFIFOMailBox[fifo_num];//����FIFO

	/* Get the Id ��RIR��ʶ���Ĵ���  */
	msg->id_ext = 0x04 & fmb->RIR;//����FIFO�����ʶ���Ĵ���, ��ȡ���ܱ��ı�ʶ��״̬����׼or��չ

	if(CAN_Id_Standard == msg->id_ext)//��׼ID
	{
		msg->id = fmb->RIR >> 21;//����21λ ȡ��11λ��׼ID
	}
	else
	{
		msg->id = fmb->RIR >> 3;//��չIDģʽ  ����3λ ȡ��29λ��չID
	}

	msg->rtr = 0x02 & fmb->RIR;//ȡ��RTRλ�� ����֡orԶ��֡

	/* Get the DLC ��RDTR ����FIFO�������ݳ��Ⱥ�ʱ����Ĵ���*/
	msg->dlc = 0x0F & fmb->RDTR;//RDTR 0~3λ   ����DLC

	/* Get the FMI ��RDTR ����FIFO�������ݳ��Ⱥ�ʱ����Ĵ���*/
	fmi = 0xFF & (fmb->RDTR >> 8);//RDTR 8~15λ   ����fmi������ƥ�����
	fmi = fmi; // avoid warning ����������

	/* Get the data field */
	msg->dat.w[0] = fmb->RDLR;//����FIFO�����λ���ݼĴ���  4�����ֽ�
	msg->dat.w[1] = fmb->RDHR;//����FIFO����͸����ݼĴ���  4�����ֽ�

	/* Release FIFOx �����־λ��
	  ͨ����CAN_RF0R�Ĵ���RFOM0��1 ���ͷŽ���FIFO����*/
	if(CAN_FIFO0 == fifo_num)
	{
		can->RF0R |= CAN_RF0R_RFOM0;
	}
	else
	{
		can->RF1R |= CAN_RF1R_RFOM1;
	}
}

/*can1 �����жϷ�����*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{	
//	device_info_t *dev=get_device_info();
//	dev->ci->can_rec_state =1;
	can_recv_isr(CAN_FIFO0);//ʹ��FIFO0Ϊ��������
}

 
/*
*@brief����ʼ��can1
*���룺fmt ���������ã�RTR�Ĵ�����  *filters ������    fmt  ������  
*/
static void can_init(device_info_t *dev,u32 fmt, const can_filt_t *filters, u32 filter_num, can_msg_t *msg_buf)
{
	u32 filt = 0;
	u32 ack = 0, fn;
	const can_filt_t *filtp;
	IRQn_Type rx0_irq;
	can_rxtx_t *can_rtx = &can_info;
	CAN_TypeDef *can = can_rtx->dev;

	rx0_irq = can_rtx->rx_irq0;

	can_rtx->msg = msg_buf;

	dbg_msgv(dev, "CAN%d: %d filter(s) configured\r\n", CAN_DEV0, filter_num);

	RCC->APB2ENR |= __BIT(0); // enable AFIO

	// PA11: CAN_RX, PA12: CAN_TX
	IO_INIT_I_PU(GPIOA, 11);
	IO_INIT_O_AF_PP(GPIOA, 12);

	RCC->APB1ENR |= __BIT(25); // eanble CAN2, Bit25=CAN1, Bit26=CAN2

	can_rtx->configured = 1;

	// Basic config

	// exit sleep mode
	can->MCR &= ~(uint32_t)CAN_MCR_SLEEP;

	// start initialization
	can->MCR |= CAN_MCR_INRQ;

	// wait hw ack
	while (((can->MSR & CAN_MSR_INAK) != CAN_MSR_INAK) && (ack != INAK_TIMEOUT))//�ȴ���ʼ������ȷ��
	{	
		Iwdg_Feed();
		ack++;
	}

	/* Set the time triggered communication mode */
//	can->MCR |= CAN_MCR_TTCM; // enable
	can->MCR &= ~(uint32_t)CAN_MCR_TTCM; // disable  the time triggered communication mode 

	/* Set the automatic bus-off management */
	can->MCR |= CAN_MCR_ABOM; // enable �Զ��˳�����(Bus-Off)����
//	can->MCR &= ~(uint32_t)CAN_MCR_ABOM; // disable

	/* Set the automatic wake-up mode */
//	can->MCR |= CAN_MCR_AWUM; // enable 
	can->MCR &= ~(uint32_t)CAN_MCR_AWUM; // disable�Զ�����ģʽ  ����������

	/* Set the no automatic retransmission */
//	can->MCR |= CAN_MCR_NART; // �����Զ��ش� 
	can->MCR &= ~(uint32_t)CAN_MCR_NART; // Cause TX-HALT after 3 mailbox full!!!!!!!!!!!!!!!!�����Զ��ش�ֱ֪�����ͳɹ�

	/* Set the receive FIFO locked mode */
	can->MCR &= ~(uint32_t)CAN_MCR_RFLM; // Disable �ڽ������ʱFIFOδ��������������FIFO�ı���δ����������һ���յ��ı��ĻḲ��ԭ�еı���

	/* Set the transmit FIFO priority */
	can->MCR &= ~(uint32_t)CAN_MCR_TXFP; // Disable���ȼ��ɱ��ĵı�ʶ��������

	/* Set the bit timing register  CANλʱ��Ĵ���  */
	can->BTR = fmt;

	/* Request leave initialisation �˳���ʼ��ģʽ */
	can->MCR &= ~(uint32_t)CAN_MCR_INRQ;

	/* Wait the acknowledge */
	ack = 0;

	while (((can->MSR & CAN_MSR_INAK) == CAN_MSR_INAK) && (ack != INAK_TIMEOUT))//�ȴ���ʼ���˳� ȷ��
	{
		Iwdg_Feed();
		ack++;
	}

	// Filter config, CAN1 & CAN2 share filter resouce

	/* Initialisation mode for the filter */
	if(filters)
	{
		CAN1->FMR |= FMR_FINIT;//CAN ���������ؼĴ��� 1Ϊ��ʼ��ģʽ

		while(filt < filter_num)//���������
		{
			filtp = &filters[filt];
			fn = filtp->filt_num;

			/* Filter Deactivation */
			CAN1->FA1R &= ~__BIT(fn);

			if(CAN_FilterScale_16bit == filtp->scale)
			{
				/* 16-bit scale for the filter */
				CAN1->FS1R &= ~__BIT(fn);

				 /* First 16-bit identifier and First 16-bit mask */
				 /* Or First 16-bit identifier and Second 16-bit identifier */
				 CAN1->sFilterRegister[fn].FR1 = ((0x0000FFFF & filtp->id_lo_mask) << 16) | (0x0000FFFF & filtp->id_lo);

				 /* Second 16-bit identifier and Second 16-bit mask */
				 /* Or Third 16-bit identifier and Fourth 16-bit identifier */
				 CAN1->sFilterRegister[fn].FR2 = ((0x0000FFFF & filtp->id_hi_mask) << 16) | (0x0000FFFF & filtp->id_hi);
			}
			else
			{
				/* 32-bit scale for the filter */
				CAN1->FS1R |= __BIT(fn);
				/* 32-bit identifier or First 32-bit identifier */
				dbg_msgv(dev, "CAN%d: %x filter(s) configured\r\n", CAN_DEV0,  ((0x0000FFFF & filtp->id_hi) << 16) | (0x0000FFFF & filtp->id_lo));
				CAN1->sFilterRegister[fn].FR1 = ((0x0000FFFF & filtp->id_hi) << 16) | (0x0000FFFF & filtp->id_lo);
				/* 32-bit mask or Second 32-bit identifier */
				CAN1->sFilterRegister[fn].FR2 = ((0x0000FFFF & filtp->id_hi_mask) << 16) | (0x0000FFFF & filtp->id_lo_mask);
			}

			/* Filter Mode */
			if(CAN_FilterMode_IdMask == filtp->mode)
			{
				/*Id/Mask mode for the filter*/
				CAN1->FM1R &= ~__BIT(fn);
			}
			else /* CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdList */
			{
				/*Identifier list mode for the filter*/
				CAN1->FM1R |= __BIT(fn);
			}

			/* Filter FIFO assignment  FIFO0������������*/
			if(CAN_Filter_FIFO0 == filtp->fifo_num)
			{
				/* FIFO 0 assignation for the filter */
				CAN1->FFA1R &= ~__BIT(fn);
			}
			else if(CAN_Filter_FIFO1 == filtp->fifo_num)
			{
				/* FIFO 1 assignation for the filter */
				CAN1->FFA1R |= __BIT(fn);
			}

			/* Filter activation */
			CAN1->FA1R |= __BIT(fn);

			filt++;
		}

		/* Leave the initialisation mode for the filter */
		CAN1->FMR &= ~FMR_FINIT;
	}

	can_rtx->r_index = 0;
	can_rtx->w_index = 0;

	if(!can_rtx->irq_enabled) // not support msg rx
	{
		return;
	}

	// IRQ config

	// enable FMP0 interrupt
	can->IER |= CAN_IT_FMP0 | CAN_IT_FMP1;//��FIFO 0\1��FMP[1:0]λΪ��0ʱ�������жϡ�
//	can->IER |= CAN_IT_ERRIE|CAN_IT_BOFIE|CAN_IT_LECIE;//ʹ�ܴ����ж�
	
	// enable �����жϺʹ����ж�
	NVIC_SetPriority(rx0_irq, 2); // call CAN device using same prio, for FIFO RX interrupt sync
	NVIC_EnableIRQ(rx0_irq);
	//MY_NVIC_Init(2,0 ,rx0_irq,2);
}


/*���ѳ�ʼ��  CAN_STB high��low�� */
static void can_wake_init(void)
{
	// transceiver enters standby mode first (EN = 1 & STB = 1)
	IO_INIT_O_PP(IO_CARCAN_STB, IOBIT_CARCAN_STB, HIGH); // standby pin
	mdelay(3);
	// tansceiver enters normal mode
	IO_SET(IO_CARCAN_STB, IOBIT_CARCAN_STB, LOW);
	mdelay(1);
}

/*can   */
static void can_wake(void)
{
	IO_SET(IO_CARCAN_STB, IOBIT_CARCAN_STB, LOW);
	mdelay(1);
}

/*can �շ��������ѹ  �ø�  standby ����*/
static void can_sleep(void)
{
	///IO_SET(IO_CARCAN_EN, IOBIT_CARCAN_EN, LOW);
	IO_SET(IO_CARCAN_STB, IOBIT_CARCAN_STB, HIGH); // STB --> 1
	udelay(50);
}


/*ֹͣcanʹ��*/
static void can_deinit_simple(void)
{
	IRQn_Type rx0_irq;
	can_rxtx_t *can_rtx = &can_info;
	CAN_TypeDef *can = can_rtx->dev;

	if(can_rtx->irq_enabled)
	{
		rx0_irq = can_rtx->rx_irq0;

		/* Flush transmit mailboxes */
		if(can->TSR & CAN_TSR_TME)
		{
			mdelay(5);
		}

		// disable IRQ
		NVIC_DisableIRQ(rx0_irq);
		can->IER &= ~(CAN_IT_FMP0 | CAN_IT_FMP1);
	}
	//can TX RX ��������Ϊ����״̬
	IO_INIT_I_FLT(GPIOA, 11);
	IO_INIT_I_FLT(GPIOA, 12);

	// disable can module
	RCC->APB1ENR &= ~__BIT(25);
}


/* can ��Ϣ����  */
static u32 can_msg_send(device_info_t *dev, can_msg_t *msg)
{
	u32 mailbox = ~0, c = 0;
	can_rxtx_t *can_rtx = &can_info;
	CAN_TypeDef *can = can_rtx->dev;
	CAN_TxMailBox_TypeDef *mb;

	while(1)
	{
		/* Select one empty transmit mailbox */
		if(CAN_TSR_TME0 == (can->TSR & CAN_TSR_TME0))
		{
			mailbox = 0;
			break;
		}
		else if(CAN_TSR_TME1 == (can->TSR & CAN_TSR_TME1))
		{
			mailbox = 1;
			break;
		}
		else if(CAN_TSR_TME2 == (can->TSR & CAN_TSR_TME2))
		{
			mailbox = 2;
			break;
		}
		else if(c >= 2)
		{
		//	dbg_msgv(dev, "CAN%d msg send fail, no transmit mailbox\r\n",  CAN_DEV0);
			return ~0;
		}

		c++;
		Iwdg_Feed();
		mdelay(3);
	}

	/* Set up the Id */
	//can->sTxMailBox[mailbox].TIR &= TMIDxR_TXRQ;
	mb = &can->sTxMailBox[mailbox];
	mb->TIR = 0;

	if(CAN_Id_Standard == msg->id_ext)
	{
		mb->TIR |= ((msg->id << 21) | msg->rtr);
	}
	else
	{
		mb->TIR |= ((msg->id << 3) | msg->id_ext | msg->rtr);
	}

	/* Set up the DLC */
	mb->TDTR &= 0xFFFFFFF0;
	mb->TDTR |= msg->dlc;

	/* Set up the data field */
	mb->TDLR = msg->dat.w[0];
	mb->TDHR = msg->dat.w[1];

	/* Request transmission */
	mb->TIR |= TMIDxR_TXRQ;

	return mailbox;
}


/******************************
**@brief:ָ�뺯����ָ��can����
**
**
*****************************/
static can_msg_t *can_msg_recv(device_info_t *dev)
{
	u32 rd, wr;
	can_msg_t *msg;
	can_rxtx_t *can_rtx = &can_info;
	rd = can_rtx->r_index;
	wr = can_rtx->w_index;
	if(rd == wr)
	{
		return NULL;
	}
	msg = &can_rtx->msg[rd];
	rd++;
	rd &= (CAN_RX_MSG_CNT - 1);
	can_rtx->r_index = rd;
	return msg;
}


/**@brief:����can����
**    
**    ����� ipc cancan_wake״̬λ
**
***/
void car_can_wake_init(device_info_t *dev)
{
	if(dev->ci->carcan_wake) // has been waken up���Ѿ�����
	{
		return;
	}
	can_wake_init();
	dev->ci->carcan_wake = 1;
	//dbg_msg(dev, "CAR CAN wakeup manually\r\n");
}

void car_can_sleep(device_info_t *dev)
{

#if ENABLE_CAR_CAN
	if(dev->ci->carcan_wake==0) // has been waken up
	{
		return;
	}

	can_sleep();

	dev->ci->carcan_wake = 0;
#endif

}

void car_can_wake(device_info_t *dev)
{
#if ENABLE_CAR_CAN
	if(dev->ci->carcan_wake) // has been waken up
	{
		return;
	}
	can_wake();
	dev->ci->carcan_wake = 1;

#endif
}



/*���ù��������ʶ��*/
#if ENABLE_CAR_CAN
// CAN1's filter num starts from 0, max 13
static const can_filt_t car_can_filters[] =
{	//Ϋ��bodyCANID   NM CANID������
	CAN_LIST_FILT16(0, CAN_Filter_FIFO0, 0x031, 0x1F0, 0x116, 0x316),
	//Diag CANID  ����Ѱַ������ѰַCANID
	CAN_LIST_FILT16(1, CAN_Filter_FIFO0, 0x7DF, 0x743, 0x278, 0xFFF),

//test
#if ENABLE_TEST
#if EXTEND_ID  
	CAN_ELIST_FILT32(1,CAN_Filter_FIFO0,GET_MCU_VER_ID,GET_MCU_VER_ACK_ID),
	CAN_ELIST_FILT32(2,CAN_Filter_FIFO0,GET_ARM_VER_ID,GET_ARM_VER_ACK_ID),
	CAN_ELIST_FILT32(3,CAN_Filter_FIFO0,SET_VIDEO_VIEW_ID,SET_VIDEO_VIEW_ID)
#else
	CAN_LIST_FILT16(1, CAN_Filter_FIFO0, GET_MCU_VER_ID, GET_MCU_VER_ACK_ID, GET_ARM_VER_ID, GET_ARM_VER_ACK_ID),
	CAN_LIST_FILT16(2, CAN_Filter_FIFO0, SET_VIDEO_VIEW_ID, SET_VIDEO_VIEW_ID, 0xFFF, 0xFFF),
#endif
#endif	
#if ENABLE_autotest
	 CAN_LIST_FILT16(2, CAN_Filter_FIFO0, 0x652, 0xFFF, 0xFFF, 0xFFF),
#endif
};

static can_msg_t car_can_rx_msg[CAN_RX_MSG_CNT];//can��Ϣ���ܽṹ��
#endif

#endif

// !!! CAN2 is slave CAN, CAN1 must be configured first !!!
void car_can_init(device_info_t *dev)
{
#if ENABLE_CAR_CAN
	can_init(dev,
	#if ENABLE_DIANDONGCHE
		CAN_FMT_NOR_250K,
	#else
		CAN_FMT_NOR_500K,
	#endif
		car_can_filters,
		ARRAY_SIZE(car_can_filters),
		car_can_rx_msg);//ARRAY_SIZE(car_can_filters) �����С����ʹ�õĹ�����������
	dbg_msg(dev, "CAR CAN initialized\r\n");
#endif
}

void car_can_reinit(device_info_t *dev)
{
#if ENABLE_CAR_CAN
	can_init(dev,
	#if ENABLE_DIANDONGCHE
		CAN_FMT_NOR_250K,
	#else
		CAN_FMT_NOR_500K,
	#endif
		car_can_filters,
		ARRAY_SIZE(car_can_filters),
		car_can_rx_msg);//ARRAY_SIZE(car_can_filters) �����С����ʹ�õĹ�����������
#endif
}


u8 get_busoff_sts(void)
{
	return (CAN1->ESR>>2)&0x01;
	}

u8 get_buserr_sts(void)
{
	return (CAN1->ESR>>4)&0x0f;
	}
  



void car_can_send(device_info_t *dev, can_msg_t *msg)
{
	u32 i=0;
#if ENABLE_CAR_CAN
//	u32 mailbox, status, i = 0;
	i=get_buserr_sts();
if(dev->ci->can_no_tx==0x00)
{
	if(i)
	{	
		if(i==0x01)
		{
			dbg_msg(dev, "bus err,λ����\r\n");
			}
		else if(i==0x02)
		{
			dbg_msg(dev, "bus err,��ʽ(Form)��\r\n");
			}
		else if(i==0x03)
		{
		//��⵽ ACK�����Զ��ط���ǰʧ�ܵ�֡������t(TX_TIMEOUT)=150ms��Ȼack��ʧ����ֹͣ���ͣ����³�ʼ��t(TX_RECOVERY)=150ms�ڷ��͡�
			//dbg_msg(dev, "bus err,ȷ��(ACK)��\r\n");
			}
		
		else if(i==0x04)
		{
			dbg_msg(dev, "bus err,����λ��\r\n");
			}
		else if(i==0x05)
		{
			dbg_msg(dev, "bus err,����λ��r\n");
			}
		else if(i==0x06)
		{
			dbg_msg(dev, "bus err,CRC����\r\n");
			}
		else
		{
			dbg_msgv(dev, "bus err,CAN1_LEC= 0x%x\r\n",get_buserr_sts());
			}
		}
	
	/*	if(get_busoff_sts()==0x01)//boff == 1 ,��ʱTEC==0xff ���ʹ��������������������ģʽ
		{
			//��ָ���һ��
			car_can_init(dev);
			dev->ci->can_no_tx=0x01;//��ֹ���ͱ���
			dev->ci->BusOffQuick=0x01;//���� ��  �ָ�  T[BusOffQuick]=100m	
			start_timer(TIMER_DEV2,100);//T[BusOffQuick]=100m		
			}
		*/
	
		can_msg_send(dev,msg); // returns 'mailbox'
#endif
		}
}

can_msg_t *car_can_recv(device_info_t *dev)
{
	can_msg_t *msg = NULL;
#if ENABLE_CAR_CAN
	msg = can_msg_recv(dev);
#endif
	return msg;
}

void car_can_deinit_simple(device_info_t *dev)
{
#if ENABLE_CAR_CAN
	can_deinit_simple();
#endif
}


u32 car_can_enabled(void)
{
	return ENABLE_CAR_CAN;
}


void can_msg_download(device_info_t *dev, message_t *msg)
{
	can_msg_t can_msg;
	can_msg.id = msg->par.w[0];//??
	can_msg.dat.w[0] = msg->par.w[1];//??
	can_msg.dat.w[1] = msg->par.w[2];
	can_msg.dlc = 8;
	can_msg.id_ext = CAN_Id_Standard;
	can_msg.rtr = CAN_RTR_Data;
	car_can_send(dev, &can_msg);

	//dbg_msgv(dev, "[CAR-CAN-TX] ID: 0x%x, DAT: %08x_%08x\r\n", can_msg.id, can_msg.dat.w[1], can_msg.dat.w[0]);
}

/**
**@brief��can��Ϣ���ء�������
**���룺һ����Ϣ�ṹ
**              
**/

void can_msg_Send_SWJ(device_info_t *dev, u32 id,u32* buf)
{
	can_msg_t can_msg;
	can_msg.id =id;
	can_msg.dat.w[0] = buf[0];
	can_msg.dat.w[1] = buf[1];
	can_msg.dlc = 8;
#if EXTEND_ID  
	can_msg.id_ext = CAN_Id_Extended;
#else
	can_msg.id_ext = CAN_Id_Standard;
#endif
	can_msg.rtr = CAN_RTR_Data;
	car_can_send(dev, &can_msg);
	//dbg_msgv(dev, "[CAR-CAN-TX] ID: 0x%x, DAT: %08x_%08x\r\n", can_msg.id, can_msg.dat.w[1], can_msg.dat.w[0]);
}


#if ENABLE_UPDATE_ARM_BY_CAN
void can_msg_Send_SWJ_Up(device_info_t *dev, u32 id,u32 *buf)
{
	can_msg_t can_msg;

	can_msg.id =id;
	can_msg.dat.w[0] = buf[0];
	can_msg.dat.w[1] = buf[1];
	can_msg.dlc = 8;
#if EXTEND_ID  
	can_msg.id_ext = CAN_Id_Extended;
#else
	can_msg.id_ext = CAN_Id_Standard;
#endif
	can_msg.rtr = CAN_RTR_Data;
	car_can_send(dev, &can_msg);

	//dbg_msgv(dev, "[CAR-CAN-TX] ID: 0x%x, DAT: %08x_%08x\r\n", can_msg.id, can_msg.dat.w[1], can_msg.dat.w[0]);
}
#endif


/*****************************************************************************
 �� �� ��  : ��ϸ���Ӧ�ظ�
 ��������  : ��ϸ���Ӧ�ظ�
 �������  : ��
      �������  : ��
 �� �� ֵ  : ��
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2018��7��20��
    ��    ��   : xz
    �޸�����   : �����ɺ���

*****************************************************************************/
void can_send_NRC(u32 SID,u32 NRC)
{	
	device_info_t *dev=get_device_info();
	
	u32 buff[2]={0};
//����Ӧ ���� 4���ֽڣ���֡��by0Ϊ����  by1Ϊ7F by2ΪSID by3 NRC��
	buff[0] = 0x03;
	buff[0] |=0x7F<<8;
	buff[0] |=SID<<16;
	buff[0] |=NRC<<24;
    can_msg_Send_SWJ_Up(dev, AVM_Phy_Resp,buff);
}