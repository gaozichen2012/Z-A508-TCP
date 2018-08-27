/*****************************************************************************
 Copyright (C), 2012, Shenzhen ABELL Industrial & Development Co., Ltd.
 System name	: ABELL Series Two-Way Radio
 File name		: DrvGT20L16S1Y.c
 Author			: JF.Wang
 Version		: V1.0
 Date			: 2012.08.10
 Description	: 字库驱动
 CPU			: STM32F103VE
 Compiler		: IAR EWARM(Embeded Workbench for ARM) 5.5
 OS				: uC/OS-II V2.85
 Others			: None
******************************************************************************
 History		: Version		Date		Author		Modification
 				1)	1.0		2012-08-10		JF.Wang		生成文件
******************************************************************************/

/*----------------------------------------------------------------------------
 Include File
----------------------------------------------------------------------------*/
#include "AllHead.h"


/*-----------------------------------------------------------------------------
 Macro Define (macro definitions used in this file internally)
-----------------------------------------------------------------------------*/
#define GT20_RWDEL 0x02
#define GT20_DUMMY 0xff
#define GT20_BASET 0x0f

//u8	TimeVal[8] = { 16, 32, 8, 8, 16, 16, 34, 34 };
//u32 BaseAdd[8] = { 0x3B7D0, 0x00000, 0x3BFC0, 0x066C0, 0x3B7C0, 0x3CF80, 0x3C2C0, 0x3D580 };//[7]=0x3D580
/*-----------------------------------------------------------------------------
 Variable Define (Variables definition used in this file internally)
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 Internal Function Prototype(declaration of functions used in this file internally)
-----------------------------------------------------------------------------*/
//static void GT20_GetDisData(u8 iLenth, u32 Addr, u8 *cBuf);
//static void GT20_GetData(u8 iLenth, u8 *cBuf);
//static void GT20_SendByte(u8 iLenth, u8 cBuf);
//static void GT20_SendCommand(u32 Addr);
//static void GT20_DelTime(u8 DelTime);
static void mcu_gt20_initial(void);

/*******************************************************************************
* Description	: GT20 Power-On Initial Process
* Input			: void
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
void drv_gt20_pwr_on_init(void)
{
  mcu_gt20_initial();
}

static void mcu_gt20_initial(void)
{
    GPIO_Init(GPIOD,GPIO_PIN_0,GPIO_MODE_OUT_PP_LOW_FAST);//clk
    GPIO_Init(GPIOE,GPIO_PIN_0,GPIO_MODE_OUT_PP_LOW_FAST);//cs
    GPIO_Init(GPIOE,GPIO_PIN_1,GPIO_MODE_OUT_PP_LOW_FAST);//si
    GPIO_Init(GPIOE,GPIO_PIN_2,GPIO_MODE_IN_PU_NO_IT);//so
}

/*******************************************************************************
* Description	: GT20 data output process
* Input			: cID  : data type
				: iCode: data encode
				: cBuf : data buffer
* Output		: cBuf : dot-data
* Return		: r : status flag
* Others		: void
********************************************************************************/
bool drv_gt20_data_output(DISP_TYPE cID, u16 iCode, u8 *cBuf)
{
	bool r = FALSE;
	u8  i, AddrL, AddrH;
	u32 DataAddr = 0x00000;
	AddrL = (iCode & 0xff);
	AddrH = (iCode >> 0x08) & 0xff;
	switch (cID & GT20_BASET)
	{
	case DISP_IDCN816:
		break;
	case DISP_IDCN1516://汉字进这里
          if (AddrL < 0xA1) { break; }
		if (AddrH == 0xA9)
		{
			r = TRUE;
			DataAddr = 121 + AddrL;
		}
		else
		{
			if ((AddrH >= 0xA1) && (AddrH <= 0xA3))
			{
				r = TRUE;
				DataAddr = (AddrH - 0xA1) * 94 + (AddrL - 0xA1);
			}
			else
			{
				if ((AddrH >= 0xB0) && (AddrH <= 0xF7))
				{
					r = TRUE;
					DataAddr = (AddrH - 0xB0) * 94 + AddrL + 685;
				}
			}
		}
         // GB2312_16_GetData(AddrH,AddrL,cBuf);//获取汉字点阵数据
                if(ValueSearch(iCode)!=0xff)
                {
                  GetPointArrayData32(ValueSearch(iCode),cBuf);
                }
                else
                {
                  GB2312_16_GetData(AddrH,AddrL,cBuf);//获取汉字点阵数据
                }
                
		break;
		
	case DISP_IDASC57:
	case DISP_IDASC78:
	case DISP_IDASC816:
	case DISP_IDASC816B://数字字母进这里0x05
	case DISP_IDASCARI:
	case DISP_IDASCTNR://数字字母进这里0x07
          		if ((iCode >= 0x20) && (iCode <= 0x7E))
		{
			r = TRUE;
			DataAddr = iCode - 0x20;
		}
                WW_GetData(iCode,cBuf);
		break;
	default:
		break;
	}
	
	if (r == TRUE)
	{
		/*DataAddr *= TimeVal[cID & GT20_BASET];//汉字：TimeVal[1]=34 字母：TimeVal[5]=16
		DataAddr += BaseAdd[cID & GT20_BASET];//汉字：BaseAdd[1]=0x0  字母：BaseAdd[5]=0x3CF80
		//GT20_GetDisData(TimeVal[cID & GT20_BASET], DataAddr, cBuf);//0x01&0x0f=0x01
                *///原字库函数屏蔽

	}
	else
	{
		for (i = 0; i < 34; i++) { cBuf[i] = 0x00; }
	}
	return r;
}

