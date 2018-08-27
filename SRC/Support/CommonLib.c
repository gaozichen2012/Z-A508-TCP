/******************************************************************************
; File Name		: CommonLib.c
; Author		: ZhangYaLing
; Version		: 1.0
; Description	: Common library process file
; Date			: 2010.07.27
; Modify		:
******************************************************************************/
#define COMLLABEL
#include "AllHead.h"

typedef union BitField8
{
	unsigned char all;				// 1byte
	unsigned char byte;				// 1byte
	struct
	{
		unsigned char low	: 4 ;	// bit3-bit0
		unsigned char high	: 4 ;	// bit7-bit4
	}nib;
	
	struct 
	{
		unsigned char b0	: 1 ;	// bit0
		unsigned char b1	: 1 ;	// bit1
		unsigned char b2	: 1 ;	// bit2
		unsigned char b3	: 1 ;	// bit3
		unsigned char b4	: 1 ;	// bit4
		unsigned char b5	: 1 ;	// bit5
		unsigned char b6	: 1 ;	// bit6
		unsigned char b7	: 1 ;	// bit7
	}bits;
}BitOf8;

#ifdef COML_FUNENABLE
const u16 csinetab[256] = {				//sin table process
	0x7F00,0x821E,0x853B,0x8858,0x8B73,0x8E8C,0x91A3,0x94B6,0x97C7,0x9AD3,0x9DDC,0xA0DF,0xA3DE,0xA6D6,0xA9C9,0xACB5, 
	0xAF9A,0xB277,0xB54D,0xB81A,0xBADE,0xBD99,0xC04B,0xC2F2,0xC58F,0xC821,0xCAA7,0xCD22,0xCF91,0xD1F4,0xD44A,0xD692, 
	0xD8CD,0xDAFB,0xDD1A,0xDF2A,0xE12C,0xE31F,0xE502,0xE6D5,0xE899,0xEA4C,0xEBEE,0xED80,0xEF01,0xF071,0xF1CF,0xF31B, 
	0xF455,0xF57D,0xF693,0xF797,0xF888,0xF966,0xFA32,0xFAEA,0xFB8F,0xFC21,0xFCA0,0xFD0B,0xFD63,0xFDA8,0xFDD9,0xFDF6, 
	0xFE00,0xFDF6,0xFDD9,0xFDA8,0xFD63,0xFD0B,0xFCA0,0xFC21,0xFB8F,0xFAEA,0xFA32,0xF966,0xF888,0xF797,0xF693,0xF57D, 
	0xF455,0xF31B,0xF1CF,0xF071,0xEF01,0xED80,0xEBEE,0xEA4C,0xE899,0xE6D5,0xE502,0xE31F,0xE12C,0xDF2A,0xDD1A,0xDAFB, 
	0xD8CD,0xD692,0xD44A,0xD1F4,0xCF91,0xCD22,0xCAA7,0xC821,0xC58F,0xC2F2,0xC04B,0xBD99,0xBADE,0xB81A,0xB54D,0xB277, 
	0xAF9A,0xACB5,0xA9C9,0xA6D6,0xA3DE,0xA0DF,0x9DDC,0x9AD3,0x97C7,0x94B6,0x91A3,0x8E8C,0x8B73,0x8858,0x853B,0x821E, 
	0x7F00,0x7BE2,0x78C5,0x75A8,0x728D,0x6F74,0x6C5D,0x694A,0x6639,0x632D,0x6024,0x5D21,0x5A22,0x572A,0x5437,0x514B, 
	0x4E66,0x4B89,0x48B3,0x45E6,0x4322,0x4067,0x3DB5,0x3B0E,0x3871,0x35DF,0x3359,0x30DE,0x2E6F,0x2C0C,0x29B6,0x276E, 
	0x2533,0x2305,0x20E6,0x1ED6,0x1CD4,0x1AE1,0x18FE,0x172B,0x1567,0x13B4,0x1212,0x1080,0x0EFF,0x0D8F,0x0C31,0x0AE5, 
	0x09AB,0x0883,0x076D,0x0669,0x0578,0x049A,0x03CE,0x0316,0x0271,0x01DF,0x0160,0x00F5,0x009D,0x0058,0x0027,0x000A, 
	0x0000,0x000A,0x0027,0x0058,0x009D,0x00F5,0x0160,0x01DF,0x0171,0x0316,0x03CE,0x049A,0x0578,0x0669,0x076D,0x0883, 
	0x09AB,0x0AE5,0x0C31,0x0D8F,0x0EFF,0x1080,0x1212,0x13B4,0x1567,0x172B,0x18FE,0x1AE1,0x1CD4,0x1ED6,0x20E6,0x2305, 
	0x2533,0x276E,0x29B6,0x2C0C,0x2E6F,0x30DE,0x3359,0x35DF,0x3871,0x3B0E,0x3DB5,0x4067,0x4322,0x45E6,0x48B3,0x4B89, 
	0x4E66,0x514B,0x5437,0x572A,0x5A22,0x5D21,0x6024,0x632D,0x6639,0x694A,0x6C5D,0x6F74,0x728D,0x75A8,0x78C5,0x7BE2
};	
/******************************************************************************
; Function Description	: u32 value conversion process
; Enter Parameter		: *pBuf		: buffer value pointer
; Return Parameter		: value
******************************************************************************/
u32 COML_LsbLongBuf(u8* pBuf)
{
	u32 l;

	l = pBuf[3]; l <<= 8;
	l |=pBuf[2]; l <<= 8;
	l |=pBuf[1]; l <<= 8;
	l |=pBuf[0];
	return l;
}

