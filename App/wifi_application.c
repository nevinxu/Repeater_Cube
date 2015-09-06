#include "includes.h"
#include "wifi_application.h"
#include "wlan.h"
#include "evnt_handler.h"
#include "nvmem.h"
#include "socket.h"
#include "cc3000_common.h"
#include "netapp.h"
#include "cc3000spi.h"
#include "hci.h"
#include "security.h"
#include <string.h>

typedef struct   {
  unsigned long addr;
}ip_addr;


#define CC3000_APP_BUFFER_SIZE                      (32)
#define CC3000_RX_BUFFER_OVERHEAD_SIZE              (20)
#define DISABLE                                     (0)
#define ENABLE                                      (1)
#define SL_VERSION_LENGTH                           (11)
#define NETAPP_IPCONFIG_MAC_OFFSET		    					(20)



extern unsigned char  CC1101DataRecFlag;

unsigned char Heart_Beat_Flag;

tNetappIpconfigRetArgs Ipconfig;

unsigned char result_ssid[32];  //������ȡ����ssidֵ


//unsigned char TCP_Mode = TCPServer_Mode;
unsigned char TCP_Mode = TCPClientMode;
 

extern unsigned char  DisplayStatus;   //Һ������ʾģʽ


extern unsigned char CC3000Rxlen;

extern unsigned short Relay_Flag;
volatile unsigned short Relay_Cty_Flag = 0;

volatile unsigned long ulSmartConfigFinished, ulCC3000Connected, ulCC3000DHCP,OkToDoShutDown, 
                       ulCC3000DHCP_configured;
volatile unsigned char ucStopSmartConfig;

volatile long ulSocket;//Socket ���

volatile unsigned char ulWifiEvent = WIFI_NON_EVENT;
volatile unsigned char ulCC3000Resetflags = 0;
// Simple Config Prefix
char aucCC3000_prefix[] = {'T', 'T', 'T'};

//device name used by smart config response
char device_name[] = "CC3000";

//AES key "smartconfigAES16"
const unsigned char smartconfigkey[] = { 0x73, 0x6d, 0x61, 0x72, 0x74, 0x63, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x41, 0x45,0x53, 0x31, 0x36 };

unsigned short WIFIRxLen = 0;
extern unsigned char CC3000RxBuf[64];

unsigned short WIFITxLen = 0;
unsigned char WIFITxBuf[WIFI_TX_BUF_MAX];

char DeviceMac_Addr[6];


char ConnectedssidName[50] ;
unsigned char ConnectedssidSecurity[50];
unsigned char ConnectedssidSecurityLength;
unsigned char deviceLanPort[2];
unsigned char deviceLanIP[4];



unsigned char pucCC3000_Rx_Buffer[CC3000_APP_BUFFER_SIZE + CC3000_RX_BUFFER_OVERHEAD_SIZE];


volatile unsigned long EventTimeOut[EVENT_MAX_COUNT] = {0,0,0,0,0,0,0,0};
volatile unsigned char SysEvent = 0;
volatile unsigned char ulCC3000IRQDelayFlags = 0;

void SetEventTimeOut(unsigned char event,unsigned long  TimeOut)
{
  SysEvent &= ~event;
  if(TimeOut)
  {
   switch(event)
   {
     case 0x01:EventTimeOut[0] = TimeOut;break;
     case 0x02:EventTimeOut[1] = TimeOut;break;
     case 0x04:EventTimeOut[2] = TimeOut;break;
     case 0x08:EventTimeOut[3] = TimeOut;break;
     case 0x10:EventTimeOut[4] = TimeOut;break;
     case 0x20:EventTimeOut[5] = TimeOut;break;
     case 0x40:EventTimeOut[6] = TimeOut;break;
     case 0x80:EventTimeOut[7] = TimeOut;break;
     default:break;
   }
  }
}

void SetEvent(unsigned char  event)
{
  unsigned char i;
  for(i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(event&(0x01<<i))
    {
      EventTimeOut[i] = 0;
    }
  }
  SysEvent |= event;
}

void ClearEvent(unsigned char event)
{
  unsigned char i;
  for(i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(event&(0x01<<i))
    {
      EventTimeOut[i] = 0;
    }
  }
  SysEvent &= ~event;
}

void GetEvent(void)
{
  unsigned char i;
  for(i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(EventTimeOut[i])
    {
     if(--EventTimeOut[i]==0)
     {
       SysEvent |= 0x01<<i;
     }
    }
  }
}