bool drv_gt20_data_output2(DISP_TYPE cID, u16 iCode, u8 *cBuf)//Unicode使用
{
	bool r = FALSE;
	u8  i, AddrL, AddrH;
	u32 DataAddr = 0x00000;
	AddrL = (iCode & 0xff);
	AddrH = (iCode >> 0x08) & 0xff;
	switch (cID & GT20_BASET)
	{
	case DISP_IDCN816:
		break;
	case DISP_IDCN1516://汉字进这里
          //if (AddrL < 0xA1) { break; }
          
		if (AddrH == 0xA9)
		{
			r = TRUE;
			DataAddr = 121 + AddrL;
		}
		else
		{
			if ((AddrH >= 0xA1) && (AddrH <= 0xA3))
			{
				r = TRUE;
				DataAddr = (AddrH - 0xA1) * 94 + (AddrL - 0xA1);
			}
			else
			{
				if ((AddrH >= 0xB0) && (AddrH <= 0xF7))
				{
					r = TRUE;
					DataAddr = (AddrH - 0xB0) * 94 + AddrL + 685;
				}
			}
		}
                r = TRUE;
                //UNICODE_16_GetData(iCode,cBuf);
                if(UnicodeValueSearch(iCode)!=0xff)
                {
                  UnicodeGetPointArrayData32(UnicodeValueSearch(iCode),cBuf);
                }
                else
                {
                    UNICODE_16_GetData(iCode,cBuf);//获取汉字点阵数据
                }
                
		break;
		
	case DISP_IDASC57:
	case DISP_IDASC78:
	case DISP_IDASC816:
	case DISP_IDASC816B://数字字母进这里0x05
	case DISP_IDASCARI:
	case DISP_IDASCTNR://数字字母进这里0x07
          		if ((iCode >= 0x20) && (iCode <= 0x7E))
		{
			r = TRUE;
			DataAddr = iCode - 0x20;
		}
                r = TRUE;
                WW_GetData(iCode,cBuf);
		break;
	default:
		break;
	}
	
	if (r == TRUE)
	{
		/*DataAddr *= TimeVal[cID & GT20_BASET];//汉字：TimeVal[1]=34 字母：TimeVal[5]=16
		DataAddr += BaseAdd[cID & GT20_BASET];//汉字：BaseAdd[1]=0x0  字母：BaseAdd[5]=0x3CF80
		//GT20_GetDisData(TimeVal[cID & GT20_BASET], DataAddr, cBuf);//0x01&0x0f=0x01
                *///原字库函数屏蔽

	}
	else
	{
		for (i = 0; i < 34; i++) { cBuf[i] = 0x00; }
	}
	return r;
}

/*******************************************************************************
* Description	: gt20 byte send process
* Input			: iLenth : send byte length
				: Addr	 : data address
				: cBuf	 : send byte buffer
* Output		: Dot-data
* Return		: void
* Others		: void
********************************************************************************/
/*static void GT20_GetDisData(u8 iLenth, u32 Addr, u8 *cBuf)
{
	MCU_GT20_CS(LO);
	GT20_SendCommand(Addr);
	GT20_GetData(iLenth, cBuf);
	MCU_GT20_CS(HI);
	return;
}*/

/*******************************************************************************
* Description	: gt20 byte send process
* Input			: Addr : data address
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
/*static void GT20_SendCommand(u32 Addr)
{
	u8 i, j, cData;
	
	GT20_SendByte(0x08, 0x0B);
	for (i = 0x00; i < 0x03; i++)
	{
		j = 0x02 - i;
		cData  = (Addr >> (j * 0x08));
		GT20_SendByte(0x08, cData);
	}
	GT20_SendByte(0x08, GT20_DUMMY);
	return;
}*/

/*******************************************************************************
* Description	: gt20 byte send process
* Input			: iLen : send byte length
				: cBuf : send byte buffer
* Output		: Dot-data
* Return		: void
* Others		: void
********************************************************************************/
/*static void GT20_GetData(u8 iLen, u8 *cBuf)//汉字：Len=32 英文：Len=16/34?
{
	u8 i, j, cData;
	
	for (i = 0; i < iLen; i++)
	{
		for (j = 0; j < 0x08; j++)
		{
			MCU_GT20_CLK(HI); 
			cData <<= 0x01;
			MCU_GT20_CLK(LO);
			if (MCU_GT20_SO_Read) cData++;
		}
		cBuf[i] = cData;
	}
	return;
}*/

/*******************************************************************************
* Description	: gt20 byte send process
* Input			: iLen : send byte length
				: cBuf : send byte buffer
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
/*static void GT20_SendByte(u8 iLen, u8 cBuf)
{
	if (iLen > 0x08) { iLen = 0x08; }
	for (; iLen > 0x00; iLen--)
	{
		MCU_GT20_CLK(LO);
		MCU_GT20_SI(((cBuf & 0x80) == 0x00) ? LO : HI);
		GT20_DelTime(GT20_RWDEL);
		MCU_GT20_CLK(HI);
		cBuf <<= 0x01;
		GT20_DelTime(GT20_RWDEL);
	}
	return;
}*/

/*******************************************************************************
* Description	: gt20 delay process
* Input			: DelTime : delay time parameter
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
/*static void GT20_DelTime(u8 DelTime)
{
	u8 i;
	
	for (i = 0; i < DelTime; i++) { nop(); }
	return;
}*/

/********************************************************************************
; ------------------------------- End of file ----------------------------------
********************************************************************************/
