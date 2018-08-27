#include "AllHead.h"

/*-----------------------------------------------------------------------------
 Macro Define (macro definitions used in this file internally)
-----------------------------------------------------------------------------*/
#define HTG_IDLE	0x00
#define HTG_RUNN	0x01
#define HTG_PAGE	0x03
#define HTG_BASE	0x0f
#define	HTG_NRTY	0x10
#define	HTG_CMAX	0x80


/*-----------------------------------------------------------------------------
 Variable Define (Variables definition used in this file internally)
-----------------------------------------------------------------------------*/
typedef struct
{
	u8 Revflag;
	u8 Renewflag;
}HTG_DRV;

static HTG_DRV HtgDrvObj;
/*-----------------------------------------------------------------------------
 Internal Function Prototype(declaration of functions used in this file internally)
-----------------------------------------------------------------------------*/
static void htg_delay_ms(u16 iDelLen);
static void htg_initial(void);
static void mcu_lcd_initial(void);

/*******************************************************************************
* Description	: LCM Power On Initial Process
* Input			: void
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
void drv_htg_pwr_on_init(void)
{
	mcu_lcd_initial();
	htg_initial();
	drv_htg_reset();
	return;
}

static void mcu_lcd_initial(void)
{
    GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(GPIOB,GPIO_PIN_0,GPIO_MODE_OUT_PP_LOW_FAST);
    
    GPIO_Init(GPIOE,GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOE,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_FAST);
    
    GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_FAST); 
    GPIO_Init(GPIOC,GPIO_PIN_2,GPIO_MODE_OUT_PP_HIGH_FAST); 
}
/*******************************************************************************
* Description	: LCM Initial Process
* Input			: void
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
static void htg_initial(void)
{
	MCU_LCD_RESET(LO);//reset?Áî0
	htg_delay_ms(50); // ëxªÙ2um

	MCU_LCD_RESET(HI);//reset?Áî1
	htg_delay_ms(20);
	
	drv_htg_write_command(0xE2);//soft-reset
	drv_htg_write_command(0xA2);//bias set
	drv_htg_write_command(0xA1);//normal
	drv_htg_write_command(0xC0);//scan direction
	drv_htg_write_command(0x2C);//internal power-manage
	drv_htg_write_command(0x2E);
	drv_htg_write_command(0x2F);
	drv_htg_write_command(0x81);//set reference voltage mode
	drv_htg_write_command(0x10);//voltage reg-L 0x00 ~ 0x3f
	drv_htg_write_command(0x21);//voltage reg-H 0x21 ~ 0x27
	drv_htg_clr_allscr();//??3y?D??
	drv_htg_write_command(0x40); //?????a??
	drv_htg_write_command(0xAF); //¨Œ??¡ÙDD?a??????

	return;
}

/*******************************************************************************
* Description	: LCM Clear Screen Process
* Input			: void
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
void drv_htg_clr_allscr(void)
{
	u8 Y_Axis, X_Axis;
	
	for (Y_Axis = 0x00; Y_Axis <= HTG_PAGE; Y_Axis++)
	{
		drv_htg_set_address(0, Y_Axis);
		for (X_Axis = 0x00; X_Axis < 0x80; X_Axis++)
		{
			drv_htg_write_data(0x00);
		}
	}
	return;
}

/*******************************************************************************
* Description	: LCM Reset Process
* Input			: void
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
void drv_htg_reset(void)
{
	HtgDrvObj.Revflag = DISABLE;
	return;
}

/*******************************************************************************
* Description	: LCM Set Address Process
* Input			: X_Axis : Display X_Axis unit Dot
				: Y_Axis : Display Y_Axis unit Dot
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
void drv_htg_set_address(u8 X_Axis, u8 Y_Axis)
{
	u8 ColAddrL, ColAddrH, PageNum;
	
	if (HtgDrvObj.Revflag != ENABLE)	{ X_Axis += 0x04; }
	ColAddrL = (X_Axis & 0x0f);
	ColAddrH = (X_Axis >> 0x04) + 0x10;
	PageNum  = Y_Axis & HTG_PAGE;
	drv_htg_write_command(0xB0 + PageNum);
	drv_htg_write_command(ColAddrH);
	drv_htg_write_command(ColAddrL);
	return;
}

/*******************************************************************************
* Description	: Screen reverse display process
* Input			: Reverse : reverse function parameter
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
void drv_htg_reverse_scr(FunctionalState Reverse)
{
	drv_htg_clr_allscr();
	
	if (Reverse == ENABLE)
	{
		drv_htg_write_command(0xA0);
		drv_htg_write_command(0xC8);
	}
	else
	{
		drv_htg_write_command(0xA1);
		drv_htg_write_command(0xC0);
	}
	
	HtgDrvObj.Revflag = ((Reverse == ENABLE) ? ENABLE : DISABLE);

	return;
}

/*******************************************************************************
* Description	: Get reverse flag process
* Input			: void
* Output		: void
* Return		: reverse flag
* Others		: void
********************************************************************************/
bool drv_htg_get_revflag(void)
{
	bool bRes = FALSE;
	
	bRes = (HtgDrvObj.Revflag == ENABLE) ? TRUE : FALSE;

	return bRes;
}