//*****************************************************************************
//
//! sendDriverPatch
//!
//! @param  Length   pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the driver patch: since there is
//!				  no patch (patches are taken from the EEPROM and not from the host
//!         - it returns NULL
//
//*****************************************************************************
char *sendDriverPatch(unsigned long *Length)
{
  *Length = 0;
  return NULL;
}

//*****************************************************************************
//
//! sendBootLoaderPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the bootloader patch: since there
//!				  is no patch (patches are taken from the EEPROM and not from the host
//!         - it returns NULL
//
//*****************************************************************************
char *sendBootLoaderPatch(unsigned long *Length)
{
  *Length = 0;
  return NULL;
}

//*****************************************************************************
//
//! sendWLFWPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the driver patch: since there is
//!				  no patch (patches are taken from the EEPROM and not from the host
//!         - it returns NULL
//
//*****************************************************************************
char *sendWLFWPatch(unsigned long *Length)
{
  *Length = 0;
  return NULL;
}

//*****************************************************************************
//
//! CC3000_UsynchCallback
//!
//! @param  lEventType   Event type
//! @param  data
//! @param  length
//!
//! @return none
//!
//! @brief  The function handles asynchronous events that come from CC3000
//!		      device and operates a LED1 to have an on-board indication
//
//*****************************************************************************
void CC3000_UsynchCallback(long lEventType, char *data, unsigned char length)
{
  switch(lEventType)
  {
  case HCI_EVNT_WLAN_UNSOL_CONNECT:
    {
      ulCC3000Connected = 1;
    }break;
  case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
    {
      if(ulWifiEvent == WIFI_SEND_RECV)
      {
        ulCC3000Connected = 0;
        ulCC3000DHCP = 0;
        ulCC3000DHCP_configured = 0;
        ulWifiEvent = WIFI_CONNECTING;
        ulCC3000Resetflags = 1;
      }
    }break;
  case HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE:
    {
      ulSmartConfigFinished = 1;
      ucStopSmartConfig = 1;
    }break;
  case HCI_EVNT_WLAN_UNSOL_INIT: break;
  case HCI_EVNT_WLAN_UNSOL_DHCP:
    {
      if ( *(data + NETAPP_IPCONFIG_MAC_OFFSET) == 0)
      {
        ulCC3000DHCP = 1;
      }
      else
      {
        ulCC3000DHCP = 0;
      }
    }break;
  case HCI_EVNT_WLAN_ASYNC_PING_REPORT:break;
  case HCI_EVNT_WLAN_KEEPALIVE:break;
  case HCI_EVNT_WLAN_TX_COMPLETE:break;
  case HCI_EVENT_CC3000_CAN_SHUT_DOWN:
    {
      OkToDoShutDown = 1;
    }break;
  default:break;
  }
}


void Set_staticIP(unsigned long ip,unsigned char DHCP)
{
  unsigned long pucIP_Addr;
  unsigned long pucIP_DefaultGWAddr;
  unsigned long pucSubnetMask;
  unsigned long pucDNS;
  
  if(DHCP)
  {
    pucIP_Addr = 0;
    pucIP_DefaultGWAddr = 0;
    pucSubnetMask = 0;
    pucDNS = 0;   
  }
  else
  {
    pucIP_Addr = ip;
    pucIP_DefaultGWAddr = (ip&0x00ffffff) + 0x01000000;
    pucSubnetMask = 0x00ffffff;
    pucDNS = 0;
  }
  
  netapp_dhcp(&pucIP_Addr, &pucSubnetMask, &pucIP_DefaultGWAddr, &pucDNS);
}

//*****************************************************************************
//
//! initDriver
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The function initializes a CC3000 device and triggers it to start
//!          operation
//
//*****************************************************************************
int Init_CC3000Driver(void)
{
  volatile int i;
  
  ucStopSmartConfig = 0;
  ulSmartConfigFinished = 0;
  ulCC3000Connected = 0;
  ulCC3000DHCP = 0;
  OkToDoShutDown = 0;
  ulCC3000DHCP_configured = 0;
  // Init Spi
  init_spi();
  
  // WLAN On API Implementation
  wlan_init(CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch,
            sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable,
            WlanInterruptDisable, WriteWlanPin);
  
  // Trigger a WLAN device
  wlan_start(0);
  
  // Mask out all non-required events from CC3000
  wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
  
  Set_staticIP(0,TRUE);  //�Զ���ȡip
 
  ulWifiEvent = WIFI_NON_CONNECTING;
  
  return (0);
  
}

