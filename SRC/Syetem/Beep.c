/******************************************************************************
; File Name		: Beep.c
; Author		: ZhangYaLing
; Version		: 1.0
; Description	: task driver program process
; Date			: 2010.06.01
; Modify		:
******************************************************************************/
#define  BEEPLABEL
#include "AllHead.h"
#ifdef	BEEP_FUNENABLE

/******************************************************************************
;
******************************************************************************/
#define BEEPMAX 		1
#define BEEPMASK		0x0f

#define BEEP_IDLE 		0
#define BEEP_RUN		1

#define BEEP_INIT		0x00
#define BEEP_ON			0x01
#define BEEP_OFF		0x02

#define BEEP_INDPOW		0x00
#define BEEP_INDPOWOff	0x01
#define BEEP_INDKEY		0x02        
#define BEEP_INDLOBATT	0x03
#define BEEP_INDHiBATT	0x04
#define BEEP_INDBUSY	0x05        
#define BEEP_INDTOT		0x06

typedef struct {								//beep run parame
	u8 Number;
	u8 OnDelay;
	u8 OffDelay;
	u16  OnFreq;
	u16  OffFreq;
}BeepParType;

const BeepParType BeepPar[] = {
	{0x01, 50, 0x00, 1000, 0x00},
	{0x01, 30, 30, 800, 1000},
	{0x02, 15, 15, 1800, 900},
	{0x01, 20, 0x00, 1000, 0x00},
	{0x02, 15, 15, 1600, 0x00},
	{0x01, 50, 0x00, 1500, 0x00},
	{0xFF, 50, 50, 1500, 600},
	{0xFF, 20, 0x00, 1800, 0x00},
	{0xFF, 50, 0x00, 1350, 0x00}
};

/******************************************************************************
;
******************************************************************************/
typedef struct {
	struct {								//beep set parame value
		union {
			struct {
				u8 bPow	: 1;
				u8 bKey	: 1;
				u8 bPtt	: 1;
				u8 bLowBatt : 1;
				u8 bRes : 4;
			}Bit;
			u8 Byte;
		}Opt;
		u8 ucBeepVolume;
		u8 Res[3];
	}Par;
	union {									//beep run status
		struct {
			u8 bSet		: 1;			
			u8 			: 7;
		}Bit;
		u8 Byte;
	}Msg;
	union {									//beep run task
		struct {
			u16 bPower		: 1;
			u16 bPowerOff		: 1;
			u16 bKey		: 1;
			u16 bLoBatt	: 1;
			u16 bHiBatt	: 1;
			u16 bBusy		: 1;
			u16 bTot		: 1;	
			u16 			: 9;
		}Bit;
		u16 Byte;
	}BeepSta;
	struct {								//beep run parame
		u8 Step;
		u8 Number;
		u8 Delay;
		u8 Amp;
		u8 OnDelay;
		u8 OffDelay;
		u16  OnFreq;
		u16  OffFreq;
	}BeepPar[BEEPMAX];
}BEEP_DRV;

static BEEP_DRV BeepDrvObj;

/******************************************************************************
; Function Description	: BEEP power on initial process program
; Enter Parameter		: void
; Return Parameter		: void
******************************************************************************/
void BEEP_PowerOnInitial(void)
{
	u8 i;

	BeepDrvObj.Msg.Byte 		= BEEP_IDLE;
	BeepDrvObj.BeepSta.Byte 	= BEEP_IDLE;
	BeepDrvObj.Par.Opt.Byte 	= DISABLE;
	for(i = 0; i < BEEPMAX; i++)
	{
		BeepDrvObj.BeepPar[i].Step  	= BEEP_INIT;
		BeepDrvObj.BeepPar[i].Delay 	= 0x00;
		BeepDrvObj.BeepPar[i].Number	= 0x00;
		BeepDrvObj.BeepPar[i].Amp   	= 0x80;
		BeepDrvObj.BeepPar[i].OnDelay 	= 0x00;
		BeepDrvObj.BeepPar[i].OffDelay 	= 0x00;
		BeepDrvObj.BeepPar[i].OnFreq   	= 0x00;
		BeepDrvObj.BeepPar[i].OffFreq  	= 0x00;
	}
        BEEP_SetOutput(BEEP_IDPowerOff,OFF,0x00,TRUE);
        BEEP_SetOutput(BEEP_IDPowerOff,OFF,0x00,TRUE);
	return;
}

