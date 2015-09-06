#include 	"includes.h" 

OS_STK GstkStart[100];
OS_STK WLANTaskStk[100];
OS_STK CC1101ReceiveTaskStk[100];
OS_STK CC1101TransmitTaskStk[100];
OS_STK UartTaskStk[100];
OS_STK FlashTaskStk[100];
OS_STK LCDTaskStk[100];

#define  TASK_START_PRIO           			3   //ԽС  ���ȼ�Խ�� 
#define  TASK_WLAN_PRIO    							9
#define  TASK_CC1101Receive_PRIO            		4
#define  TASK_CC1101Transmit_PRIO            		5
#define  TASK_UART_PRIO            			7
#define  TASK_FLASH_PRIO            		8
#define  TASK_LCD_PRIO            			6

static void  	taskStart (void  *parg);
extern void  	taskwlan (void  *parg);
static void  	taskcc1101receive(void  *parg);
static void  	taskcc1101transmit(void  *parg);
static void 	taskflash(void *pdata);
//static void 	tasklcd(void *pdata);
static void  taskuart(void  *parg);


OS_EVENT *UartQSem;
	
INT8U err;

unsigned char WiFi_Status;

extern unsigned char  CC1101DataRecFlag;
extern unsigned char CC1101RxBuf[64];

unsigned char CommandAckTimerOut = 0;    //ACK���� ��ʱ


unsigned char  DisplayStatus;   //Һ������ʾģʽ
unsigned char  DataDisplayRefreshFlag =0; 

extern enum DeviceRunStatus TerminalStatus[CLIENTNUM];  //10���ն�����״̬
extern unsigned char Heart_Beat_Flag;
extern bool WorEnableFlag[CLIENTNUM];

unsigned char CurrentAddress = 1;  //��ǰ��ַ
unsigned char reconnectnum;


unsigned char SYNC0_addr;


extern char ConnectedssidName[50];
extern unsigned char ConnectedssidSecurity[50];
extern unsigned char ConnectedssidSecurityLength;
extern unsigned char deviceLanPort[2];
extern unsigned char deviceLanIP[4];


int main(void)
{   
    OSInit(); 
    OSTaskCreate(taskStart, (void *)0, &GstkStart[99], TASK_START_PRIO);	 //ָ���ַ��������GstkStart
    OSStart(); 
}

void taskStart (void  *parg)
{
    (void)parg;
	
		BSP_Init();			  //ϵͳʱ�ӳ�ʼ��    modify by  nevinxu 2014.2.8

		OSTaskCreate ( 	taskwlan,//
										(void *)0, 
										&WLANTaskStk[99],     //ָ���ַ��������
										TASK_WLAN_PRIO);   	
//		OSTaskCreate ( 	taskcc1101receive,//
//										(void *)0, 
//										&CC1101ReceiveTaskStk[99], 
//										TASK_CC1101Receive_PRIO);  
//		OSTaskCreate ( 	taskcc1101transmit,//
//										(void *)0, 
//										&CC1101TransmitTaskStk[99], 
//										TASK_CC1101Transmit_PRIO); 
//		OSTaskCreate ( tasklcd,//
//                   (void *)0, 
//                   &LCDTaskStk[99], 
//                   TASK_LCD_PRIO); 
//			OSTaskCreate ( taskuart,//
//                   (void *)0, 	
//                   &UartTaskStk[99], 
//                   TASK_UART_PRIO); 
// 			OSTaskCreate ( taskflash,//
//                   (void *)0, 
//                   &FlashTaskStk[99], 
//                   TASK_FLASH_PRIO); 

    while (1) {   
		OSTaskSuspend(OS_PRIO_SELF);  
    }
}

/********************************************************************************************/


void CC1101RecDataFunction()
{
	CC1101ReceivePacket(CC1101RxBuf);   				
	CC1101DateRecProcess();
}

