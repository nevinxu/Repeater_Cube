#ifndef _CC1101_CONFIG_H_
#define _CC1101_CONFIG_H_

#include"stm32f10x.h"

#define BUSY_Flag  0x01 /* Ready/busy status flag */
#define Dummy_Byte 0xff

/*******************************************************************************/
#define 	WRITE_BURST     		0x40						//连续写入
#define 	READ_SINGLE     		0x80						//读
#define 	READ_BURST      		0xC0						//连续读
#define 	BYTES_IN_RXFIFO     0x7F  						//接收缓冲区的有效字节数
#define 	CRC_OK              0x80 						//CRC校验通过位标志
// CC1100 STROBE, CONTROL AND STATUS REGSITER
#define CCxxx0_IOCFG2       0x00        // GDO2 output pin configuration
#define CCxxx0_IOCFG1       0x01        // GDO1 output pin configuration
#define CCxxx0_IOCFG0       0x02        // GDO0 output pin configuration
#define CCxxx0_FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define CCxxx0_SYNC1        0x04        // Sync word, high INT8U
#define CCxxx0_SYNC0        0x05        // Sync word, low INT8U
#define CCxxx0_PKTLEN       0x06        // Packet length
#define CCxxx0_PKTCTRL1     0x07        // Packet automation control
#define CCxxx0_PKTCTRL0     0x08        // Packet automation control
#define CCxxx0_ADDR         0x09        // Device address
#define CCxxx0_CHANNR       0x0A        // Channel number
#define CCxxx0_FSCTRL1      0x0B        // Frequency synthesizer control
#define CCxxx0_FSCTRL0      0x0C        // Frequency synthesizer control
#define CCxxx0_FREQ2        0x0D        // Frequency control word, high INT8U
#define CCxxx0_FREQ1        0x0E        // Frequency control word, middle INT8U
#define CCxxx0_FREQ0        0x0F        // Frequency control word, low INT8U
#define CCxxx0_MDMCFG4      0x10        // Modem configuration
#define CCxxx0_MDMCFG3      0x11        // Modem configuration
#define CCxxx0_MDMCFG2      0x12        // Modem configuration
#define CCxxx0_MDMCFG1      0x13        // Modem configuration
#define CCxxx0_MDMCFG0      0x14        // Modem configuration
#define CCxxx0_DEVIATN      0x15        // Modem deviation setting
#define CCxxx0_MCSM2        0x16        // Main Radio Control State Machine configuration
#define CCxxx0_MCSM1        0x17        // Main Radio Control State Machine configuration
#define CCxxx0_MCSM0        0x18        // Main Radio Control State Machine configuration
#define CCxxx0_FOCCFG       0x19        // Frequency Offset Compensation configuration
#define CCxxx0_BSCFG        0x1A        // Bit Synchronization configuration
#define CCxxx0_AGCCTRL2     0x1B        // AGC control
#define CCxxx0_AGCCTRL1     0x1C        // AGC control
#define CCxxx0_AGCCTRL0     0x1D        // AGC control
#define CCxxx0_WOREVT1      0x1E        // High INT8U Event 0 timeout
#define CCxxx0_WOREVT0      0x1F        // Low INT8U Event 0 timeout
#define CCxxx0_WORCTRL      0x20        // Wake On Radio control
#define CCxxx0_FREND1       0x21        // Front end RX configuration
#define CCxxx0_FREND0       0x22        // Front end TX configuration
#define CCxxx0_FSCAL3       0x23        // Frequency synthesizer calibration
#define CCxxx0_FSCAL2       0x24        // Frequency synthesizer calibration
#define CCxxx0_FSCAL1       0x25        // Frequency synthesizer calibration
#define CCxxx0_FSCAL0       0x26        // Frequency synthesizer calibration
#define CCxxx0_RCCTRL1      0x27        // RC oscillator configuration
#define CCxxx0_RCCTRL0      0x28        // RC oscillator configuration
#define CCxxx0_FSTEST       0x29        // Frequency synthesizer calibration control
#define CCxxx0_PTEST        0x2A        // Production test
#define CCxxx0_AGCTEST      0x2B        // AGC test
#define CCxxx0_TEST2        0x2C        // Various test settings
#define CCxxx0_TEST1        0x2D        // Various test settings
#define CCxxx0_TEST0        0x2E        // Various test settings

// Strobe commands
#define CCxxx0_SRES         0x30        // Reset chip.
#define CCxxx0_SFSTXON      0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                        // If in RX/TX: Go to a wait state where only the synthesizer is
                                        // running (for quick RX / TX turnaround).
#define CCxxx0_SXOFF        0x32        // Turn off crystal oscillator.
#define CCxxx0_SCAL         0x33        // Calibrate frequency synthesizer and turn it off
                                        // (enables quick start).
#define CCxxx0_SRX          0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                        // MCSM0.FS_AUTOCAL=1.
#define CCxxx0_STX          0x35        // In IDLE state: Enable TX. Perform calibration first if
                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                        // Only go to TX if channel is clear.
