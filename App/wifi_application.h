#ifndef WIFI_APPLICATION_H
#define WIFI_APPLICATION_H

#ifdef __cplusplus
extern "C"
{
#endif
#define WIFI_NON_CONNECTING          0
#define WIFI_CONNECTING              1
#define WIFI_CONNECT_FINISED         2
#define WIFI_SMARTCONFIG             3
#define WIFI_SMARTCONFIG_FINISED     4
#define WIFI_SEND_RECV               5
#define WIFI_NON_EVENT               6
#define WIFI_SOCKETCONNECTING     	 7
	
	
//#define DEFAULT_SSID       "caiyongfeng"
//#define AP_SECURITY                   "cai88619171"		
	
//#define DEFAULT_SSID       "caiyongfeng1"
//#define AP_SECURITY                   "12345678"			
	

#define DEFAULT_SSID       									"sbsbsbsb"
#define AP_SECURITY                   "fangfang0429"	
	
//#define DEFAULT_SSID       							"AndroidAP"
//#define AP_SECURITY                   "1234567890"		
#define SERVER_LAN_PORT         7890            //Ĭ�϶˿ں�
#define SERVER_LAN_IP   				0x8863A8C0
//#define DEVICE_LAN_IP   				0x6A01A8C0

//#define DEVICE_LAN_IP   				0x030A14AC

//#define DEVICE_LAN_IP   				0x6B01A8C0
//#define DEVICE_LAN_IP   				0x312BA8C0


	

#define WIFI_RX_BUF_MAX    1024
#define WIFI_TX_BUF_MAX    1024
 
	
#define TCPClientMode 1
#define TCPServerMode 2	
#define UDPMode 3	
  
extern volatile unsigned long ulSmartConfigFinished, ulCC3000Connected, ulCC3000DHCP,OkToDoShutDown, ulCC3000DHCP_configured;
extern volatile unsigned char ucStopSmartConfig;
extern volatile unsigned char ulWifiEvent;
extern unsigned short WIFIRxLen;
extern unsigned char WIFIRxBuf[WIFI_RX_BUF_MAX];
extern unsigned short WIFITxLen;
extern unsigned char WIFITxBuf[WIFI_TX_BUF_MAX];
extern char DeviceMac_Addr[6];
extern volatile unsigned char ulWifiEvent;
extern volatile unsigned char ulCC3000IRQDelayFlags;


#define in_range(c, lo, up)  ((unsigned char)c >= lo && (unsigned char)c <= up)
#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))  

extern int Init_CC3000Driver(void);
extern unsigned char  GetdeviceInfo(void);
extern void WifiRecvData(void);
extern void Wifi_send_data(unsigned char *data,unsigned short Len);
extern unsigned char ConnectionAP(void);
extern void Wifi_event_handler(void);
extern void StartSmartConfig(void);
extern void  ReConnectSocket(unsigned long IP,unsigned short Port,unsigned char Mode);
extern void Init_Client(signed long* Socket,char * hname, int port);
extern void  Wifi_Scan(void);
extern void Wifisend_Function(void);
extern void Wifireceive_Function(void);
extern void CC3000SendPacket(unsigned char *TxBuffer,unsigned char Size);
extern void SetTcpConnect(unsigned char TCPMode);
#ifdef __cplusplus
}
#endif

#endif