void CC1101SendDataFunction()
{
	static unsigned char LastAddress  = 0xff;
	static unsigned char WorReqTimeOut = 0; 
/************************************��ַѭ��********************************************/
	if(LastAddress != CurrentAddress)
	{
		CC1101SYNCSet(CurrentAddress);   //cc1101��ַ����
	}
	LastAddress = CurrentAddress;
/*************************************���ͺ���************************************************/					
	if(WorEnableFlag[CurrentAddress] == FALSE)    //Wor δ����״̬  �����ķ��������
	{
		if(WorReqTimeOut==0) 
		{
			WorAckTransmit(CurrentAddress,CC1101Target);
			WorReqTimeOut++;	
			SetLEDStatus(LED4,FALSE);
		}
		else if(WorReqTimeOut>=20)
		{
			WorReqTimeOut = 0;
			CurrentAddress++;
			if(CurrentAddress>CLIENTNUM)
			{
				CurrentAddress = 1;
			}			
		}
		else
		{
			WorReqTimeOut++;	
		}
	
	}
	else if (WorEnableFlag[CurrentAddress] == TRUE)  //��������������
	{				
/*************************************���ͺ���************************************************/			
		if(TerminalStatus[CurrentAddress] == StartStatus)   //δ��½
		{
			LoginReqTransmit(CurrentAddress,CC1101Target);  //��½ѯ��
			TerminalStatus[CurrentAddress] = CC1101LoginReqStatus;
			CommandAckTimerOut = 0;
			//WorEnableFlag[CurrentAddress] = FALSE;
		}
		else if(TerminalStatus[CurrentAddress] == CC3000LoginAckStatus) //��½
		{
			WorkingStateMsgReqTransmit(CurrentAddress,CC1101Target);
			TerminalStatus[CurrentAddress] = CC1101WorkingStateMsgReqStatus;
			CommandAckTimerOut = 0;
			//WorEnableFlag[CurrentAddress] = FALSE;
		}
		CommandAckTimerOut++;
		if(CommandAckTimerOut >= 10)    
		{
			CommandAckTimerOut = 0;
			if(TerminalStatus[CurrentAddress] == CC3000WorkingStateMsgAckStatus)   //Ϊ��һ�εķ���׼����
			{
				TerminalStatus[CurrentAddress] = CC3000LoginAckStatus;
			}
			else if(TerminalStatus[CurrentAddress] == CC1101WorkingStateMsgReqStatus) 
			{
				TerminalStatus[CurrentAddress] = CC3000LoginAckStatus;
			}
			else
			{
				TerminalStatus[CurrentAddress] = StartStatus;	
			}
			
			WorEnableFlag[CurrentAddress] = FALSE;
			WorReqTimeOut = 0;
			
			CurrentAddress++;   //��ַ���л�ʱ��̶�							
			if(CurrentAddress>CLIENTNUM)
			{
				CurrentAddress = 1;
			}
		}
	}

}



/*********************************CC1101������**********************************************/
static void taskcc1101transmit(void *pdata)
{
	
	int i;
	unsigned char ReSendCommandTimer[CLIENTNUM+1];   //��һ��״̬

	pdata = pdata; 
	
//	memset(WorEnableFlag,0,CLIENTNUM);

//	
//	for( i= 0;i<CLIENTNUM;i++)
//	{
//		TerminalStatus[i] = StartStatus;
//		WorEnableFlag[i] = FALSE;
//	}
	
	while(1)
	{
//		if(ulWifiEvent == WIFI_SEND_RECV)
//		{		
//			CC1101SendDataFunction();
//		}
		OSTimeDly(OS_TICKS_PER_SEC/20);
	}
}
	

static void taskcc1101receive(void *pdata)
{
	CC1101Init();                                    //CC1101 ��ʼ��
	while(1)
	{
//		if(ulWifiEvent == WIFI_SEND_RECV)
		{
			if(CC1101DataRecFlag == TRUE)
			{
				CC1101RecDataFunction();
				CC1101DataRecFlag = FALSE;
			}	
		}
		OSTimeDly(OS_TICKS_PER_SEC/60);
	}

}
			
//					ReceiveAckTimeOut++;
///********************************************************************************************/	
//					if(ReceiveAckTimeOut >= 20)  //1000ms�ĳ�ʱ
//					{
//						ReceiveAckTimeOut = 0;
//						
//						WorReqTimeOut = 0;
//						WorEnableFlag[CurrentAddress] = 0;
//						
//						
//						if(TerminalStatus[CurrentAddress] < 4)   //�޷���½��һֱ���͵�½����
//						{
//							TerminalStatus[CurrentAddress] = 0;    
//						}
//						else if(TerminalStatus[CurrentAddress] == 8)   //Ϊ��һ�εķ���׼����
//						{
//							TerminalStatus[CurrentAddress] = 4;
//							ReSendCommandTimer[CurrentAddress] = 0;
//						}
//						else if(TerminalStatus[CurrentAddress] != 8)   //Ϊ�����ΰ��ķ���   
//						{
//							TerminalStatus[CurrentAddress] = 4;
//							ReSendCommandTimer[CurrentAddress]++;
//							CurrentAddress--;
//							if(ReSendCommandTimer[CurrentAddress+1] >= 5)   //����5��
//							{
//								ReSendCommandTimer[CurrentAddress+1] = 0;
//								TerminalStatus[CurrentAddress+1] = 0;  //�������µ�½����
//							}				
//						}
//						
//						CurrentAddress++;   //��ַ���л�ʱ��̶�							
//						if(CurrentAddress>CLIENTNUM)
//						{
//							CurrentAddress = 1;
//						}
//					}
//				}
//			}	
//			else
//			{
//				WorEnableFlag[CurrentAddress] = 0;
//			}
//		}
//		OSSemPost(CC1101Rec_Semp);
//	}
//				
//	}
//}
/********************************************************************************************/


