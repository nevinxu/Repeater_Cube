#include 	"includes.h" 

unsigned char Sp_Fw[5] = {0};

/*********************************WIFI ������**********************************************/
void  taskwlan(void  *parg)
{

	(void)parg;
	//DisplayStatus = WifiStatusDisplayStatus;   //��ʾwifi����״̬
  Init_CC3000Driver();  		//��ʼ��cc3000
	nvmem_read_sp_version(Sp_Fw);	
	
	while(1)
	{
#ifdef IWDGENABLE
		IWDG_ReloadCounter();
#endif
		
		Wifi_event_handler();  //wifi״̬�л�

//		CC3000DataRecProcess(CurrentAddress);    //����û�ж���֮��
//		if(ulWifiEvent == WIFI_SEND_RECV)
//		{ 
//				if(TerminalStatus[CurrentAddress] == CC1101LoginAckStatus)   //�ն˵�½
//				{
//					TerminalStatus[CurrentAddress] = CC3000LoginReqStatus;
//					LoginTransmit(CurrentAddress,CC3000Target);
//					CommandAckTimerOut = 0;
//				}
//				else if (TerminalStatus[CurrentAddress] == CC1101WorkingStateMsgAckStatus)
//				{
//					WorkingStateMsgTransmit(CurrentAddress,CC3000Target);
//					TerminalStatus[CurrentAddress] = CC3000WorkingStateMsgReqStatus;  //�ȴ�״̬   cc1101��������  �����״̬
//					CommandAckTimerOut = 0;
//				}
//		}

		OSTimeDly(OS_TICKS_PER_SEC/100);
	}
}