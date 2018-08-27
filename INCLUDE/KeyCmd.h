#ifndef __KEYCMD_H
#define __KEYCMD_H
typedef enum {					//key command id type
	KCMD_IDNULL	 	= 0x00,
	KCMD_IDKEY	 	= 0x01,
	KCMD_IDCHINFO	= 0x02,
	KCMD_IDTOT	 	= 0x03,
	KCMD_IDTXFREQ	= 0x04,
	KCMD_IDRXFREQ	= 0x05,
	KCMD_IDVOX	 	= 0x06,
	KCMD_IDPOWSAV	= 0x07,
	KCMD_IDSQ	 	= 0x08,
	KCMD_IDLANG	 	= 0x09,
	KCMD_IDBEEP  	= 0x0A,
	KCMD_IDUART		= 0x0B,
	KCMD_IDChExt	= 0x0c,
	KCMD_IDBatFun	= 0x0D,
	KCMD_IDABLed	= 0x0E,
	KCMD_IDFun		= 0x0F,

	eKCMD_IDOrder	= 0x11,
	KCMD_IDSCANBACK	= 0x12,			//SCAN BACKUP ID

	KCMD_IDRESET 	= 0xFF
}KCMD_IDTYPE;	//function option id list

extern u8 KCMD_KeyCmdGet(u8 cId,u8* pBuf);
extern void MCU_VERSIONForMenu(void);
#endif