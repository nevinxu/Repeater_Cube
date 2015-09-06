#include "includes.h"

MSG_HEAD MsgHead;   //消息头
Terminal_Working_State_MSG_BODY MsgWorkStateBody;  //数据状态 消息体
HEART_BEAT_Ack_BODY HeartBeatAckBody;
Login_MSG_BODY LoginMsgBody;

unsigned char TxBuffer[64];
unsigned int MsgBegin = 0xA5B4;   //消息识别符
unsigned int MsgLength;  //消息长度  `
long SequenceId = 0;   //消息流水号

unsigned char CommandId[CLIENTNUM+1]; //命令或相应类型
unsigned int ModelAddress[CLIENTNUM+1];  //床位号
unsigned char MsgStatus[CLIENTNUM+1];  //消息状态
unsigned char TerminalID[CLIENTNUM+1][6];//唯一标识该终端
enum DeviceRunStatus TerminalStatus[CLIENTNUM+1];  //10个终端登陆状态
bool WorEnableFlag[CLIENTNUM+1];



unsigned char LastTimeDelay;
unsigned char PackageLoseNum;

#define MessageHeaderLength  20

unsigned short ProtocolVersion[CLIENTNUM+1];
unsigned short HardwareVersion[CLIENTNUM+1];

#define Reversed                0x00

unsigned char CurrentSpeed[CLIENTNUM+1];
unsigned int TotalDrip[CLIENTNUM+1];
unsigned char TerminalPowerPrecent[CLIENTNUM+1];
unsigned char MaxSpeedLimited[CLIENTNUM+1];
unsigned char MinSpeedLimited[CLIENTNUM+1];

unsigned char WorkingStatus[CLIENTNUM+1];    //工作状态
unsigned char LoginStatus[CLIENTNUM+1];
unsigned char LogoutStatus[CLIENTNUM+1];
unsigned char HeartBeatStatus[CLIENTNUM+1];


unsigned char CC1101RxBuf[64];  //cc1101数据接收缓存
unsigned char CC3000RxBuf[64];  //cc3000数据接收缓存
unsigned char TxBuffer[64];
unsigned char CC3000Rxlen = 0;
unsigned char  CC1101DataRecFlag; //cc1101  数据接收更新标志

extern unsigned char CurrentAddress;
extern unsigned char reconnectnum;;


void MessageHeader(unsigned char ClientNum)
{
//	unsigned char i;
//  MsgHead[ClientNum].m_MSGBegin = MsgBegin;
//  MsgHead[ClientNum].m_ModelAddress = ModelAddress[ClientNum];
//	for(i = 0;i<8;i++)
//	
//	{
//		MsgHead[ClientNum].m_Terminal_ID[i] =  TerminalID[ClientNum][i];
//	}
//		memset(&TxBuffer,0,sizeof(TxBuffer));  //先TxBuffer 清零，然后再Copy 数据
//		memcpy(&TxBuffer,&(MsgHead[ClientNum]),sizeof(MSG_HEAD));  
}

void WorkingStateMsgReqTransmit(unsigned char ClientNum, unsigned char RecTarget)
{	
//	MsgHead[ClientNum].m_Status = WorkingStatus[ClientNum];
//  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
//  MsgHead[ClientNum].m_Command_Id = TerminalWorkingStateReqCommand;
//	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
//	SequenceId++;
//  MessageHeader(ClientNum);
//   
//	
//	if(RecTarget == CC1101Target)
//	{
//		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
//	}
//	else if(RecTarget == CC3000Target)
//	{
//		;
//	}
}

