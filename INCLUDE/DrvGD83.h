#ifndef __DRVGD83_H
#define __DRVGD83_H

#define DrvGD83_FUNENABLE

#ifdef DrvGD83_FUNENABLE
  #define DrvGD83_API
#else
  #define DrvGD83_API extern
#endif

DrvGD83_API void DrvMC8332_Software_Initial(void);
DrvGD83_API void DrvMC8332_TxPort_SetValidable(IO_ONOFF onoff);
DrvGD83_API void DrvGD83_UART_TxCommand(u8 * pBuf, u16 len);
DrvGD83_API bool DrvMc8332_UART_TxTail(void);
DrvGD83_API void DrvMC8332_UART_TxInterrupt(void);
DrvGD83_API void DrvMC8332_UART_Interrupt(void);

DrvGD83_API u8 DrvMC8332_CaretNotify_Queue_front(u8 **pBuf);
DrvGD83_API u8 DrvMC8332_AtNotify_Queue_front(u8 **pBuf);
DrvGD83_API u8 DrvMC8332_PocNotify_Queue_front(u8 **pBuf);
DrvGD83_API u8 DrvMC8332_GpsNotify_Queue_front(u8 **pBuf);
#endif