/*******************************************************************************
* Description	: write data process
* Input: cData : data
* Output: void
* Return: void
* Others: void
********************************************************************************/
void drv_htg_write_data(u8 cData)
{
	u8 iBit;
	
	MCU_LCD_SCK(HI);
	MCU_LCD_RS(HI);
	MCU_LCD_CS(LO);
	for (iBit = 0; iBit < 0x08; iBit++)
	{
		MCU_LCD_SCK(LO);
		//api_time_soft1us(2);
		MCU_LCD_SDI(((cData & 0x80) == 0x00) ? LO : HI);
		MCU_LCD_SCK(HI);
		cData <<= 0x01;
	}
	MCU_LCD_CS(HI);
	MCU_LCD_RS(LO);
	return;
}

/*******************************************************************************
* Description	: write command process
* Input			: cCmd : command
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
void drv_htg_write_command(u8 cCmd)
{
	u8 iBit;
	
	MCU_LCD_SCK(HI);
	MCU_LCD_RS(LO);
	MCU_LCD_CS(LO);
	for (iBit = 0; iBit < 0x08; iBit++)
	{
		MCU_LCD_SCK(LO);
		MCU_LCD_SDI(((cCmd & 0x80) == 0x00) ? LO : HI);
		MCU_LCD_SCK(HI);
		cCmd <<= 0x01;
	}
	MCU_LCD_CS(HI);
	MCU_LCD_RS(HI);
	return;
}

/*******************************************************************************
* Description	: set command process
* Input			: cID : command ID
				: cIndex : command description
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
void drv_htg_set_command(HTG_COMMAND cID, u8 cIndex)
{
	u8 cCmd;
	
	switch (cID)
	{
	case HTG_IDDON:
		cCmd = (cIndex != OFF) ? 0xAF : 0xAE;	//display on : off
		break;
	case HTG_IDREV:
		cCmd = (cIndex != OFF) ? 0xA6 : 0xA7;	//reverse : normal
		break;
	case HTG_IDEON:
		cCmd = (cIndex != OFF) ? 0xA5 : 0xA4;	//entire display on : normal
		break;
	case HTG_IDBIAS:
		cCmd = (cIndex != OFF) ? 0xA3 : 0xA2;	//1/7 Bias : 1/9 Bias
		break;
	case HTG_IDADC:								//set the display RAM address SEG output correspondence
		cCmd = (cIndex != OFF) ? 0xA0 : 0xA1;	//reverse : normal
		break;
	case HTG_IDSSE:
		cCmd = (cIndex != OFF) ? 0xC8 : 0xC0;
		break;
	case HTG_IDDSLS:							//set the display RAM display start line address
		cCmd = ((cIndex & 0x3f) | 0x40);		//cIndex:-dispaly start address
		break;
	case HTG_IDSPA:								//set the display RAM page address
		cCmd = ((cIndex & 0x0f) | 0xB0);		//cIndex:-page address
		break;
	case HTG_IDPCS:
		cCmd = ((cIndex & 0x0f) | 0x28);
		break;
	case HTG_IDRRS:
		cCmd = ((cIndex & 0x0f) | 0x20);
		break;
	case HTG_IDSCUA:							//set column upper address
		cCmd = ((cIndex & 0x0f) | 0x10);		//cIndex:-col.upper.address
		break;
	case HTG_IDSCLA:							//set column lower address
		cCmd = ((cIndex & 0x0f) | 0x00);		//cIndex:-col.lower.address
		break;
	case HTG_IDRRMW:							//reset read-modify-write
		cCmd = 0xEE;							//clear the "read-modify-write"mode
		break;
	case HTG_IDRESET:
		cCmd = 0xE2;							//resets the lcd module
		break;
	case HTG_IDSRMW:							//set read-modify-write
		cCmd = 0xE0;							//enter the "read-modify-write"mode
		break;
	case HTG_IDEVMS:
		cCmd = 0x81;
		break;
	case HTG_IDNOP:
	default:
		cCmd = 0xE3;							//NOP
		break;
	}
	drv_htg_write_command(cCmd);
	return;
}

void drv_htg_soft_reset(void)
{
	drv_htg_write_command(0xE2);		//soft-reset
	drv_htg_write_command(0xA2);		//bias set
	drv_htg_write_command(0xA1);		//normal
	drv_htg_write_command(0xC0);		//scan direction
	drv_htg_write_command(0x2C);		//internal power-manage
	drv_htg_write_command(0x2E);
	drv_htg_write_command(0x2F);
	drv_htg_write_command(0x81);		//set reference voltage mode
	drv_htg_write_command(0x10);		//voltage reg-L 0x00 ~ 0x3f
	drv_htg_write_command(0x23);		//voltage reg-H 0x21 ~ 0x27
	drv_htg_write_command(0x40); //?????a??
	//drv_htg_write_command(0xAF); //¨Œ??¡ÙDD?a??????
}

void drv_htg_hard_reset(void)
{
	MCU_LCD_RESET(LO);
	htg_delay_ms(50); // //¡Ù???2um

	MCU_LCD_RESET(HI);
	htg_delay_ms(20);
}
/*******************************************************************************
* Description	: delay process
* Input			: iDelLen : delay time parameter
* Output		: void
* Return		: void
* Others		: void
********************************************************************************/
static void htg_delay_ms(u16 iDelLen)
{
	u8 iInCyc;
	
	for (; iDelLen > 0; iDelLen--)
	{
		for (iInCyc = 0; iInCyc < 0x05; iInCyc++)
		{
			nop();
			nop();
			nop();
			nop();
			nop();
		}
	}
	return;
}

/*******************************************************************************
 ------------------------------- End of file ----------------------------------
********************************************************************************/
