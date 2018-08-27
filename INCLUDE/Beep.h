/******************************************************************************
; File Name		: Beep.h
; Author		: ZhangYaLing
; Version		: 1.0
; Description	: task driver program process
; Date			: 2010.06.01
; Modify		:
******************************************************************************/
#ifndef BEEP_H
#define BEEP_H

#define BEEP_FUNENABLE

#ifdef	BEEPLABEL
	#define BEEPAPI
#else
	#define BEEPAPI	extern
#endif

typedef enum {
	BEEP_IDFREE		= 0x00,
	BEEP_IDPOWER 	= 0x01,
	BEEP_IDKEY		= 0x02,
	BEEP_IDNULLCH	= 0x03,
	BEEP_IDBUSY		= 0x04,
	BEEP_IDLOBATT	= 0x05,
	BEEP_IDSIG		= 0x06,
	BEEP_IDTOT		= 0x07,	
	BEEP_IDEMG		= 0x08,
	BEEP_IDHIBATT	= 0x09,
	BEEP_IDLOBATTArm =0x0A,
	BEEP_IDPowerOff	= 0x0b,
	
	BEEP_IDALLOFF	= 0xFF
}BEEP_TYPE;

BEEPAPI	void BEEP_PowerOnInitial(void);
BEEPAPI	void BEEP_LoadData(void);
BEEPAPI	bool BEEP_SetCommand(u8* pBuf);
BEEPAPI	u8 BEEP_GetCommand(u8* pBuf);
BEEPAPI void BEEP_SetOutput(BEEP_TYPE Id,IO_ONOFF bOnOFF,u8 cIndex,bool bRenew);
BEEPAPI	void BEEP_Interrupt10ms(void);
BEEPAPI bool BEEP_GetStatus(BEEP_TYPE Id);
BEEPAPI bool BEEP_GetPttCfg(void);
BEEPAPI bool BEEP_GetLowBattCfg(void);
BEEPAPI u8 BEEP_GetOffLineToneIntervalCfg(void);
BEEPAPI void BEEP_Time(u16 x);

#endif