/******************************************************************************
; Function Description	: u32 value conversion process
; Enter Parameter		: *pBuf		: buffer value pointer
; Return Parameter		: value
******************************************************************************/
u32 COML_UnLsbLongBuf(u8* pBuf)
{
	u32 l;

	l = pBuf[3];  l <<= 8;
	l |= pBuf[2]; l <<= 8;
	l |= pBuf[1]; l <<= 8;
	l |= pBuf[0];
	return l;
}

/******************************************************************************
; Function Description	: u16 value conversion process
; Enter Parameter		: *pBuf		: buffer value pointer
; Return Parameter		: value
******************************************************************************/
u16 COML_LsbIntBuf(u8* pBuf)
{
	u16 i;
	
	i =pBuf[1]; 
	i <<= 8;
	i |=pBuf[0];
	return i;
}

/******************************************************************************
; Function Description	: u16 value conversion process
; Enter Parameter		: *pBuf		: buffer value pointer
; Return Parameter		: value
******************************************************************************/
u16 COML_UnLsbIntBuf(u8* pBuf)
{
	u16 i;

	i = pBuf[1]; 
	i <<= 8;
	i |= pBuf[0];
	return i;
}

/******************************************************************************
; Function Description	: u8 < max and > min process
; Enter Parameter		: Value		: 
;						: Max		:
;						: Min		:
; Return Parameter		: value
******************************************************************************/
u8 COML_U8CharMaxMin(u8 Value,u8 Max,u8 Min)
{
	u8 i;

	if (Max < Min) {i = Max; Max = Min; Min = i;} 
	if (Value > Max) {Value = Max;}
	if (Value < Min) {Value = Min;}
	return Value;
}

/******************************************************************************
; Function Description	: u8 < max and > min process
; Enter Parameter		: Value		: 
;						: Max		:
;						: Min		:
; Return Parameter		: value
******************************************************************************/
u32 COML_AscToDec(u8 *buf,u8 len)
{
	u32 Value;
	u8 i;

	Value = 0;
	for(i = 0x00; i < len ; i++)
	{
		if(buf[i] <= 0x39 && buf[i] >= 0x30)
		{
			Value *= 10;
			Value += (buf[i] - 0x30);
		}
		else
		{
			break;
		}
	}
	return Value;
}

/******************************************************************************
; Function Description	: u8 < max and > min process
; Enter Parameter		: Value		: 
;						: Max		:
;						: Min		:
; Return Parameter		: value
******************************************************************************/
u8 COML_DecToAsc(u32 Value, u8 *buf)
{
	u8 i;
	
	for(i = 0x00; Value != 0; i++)
	{
		buf[i] = 	Value % 10 + 0x30;
		Value /= 10;
	}
	return i;
}

/******************************************************************************
; Function Description	: u8 < max and > min process
; Enter Parameter		: Value		: 
;						: Max		:
;						: Min		:
; Return Parameter		: value
******************************************************************************/
u32 COML_AscToHex(u8 *buf,u8 len)
{
	u32 Value;
	u8 i;

	Value = 0;
	for(i = 0x00; i < len ; i++)
	{
		if(buf[i] <= 0x39 && buf[i] >= 0x30)
		{
			Value <<= 0x04;
			Value += (buf[i] - 0x30);
		}
		else
		{
			if(buf[i] <= 0x46 && buf[i] >= 0x41)
			{
				Value <<= 0x04;
				Value += (buf[i] - 0x37);
			}
			else
			{
				if(buf[i] <= 0x66 && buf[i] >= 0x61)
				{
					Value <<= 0x04;
					Value += (buf[i] - 0x57);
				}
				else
				{
					break;
				}
			}
		}
	}
	return Value;
}

