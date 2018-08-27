#ifndef __TASK_H
#define __TASK_H

#ifdef TASKABLE
#define TASKAPI
#else 
#define TASKAPI extern
#endif

typedef enum {					//task id code process
	TASK_IDLE 		= 0x00,
	TASK_ERROR		= 0x01,
	TASK_COPY		= 0x02,
	TASK_LOBATT 	        = 0x03,
	TASK_ORDER 		= 0x04,
	TASK_SCAN		= 0x05,
	TASK_EMGE 		= 0x06,
	TASK_REPEAT		= 0x07,
	TASK_WRITEFREQ	= 0x08,
	TASK_mPowerOff	= 0x09,
	TASK_UpdataCdma	= 0x0A,
	TASK_HIBATT		= 0x0B,
	Task_Start              =0x0C,//Tom added in 2017.11.17
        Task_NormalOperation    =0x0D,
	TASK_RESET		= 0xFF
}TASK_CODE;

typedef enum{
  Key3_OptionZero       =       0x00,
  Key3_OptionOne        =       0x01,
  Key3_OptionTwo        =       0x02,
  Key3_OptionThree      =       0x03,
  Key3_OptionFour       =       0x04,
  Key3_OptionFive       =       0x05
}Key3_OptionType;

TASKAPI bool KEY_4_Flag;
TASKAPI Key3_OptionType Key3Option;
TASKAPI bool EnterPttMoment_Flag;
TASKAPI bool NoUseNum;
TASKAPI u8 NetworkType_2Gor3G_Flag;
TASKAPI bool TASK_Ptt_StartPersonCalling_Flag;
TASKAPI bool Task_Landing_Flag;
TASKAPI void Delay_100ms(u8 T);
TASKAPI u8 Key_PersonalCalling_Flag;
TASKAPI void Task_RunStart(void);
TASKAPI void Task_RunNormalOperation(void);
TASKAPI void TASK_WriteFreq(void);
TASKAPI void Task_RunNormalOperation(void);
TASKAPI void TASK_RunLoBattery(void);
TASKAPI u8 PersonCallIco_Flag;//根据显示组呼个呼图标判断状态
TASKAPI bool EnterKeyTime_2s_Flag;
TASKAPI u8 TaskStartDeleteDelay1;
TASKAPI u8 TaskStartDeleteDelay2;
TASKAPI u8 TaskStartDeleteDelay3;
TASKAPI u8 TaskStartDeleteDelay4;
TASKAPI u8 TaskStartDeleteDelay5;
TASKAPI u8 TaskStartDeleteDelay6;
extern u8 key_warning_flag;
extern u8 key_warning_bubiao_flag;
#endif