/******************************************************************************
; Function Description	: BEEP load data process program
; Enter Parameter		: void
; Return Parameter		: void
******************************************************************************/
/*void BEEP_LoadData(void)
{	
	ADRLEN_INF	adr;

	adr = CFG_GetCurAdr(ADR_IDWarningTone);
	FILE_Read(adr.Adr,adr.Len,(u8*)(&BeepDrvObj.Par));
	//BeepDrvObj.Par.Opt.Byte = 0xff;
	//BeepDrvObj.Par.ucBeepVolume = 8;
	//BeepDrvObj.Par.Opt.Bit.bPow = ON;
	//BeepDrvObj.Par.Opt.Bit.bKey = ON;
	//BeepDrvObj.Par.Opt.Bit.bPtt = OFF;
	//BeepDrvObj.Par.Opt.Bit.bLowBatt = ON;
	return;
}*/

/******************************************************************************
; Function Description	: BEEP uart command set process
; Enter Parameter		: *pBuf 	: uart command data pointer
; Return Parameter		: void
******************************************************************************/
/*BOOL BEEP_SetCommand(u8* pBuf)
{
	ADRLEN_INF	adr;

	BeepDrvObj.Par.Opt.Byte = pBuf[0];
	BeepDrvObj.Par.Res[0]   = pBuf[1];
	BeepDrvObj.Par.Res[1]   = pBuf[2];
	BeepDrvObj.Par.Res[2]   = pBuf[3];
	adr = CFG_GetCurAdr(ADR_IDWarningTone);
	return FILE_Write(adr.Adr,adr.Len,(u8*)(&BeepDrvObj.Par));
}*/

/******************************************************************************
; Function Description	: BEEP uart command get process
; Enter Parameter		: *pBuf 	: uart command data pointer
; Return Parameter		: void
******************************************************************************/
u8 BEEP_GetCommand(u8* pBuf)
{
	pBuf[0] = BeepDrvObj.Par.Opt.Byte;
	pBuf[1] = BeepDrvObj.Par.Res[0];
	pBuf[2] = BeepDrvObj.Par.Res[1];
	pBuf[3] = BeepDrvObj.Par.Res[2];	
	return 0x04;
}
/*******************************************************************************
 functioname	:	BEEP_GetStatus
 input			:Id -beep id	 
 output			:True-beep running,False-beep stop	 
 AUTHOR			:	abell  miaoyuanhua
 function		:	get beep status 
 date			:	2011-1-10
*******************************************************************************/
bool BEEP_GetStatus(BEEP_TYPE Id)
{
	bool r=FALSE;
	switch (Id)
	{
	case BEEP_IDPOWER:
		r=BeepDrvObj.BeepSta.Bit.bPower;
		break;
	case BEEP_IDPowerOff:
		r=BeepDrvObj.BeepSta.Bit.bPowerOff;
		break;
	case BEEP_IDKEY:
		r=BeepDrvObj.BeepSta.Bit.bKey;
		break;
	case BEEP_IDNULLCH:
		break;
	case BEEP_IDBUSY:	
		r=BeepDrvObj.BeepSta.Bit.bBusy;
		break;
	case BEEP_IDLOBATT:
		r=BeepDrvObj.BeepSta.Bit.bLoBatt;	
		break;
	case BEEP_IDLOBATTArm:
		r=BeepDrvObj.BeepSta.Bit.bLoBatt;	
		break;
	case BEEP_IDHIBATT:
		r=BeepDrvObj.BeepSta.Bit.bLoBatt;
		break;
	case BEEP_IDSIG:
		break;
	case BEEP_IDTOT:
		r=BeepDrvObj.BeepSta.Bit.bTot;
		break;
	}
	if (r != 0) 							
		{
		return (TRUE);
		}
	else
		{
		return (FALSE);
		}
}



