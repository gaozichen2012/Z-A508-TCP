#ifndef __TONE_H
#define __TONE_H

#define TONE_FUNENABLE
#ifdef  TONELABEL
	#define TONEAPI
#else
	#define TONEAPI	extern 
#endif
TONEAPI void TIM1_PWM_Init(void);
TONEAPI void Set_TIM1_PWM_Frequency(uint16_t TIM1_Period);
TONEAPI void Set_TIM1_PWM1_DutyCycle( uint16_t TIM1_Pulse);
TONEAPI void Test_PWM_LED(void);

TONEAPI	void TONE_PowerOnInitial(void);
TONEAPI	void TONE_Start(void);
TONEAPI	void TONE_Stop(void);
TONEAPI	void TONE_SetMute(IO_ONOFF OnOff);

#endif