//����Socket�������
void Set_ulSocket(unsigned char Mode)
{
	unsigned long timeout = 4;
	if(Mode == UDPMode)
	{
		ulSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
	}
	else
	{
		ulSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	}
  delay_ms(100); 
  setsockopt(ulSocket, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &timeout, 4);
	delay_ms(100);
	setsockopt(ulSocket, SOL_SOCKET, SOCKOPT_RECV_NONBLOCK, &timeout, 4);
	//setsockopt(ulSocket, SOL_SOCKET, SOCKOPT_ACCEPT_NONBLOCK, &timeout, 4);


}

//�󶨶˿ں�
unsigned char Set_TCP(unsigned long IP,unsigned short Port,unsigned char Mode)
{
  sockaddr tSocketAddr;  
//	long AucDHCP = 14400;
//	long AucARP = 3600;
//	long AucKeepalive = 10;
//	long AucInactivity = 15;
	 
  tSocketAddr.sa_family = AF_INET;
  
  // the source port
  tSocketAddr.sa_data[0] = MSB(Port);
  tSocketAddr.sa_data[1] = LSB(Port);
  
  tSocketAddr.sa_data[2] = IP & 0xff;
  tSocketAddr.sa_data[3] = (IP & 0xff00) >> 8;
  tSocketAddr.sa_data[4] = (IP & 0xff0000) >> 16;
  tSocketAddr.sa_data[5] = IP >> 24;
  
  
	//netapp_timeout_values(&AucDHCP, &AucARP,&AucKeepalive,	&AucInactivity);

	
	
	if(Mode == TCPServerMode)	
	{
		;
//		ret = bind(ulSocket, &tSocketAddr, sizeof(sockaddr));
//		ret = listen(ulSocket, 100);
//		ulTcpSocket = accept(ulSocket, &tSocketRecAddr, &tSockRecLen);
//		if(ret>0)  //��ȡ��Ч���
//		{
//		Connectedipaddr = (tSocketRecAddr.sa_data[2]<<24) + (tSocketRecAddr.sa_data[3]<<16) + (tSocketRecAddr.sa_data[4]<<8) + tSocketRecAddr.sa_data[5];
//		}
		
	}	
	else if(Mode == TCPClientMode)
	{
		while(connect(ulSocket, &tSocketAddr, sizeof(sockaddr)) == -1)
		{
			closesocket(ulSocket);   
			delay_ms(700);
			ulSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
			delay_ms(700);
		}
	}
	return 0;
}


unsigned char Set_UDP(unsigned short Port)
{
	sockaddr tSocketAddr;
	signed long ret;
  
  tSocketAddr.sa_family = AF_INET;
  
  // the source port
  tSocketAddr.sa_data[0] = MSB(Port);
  tSocketAddr.sa_data[1] = LSB(Port);
	
	// all 0 IP address
  memset (&tSocketAddr.sa_data[2], 0, 4);
	
	ret = bind(ulSocket, &tSocketAddr, sizeof(sockaddr));
	return ret;
}


//�������� SOCKET 
void  ReConnectSocket(unsigned long IP,unsigned short Port,unsigned char Mode)
{
	
	sockaddr tSocketAddr;
  
  tSocketAddr.sa_family = AF_INET;
  
  // the source port
  tSocketAddr.sa_data[0] = MSB(Port);
  tSocketAddr.sa_data[1] = LSB(Port);
  
	tSocketAddr.sa_data[2] = IP & 0xff;
	tSocketAddr.sa_data[3] = (IP & 0xff00) >> 8;
	tSocketAddr.sa_data[4] = (IP & 0xff0000) >> 16;
	tSocketAddr.sa_data[5] = IP >> 24;
	if(Mode == TCPClientMode)
	{
			closesocket(ulSocket);
			Set_ulSocket(Mode);
			while(connect(ulSocket, &tSocketAddr, sizeof(sockaddr))== -1)
			{
				closesocket(ulSocket);
				Set_ulSocket(Mode);
			}
	}
}