void WorkingStateMsgTransmit(unsigned char ClientNum, unsigned char RecTarget)
{	
//	MsgHead[ClientNum].m_Status = WorkingStatus[ClientNum];
//  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength+sizeof(Terminal_Working_State_MSG_BODY);
//  MsgHead[ClientNum].m_Command_Id = TerminalWorkingStateCommand;
//	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
//	SequenceId++;
//  MessageHeader(ClientNum);
//	
//  MsgWorkStateBody[ClientNum].m_Current_Speed = CurrentSpeed[ClientNum];
////	  MsgWorkStateBody[ClientNum].m_Current_Speed = 0x55;
//  MsgWorkStateBody[ClientNum].m_Total_Drip = TotalDrip[ClientNum];
////	  MsgWorkStateBody[ClientNum].m_Total_Drip = 0x77;
//	MsgWorkStateBody[ClientNum].m_Terminal_Power = TerminalPowerPrecent[ClientNum];
//	MsgWorkStateBody[ClientNum].m_Max_Speed_Limited = MaxSpeedLimited[ClientNum];
//	MsgWorkStateBody[ClientNum].m_Min_Speed_Limited = MinSpeedLimited[ClientNum];
//	
//	memset(&TxBuffer[0]+sizeof(MSG_HEAD),0,sizeof(Terminal_Working_State_MSG_BODY));
//	memcpy(&TxBuffer[0]+sizeof(MSG_HEAD),&MsgWorkStateBody[ClientNum],sizeof(Terminal_Working_State_MSG_BODY));
//   
//	if(RecTarget == CC1101Target)
//	{
//		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
//	}
//	else if(RecTarget == CC3000Target)
//	{
//		CC3000SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length);
//	}
}


//void HeartBeatTransmit(unsigned char RecTarget)
//{
//  MsgStatus = HeartBeatStatus;
//  MsgLength = MessageHeaderLength+2;
////  SequenceId++;
//  CommandId = HEARTBEATCommand;
//  TxBuffer[18] = LastTimeDelay;
//  TxBuffer[19] = PackageLoseNum;
//  MessageHeader();
//	if(RecTarget == CC1101Target)
//	{
//		CC1101SendPacket( TxBuffer, MsgLength); 
//	}
//	else if(RecTarget == CC3000Target)
//	{
//		CC3000SendPacket( TxBuffer, MsgLength);
//	}
//}


void WorkingStateMsgAckTransmit(unsigned char ClientNum,unsigned char RecTarget)
{

	
//	MsgHead[ClientNum].m_Status = WorkingStatus[ClientNum];
//  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
//  MsgHead[ClientNum].m_Command_Id = TerminalWorkingStateAckCommand;
//	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
//	SequenceId++;
//  MessageHeader(ClientNum);
//   
//	
//	if(RecTarget == CC1101Target)
//	{
//		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
//	}
//	else if(RecTarget == CC3000Target)
//	{
//		;
//	}
}

void LoginReqTransmit(unsigned char ClientNum,unsigned char RecTarget)
{
//	MsgHead[ClientNum].m_Status = LoginStatus[ClientNum];
//  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
//  MsgHead[ClientNum].m_Command_Id = TerminalLogin_ReqCommand ;
//	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
//	SequenceId++;
//  MessageHeader(ClientNum);
//	if(RecTarget == CC1101Target)
//	{
//		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
//	}
//	else if(RecTarget == CC3000Target)
//	{
//		CC3000SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length);
//	} 
}


void LoginTransmit(unsigned char ClientNum,unsigned char RecTarget)
{
//	MsgHead[ClientNum].m_Status = LoginStatus[ClientNum];
//  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength+6;
//  MsgHead[ClientNum].m_Command_Id = TerminalLoginCommand;
//	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
//	SequenceId++;
//  MessageHeader(ClientNum);
//  TxBuffer[18] = ProtocolVersion[ClientNum];
//  TxBuffer[19] = ProtocolVersion[ClientNum]>>8;
//  TxBuffer[20] = HardwareVersion[ClientNum];
//  TxBuffer[21] = HardwareVersion[ClientNum]>>8;       
//  TxBuffer[22] = Reversed;
//  TxBuffer[23] = Reversed;
//	if(RecTarget == CC1101Target)
//	{
//		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
//	}
//	else if(RecTarget == CC3000Target)
//	{
//		CC3000SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length);
//	} 
}

void LoginAckTransmit(unsigned char ClientNum,unsigned char RecTarget)
{
//	MsgHead[ClientNum].m_Status = LoginStatus[ClientNum];
//  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
//  MsgHead[ClientNum].m_Command_Id = TerminalLogin_AckCommand;
//	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
//	SequenceId++;
//	
//  MessageHeader(ClientNum);
//	if(RecTarget == CC1101Target)
//	{
//		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
//	}
//	else if(RecTarget == CC3000Target)
//	{
//		CC3000SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length);
//	} 
}

