#include 	"includes.h" 

unsigned char Sp_Fw[5] = {0};

/*********************************WIFI 任务函数**********************************************/
void  taskwlan(void  *parg)
{

	(void)parg;
	//DisplayStatus = WifiStatusDisplayStatus;   //显示wifi连接状态
  Init_CC3000Driver();  		//初始化cc3000
	nvmem_read_sp_version(Sp_Fw);	
	
	while(1)
	{
#ifdef IWDGENABLE
		IWDG_ReloadCounter();
#endif
		
		Wifi_event_handler();  //wifi状态切换

//		CC3000DataRecProcess(CurrentAddress);    //接收没有对象之分
//		if(ulWifiEvent == WIFI_SEND_RECV)
//		{ 
//				if(TerminalStatus[CurrentAddress] == CC1101LoginAckStatus)   //终端登陆
//				{
//					TerminalStatus[CurrentAddress] = CC3000LoginReqStatus;
//					LoginTransmit(CurrentAddress,CC3000Target);
//					CommandAckTimerOut = 0;
//				}
//				else if (TerminalStatus[CurrentAddress] == CC1101WorkingStateMsgAckStatus)
//				{
//					WorkingStateMsgTransmit(CurrentAddress,CC3000Target);
//					TerminalStatus[CurrentAddress] = CC3000WorkingStateMsgReqStatus;  //等待状态   cc1101发送数据  激活发送状态
//					CommandAckTimerOut = 0;
//				}
//		}

		OSTimeDly(OS_TICKS_PER_SEC/100);
	}
}