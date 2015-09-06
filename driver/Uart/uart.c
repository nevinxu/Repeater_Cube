#include  <includes.h>
#include  "uart.h"

static int8 UartTxBuffer[UARTBUFLEN];
static uint8 UartRxBuffer[UARTBUFLEN];
static int8 UartBuffer[UARTBUFLEN];

uint8 UartSendFlag;    //串口发送使能
extern OS_EVENT *UartQSem;

extern char ConnectedssidName[50];
extern unsigned char ConnectedssidSecurity[50];
extern unsigned char ConnectedssidSecurityLength;
extern unsigned char deviceLanPort[2];
extern unsigned char deviceLanIP[4];

/*
********************************************************************************************************
os event define
********************************************************************************************************
*/


/*
********************************************************************************************************
Constants 
********************************************************************************************************
*/


void USART_Configuration()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  USART_InitTypeDef USART_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA + RCC_APB2Periph_USART1+ RCC_APB2Periph_AFIO, ENABLE);
	
	 /* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	
  /* Configure USARTx_Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USARTx_Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = USART_BAUND;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;    //USART_Parity_Even ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
  USART_Init(USART1, &USART_InitStructure);
	
	/* Enable USART1 DMA TX request */
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	
	/* Enable USART1 DMA RX request */
//	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	
	/* Enable the USART1 Receive Interrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);  
  USART_Cmd(USART1, ENABLE);
	
	
	/* Enable USARTy DMA TX Channel */
	DMA_Cmd(DMA1_Channel4 , ENABLE);


}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USART0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	//DMA发送中断设置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
		//DMA接收中断设置
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the DMA.
  * @param  None
  * @retval None
  */
void DMA_Configuration(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /* USARTy_Tx_DMA_Channel (triggered by USARTy Tx event) Config */
  DMA_DeInit(DMA1_Channel4);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR); 
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UartTxBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = UARTBUFLEN;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	
	
	/* USARTy RX DMA1 Channel (triggered by USARTy Rx event) Config */
	DMA_DeInit(DMA1_Channel5);  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR); 
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UartRxBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = UARTBUFLEN;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);

	
	DMA_ITConfig(DMA1_Channel5,DMA_IT_TC, ENABLE);  
  
}

void USART1_IRQHandler(void)
{
	uint32_t temp = 0;
	static uint16_t i = 0;
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
     UartRxBuffer[++i] = USART_ReceiveData(USART1);
  }

	
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
    	USART_ClearFlag(USART1,USART_IT_IDLE);
    	temp = USART1->DR; //清USART_IT_IDLE标志
			UartRxBuffer[0] = i;
			i = 0;
			OSQPost(UartQSem,UartRxBuffer);
    }
}


void DMA1_Channel5_IRQHandler()
{
	DMA_ClearFlag(DMA1_FLAG_TC5);  
}


void UartSend4DMA(signed char *data)
{
		UartBufferSet(data);
		DMA_Cmd(DMA1_Channel4 , DISABLE);    //必须先关闭DMA   再打开
		DMA_SetCurrDataCounter(DMA1_Channel4,data[0]);
		DMA_Cmd(DMA1_Channel4 , ENABLE);
		while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);
			/* Wait until USARTy TX DMA1 Channel  Transfer Complete */
		
}

void UartBufferSet(signed char *buffer)
{
	memset(UartTxBuffer,0,buffer[0]);
	memcpy(UartTxBuffer,&buffer[1],buffer[0]);
}


void UartInit()
{
	NVIC_Configuration();
	USART_Configuration();
	DMA_Configuration();
}


