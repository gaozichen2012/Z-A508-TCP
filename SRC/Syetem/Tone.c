#define  TONELABEL
#include "AllHead.h"
#ifdef TONE_FUNENABLE

#define TONE_IDLE 	0x00
#define TONE_RUN 	0x01
#define TONE_MAXFREQ	0x02

  u16 Duty_Val;
  u8 j=0;

typedef struct {						//DA tone freq parameter process
	u8 cLevel;
	u16  iStep;
	WordType iStepSum;
}TONE_PAR;

typedef struct 
	{
	union 
		{
		struct {
			u8 bSyn 	: 1;
			u8 bSet 	: 1;
			u8 bRun		: 1;
			u8 bMute	: 1;
			u8 			: 4;
		}Bit;
		u8 Byte;
	}Msg;
	u8 Number;
	u16  Freq0;
	u8 Level0;
	u16  Freq1;
	u8 Level1;
	TONE_PAR TonePar[TONE_MAXFREQ];
}TONE_DRV;

static void Delay(u32 nCount);
static TONE_DRV	ToneDrvObj;
void TIM1_PWM_Init(void)
{
	GPIO_Init(GPIO_BEEP, GPIO_PIN_BEEP, GPIO_MODE_OUT_PP_LOW_FAST);
        TIM1_DeInit();
        /*
	 * TIM1 Frequency = TIM1 counter clock/(ARR + 1) 
	 * 这里设置TIM2的计数频率为 8M/8/(999+1)=1K
	 */
       // TIM1_TimeBaseInit(1, TIM1_COUNTERMODE_UP, 4000, 0);
	//TIM1_TimeBaseInit(7, TIM1_COUNTERMODE_UP, 999, 0);//8M/(7+1)/(999+1)=1KHz
        //TIM1_TimeBaseInit(1, TIM1_COUNTERMODE_UP, 999, 0);//8M/(1+1)/(999+1)=4KHz
        //TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, 999, 0);//8M/(0+1)/(999+1)=8KHz
        TIM1_TimeBaseInit(3, TIM1_COUNTERMODE_UP, 1999, 0);//8M/(3+1)/(199+1)=10KHz---分出10K的分辨率方便后面计算
	/* 
	 * PWM1 Mode configuration: Channel1
	 * TIM2 Channel1 duty cycle = [TIM2_CCR1/(TIM2_ARR + 1)] * 100 = 50%
	 * TIM2 Channel2 duty cycle = [TIM2_CCR2/(TIM2_ARR + 1)] * 100 = 50%
	 */ 
//中断定时
           TIM1_SetCounter(0);/* 将计数器初值设为0 */
           TIM1_ARRPreloadConfig(DISABLE);	/* 预装载不使能 */
           TIM1_ITConfig(TIM1_IT_UPDATE , ENABLE);	/* 计数器向上计数/向下计数溢出更新中断 */
	/* 测试通道1 */
	TIM1_OC1Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_ENABLE,TIM1_OUTPUTNSTATE_ENABLE ,1000,
                     TIM1_OCPOLARITY_LOW,TIM1_OCNPOLARITY_HIGH,TIM1_OCIDLESTATE_SET,TIM1_OCNIDLESTATE_SET);
	TIM1_CCxCmd(TIM1_CHANNEL_1, ENABLE);  
        TIM1_OC1PreloadConfig(ENABLE);
        TIM1_CtrlPWMOutputs(ENABLE);
        TIM1_Cmd(ENABLE);
}

/*******************************************************************************
 * 名称: Set_TIM2_PWM_Frequency
 * 功能: 设置TIM2-PWM频率
 * 形参: TIM2_Period -> 要设置的频率
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Set_TIM1_PWM_Frequency(uint16_t TIM1_Period)
{
	/* 设置PWM频率 */
	TIM1->ARRH = (uint8_t)(TIM1_Period >> 8);
	TIM1->ARRL = (uint8_t)(TIM1_Period);
}

/*******************************************************************************
 * 名称: Set_TIM1_PWM1_DutyCycle
 * 功能: 设置TIM1-PWM通道1占空比
 * 形参: TIM1_Pulse -> 要设置的占空比
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Set_TIM1_PWM1_DutyCycle( uint16_t TIM1_Pulse)
{
    /* 设置通道1占空比 */
    TIM1->CCR1H = (uint8_t)(TIM1_Pulse >> 8);
    TIM1->CCR1L = (uint8_t)(TIM1_Pulse);
}

/*******************************************************************************
 * 名称: Test_PWM_LED
 * 功能: 测试通道2
 * 形参: 无
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void Test_PWM_LED(void)
{

  BEEP_SetOutput(BEEP_IDPowerOff,ON,0x00,TRUE);

       if(j>=1)
	{
          Duty_Val+=100;
          if(Duty_Val>=3300)
          {
            Duty_Val=3200;
            j=0;
            Set_RedLed(LED_ON);
            Set_GreenLed(LED_OFF);
          }
	}
	else
	{
          Duty_Val-=100;
          if(Duty_Val>3300)
          {
            Duty_Val=3200;
          }
          if(Duty_Val<=1300)
          {
            Duty_Val=1200;
            j++;
            Set_RedLed(LED_OFF);
            Set_GreenLed(LED_OFF);
          }
	}
      Set_TIM1_PWM_Frequency(Duty_Val);
      Set_TIM1_PWM1_DutyCycle(Duty_Val/2);

}

static void Delay(u32 nCount)
{
	/* Decrement nCount value */
	while (nCount != 0)
	{
		nCount--;
	}
}

void TONE_PowerOnInitial(void)
{
	u8 i;
	
        TIM1_PWM_Init();
	
	for(i = 0; i < TONE_MAXFREQ; i++)
	{
		ToneDrvObj.TonePar[i].cLevel  = 0;
		ToneDrvObj.TonePar[i].iStep   = 0;  
		ToneDrvObj.TonePar[i].iStepSum.Word   = 0;
	}
	ToneDrvObj.Number   = 0x00;
	ToneDrvObj.Freq0	= 0x00;
	ToneDrvObj.Level0	= 0x00;
	ToneDrvObj.Freq1	= 0x00;
	ToneDrvObj.Level1	= 0x00;
	ToneDrvObj.Msg.Byte = TONE_IDLE;
	return;
}

void TONE_Start(void)
{

	if (ToneDrvObj.Msg.Bit.bRun == TONE_IDLE)
	{
		ToneDrvObj.Msg.Bit.bRun = TONE_RUN;
		ToneDrvObj.Msg.Bit.bSet = TONE_RUN;
		TIM1_CtrlPWMOutputs(ENABLE);
	}
	else
	{
		TONE_Stop();
	}
	return;
}
void TONE_Stop(void)
{
	ToneDrvObj.Msg.Byte = TONE_IDLE;
	TIM1_CtrlPWMOutputs(DISABLE);
	return;
}

void TONE_SetMute(IO_ONOFF OnOff)
{
	if (OnOff == OFF)
	{
		GPIO_WriteLow(GPIOD, GPIO_PIN_7);//Noise-Mute
	}
	else
	{
		GPIO_WriteHigh(GPIOD, GPIO_PIN_7);//Noise-Mute
	}
	return;
}

#endif