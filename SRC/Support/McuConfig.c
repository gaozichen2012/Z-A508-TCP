#include "McuConfig.h"

static u32 TimingDelay;

/******************************************************************************
;--------1-LED hardware macro define
******************************************************************************/
void LED_Init(void)
{
  GPIO_Init(GPIO_LED_Green,GPIO_PIN_LED_Green,GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIO_LED_Red,GPIO_PIN_LED_Red,GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIO_RSSI,GPIO_PIN_RSSI,GPIO_MODE_IN_PU_NO_IT);//RSSI
}

void Set_GreenLed(u8 state)
{
  switch(state)
  {
  case LED_ON:
      GPIO_WriteHigh(GPIO_LED_Green, GPIO_PIN_LED_Green);
    break;
  case LED_OFF:
      GPIO_WriteLow(GPIO_LED_Green, GPIO_PIN_LED_Green);
    break;
  }
}

void Set_RedLed(u8 state)
{
  switch(state)
  {
  case LED_ON:
      GPIO_WriteHigh(GPIO_LED_Red, GPIO_PIN_LED_Red);
    break;
  case LED_OFF:
      GPIO_WriteLow(GPIO_LED_Red, GPIO_PIN_LED_Red);
      break;
  }
}

/******************************************************************************
;--------2-TIM3:DELAY hardware macro define
******************************************************************************/
void Tim3_Timer_Init(void)
{
#ifdef HSI_CLK
  	/*BaseTime=1/(16000000/16)*(999+1)=1ms*/
	TIM3_TimeBaseInit(TIM3_PRESCALER_16 ,999);
	TIM3_PrescalerConfig(TIM3_PRESCALER_16,TIM3_PSCRELOADMODE_IMMEDIATE);
#endif
#ifdef HSE_CLK
    /*BaseTime=1/(24000000/8)*(2999+1)=1ms*/
	TIM3_TimeBaseInit(TIM3_PRESCALER_8 ,499);
	TIM3_PrescalerConfig(TIM3_PRESCALER_16,TIM3_PSCRELOADMODE_IMMEDIATE);
#endif
	TIM3_ARRPreloadConfig(ENABLE);
	TIM3_ITConfig(TIM3_IT_UPDATE , ENABLE);
	TIM3_Cmd(ENABLE);
}
/*
void TimingDelay_Decrement(void)
{
	TimingDelay--;
}

void Delay_ms(u16 nms)
{
  	
	TimingDelay = nms;
	
	while(TimingDelay != 0);
}*/

/******************************************************************************
;--------3-UART1:GD83 hardware macro define
******************************************************************************/
/*void DrvGD83_Init(void)
{
    GPIO_Init(GPIO_C_Reset,GPIO_PIN_C_Reset,GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIO_C_ONOFF,GPIO_PIN_C_ONOFF,GPIO_MODE_OUT_PP_LOW_FAST);
}

void GD83_ON(void)
{
  GPIO_WriteLow(GPIO_C_ONOFF,GPIO_PIN_C_ONOFF);
  GPIO_WriteHigh(GPIO_C_ONOFF,GPIO_PIN_C_ONOFF);
  //Delay_ms(1500);
  GPIO_WriteLow(GPIO_C_ONOFF,GPIO_PIN_C_ONOFF);
}*/

void Uart1_Init(void)
{
    UART1_DeInit();		/* 将寄存器的值复位 */
	
	/*
	 * 将UART1配置为：
	 * 波特率 = 115200
	 * 数据位 = 8
	 * 1位停止位
	 * 无校验位
	 * 使能接收和发送
	 * 使能接收中断
	 */
    UART1_Init((u32)345600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, \
    	UART1_PARITY_NO , UART1_SYNCMODE_CLOCK_DISABLE , UART1_MODE_TXRX_ENABLE);
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
    UART1_Cmd(ENABLE);
    
    GPIO_Init(GPIOA,GPIO_PIN_4,GPIO_MODE_IN_PU_NO_IT );//接收
}

/*******************************************************************************
 * 名称: UART1_ReceiveByte
 * 功能: UART1接收一个字符
 * 形参: 无
 * 返回: 接收到的字符
 * 说明: 无 
 ******************************************************************************/
u8 UART1_ReceiveByte(void)
{
	u8 USART1_RX_BUF; 
	
	/* 等待接收完成 */
	while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET)
	  	;
	
	USART1_RX_BUF = UART1_ReceiveData8();
	
	return  USART1_RX_BUF;
}

/******************************************************************************
;--------4-Audio&MIC hardware macro define
******************************************************************************/
void MIC_GPIO_Init(void)
{GPIO_Init(GPIO_MIC_Mute,GPIO_PIN_MIC_Mute,GPIO_MODE_OUT_PP_LOW_FAST);}

void AF_Mute_Init(void)
{GPIO_Init(GPIO_AF_Mute,GPIO_PIN_AF_Mute,GPIO_MODE_OUT_PP_LOW_FAST);}

void Noise_Mute_Init(void)
{GPIO_Init(GPIO_Noise_Mute,GPIO_PIN_Noise_Mute,GPIO_MODE_OUT_PP_LOW_FAST);}

void C_TEST_OUT_SET(void)
{
  GPIO_Init(GPIO_MIC_Mute,GPIO_PIN_MIC_Mute,GPIO_MODE_OUT_PP_LOW_FAST);
}

void MIC_IOMUT(IO_ONOFF OnOff)
{
  switch(OnOff)
  {
  case ON:
      GPIO_WriteHigh(GPIO_MIC_Mute,GPIO_PIN_MIC_Mute);
    break;
  case OFF:
      GPIO_WriteLow(GPIO_MIC_Mute,GPIO_PIN_MIC_Mute);
    break;
  }
}



void AUDIO_IOAFMUT(IO_ONOFF OnOff)
{
  switch(OnOff)
  {
  case ON:
      GPIO_WriteHigh(GPIO_AF_Mute, GPIO_PIN_AF_Mute);
    break;
  case OFF:
      GPIO_WriteLow(GPIO_AF_Mute, GPIO_PIN_AF_Mute);
    break;
  default:break; 
  }
}

void AUDIO_IOAFPOW(IO_ONOFF OnOff)
{
  switch(OnOff)
  {
  case ON:
      GPIO_WriteHigh(GPIO_Noise_Mute, GPIO_PIN_Noise_Mute);
    break;
  case OFF:
      GPIO_WriteLow(GPIO_Noise_Mute, GPIO_PIN_Noise_Mute);
    break;
  default:break; 
  }
}

/******************************************************************************
;--------5-KEY hardware macro define
******************************************************************************/
void Key_Init(void)
{
  GPIO_Init(GPIO_Key_2,GPIO_PIN_Key_2,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIO_Key_3,GPIO_PIN_Key_3,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIO_Key_4,GPIO_PIN_Key_4,GPIO_MODE_IN_PU_NO_IT);
  GPIO_Init(GPIO_Key_PTT,GPIO_PIN_Key_PTT,GPIO_MODE_IN_PU_NO_IT);
}

/******************************************************************************
;--------6-北斗及NFC hardware macro define
******************************************************************************/
