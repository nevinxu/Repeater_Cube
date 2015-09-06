#ifndef __APPUART_H 
#define __APPUART_H 

#ifdef __cplusplus
extern "C" {
#endif

  /*
********************************************************************************************************
os event define
********************************************************************************************************
*/

#define UARTBUFLEN                    50
	
	
#define GETDATAREQCOMMAND									0x01
#define SENDWIFINAMECOMMAND   						0x02
#define SENDWIFIPASSWORDCOMMAND   				0x03
#define SENDWIFITCPNAMECOMMAND   						0x04
#define SENDWIFITCPPORTCOMMAND   						0x05
#define SENDWIFIGETDEVICENAMECOMMAND   						0x06
#define SENDWIFISENDDEVICENAMECOMMAND   						0x08
#define SENDWIFIHEARTCOMMAND   						0x09	
	
/*
********************************************************************************************************
TASK PRIORITIES 
********************************************************************************************************
*/

							          
  /*
********************************************************************************************************
TASK STACK SIZES 
********************************************************************************************************
*/

  /*
********************************************************************************************************
FUNCTION PROTOTYPES
********************************************************************************************************
*/


  /*
********************************************************************************************************
FUNCTION PROTOTYPES
********************************************************************************************************
*/

#define USART1_DR_Base           0x40013804
#define USART_BAUND							115200

extern void UartInit(void);
extern void UartBufferSet(signed char *buffer);
extern void UartSend4DMA(signed char *data);
extern void UartSendCommand(unsigned char command);
extern void CheckReceiveCommand(unsigned char *data);
#ifdef __cplusplus
}
#endif

#endif