static int
ipaddr_aton(const char *cp, ip_addr *addr)
{
    unsigned long val;
    unsigned char base;
    char c;
    unsigned long parts[4];
    unsigned long *pp = parts;

    c = *cp;
    for (;;) {
        if (!isdigit(c))
            return (0);
        val = 0;
        base = 10;
        if (c == '0') {
            c = *++cp;
            if (c == 'x' || c == 'X') {
                base = 16;
                c = *++cp;
            } else
                base = 8;
        }
        for (;;) {
            if (isdigit(c)) {
                val = (val * base) + (int)(c - '0');
                c = *++cp;
            } else if (base == 16 && isxdigit(c)) {
                val = (val << 4) | (int)(c + 10 - (islower(c) ? 'a' : 'A'));
                c = *++cp;
            } else
                break;
        }
        if (c == '.') {
            if (pp >= parts + 3) {
                return (0);
            }
            *pp++ = val;
            c = *++cp;
        } else
            break;
    }
    if (c != '\0' && !isspace(c)) {
        return (0);
    }
    switch (pp - parts + 1) {

    case 0:
        return (0);

    case 1:        
        break;

    case 2:        
        if (val > 0xffffffUL) {
            return (0);
        }
        val |= parts[0] << 24;
        break;

    case 3:        
        if (val > 0xffff) {
            return (0);
        }
        val |= (parts[0] << 24) | (parts[1] << 16);
        break;

    case 4:        
        if (val > 0xff) {
            return (0);
        }
        val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
        break;
    default:
        break;
    }
    if (addr) {
        ip4_addr_set_u32(addr, htonl(val));
    }
    return (1);
}

char initClient(signed long* Socket,sockaddr* tSocketAddr, char * hname, int port)
{
    ip_addr ipaddr;
    unsigned char *ptr=&(tSocketAddr->sa_data[2]);
	
    if(ipaddr_aton(hname,&ipaddr)<1)
   	return 1;
   	
    *Socket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*Socket == -1)
    {
        wlan_stop();
        return 1;
    }
    tSocketAddr->sa_family = 2;
    tSocketAddr->sa_data[0] = (port & 0xFF00) >> 8;
    tSocketAddr->sa_data[1] = (port & 0x00FF);
    ptr=UINT32_TO_STREAM(ptr,ipaddr.addr);
    if (connect(*Socket, tSocketAddr, sizeof(tSocketAddr)) < 0)
    {
        return 1;
    }
    return 0;
}


void Init_Client(signed long *Socket,char * hname, int port)
{
  sockaddr tSocketAddr;
  initClient(Socket,&tSocketAddr,hname,port);
}

static void Resetcc3000(void)
{
  wlan_stop();
  delay_ms(1000); 
  wlan_start(0);
  // Mask out all non-required events
  wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
}

//*****************************************************************************
//
//! StartSmartConfig
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The function triggers a smart configuration process on CC3000.
//!			it exists upon completion of the process
//
//*****************************************************************************
void StartSmartConfig(void)
{
  ulSmartConfigFinished = 0; 
  ucStopSmartConfig = 0;
  ulCC3000Connected = 0;
  ulCC3000DHCP = 0;
  OkToDoShutDown=0;

  if(ulWifiEvent == WIFI_SMARTCONFIG)
  {   
    // Reset all the previous configuration
    wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);
      
    wlan_ioctl_del_profile(255);
    
    //Wait until CC3000 is disconnected
    while (ulCC3000Connected == 1)
    {
      delay_ms(1);
      hci_unsolicited_event_handler();
    }
      
    Resetcc3000();
      
      
    wlan_smart_config_set_prefix((char*)aucCC3000_prefix);
    // Start the SmartConfig start process
    wlan_smart_config_start(0);
    
    // Wait for Smart config to finish
    while (ulSmartConfigFinished == 0)
    {	
      delay_ms(5);   
    }    
    
    // create new entry for AES encryption key
    nvmem_create_entry(NVMEM_AES128_KEY_FILEID,16);
    
    // write AES key to NVMEM
    aes_write_key((unsigned char *)(&smartconfigkey[0]));
    
    // Decrypt configuration information and add profile
    wlan_smart_config_process();
        
    // Configure to connect automatically to the AP retrieved in the
    // Smart config process
    //while(wlan_ioctl_set_connection_policy(DISABLE, DISABLE, ENABLE));
		while(wlan_ioctl_set_connection_policy(DISABLE, ENABLE, ENABLE));
    
    delay_ms(50);
    
    // reset the CC3000
    Resetcc3000();
      
    ulWifiEvent = WIFI_CONNECTING;

    while((ulCC3000DHCP == 0)||(ulCC3000Connected ==0))
    {
        delay_ms(200); 
        hci_unsolicited_event_handler();  
    }
      
    if((ucStopSmartConfig == 1) && (ulCC3000DHCP == 1) && (ulCC3000Connected == 1))
    {
      unsigned char loop_index = 0;	
      while (loop_index < 5)
      {
        mdnsAdvertiser(1,device_name,strlen(device_name));
        loop_index++;
      }
      ucStopSmartConfig = 0;
    }
    
		
    ulWifiEvent = WIFI_SMARTCONFIG_FINISED;
    
    GetdeviceInfo();
  }
}