void UartSendCommand(unsigned char command)
{
	unsigned char checksum = 0;
	unsigned char i;
	static unsigned char sequence;
	switch(command)
	{
		case 	SENDWIFIHEARTCOMMAND:
		case	SENDWIFISENDDEVICENAMECOMMAND:
			UartBuffer[0] = 0x06;   //发送长度
		
			UartBuffer[1] = 0x24;
			UartBuffer[2] = sequence++;
			UartBuffer[3] = command;
			UartBuffer[4] = 0;
			checksum = UartBuffer[2] + UartBuffer[3] + UartBuffer[4];
			UartBuffer[5] = checksum;
			UartBuffer[6] = 0x23;
			UartSend4DMA(UartBuffer);
		break;
		case SENDWIFINAMECOMMAND:
		
			UartBuffer[1] = 0x24;
			UartBuffer[2] = sequence++;
			UartBuffer[3] = command;
			UartBuffer[4] = strlen(ConnectedssidName);
			UartBuffer[0] = 0x06 + UartBuffer[4];   //发送长度
			strcpy(&UartBuffer[5],ConnectedssidName);
			checksum = UartBuffer[2] + UartBuffer[3] + UartBuffer[4];
			for(i = 0;i<UartBuffer[4];i++)
			{
				checksum += UartBuffer[5+i];
			}
			UartBuffer[5+i] = checksum;
			UartBuffer[6+i] = 0x23;
			UartSend4DMA(UartBuffer);
			break;
		case SENDWIFIPASSWORDCOMMAND:
			UartBuffer[1] = 0x24;
			UartBuffer[2] = sequence++;
			UartBuffer[3] = command;
			UartBuffer[4] = ConnectedssidSecurityLength;
			UartBuffer[0] = 0x06 + UartBuffer[4];   //发送长度
			memcpy(&UartBuffer[5],ConnectedssidSecurity,ConnectedssidSecurityLength);
			checksum = UartBuffer[2] + UartBuffer[3] + UartBuffer[4];
			for(i = 0;i<UartBuffer[4];i++)
			{
				checksum += UartBuffer[5+i];
			}
			UartBuffer[5+i] = checksum;
			UartBuffer[6+i] = 0x23;
			UartSend4DMA(UartBuffer);		
			break;
			
			case SENDWIFITCPNAMECOMMAND:
			UartBuffer[1] = 0x24;
			UartBuffer[2] = sequence++;
			UartBuffer[3] = command;
			UartBuffer[4] = 4;
			UartBuffer[0] = 0x06 + UartBuffer[4];   //发送长度
			memcpy(&UartBuffer[5],deviceLanIP,4);
			checksum = UartBuffer[2] + UartBuffer[3] + UartBuffer[4];
			for(i = 0;i<UartBuffer[4];i++)
			{
				checksum += UartBuffer[5+i];
			}
			UartBuffer[5+i] = checksum;
			UartBuffer[6+i] = 0x23;
			UartSend4DMA(UartBuffer);		
			break;
			
			case SENDWIFITCPPORTCOMMAND:
			UartBuffer[1] = 0x24;
			UartBuffer[2] = sequence++;
			UartBuffer[3] = command;
			UartBuffer[4] = 2;
			UartBuffer[0] = 0x06 + UartBuffer[4];   //发送长度
			memcpy(&UartBuffer[5],deviceLanPort,2);
			checksum = UartBuffer[2] + UartBuffer[3] + UartBuffer[4];
			for(i = 0;i<UartBuffer[4];i++)
			{
				checksum += UartBuffer[5+i];
			}
			UartBuffer[5+i] = checksum;
			UartBuffer[6+i] = 0x23;
			UartSend4DMA(UartBuffer);		
			break;
		default:break;
			
	}

}

void CheckReceiveCommand(unsigned char *data)
{
	unsigned char  Buffer[200];
	unsigned char length = data[0];
	unsigned char checksum = 0;
	unsigned char i;
	if(data[1] == 0x24)
	{
		checksum = data[2] + data[3] + data[4];
		for(i = 0; i<data[4]; i++)
		{
			checksum += data[5+i];
		}
		if(checksum == data[5+i])
		{
			switch(data[3])
			{
				case SENDWIFIGETDEVICENAMECOMMAND:UartSendCommand(SENDWIFISENDDEVICENAMECOMMAND);break;
				case GETDATAREQCOMMAND: 
							UartSendCommand(SENDWIFINAMECOMMAND);
							Delay( 300000 );
							UartSendCommand(SENDWIFIPASSWORDCOMMAND);
							Delay( 300000 );
							UartSendCommand(SENDWIFITCPNAMECOMMAND);
							Delay( 300000 );
							UartSendCommand(SENDWIFITCPPORTCOMMAND);
							break;
				case SENDWIFITCPNAMECOMMAND:
							memset(deviceLanIP,0,4);
							memcpy(deviceLanIP,&data[5] ,4);
							break;
				case SENDWIFITCPPORTCOMMAND:
							memset(deviceLanPort,0,2);
							memcpy(deviceLanPort,&data[5] ,2);
							
							break;
				case SENDWIFINAMECOMMAND:
							memset(ConnectedssidName,0,40);
							memcpy(ConnectedssidName,&data[5] ,data[4]);
							break;
				case SENDWIFIPASSWORDCOMMAND:
							memset(ConnectedssidSecurity,0,40);
							memcpy(ConnectedssidSecurity,&data[5] ,data[4]);
							ConnectedssidSecurityLength = data[4];
							#if 1
				
								strcpy(Buffer,ConnectedssidName);
								Buffer[40] = ConnectedssidSecurityLength;
								memcpy(Buffer+41,ConnectedssidSecurity,39);
								memcpy(Buffer+80,deviceLanIP,4);
								memcpy(Buffer+84,deviceLanPort,2);
								SPI_FLASH_SectorErase(0);
								SPI_FLASH_BufferWrite(Buffer, 0, 200);
								UartSendCommand(SENDWIFINAMECOMMAND);
								Delay( 300000 );
								UartSendCommand(SENDWIFIPASSWORDCOMMAND);
								Delay( 300000 );
								UartSendCommand(SENDWIFITCPNAMECOMMAND);
								Delay( 300000 );
								UartSendCommand(SENDWIFITCPPORTCOMMAND);
							#endif
							break;
				default:break;
			}
		}
	}

}
	