#define CCxxx0_SIDLE        0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                        // Wake-On-Radio mode if applicable.
#define CCxxx0_SAFC         0x37        // Perform AFC adjustment of the frequency synthesizer
#define CCxxx0_SWOR         0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define CCxxx0_SPWD         0x39        // Enter power down mode when CSn goes high.
#define CCxxx0_SFRX         0x3A        // Flush the RX FIFO buffer.
#define CCxxx0_SFTX         0x3B        // Flush the TX FIFO buffer.
#define CCxxx0_SWORRST      0x3C        // Reset real time clock.
#define CCxxx0_SNOP         0x3D        // No operation. May be used to pad strobe commands to two
                                        // INT8Us for simpler software.

#define CCxxx0_PARTNUM      0x30
#define CCxxx0_VERSION      0x31
#define CCxxx0_FREQEST      0x32
#define CCxxx0_LQI          0x33
#define CCxxx0_RSSI         0x34
#define CCxxx0_MARCSTATE    0x35
#define CCxxx0_WORTIME1     0x36
#define CCxxx0_WORTIME0     0x37
#define CCxxx0_PKTSTATUS    0x38
#define CCxxx0_VCO_VC_DAC   0x39
#define CCxxx0_TXBYTES      0x3A
#define CCxxx0_RXBYTES      0x3B

#define CCxxx0_PATABLE      0x3E
#define CCxxx0_TXFIFO       0x3F
#define CCxxx0_RXFIFO       0x3F



#define CC1101_CSN_0  GPIO_ResetBits(GPIOB,GPIO_Pin_12);  //CS enable
#define CC1101_CSN_1  GPIO_SetBits(GPIOB,GPIO_Pin_12);	 //CS disable

#define  	RCC_APB1Periph_SPI_CC1101				RCC_APB1Periph_SPI2
#define  	RCC_APB2Periph_GPIO_CC1101			RCC_APB2Periph_GPIOB
#define  	RCC_APB2Periph_GPIO_GDO0				RCC_APB2Periph_GPIOB
#define  	RCC_APB2Periph_GPIO_GDO2				RCC_APB2Periph_GPIOA
#define  	CC1101SPI    	SPI2
#define 	CC1101GPIO		GPIOB
#define 	CC1101MISO		GPIO_Pin_14
#define 	CC1101MOSI		GPIO_Pin_15
#define 	CC1101SCK			GPIO_Pin_13
#define 	CC1101CSN			GPIO_Pin_12


//#define 	GDO0_GPIO			GPIOA
//#define 	GDO0					GPIO_Pin_8
//#define 	GDO2_GPIO			GPIOB
//#define 	GDO2					GPIO_Pin_11
#define 	GDO0_GPIO			GPIOB
#define 	GDO0					GPIO_Pin_11
#define 	GDO2_GPIO			GPIOA
#define 	GDO2					GPIO_Pin_8

#define 	RCC_APB2Periph_GDO2			RCC_APB2Periph_GPIOA
#define 	GPIO_PortSourceGPO2			GPIO_PortSourceGPIOA
#define 	GPIO_PinSourceGPO2			GPIO_PinSource8

//#define 	RCC_APB2Periph_GDO2			RCC_APB2Periph_GPIOB
//#define 	GPIO_PortSourceGPO2			GPIO_PortSourceGPIOB
//#define 	GPIO_PinSourceGPO2			GPIO_PinSource11

#define 	EXTI_Line_GPO2					EXTI_Line8
#define 	EXTI_IRQn_GPO2					EXTI9_5_IRQn

//#define 	EXTI_Line_GPO2					EXTI_Line11
//#define 	EXTI_IRQn_GPO2					EXTI9_5_IRQn

#define CC1101RECDATABIT						0x01

//SPI初始化
//从SPI读一个字节 
u8 SPI_ReadByte(void);
//向SPI读写一个字节 
u8 SPI_ReadWrite(u8 writedat);

void SpiC1101Init(void);
//向对应地址写数据
void SPiCWriteReg(u8 Addr,u8 Value);
//连续写配置寄存器
void SpiCWriteBurstReg(u8 Addr,u8 *Buffer,u8 Count);
// 读取寄存器值
u8 SpiCReadReg(u8 Addr);
//读取以addr地址开始的一串数据，存入Buffer
void SpiCReadBurstReg(u8 Addr,u8 *Buffer,u8 Count);
//读取状态
u8 SpiCReadStatus(u8 Addr);
//发送命令
void SpiCStrobe(u8 Strobe);
//上电复位
void SPiCPowerUpReset(void);
//寄存器配置
void WriteRfSetting(void);
//发送一组数组
extern void CC1101SendPacket(u8 *TxBuffer,u8 Size);
//接收数据 
extern u8 CC1101ReceivePacket(u8 *RxBuffer); 
//初始化
extern void CC1101Init(void);

extern void CC1101AddSet(unsigned int  addr);

extern void CC1101SYNCSet(unsigned int  addr);
#endif

