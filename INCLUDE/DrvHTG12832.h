#ifndef __DRV_HTG12832_H
#define	__DRV_HTG12832_H

#include "stm8s.h"
//#include "CtypeDef.h"
//#include "ParamDef.h"

typedef enum
{
	HTG_IDDON		= 0x00,
	HTG_IDREV		= 0x01,
	HTG_IDEON		= 0x02,
	HTG_IDBIAS		= 0x03,
	HTG_IDADC		= 0x04,
	HTG_IDDSLS		= 0x05,
	HTG_IDSPA		= 0x06,
	HTG_IDSCUA		= 0x07,
	HTG_IDSCLA		= 0x08,
	HTG_IDRESET		= 0x09,
	HTG_IDSRMW		= 0x0A,
	HTG_IDRRMW		= 0x0B,
	HTG_IDSSE		= 0x0C,
	HTG_IDPCS,
	HTG_IDRRS,
	HTG_IDEVMS,
	HTG_IDNOP,
	HTG_IDNULL		= 0xFF
}HTG_COMMAND;

void LCD_Init(void);
extern void drv_htg_reset(void);
extern void drv_htg_soft_reset(void);
extern void drv_htg_pwr_on_init(void);
extern void drv_htg_clr_allscr(void);//«Â∆¡
extern bool drv_htg_get_revflag(void);
extern void drv_htg_write_data(u8 cData);
extern void drv_htg_write_command(u8 cCmd);
extern void drv_htg_set_address(u8 X_Axis, u8 Y_Axis);
extern void drv_htg_reverse_scr(FunctionalState Reverse);
extern void drv_htg_set_command(HTG_COMMAND cID, u8 cIndex);
extern void drv_htg_hard_reset(void);                              
                                
#endif