#ifndef __CONFIG_H
#define __CONFIG_H

typedef enum {					//config  function option address lise 
	ADR_IDEepromChk	  	= 0x00,
	ADR_IDSerialNo    	= 0x01,
	ADR_IDDealerID   	= 0x02,
	ADR_IDMaxUserSum   = 0x03,
	ADR_IDMaxGrpSum	  	= 0x04,
	ADR_IDSysMode	   	= 0x05,
	ADR_IDRadioInfo		= 0x06,
	ADR_IDUserID	      	= 0x07,
	ADR_IDEEPROMVER		= 0x08,
	ADR_IDRadioMode	 	= 0x10,
	ADR_IDSoftPowerOn	= 0x11,
	
	nAddr_CardUserPassword=0x12,
	ADR_IDLocalUserInfo= 0x13,
	ADR_IDWorkMode		= 0x14,
	ADR_IDWarningTone	= 0x15,
	ADR_IDSpeech	 		= 0x16,
	ADR_IDDisplay		= 0x17,
	ADR_IDPowerManage	= 0x18,
	ADR_IDKeyProg		= 0x19,
	ADR_IDTOT		     	= 0x1A,
	ADR_IDGpsFun			= 0x1B,

	
	nAddrBatteryConfig	= 0x82,
	nAddr_AudioConfig	= 0x83
}CFG_ADRTYPE;

typedef enum{
	nAddr_GpsInfoChecksum 	= 0x00,
	nAddr_GpsInfoBackupHead	= 0x01,
	nAddr_GpsInfoBackup		= 0x02
}CFG_GpsInfoADRTYPE;

typedef enum {					//config option id type select
	CFG_IDEEPMAX	= 0x00,
	CFG_IDNEWOLD 	= 0x01,
	CFG_IDCHMAX	 	= 0x02,	
	CFG_ID2TONEMAX	
}CFG_OPTTYPE;

typedef struct {							//address group inforamtion 
	u32  Adr;
	u32 Len;
	u32 Group;
}ADRLEN_GPSRecordInf;

extern	ADRLEN_INF CFG_GetCurAdr(CFG_ADRTYPE cId);
extern	u8 CFG_GetValue(CFG_OPTTYPE cId);
extern ADRLEN_GPSRecordInf CFG_GetGpsAdr(CFG_GpsInfoADRTYPE cId);

#endif