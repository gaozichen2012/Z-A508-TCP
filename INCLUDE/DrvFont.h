/*******************************************************************************
 Copyright (C), 2013, Shenzhen ABELL Industrial & Development Co., Ltd.
 System name	: ABELL Series Two-Way Radio
 File name		: DrvHTG12832.h
 Author		: David Dai
 Version		: V1.0
 Date			: 2013-03-16
 Description	: 字库字模提取
 CPU			: STM32F103VE
 Compiler		: IAR EWARM (Embeded WorkBench for ARM) 5.5
 OS				: uC/OS-II V2.85
 Others			: None
********************************************************************************
 History		: Version		Date		Author		Modification
 				1) 1.0		2012-08-10		JF.Wang		生成文件
********************************************************************************/

/* -----------------------------------------------------------------------------
 Define to prevent recursive inclusion
------------------------------------------------------------------------------ */
#ifndef _DRV_FONT_H
#define _DRV_FONT_H

/* -----------------------------------------------------------------------------
 Include File
------------------------------------------------------------------------------ */
//#include "CtypeDef.h"
//#include "ParamDef.h"

#define EXT_HZK

//---字库IC使能，关闭使用程序内部字库
#define FONT_DRIVER_IC_EN




typedef enum
{
	DISP_IDCN816		= 0x00,	//base type display
	DISP_IDCN1516		= 0x01,
	DISP_IDASC57		= 0x02,
	DISP_IDASC78		= 0x03,
	DISP_IDASC816		= 0x04,
	DISP_IDASC816B		= 0x05,
	DISP_IDASCARI		= 0x06,
	DISP_IDASCTNR		= 0x07,

	DISP_IDCN816N		= 0x10,	//distance compress display
	DISP_IDCN1516N		= 0x11,
	DISP_IDASC57N		= 0x12,
	DISP_IDASC78N		= 0x13,
	DISP_IDASC816N		= 0x14,
	DISP_IDASC816BN		= 0x15,

	DISP_IDCN816R		= 0x20,	//reversed display cn816
	DISP_IDCN1516R		= 0x21,	//reversed display cn1516
	DISP_IDASC57R		= 0x22,	//reversed display asc57
	DISP_IDASC78R		= 0x23,	//reversed display asc78
	DISP_IDASC816R		= 0x24,	//reversed display asc816
	DISP_IDASC816BR		= 0x25,	//reversed display asc816b

	DISP_IDCN816NR		= 0x30,	//distance compress and reversed display
	DISP_IDCN1516NR		= 0x31,
	DISP_IDASC57NR		= 0x32,
	DISP_IDASC78NR		= 0x33,
	DISP_IDASC816NR		= 0x34,
	DISP_IDASC816BNR	= 0x35,

	DISP_IDCNASC57		= 0x82, //CN1516 & ASC57 commix display//小号英文字体（中文无效）
	DISP_IDCNASC78		= 0x83,	//CN1516 & ASC78 //小号英文字体加粗（中文无效）
	DISP_IDCNASC816		= 0x84,	//CN1516 & ASC816//最大字体（中文有效）
	DISP_IDCNASC816B	= 0x85,	//CN1516 & ASC816B//最大字体加粗（中文有效）
	
	DISP_ID12X12		= 0xA0	//12*12				
}DISP_TYPE;
typedef enum
{
	eICO_IDBATT		= 0,
	eICO_IDBATT1    = 1,
	eICO_IDBATT2    = 2,
	eICO_IDBATT3    = 3,
	eICO_IDBATT4    = 4,
	eICO_IDBATT5    = 5,
	eICO_IDLOCKED   = 6,
	eICO_IDMESSAGE  = 7,
	eICO_IDRXFULL   = 8,
	eICO_IDRXNULL   = 9,
	eICO_IDSCAN     = 10,
	eICO_IDSCANPA   = 11, //scan pause
	eICO_IDSPEAKER  = 12, //speaker
	eICO_IDTALKAR   = 13,
	eICO_IDTX       = 14,
	eICO_IDVOX      = 15,
	eICO_IDEmergency= 16,
	eICO_IDPOWERL   = 17,
	eICO_IDPOWERM   = 18,
	eICO_IDPOWERH   = 19,
	eICO_IDTemper   = 20,
	eICO_IDMONITER  = 21,
	eICO_IDBANDWIDTHW = 22,
	eICO_IDBANDWIDTHN = 23,
        eICO_IDMESSAGEOff = 24,
	//zengdi
	eICO_IDOffStart,
	eICO_IDVOXOff,
	eICO_IDSCANOff,
	eICO_IDLOCKEDOff,
	eICO_IDTALKAROff,
	
	eICO_IDSPEAKEROff,
	eICO_IDMONITEROff,	
	eICO_IDEMERGENCYOff,
	eICO_IDOffEnd,
}DISP_ICOID;
typedef enum
{
	egImage_NONE			= 0,
	eImage_1man	,
	eImage_man,
	eImage_phone,
	eImage_in,
	eImage_out,
	eImage_Anglogy,
	eImage_Digit
}ICO_TYPE;

/* -----------------------------------------------------------------------------
 Function To External (Functions defined in this file, can be used externally)
------------------------------------------------------------------------------ */



bool drv_Font_GetData(DISP_TYPE cID, u16 iCode, u8 *cBuf);
void drv_Font_Hzk12_Flag_set(bool set);
void drv_Font_ico_get(u8 ico, u8 *cbuf);




#endif
/*******************************************************************************
; ---------------------------------- End of file ------------------------------
*******************************************************************************/

