#ifndef _DRV_GT20L16S1Y_H
#define	_DRV_GT20L16S1Y_H

#include "stm8s.h"
#include "DrvFont.h"

extern void drv_gt20_pwr_on_init(void);
extern bool drv_gt20_data_output(DISP_TYPE cID, u16 iCode, u8 *cBuf);
extern bool drv_gt20_data_output2(DISP_TYPE cID, u16 iCode, u8 *cBuf);//Unicode π”√
#endif