unsigned char ConnectionAP(void)
{ 
	unsigned char ConnectedTimeout = 50;
 
	while (((ulCC3000DHCP == 0) || (ulCC3000Connected == 0))&&(ConnectedTimeout > 0))
	{    
		delay_ms(200); 
		hci_unsolicited_event_handler();
		ConnectedTimeout--;
	} 
	if(ConnectedTimeout == 0)
	{
		if(ulCC3000DHCP == 0 || ulCC3000Connected == 0)
		{
			Resetcc3000();
			ulWifiEvent = WIFI_CONNECTING;
			return FALSE;
		}
	}
//   while(!ulCC3000Connected)
// 		{
// 			delay_ms(5); 
// 		}
		delay_ms(500);   //��ʱ��Ҫ��ô��   	
	//delay_ms(50);
	//GetInfo();
  GetdeviceInfo();	
	//ulWifiEvent = WIFI_CONNECT_FINISED;	
	ulWifiEvent = WIFI_SOCKETCONNECTING;
		
	return TRUE;
}
// unsigned char ConnectionAP(void)
// { 
// 	unsigned char ConnectedTimeout = 50;

//   if(ulWifiEvent == WIFI_CONNECTING)
//   {   
//     while (((ulCC3000DHCP == 0) || (ulCC3000Connected == 0))&&(ConnectedTimeout > 0))
//     {    
//       delay_ms(200); 
//       hci_unsolicited_event_handler();
//       ConnectedTimeout--;
//     } 
//     if(ConnectedTimeout == 0)
//     {
//       if(ulCC3000DHCP == 0 || ulCC3000Connected == 0)
//       {
//         Resetcc3000();
//        //ulWifiEvent = WIFI_SMARTCONFIG;
// 				ulWifiEvent =WIFI_CONNECTING ;
//         return 0;
//       }
//     }     
//     GetdeviceInfo();  
//   }
//   return 1;
// }

int TCPClient()
{
	unsigned ret;
	Set_ulSocket(TCPClientMode);    
	delay_ms(1000);       //nevinxu    ����Ҫ��ʱ��ô��!!!!!!
//	ret = Set_TCP(DEVICE_LAN_IP,DEVICE_LAN_PORT,TCPClientMode);
	if(!ret)
	{
		ulWifiEvent = WIFI_CONNECT_FINISED;
	}
	else
	{
		Resetcc3000();
		delay_ms(1000);
		ulWifiEvent = WIFI_CONNECTING;
	}
	return 0;
}

int TCPServer()
{
	Set_ulSocket(TCPServerMode);    
	delay_ms(1000);     //nevinxu    ����Ҫ��ʱ��ô��!!!!!!
	//Set_TCP(INADDR_ANY,SERVER_LAN_PORT,TCPServerMode);
	ulWifiEvent = WIFI_CONNECT_FINISED;
	return 0;
}




int UDPProtocal()
{
	unsigned ret;
	Set_ulSocket(UDPMode); 
	delay_ms(1000);       //nevinxu    ????????!!!!!!
	ret = Set_UDP(deviceLanPort[1]+(deviceLanPort[0]<<8));
	if(!ret)
	{
		ulWifiEvent = WIFI_CONNECT_FINISED;
	}
	else
	{
		Resetcc3000();
		delay_ms(1000);
		ulWifiEvent = WIFI_CONNECTING;
	}
	return ret;
}


