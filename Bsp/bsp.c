#include "includes.h" 
#include "evnt_handler.h"
#include "wifi_application.h"

GPIO_TypeDef* LED_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT,
                                LED4_GPIO_PORT};
const uint16_t LED_PIN[LEDn] ={
    LED1_GPIO_PIN, LED2_GPIO_PIN, LED3_GPIO_PIN,
    LED4_GPIO_PIN
};
	

//��̬��������
static void SysTick_Configuration(void);
volatile unsigned int delayCount = 0;
volatile unsigned char Key_event_Flag = 0;
volatile unsigned short Key_event_timeout = 0;
volatile unsigned short Relay_Flag = 0;



static void SysTick_Configuration(void)
{
    if (SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC))
    { 
        /* Capture error */ 
        while (1);
    }
}
 
uint32_t  BSP_CPU_ClkFreq (void)
{
  RCC_ClocksTypeDef  rcc_clocks;
  RCC_GetClocksFreq(&rcc_clocks);
  return ((uint32_t)rcc_clocks.HCLK_Frequency);
}

uint32_t  OS_CPU_SysTickClkFreq (void)
{
  uint32_t  freq;
	freq = BSP_CPU_ClkFreq();
  return (freq);
}

//��ʱ����
void delay_ms(unsigned int ms)
{
  delayCount = (int) ms/10;
  while (delayCount > 0);
}

//IO��ʼ��
void HW_IO_init(void)
{
  GPIO_InitTypeDef gpio;
  
  RCC_APB2PeriphClockCmd(LED1_CLOCK, ENABLE);    //LEDʱ�ӳ�ʼ��
	RCC_APB2PeriphClockCmd(LED2_CLOCK, ENABLE);    //LEDʱ�ӳ�ʼ��
	RCC_APB2PeriphClockCmd(LED3_CLOCK, ENABLE);    //LEDʱ�ӳ�ʼ��
	RCC_APB2PeriphClockCmd(LED4_CLOCK, ENABLE);    //LEDʱ�ӳ�ʼ��
  
//  //Remap JTAG (SWD ONLY)
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  //״ָ̬ʾ��
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED1_GPIO_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED1_GPIO_PORT, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED2_GPIO_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED2_GPIO_PORT, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED3_GPIO_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED3_GPIO_PORT, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED4_GPIO_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED4_GPIO_PORT, &gpio);

  SetLEDStatus(LED1,FALSE);  //�ϵ��Ĭ��Ϊ�ر�״ָ̬ʾ��
	SetLEDStatus(LED2,FALSE);  //�ϵ��Ĭ��Ϊ�ر�״ָ̬ʾ��
	SetLEDStatus(LED3,FALSE);  //�ϵ��Ĭ��Ϊ�ر�״ָ̬ʾ��
	SetLEDStatus(LED4,FALSE);  //�ϵ��Ĭ��Ϊ�ر�״ָ̬ʾ��
  
}




void TIM2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  
  TIM_DeInit(TIM2);                                            //��λTIM2��ʱ��
  
  /* TIM2 configuration */
  TIM_TimeBaseStructure.TIM_Period = 200;                     // �Զ���װ�Ĵ�����ֵ(��ʱ����100ms)  
  TIM_TimeBaseStructure.TIM_Prescaler = 35999;                 // ʱ��Ԥ��Ƶ��     
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // ʱ�ӷ�Ƶ 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //�����������ϼ���
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);                        // Clear TIM2 update pending flag[���TIM2����жϱ�־] 
   
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                   // Enable TIM2 Update interrupt [TIM2����ж�����]
    
  TIM_Cmd(TIM2, ENABLE);                                       // TIM2 enable counter [����tim2����]
  
  //�������ȼ���
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  //����TIM2��
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  //�����������ȼ�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  //������Ӧ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //ʹ�����ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //���ô���Ĵ���
  NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
}


void BSP_Init(void)
{
#ifdef DEBUG_IN_RAM
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0);
#endif 
    SysTick_Configuration();
	
	  HW_IO_init();	
		TIM2_Configuration();    //��ʱ����ʼ��  ���ڰ���   ������ʱ��

		/* LSI������*/
		RCC_LSICmd(ENABLE);//��LSI
		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);//�ȴ�ֱ��LSI�ȶ�
	
