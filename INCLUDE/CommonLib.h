/******************************************************************************
; File Name		: CommonLib.h
; Author		: ZhangYaLing
; Version		: 1.0
; Description	: Common head interface file
; Date			: 2010.07.27
; Modify		:
******************************************************************************/
#ifndef COMMONLIB_H
#define COMMONLIB_H

#define COML_FUNENABLE

#ifdef	COMLLABEL
	#define COMLAPI
#else
	#define COMLAPI	extern
#endif
COMLAPI const u16 csinetab[256];
COMLAPI u32 COML_LsbLongBuf(u8* pBuf);
COMLAPI u32 COML_UnLsbLongBuf(u8* pBuf);
COMLAPI u16 COML_LsbIntBuf(u8* pBuf);
COMLAPI u16 COML_UnLsbIntBuf(u8* pBuf);
COMLAPI u8 COML_U8CharMaxMin(u8 Value,u8 Max,u8 Min);
COMLAPI u32 COML_AscToDec(u8 *buf,u8 len);
COMLAPI u8 COML_DecToAsc(u32 Value, u8 *buf);
COMLAPI u32 COML_AscToHex(u8 *buf,u8 len);
COMLAPI u8 COML_HexToAsc2(u32 Value, u8 *buf);//适配GPS部标格式修改
COMLAPI u8 COML_HexToAsc(u32 Value, u8 *buf);
COMLAPI u8 Combine2Hex(const u8* pInHexBuf, u8 uclen, u8* pOutHexBuf);
COMLAPI void Dec2Hex(const u8* dec, u16 len, u8* hex);
extern void COML_StringReverse(u8 Len, u8 *buf);
extern void COML_StringReverse2(u8 Len, u8 *buf);
#endif
/******************************************************************************
;------------------end page
******************************************************************************/