int ConnectUsingSSID(char * ssidName,unsigned char *ssidSecurity)
{   
    wlan_ioctl_set_connection_policy(0, 0, 0);
    wlan_disconnect();
    delay_ms(100); 
    wlan_connect(WLAN_SEC_WPA2, ssidName, strlen(ssidName), 
                 NULL, ssidSecurity,strlen((char*)ssidSecurity)); 
	
	  //while(wlan_ioctl_set_connection_policy(DISABLE, ENABLE, ENABLE));
    
    //delay_ms(50);
      
		while(!ulCC3000Connected)
		{
			delay_ms(5); 
		}
		delay_ms(500);   //��ʱ��Ҫ��ô��  
		return 0;		
}


void SetTcpConnect(unsigned char TCPIPMode)
{
	if(TCPIPMode == TCPClientMode)
	{
			TCPClient();
	}
	else if(TCPIPMode == TCPServerMode)
	{
			TCPServer();
	}  
	else if(TCPIPMode == UDPMode)
	{
			UDPProtocal();
	}
}


/*********************Modify cai*******************************/
int Connect_Ssid(char * ssidName,unsigned char *ssidSecurity) 
{
	wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);
	wlan_ioctl_del_profile(255);
	
	delay_ms(50);
	if(!wlan_connect(WLAN_SEC_WPA2, ssidName, strlen(ssidName), 
                 NULL, ssidSecurity,strlen((char*)ssidSecurity)))
	{
		//while(wlan_ioctl_set_connection_policy(DISABLE, ENABLE, ENABLE)
		while(wlan_ioctl_set_connection_policy(DISABLE, ENABLE, ENABLE));
		delay_ms(50);
      
    ulWifiEvent = WIFI_CONNECTING;

		if(!ConnectionAP())
		{
			return FALSE;
		}
		return TRUE;	
	}
	return 	FALSE;
}


unsigned char  GetdeviceInfo(void)
{

	netapp_ipconfig(&Ipconfig);
  
//	Deviceipaddr = ((Ipconfig.aucIP[0]<<24)+(Ipconfig.aucIP[1]<<16)+(Ipconfig.aucIP[2]<<8)+Ipconfig.aucIP[3]);
//	if(ipaddr_aton(Ipconfig.aucIP,&ipaddr)<1)
//  	return 1;
//	else
	return 0;
}

void  Wifi_Scan()
{
	long ret;
	unsigned char ScanResult[1000];
	unsigned long aiIntervalList[2];
//	long result_num;
	aiIntervalList[0] = 2000;
	aiIntervalList[1] = 2000;
	ret = wlan_ioctl_set_scan_params(1, 100,
													100,
													5,
													0x7ff,-120,
													0,
													205,
													aiIntervalList);
	delay_ms(5000);
	if(ret == 0)
	{
		ret = wlan_ioctl_get_scan_results(0,
                            ScanResult);
	}
	//result_num = ScanResult[0]+(ScanResult[1]<<8)+(ScanResult[2]<<16)+(ScanResult[3]<<24);
	delay_ms(5000);
}

void Wifi_send_data(unsigned char *data,unsigned short Len)
{ 
  static unsigned char timesCnt = 0;
	sockaddr tSocketAddr;
	
  
	tSocketAddr.sa_family = AF_INET;
  
  // the source port
  tSocketAddr.sa_data[0] = deviceLanPort[0];
  tSocketAddr.sa_data[1] = deviceLanPort[1];
	  
  tSocketAddr.sa_data[2] = deviceLanIP[0];
  tSocketAddr.sa_data[3] = deviceLanIP[1];
  tSocketAddr.sa_data[4] = deviceLanIP[2];
  tSocketAddr.sa_data[5] = deviceLanIP[3];
	
	
	
  if(ulWifiEvent == WIFI_SEND_RECV)
  {
    
    if((WIFI_TX_BUF_MAX - WIFITxLen) >= Len)
    {
      memcpy(&WIFITxBuf[WIFITxLen],data,Len);
      WIFITxLen += Len; 
    }
		if(sendto(ulSocket, WIFITxBuf, WIFITxLen, 0,&tSocketAddr, sizeof(tSocketAddr)))
  //  if (send(ulSocket, WIFITxBuf, WIFITxLen, 0))
			
    {
      timesCnt = 0; 
    }
    else
    {
      if(timesCnt++ >= 10)
      {
        timesCnt = 0;
        ulWifiEvent = WIFI_NON_CONNECTING;
      }
    }
    WIFITxLen = 0;  
  }
  else
  {
    timesCnt = 0;
  }
}