/**********************************FLASH������**********************************************/

static void taskflash(void *pdata)
{
	//static unsigned char	DeviceID = 0;
//	static unsigned int FlashID = 0;
	static unsigned char  Buffer[200];
	unsigned char defaultIP[4];
	unsigned char defaultPort[2];
	
//	deviceLanPort[1] = (unsigned char)DEVICE_LAN_PORT;
//	deviceLanPort[0] = (unsigned char)(DEVICE_LAN_PORT>>8);
//	
//	deviceLanIP[0] = (unsigned char)DEVICE_LAN_IP;
//	deviceLanIP[1] = (unsigned char)(DEVICE_LAN_IP>>8);
//	deviceLanIP[2] = (unsigned char)(DEVICE_LAN_IP>>16);
//	deviceLanIP[3] = (unsigned char)(DEVICE_LAN_IP>>24);
	
	strcpy(ConnectedssidName,DEFAULT_SSID);
	strcpy(ConnectedssidSecurity,AP_SECURITY);
	pdata = pdata;
//	SPI_FLASH_Init();
  /* Get SPI Flash Device ID */
 // DeviceID = SPI_FLASH_ReadDeviceID();  
	//Delay( 200 );
  /* Get SPI Flash ID */
 // FlashID = SPI_FLASH_ReadID();
	
#if 0
	memcpy(Buffer,DEFAULT_SSID,40);
	Buffer[40] = 12;
	memcpy(Buffer+41,AP_SECURITY,39);
	memcpy(Buffer+80,defaultIP,4);
	memcpy(Buffer+84,defaultPort,2);
	SPI_FLASH_SectorErase(0);
	SPI_FLASH_BufferWrite(Buffer, 0, 200);
#endif
	
	
#if 0
	SPI_FLASH_BufferRead(Buffer, 0, 200);
	strcpy(ConnectedssidName,(const char*)&Buffer);
	ConnectedssidSecurityLength = Buffer[40];
	memcpy(ConnectedssidSecurity , Buffer+41, ConnectedssidSecurityLength);
	memcpy(deviceLanIP , Buffer+80, 4);
	memcpy(deviceLanPort , Buffer+84, 2);
#endif
	
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC); 
	}
}
/********************************************************************************************/


void  taskuart(void  *parg)
{
	void    *MyArrayOfMsg[5];
	static INT8U   buffer[50];

	(void)parg;
	UartInit();
	
	UartQSem = OSQCreate(&MyArrayOfMsg[0],5);

	while (1) {
		void   *GETQ;
		INT8U   err;
		GETQ   = (void *)OSQPend(UartQSem, OS_TICKS_PER_SEC*3, &err);

		if(err == OS_ERR_TIMEOUT)
		{
			;
			//UartSendCommand(SENDWIFIHEARTCOMMAND);
		}
		else if(err == OS_ERR_NONE)
		{
			memcpy(buffer,GETQ,50);	
			CheckReceiveCommand(buffer);
		}
		OSTimeDly(OS_TICKS_PER_SEC/20);
	}
					     
}


/***********************************��ʾ������*********************************************/
//static void tasklcd(void *pdata)
//{
//	static unsigned char LastDisplayStatus = 0;  //��ʾ״̬�л� 
//	pdata = pdata;
//	Initial_Lcd();
//	DisplayClearAll();

//	while(1)
//	{
//		OSTimeDly(OS_TICKS_PER_SEC);  //һ��ˢ��һ��
//		OSSemPend(CC1101Rec_Semp,0,&err);
//		if(DisplayStatus != LastDisplayStatus )
//		{
//			DisplayClearAll();
//		}
//		if(DisplayStatus == WifiStatusDisplayStatus)
//		{
//			WifiStatusDisplay();
//		}
//		else if(DisplayStatus == DataDisplayStatus)
//		{
//			DataDisplay();
//		}
//		LastDisplayStatus = DisplayStatus;
//		OSSemPost(CC1101Rec_Semp);
//	}
//}
/********************************************************************************************/