/******************************************************************************
; Function Description	: BEEP set output process
; Enter Parameter	: Id	: Beep type select
;			: OnOff	: beep type status
;			: cIndex: beep type source index selec
;			: bRenew: beep updata now option
; Return Parameter	: void
******************************************************************************/
void BEEP_SetOutput(BEEP_TYPE Id,IO_ONOFF OnOff,u8 cIndex,bool bRenew)
{
	switch (Id)
	{
	case BEEP_IDFREE:							//beep free process
		break;
	case BEEP_IDPOWER:
		if (BeepDrvObj.Par.Opt.Bit.bPow == DISABLE) {goto BEEPSetOutput_Exit;}
		if (BeepDrvObj.BeepSta.Bit.bPower == OnOff) {goto BEEPSetOutput_Exit;}
		BeepDrvObj.BeepSta.Bit.bPower = OnOff;
		break;
	case BEEP_IDPowerOff:
		if (BeepDrvObj.BeepSta.Bit.bPowerOff == OnOff) {goto BEEPSetOutput_Exit;}
		BeepDrvObj.BeepSta.Bit.bPowerOff = OnOff;
		break;
	case BEEP_IDKEY:
		if (BeepDrvObj.Par.Opt.Bit.bKey == DISABLE) {goto BEEPSetOutput_Exit;}
		if (BeepDrvObj.BeepSta.Bit.bKey == OnOff)	{goto BEEPSetOutput_Exit;}
		BeepDrvObj.BeepSta.Bit.bKey = OnOff;	
		switch (cIndex)									//key status process
		{
		case KSTA_UP:
		case KSTA_DOWN:
			goto BEEPSetOutput_Exit;
		case KSTA_ERR:
			break;
		case KSTA_ENTERY:
			break;
		case KSTA_EXIT:
			break;
		default:
			goto BEEPSetOutput_Exit;
		}
		break;
	case BEEP_IDNULLCH:
		break;
	case BEEP_IDBUSY:	
		if (BeepDrvObj.BeepSta.Bit.bBusy == OnOff) {goto BEEPSetOutput_Exit;}
		BeepDrvObj.BeepSta.Bit.bBusy = OnOff;	
		break;
	case BEEP_IDLOBATT:
		if (BeepDrvObj.Par.Opt.Bit.bLowBatt == DISABLE) {goto BEEPSetOutput_Exit;}
		if (BeepDrvObj.BeepSta.Bit.bLoBatt == OnOff) {goto BEEPSetOutput_Exit;}
		BeepDrvObj.BeepSta.Bit.bLoBatt = OnOff;			
		break;
	case BEEP_IDLOBATTArm:
		break;
	case BEEP_IDHIBATT:
		if (BeepDrvObj.BeepSta.Bit.bHiBatt == OnOff) {goto BEEPSetOutput_Exit;}
		BeepDrvObj.BeepSta.Bit.bHiBatt = OnOff;			
		break;
	case BEEP_IDSIG:
		break;
	case BEEP_IDTOT:
		if (BeepDrvObj.BeepSta.Bit.bTot == OnOff) {goto BEEPSetOutput_Exit;}
		BeepDrvObj.BeepSta.Bit.bTot = OnOff;
		break;
	default:
		BeepDrvObj.BeepSta.Byte = BEEP_IDLE;
		bRenew = TRUE;
		break;
	}
	BeepDrvObj.Msg.Bit.bSet = BEEP_RUN;
	if (bRenew == TRUE) 							//beep renew process
	{
		BEEP_Interrupt10ms();
	}
        BEEPSetOutput_Exit:	
	return;
}

/******************************************************************************
; Function Description	: BEEP Interrupt 10ms process
; Enter Parameter		: void
; Return Parameter		: void
******************************************************************************/
void BEEP_Interrupt10ms(void)
{
  if (BeepDrvObj.BeepSta.Byte != 0x00)						//beep status process
  {
    TONE_Start();
    //AUDI_SetOutput(AUDI_IDBEEP,ON,TRUE);
  // MIC_SetOutput(MIC_IDBEEP,ON,TRUE);			
  }
  else
  {	
    if (BeepDrvObj.Msg.Bit.bSet == BEEP_RUN)
    {
      BeepDrvObj.Msg.Bit.bSet = BEEP_IDLE;
      TONE_Stop();
    // AUDI_SetOutput(AUDI_IDBEEP,OFF,TRUE);
    // MIC_SetOutput(MIC_IDBEEP,OFF,TRUE);
    }
  }
    return;
}

#endif
/******************************************************************************
;---------------end page
******************************************************************************/
void BEEP_Time(u16 x)
{
    BEEP_SetOutput(BEEP_IDPowerOff,ON,0x00,TRUE);
    DEL_SetTimer(0,x);
    while(1)
    {
      if(DEL_GetTimer(0) == TRUE) {break;}
    }
    BEEP_SetOutput(BEEP_IDPowerOff,OFF,0x00,TRUE);
}