void WifiRecvData(void)
{
	sockaddr m_tSocketAddr_r;
	socklen_t  tRxPacketLength;	
	
  unsigned char iReturnValue;   //��ʱ��ʾ���յ�ÿ�����Ϊ256
	CC3000Rxlen = 0;	
	
  if(ulWifiEvent == WIFI_SEND_RECV)
  {
    do
    {
	 // iReturnValue = recv(ulSocket, pucCC3000_Rx_Buffer, 
		//					  CC3000_APP_BUFFER_SIZE, 0);
			
		iReturnValue = recvfrom(ulSocket, pucCC3000_Rx_Buffer, CC3000_APP_BUFFER_SIZE, 0, &m_tSocketAddr_r,&tRxPacketLength);
			
      if(iReturnValue > 0)
      {
        if((WIFI_RX_BUF_MAX - CC3000Rxlen) > iReturnValue)
        {
          memcpy(&CC3000RxBuf[CC3000Rxlen],pucCC3000_Rx_Buffer,iReturnValue);
          CC3000Rxlen += iReturnValue; 
        }
      }
      else
      {
        iReturnValue = 0;
      }
    }while(iReturnValue==CC3000_APP_BUFFER_SIZE);  //һ�����յ����ݴ���32���ֽ�
		memset(&pucCC3000_Rx_Buffer[0],0,sizeof(pucCC3000_Rx_Buffer));
  }
}


void Wifi_event_handler(void)
{
	static long wifistatus;
  if(ulWifiEvent != WIFI_SEND_RECV)
  {
    switch(ulWifiEvent)
    {
    case WIFI_NON_CONNECTING:
      {
        ClearEvent(ALL_EVENT_HANDLER);
        ulWifiEvent = WIFI_CONNECTING;
				wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);
				wlan_ioctl_del_profile(255);
				delay_ms(50);
				while(wlan_ioctl_set_connection_policy(ENABLE, ENABLE, ENABLE));
				delay_ms(50);
				Resetcc3000();
      } break;
    case WIFI_CONNECTING:
      {
        ClearEvent(ALL_EVENT_HANDLER);
				while((ulCC3000DHCP == 0)||(ulCC3000Connected ==0))
			{
        delay_ms(200); 
        hci_unsolicited_event_handler();  
			}
				wifistatus = wlan_ioctl_statusget();
				if(wifistatus == 0)
				{
					ConnectUsingSSID(ConnectedssidName,ConnectedssidSecurity);
					ulWifiEvent = WIFI_CONNECT_FINISED;
				}
			//	
      }break;
//		case 	WIFI_SOCKETCONNECTING:
//		 { 
//			SetTcpConnect(UDPMode);
//		  ClearEvent(ALL_EVENT_HANDLER);
//		 } break;
			
    case WIFI_CONNECT_FINISED:
      {
        SetEvent(RECV_EVENT_HANDLER | SEND_EVENT_HANDLER |LED_EVENT_HANDLER);
        ulWifiEvent = WIFI_SEND_RECV;
				DisplayStatus = DataDisplayStatus;
      }break;
    case WIFI_SMARTCONFIG:
      {
        ClearEvent(ALL_EVENT_HANDLER);
        StartSmartConfig();
        ClearEvent(ALL_EVENT_HANDLER);
      }break;
    case WIFI_SMARTCONFIG_FINISED:
      {
        SetEvent(RECV_EVENT_HANDLER | SEND_EVENT_HANDLER | LED_EVENT_HANDLER);      
        ulWifiEvent = WIFI_SEND_RECV;
      }break;
    default: break;
    }
  }
  else 
  { 
      //  m_tLoop(NULL);
		if(ulCC3000IRQDelayFlags == 1)
		{
					ulCC3000IRQDelayFlags = 0;
					ulCC3000Connected = 0;
					ulCC3000DHCP = 0;
					ulCC3000DHCP_configured = 0;
					init_spi();
					ulWifiEvent = WIFI_CONNECTING;
					ulCC3000Resetflags = 1;
		}
		else
		{
			WifiRecvData();
		}
    return;
  }
}

void CC3000SendPacket(u8 *TxBuffer,u8 Size)
{
	Set_UDP(deviceLanPort[1]+(deviceLanPort[0]<<8));
	Wifi_send_data(TxBuffer,Size);
}


