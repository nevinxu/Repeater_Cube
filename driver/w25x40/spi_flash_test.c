
//#include "stm32f10x.h"
#include "spi_flash.h"
#include "debug.h"
#include "spi_flash_ctrl.h"
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* ���ͻ�������ʼ�� */
u8 Tx_Buffer[] = "OK!";
u8 Rx_Buffer[BufferSize];

u8 DeviceID = 0;
volatile u32 FlashID = 0;
volatile TestStatus TransferStatus1 = FAILED;

TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/*
 * ��������Buffercmp
 * ����  ���Ƚ������������е������Ƿ����
 * ����  ��-pBuffer1     src������ָ��
 *         -pBuffer2     dst������ָ��
 *         -BufferLength ����������
 * ���  ����
 * ����  ��-PASSED pBuffer1 ����   pBuffer2
 *         -FAILED pBuffer1 ��ͬ�� pBuffer2
 */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


void FLSASH_Test_Ctrl(void)
{

  /* Get SPI Flash Device ID */
  DeviceID = SPI_FLASH_ReadDeviceID();
  
	Delay( 200 );

  /* Get SPI Flash ID */
  FlashID = SPI_FLASH_ReadID();

  printf("\r\n FlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);

  /* Check the SPI Flash ID */
  if (FlashID == sFLASH_ID)  /* #define  sFLASH_ID  0xEF3015 */
  {
  
    printf("\r\n ��⵽����flash W25X40 !\r\n");
    
    /* Erase SPI FLASH Sector to write on */
    SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 
   
    /* �����ͻ�����������д��flash�� */
    SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
		printf("\r\n д�������Ϊ��%s \r\t", Tx_Buffer);

    /* ���ո�д������ݶ������ŵ����ջ������� */
    SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("\r\n ����������Ϊ��%s \r\n", Tx_Buffer);

    /* ���д�������������������Ƿ���� */
    TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);

    if( PASSED == TransferStatus1 )
    {    
        printf("\r\n 4M����flash(W25X40)���Գɹ�!\n\r");
    }
    else
    {        
        printf("\r\n 4M����flash(W25X40)����ʧ��!\n\r");
    }
  }
  else
  {    
    printf("\r\n ��ȡ���� W25X40 ID!\n\r");
  }

  SPI_Flash_PowerDown();  
}