void WorAckTransmit(unsigned char ClientNum,unsigned char RecTarget)
{
//	MsgHead[ClientNum].m_Status = LoginStatus[ClientNum];
//  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
//  MsgHead[ClientNum].m_Command_Id = TerminalWOR_AckCommand ;
//	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
//	SequenceId++;
//  MessageHeader(ClientNum);
//	if(RecTarget == CC1101Target)
//	{
//		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
//	}
//	else if(RecTarget == CC3000Target)
//	{
//		CC3000SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length);
//	} 
}


void CC1101DateRecProcess()
{
		CurrentAddress = CC1101RxBuf[0];
		memcpy(&MsgHead,&CC1101RxBuf[1],sizeof(MsgHead));
	
//	if(CC1101RxBuf[CommandIdByte] == TerminalWorkingStateCommand)   //工作状态接收包
//	{
//		if(CC1101RxBuf[StatusByte] == 0) //数据正常
//		{
//			if(ClientNum == CC1101RxBuf[ModelAddressByte])
//			{
//				ModelAddress[ClientNum] = CC1101RxBuf[ModelAddressByte];
//				WorkingStatus[ClientNum] = CC1101RxBuf[StatusByte];
//				CurrentSpeed[ClientNum] = CC1101RxBuf[CurrentSpeedByte];
//				TotalDrip[ClientNum] = CC1101RxBuf[TotalDripByte]+ (CC1101RxBuf[TotalDripByte+1]<<8);
//				TerminalPowerPrecent[ClientNum] = CC1101RxBuf[TerminalPowerByte];		
//				MaxSpeedLimited[ClientNum] = CC1101RxBuf[MaxSpeedLimitedByte];
//				MinSpeedLimited[ClientNum] = CC1101RxBuf[MinSpeedLimitedByte];
//				WorkingStateMsgAckTransmit(ClientNum,CC1101Target);
//				TerminalStatus[ClientNum] = CC1101WorkingStateMsgAckStatus;
//			}
//		}
//	}
//	else if(CC1101RxBuf[CommandIdByte] == TerminalLoginCommand)  //登陆状态接收包
//	{
//		if(CC1101RxBuf[StatusByte] == 0)  //数据正常
//		{
//			if (ClientNum == CC1101RxBuf[ModelAddressByte])
//			{
//				ModelAddress[ClientNum] = CC1101RxBuf[ModelAddressByte];
//				ProtocolVersion[ClientNum] = CC1101RxBuf[ProtocolVersionByte] + (CC1101RxBuf[ProtocolVersionByte+1]<<8);
//				HardwareVersion[ClientNum] = CC1101RxBuf[HardwareVersionByte] + (CC1101RxBuf[HardwareVersionByte+1]<<8);
//				LoginAckTransmit(ClientNum,CC1101Target);
//				TerminalStatus[ClientNum] = CC1101LoginAckStatus;
//			}
//		}
//	}
//	else if(CC1101RxBuf[CommandIdByte] == TerminalWOR_ReqCommand)  //Wor 请求回应包接收
//	{
//		if (ClientNum == CC1101RxBuf[ModelAddressByte])
//		{
//			WorEnableFlag[CurrentAddress] = TRUE;
//		}
//	}
}

void CC1101DateSendProcess(unsigned char ClientNum)
{
	WorkingStateMsgReqTransmit(ClientNum,CC1101Target);
}

void CC3000DataRecProcess(unsigned char ClientNum)
{
//	if(CC3000Rxlen != 0)
//	{
//		memcpy(&MsgHead[ClientNum],CC3000RxBuf,sizeof(MSG_HEAD));
//		if(MsgHead[ClientNum].m_MSGBegin == 0xA5B4)
//		{
//			if(MsgHead[ClientNum].m_Command_Id == TerminalLogin_AckCommand) 
//			{
//				if(MsgHead[ClientNum].m_Status == 0)
//				{
//					TerminalStatus[ClientNum] = CC3000LoginAckStatus;  
//				}
//				reconnectnum = 0;
//				SetLEDStatus(LED4,TRUE);
//			}
//			else if(MsgHead[ClientNum].m_Command_Id == TerminalWorkingStateAckCommand) 
//			{
//				if(MsgHead[ClientNum].m_Status == 0)
//				{
//					TerminalStatus[ClientNum] = CC3000WorkingStateMsgAckStatus;  
//				}
//				reconnectnum = 0;
//				SetLEDStatus(LED4,TRUE);
//			}
//		}
//		
//		memset (CC3000RxBuf, 0, sizeof(CC3000RxBuf));     //清除
//	}
}