/******************************************************************************
; Function Description	: u8 < max and > min process
; Enter Parameter		: Value		: 
;						: Max		:
;						: Min		:
; Return Parameter		: value
******************************************************************************/
u8 COML_HexToAsc(u32 Value, u8 *buf)
{
	u8 i=1;

	if(Value == 0x00)
	{
		buf[0] = 0x30;
                
	}
	else
	{
		for(i = 0x00; Value != 0; i++)
		{
			buf[i] = 	Value % 16;
			if(buf[i] <= 0x09)
			{
				buf[i] += 0x30;
			}
			else
			{
				buf[i] += 0x37;
			}
			Value /= 16;
		}
	}
	return i;
}
u8 COML_HexToAsc2(u32 Value, u8 *buf)//适配GPS部标格式修改
{
	u8 i=0;
        for(i = 0x00; i<2; i++)
        {
          buf[i] = 	Value % 16;
          if(buf[i] <= 0x09)
          {
            buf[i] += 0x30;
          }
          else
          {
            buf[i] += 0x37;
          }
          Value /= 16;
        }

        
	
	return i;
}
void COML_StringReverse(u8 Len, u8 *buf)
{
	u8 i, j, Temp;

	j = Len;
	for(i = 0x00; i < j; i++)
	{
		j = Len - (i + 1);
		Temp = buf[i];
		buf[i] = 	buf[j];
		buf[j] = Temp;
	}
}
void COML_StringReverse2(u8 Len, u8 *buf)
{
	u8 i, j, Temp;

	j = 0;
	for(i = 0x00; i < Len; i++)
	{
		j = i ;
		Temp = buf[i];
		buf[i] = 	buf[j];
		buf[j] = Temp;
	}
}

u8 Combine2Hex(const u8* pInHexBuf, u8 uclen, u8* pOutHexBuf)
{
	BitOf8 stOutData;
	u8 i = 0x00;
	u8 ucIndex = 0x00;
	u8 ucBufLow = 0x00;
	u8 ucBufHigh = 0x00;
	
	for(i=0; i<uclen;)
	{
		if( (pInHexBuf[i]>=0x30) && (pInHexBuf[i]<=0x39) )
		{
	        ucBufLow = pInHexBuf[i] - 0x30;
		}
	    else if( (pInHexBuf[i]>=0x41) && (pInHexBuf[i]<=0x46) )//
		{
	        ucBufLow = pInHexBuf[i] - 0x37;
		}
	    else if( (pInHexBuf[i]>=0x61) && (pInHexBuf[i]<=0x66) )
		{
	       ucBufLow = pInHexBuf[i] - 0x57;
		}
	    else 
		{
			ucBufLow = 0xFF;
		}
		i++;
		if( (pInHexBuf[i]>=0x30) && (pInHexBuf[i]<=0x39) )
		{
	        ucBufHigh = pInHexBuf[i] - 0x30;
		}
	    else if( (pInHexBuf[i]>=0x41) && (pInHexBuf[i]<=0x46) )
		{
	        ucBufHigh = pInHexBuf[i] - 0x37;
		}
	    else if( (pInHexBuf[i]>=0x61) && (pInHexBuf[i]<=0x66) )
		{
	       ucBufHigh = pInHexBuf[i] - 0x57;
		}
	    else 
		{
			ucBufHigh = 0xFF;
		}
		i++;
		if(ucBufHigh < 0x10 && ucBufLow < 0x10)
		{
			stOutData.nib.high = ucBufLow;
			stOutData.nib.low  = ucBufHigh;
			pOutHexBuf[ucIndex] = stOutData.byte;
			
			ucIndex++;
		}
		else
		{
			break;
		}
	}
	return ucIndex;
}

void Dec2Hex(const u8* dec, u16 len, u8* hex)
{
    const u8* HexSignals = "0123456789abcdef"; 
	u16 i = 0;
	
    for(i = 0; i < len; i++)
	{
        hex[i * 2 + 1] = HexSignals[dec[i] & 0x0F];
        hex[i * 2] = HexSignals[(dec[i] & 0xF0) >> 4];
	}
}

#endif
/******************************************************************************
;------------end page
******************************************************************************/