#ifdef IWDGENABLE	
		//�����������Ź�
		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //����֮ǰҪ����ʹ�ܼĴ���д
		IWDG_SetPrescaler(IWDG_Prescaler_64);//64��Ƶ һ������1.6ms
		IWDG_SetReload(4095);//�12λ [0,4096] 800*1.6=1.28S
		/* Reload IWDG counter */
		IWDG_ReloadCounter();
		IWDG_Enable();// Enable IWDG (the LSI oscillator will be enabled by hardware)
#endif

}


void SetLEDStatus(Led_TypeDef Led, unsigned char status)
{
  if(!status)
  {
    GPIO_SetBits(LED_PORT[Led], LED_PIN[Led]);
  }
  else
  {
    GPIO_ResetBits(LED_PORT[Led], LED_PIN[Led]);
  }
}

void SetLEDToggle(Led_TypeDef Led)
{
  if(GPIO_ReadInputDataBit(LED_PORT[Led],LED_PIN[Led]))
  {
    GPIO_ResetBits(LED_PORT[Led],LED_PIN[Led]);
  }
  else
  {
    GPIO_SetBits(LED_PORT[Led], LED_PIN[Led]); 
  }
}

void SetRELAYToggle(Led_TypeDef Led)
{
  if(GPIO_ReadInputDataBit(LED_PORT[Led],LED_PIN[Led] ))
  {
    GPIO_ResetBits(LED_PORT[Led],LED_PIN[Led] );
    Relay_Flag = 0;
  }
  else
  {
    GPIO_SetBits(LED_PORT[Led],LED_PIN[Led]);
    Relay_Flag = 1; 
  }
}

//��ʱ��2�жϺ���
void TIM2_IRQHandler(void)
{ 
  static unsigned char hci_unsolicited_timeout = 0;
  static unsigned char wifi_status_led_timeout = 0;
  static unsigned char sendratadata_timeout = 0;

  if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
  {
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    
    if (ulWifiEvent == WIFI_SEND_RECV)
    {
      if(++hci_unsolicited_timeout>=5)
      {
        hci_unsolicited_timeout = 0;
        hci_unsolicited_event_handler();
      }
      if(++sendratadata_timeout >= 10)
      {
        sendratadata_timeout =0;
      }
       
      SetLEDStatus(LED2,TRUE); 
    }  
    else if(ulWifiEvent == WIFI_CONNECTING)
    {
      if(wifi_status_led_timeout++>=5)
      {
        wifi_status_led_timeout = 0;
				SetLEDStatus(LED2,FALSE);
        SetLEDToggle(LED3);
      }
    }
		else if(ulWifiEvent == WIFI_SOCKETCONNECTING)  //��������TCP/IP
    {
      if(wifi_status_led_timeout++>=2)
      {
        wifi_status_led_timeout = 0;
				SetLEDStatus(LED3,TRUE);
        SetLEDToggle(LED2);
      }
		}
		
    else if(ulWifiEvent == WIFI_SMARTCONFIG)
    {
      SetLEDToggle(LED1);
    }
    else
    {
;
    } 
  }
}


/*******************************************************************************
 * Function Name  : SysTick_Handler
 * Description    : This function handles SysTick Handler.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SysTick_Handler(void)
{ 
	
    OS_CPU_SR  cpu_sr;

    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
	  GetEvent();
  if(delayCount)delayCount--;
    OS_EXIT_CRITICAL();

    OSTimeTick();                                /* Call uC/OS-II's OSTimeTick()                       */

    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR          */
}
   





#ifdef USE_FULL_ASSERT
/*
 Function    : assert_failed
 Description : Reports the name of the source file and the source
 line number where the assert_param error has occurred.
 Input       : - file: pointer to the source file name
 - line: assert_param error line source number
 Output      : None
 Return      : None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /*
     User can add his own implementation to report the file name
     and line number, ex:
     printf("Wrong parameters value: file %s on line %d\r\n", file, line)
     */
    /* Infinite loop */
    while (1)
    {
    }
}
#endif

