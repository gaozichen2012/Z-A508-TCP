#include "AllHead.h"

#define Drv_Rx_Idle             0x00
#define Drv_Rx_Ok               0x01
#define Drv_Rx_Error		0x02

#define DrvCommHead_Len         15
#define DrvMC8332_UART_NoMAX    3	//define UART Tx buffer length value
#define DrvMC8332Poc_UART_NoMAX 10	//define UART Tx buffer length value//=7
#define DrvMC8332_UART_NoLEN    50	//define UART Rx buffer length value
#define DrvMC8332Caret_UART_NoLEN  40	//define UART Rx buffer length value
#define DrvMC8332At_UART_NoLEN  40	//define UART Rx buffer length value
#define DrvMC8332Poc_UART_NoLEN 88	//define UART Rx buffer length value-poc数据
#define DrvMC8332_UART_IDLE     0x00	//define UART constant value flag

#define DrvMC8332_UART_RUN      0x01
#define DrvMC8332_UART_TXBUSY   0	//define UART Tx busy delay length value

#define DrvMC8332_UART_RXDELAY	0x00200	//define UART Rx delay softwave delay length	
#define DrvGD83_UART_TXMAXDEL 0x0040	//define UART Tx MAX softwave delay length value

typedef enum{
  Head_At               = 0x01,
  Head_AtPoc            = 0x02,
  Head_Plus             = 0x03,
  Head_PlusPoc          = 0x04,
  Head_Caret	        = 0x05,
  Head_RecevTcp	        = 0x06,
  Head_CaretSIMST	= 0x07,
  Head_Null             = 0x00
}HEAD_Type;

const u8 *ucTxPocHeadInfo = "AT+POC=:";
const u8 *ucRxPocHeadInfo = "+POC:";
const u8 *ucRxOkHeadInfo = "OK";
const u8 *ucCaretSIMST = "^SIMST:";
const u8 *ucRxTcpDataHeadInfo = "^RECV:";
/******************************************************************************
;--------UART hardware drive macro define process
******************************************************************************/
typedef struct{							//define UART drive data type 
  struct{
    union{
      struct{
        u8 bWorkEnable  : 1;
        u8 bWorkMode    : 2;
        u8 bStep        : 4;
        u8 bUartValid	: 1;
      }Bits;
      u8 Byte;
    }Msg;
    u8 Delay;
    u16 UartDelay;
  }HardWare;	//hard config
  struct{
    union{
      struct{
        u16 bUartTxBusy         : 1;
        u16 bNewNot             : 1;
        u16 bAtPos              : 1;
        u16 bPlusPos            : 1;
        u16 bOkOErrorPos        : 1;	//o head
        u16 bOkOrError          : 2;
        u16 bAtPoc              : 1;
        u16 bPlusPoc            : 1;
        u16 bCaretPos           : 1;//^
        u16 bRecevTcp           : 1;
        u16 bTxOnOff            : 1;
        u16                     : 4;
      }Bits;
      u16 Byte;
    }Msg;
    u8 cHeadFlag;
    u8 cRxBuf[DrvCommHead_Len];
    u8 cRxLen;
    //u8 cTxBuf[150];
    //u8 cTxLen;
  }TxRxCmd;	//comand tx/rx
  
  
   struct{
   u8 cNotifyHead, cNotifyTail, cNotifyLen;
   u8 Notify[DrvMC8332_UART_NoMAX][DrvMC8332Caret_UART_NoLEN];
   u8 NotifyLen[DrvMC8332_UART_NoMAX];
  }RxCaretNotifyBuf;
   struct{
   u8 cNotifyHead, cNotifyTail, cNotifyLen;
   u8 Notify[DrvMC8332_UART_NoMAX][DrvMC8332At_UART_NoLEN];
   u8 NotifyLen[DrvMC8332_UART_NoMAX];
  }RxAtNotifyBuf;
  struct {
    u8 ucStartStop;
    u16 ucReceiveLen;
    u8 cNotifyHead, cNotifyTail, cNotifyLen;
    u8 Notify[DrvMC8332_UART_NoMAX][DrvMC8332_UART_NoLEN];
    u8 NotifyLen[DrvMC8332_UART_NoMAX];
  }RxGpsNotifyBuf;
  struct{
    u8 cNotifyHead, cNotifyTail, cNotifyLen;
    u8 Notify[DrvMC8332Poc_UART_NoMAX][DrvMC8332Poc_UART_NoLEN];
    u8 NotifyLen[DrvMC8332Poc_UART_NoMAX];
  }RxPocNotifyBuf;
}DrvMC8332_UART_DRV;

static DrvMC8332_UART_DRV       DrvGD83DrvObj;

static void CaretNotify_Queue_Start(u8 *buf, u8 len);
static void AtNotify_Queue_Start(u8 *buf, u8 len);
static void PocNotify_Queue_Start(u8 *buf, u8 len);
static void GpsNotify_Queue_Start(u8 *buf, u8 len);
static void AtNotify_Queue(u8 buf);
static void PocNotify_Queue(u8 buf);
static void Notify_Queue_Stop(void);
static void CaretNotify_Queue(u8 buf);
static void GpsNotify_Queue(u8 buf);

void DrvMC8332_Software_Initial(void)
{
  DrvGD83DrvObj.HardWare.Msg.Byte = 0x00;
  DrvGD83DrvObj.HardWare.Delay = 0x00;
  DrvGD83DrvObj.HardWare.UartDelay = 0x00;
  DrvGD83DrvObj.TxRxCmd.Msg.Byte = 0x00;
  DrvGD83DrvObj.TxRxCmd.cHeadFlag = Head_Null;
  DrvGD83DrvObj.TxRxCmd.cRxLen = 0x00;
  DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead = 0x00;
  DrvGD83DrvObj.RxAtNotifyBuf.cNotifyTail = 0x00;
  DrvGD83DrvObj.RxAtNotifyBuf.cNotifyLen = 0x00;
	
  DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead = 0x00;
  DrvGD83DrvObj.RxPocNotifyBuf.cNotifyTail = 0x00;
  DrvGD83DrvObj.RxPocNotifyBuf.cNotifyLen = 0x00;

  DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead = 0x00;
  DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyTail = 0x00;
  DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyLen = 0x00;
}
void DrvMC8332_TxPort_SetValidable(IO_ONOFF onoff)
{
  DrvGD83DrvObj.TxRxCmd.Msg.Bits.bUartTxBusy = onoff;
  DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError = Drv_Rx_Idle;
}

//test ok
void DrvGD83_UART_TxCommand(u8 * pBuf, u16 len)
{
  u16 i=0;
  u16 del;
  if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError == Drv_Rx_Idle)
  {
    for(; i < len; i++)
    {
      del = 0;
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bTxOnOff = OFF;
      UART1_SendData8((unsigned char)pBuf[i]);
      while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
      {
        del++;
        if (DrvGD83_UART_TXMAXDEL < del)
        {
          DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError = Drv_Rx_Error;
          break;
        }
      }
      del = UART1_FLAG_TXE;
    }
  }
}

bool DrvMc8332_UART_TxTail(void)
{
  bool r = TRUE;
  u8 i;
  u16 del;
  DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError =Drv_Rx_Idle;
  if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError == Drv_Rx_Idle)
  {
    for(i = 0; i < 2; i++)
    {
      if(i == 0)
      {
        UART1_SendData8(0x0D);
      }
      else
      {
        UART1_SendData8(0x0A);
      }
      del = 0;
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bTxOnOff = OFF;
      while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
      {
        del++;
        if (DrvGD83_UART_TXMAXDEL < del)
        {
          DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError = Drv_Rx_Error;
          break;
        }
      }
      del = UART1_FLAG_TXE;
    }
    i = 0;
    del = 0;
    if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError == Drv_Rx_Idle)
    {
      while(1)
      {
        if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError != Drv_Rx_Idle)
        {
          break;
        }
        del++;
        DEL_Soft1ms(1);
        //Delay_ms(1);
        if (del > DrvMC8332_UART_RXDELAY)
        {
          DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError = Drv_Rx_Error;
          break;
        }
      }
    }
  }
  if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError != Drv_Rx_Ok)
  {
    r = FALSE;
  }
  return r;
}

void DrvMC8332_UART_TxInterrupt(void)
{
  DrvGD83DrvObj.TxRxCmd.Msg.Bits.bTxOnOff = ON;
}

void DrvMC8332_UART_Interrupt(void)
{
  static u8 head; //, pbuf[50]
  u8 ucLen;
  u16 buf;
  DrvGD83DrvObj.HardWare.Msg.Bits.bUartValid = ON;
  buf = UART1_ReceiveData8();
  head++;
  if(head >= 50)
  {
    head = 0x00;
  }
  //pbuf[head] = buf;
  switch((u8)buf)
  {
  case 0x0D:
  case 0x0A:
    switch(DrvGD83DrvObj.TxRxCmd.cHeadFlag)
    {
    case Head_RecevTcp://bubiao
    //case Head_RecevUdp:
      if(DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop == 0x02)
      {
        if(DrvGD83DrvObj.RxGpsNotifyBuf.ucReceiveLen != 0x00)
        {
          DrvGD83DrvObj.RxGpsNotifyBuf.ucReceiveLen--;
          GpsNotify_Queue(buf);
          return;
        }
        else
        {
        }
      }
      break;
    case Head_At:
    case Head_Plus:
    case Head_AtPoc:
    case Head_PlusPoc:
    default:
      break;
    }
    Notify_Queue_Stop();
    DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop=0;
    DrvGD83DrvObj.RxGpsNotifyBuf.ucReceiveLen=0;
    DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot = OFF;
    DrvGD83DrvObj.TxRxCmd.cHeadFlag = Head_Null;
    DrvGD83DrvObj.TxRxCmd.Msg.Bits.bCaretPos = OFF;
    DrvGD83DrvObj.TxRxCmd.Msg.Bits.bAtPos = OFF;
    DrvGD83DrvObj.TxRxCmd.Msg.Bits.bPlusPos = OFF;
    DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOErrorPos = OFF;
    DrvGD83DrvObj.TxRxCmd.Msg.Bits.bPlusPoc = OFF;
    DrvGD83DrvObj.TxRxCmd.Msg.Bits.bCaretPos = OFF;
    DrvGD83DrvObj.TxRxCmd.Msg.Bits.bRecevTcp = OFF;
    DrvGD83DrvObj.TxRxCmd.cRxLen = 0;
    return;
    break;
  case 'A':
    if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot == OFF)
    {
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot = ON;
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bAtPos = ON;
      DrvGD83DrvObj.TxRxCmd.cRxBuf[DrvGD83DrvObj.TxRxCmd.cRxLen] = buf;
      DrvGD83DrvObj.TxRxCmd.cRxLen++;
      return;
    }
    break;
  case '+':
    if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot == OFF)
    {
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot = ON;
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bPlusPos = ON;
      DrvGD83DrvObj.TxRxCmd.cRxBuf[DrvGD83DrvObj.TxRxCmd.cRxLen] = buf;
      DrvGD83DrvObj.TxRxCmd.cRxLen++;
      return;
    }
    break;
  case 'O':
  case 'E':
    if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot == OFF)
    {
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot = ON;
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOErrorPos = ON;
      DrvGD83DrvObj.TxRxCmd.cRxBuf[DrvGD83DrvObj.TxRxCmd.cRxLen] = buf;
      DrvGD83DrvObj.TxRxCmd.cRxLen++;
      return;
    }
    break;
  case '^':
    if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot == OFF)
    {
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot = ON;
      DrvGD83DrvObj.TxRxCmd.Msg.Bits.bCaretPos = ON;//插入符号^
      DrvGD83DrvObj.TxRxCmd.cRxBuf[DrvGD83DrvObj.TxRxCmd.cRxLen] = buf;
      DrvGD83DrvObj.TxRxCmd.cRxLen++;
      return;
    }
    break; 
  default:
    break;  
  }
  if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bNewNot == ON && DrvGD83DrvObj.TxRxCmd.cHeadFlag == Head_Null)//接收一段指令后收到换行的指令
  {
    DrvGD83DrvObj.TxRxCmd.cRxBuf[DrvGD83DrvObj.TxRxCmd.cRxLen] = buf;
    DrvGD83DrvObj.TxRxCmd.cRxLen++;
    ucLen = DrvGD83DrvObj.TxRxCmd.cRxLen-1;
#if 1//插入符^判断 Tom added in 2017.11.17
    if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bCaretPos == ON)//判断^
    {
      if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bRecevTcp == OFF)//判断是否是TCP的指令
      {
        if(DrvGD83DrvObj.TxRxCmd.cRxBuf[ucLen] == ucRxTcpDataHeadInfo[ucLen])//strlen(ucRxTcpDataHeadInfo));
        {
          if(DrvGD83DrvObj.TxRxCmd.cRxLen == 6)//strlen(ucRxTcpDataHeadInfo))	//rx tcp
          {
            DrvGD83DrvObj.TxRxCmd.cHeadFlag = Head_RecevTcp;
            GpsNotify_Queue_Start(DrvGD83DrvObj.TxRxCmd.cRxBuf, 0);
          }
        }
        else
        {
          DrvGD83DrvObj.TxRxCmd.Msg.Bits.bRecevTcp = ON;
        }  
      }
      if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bRecevTcp != OFF)
      {
        DrvGD83DrvObj.TxRxCmd.cHeadFlag = Head_Caret;	
        CaretNotify_Queue_Start(DrvGD83DrvObj.TxRxCmd.cRxBuf, DrvGD83DrvObj.TxRxCmd.cRxLen);
      }
    }
#endif
    if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bPlusPos == ON)//有"+"
    {
      if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bPlusPoc == OFF)//非"+POC"
      {
        if(DrvGD83DrvObj.TxRxCmd.cRxBuf[ucLen] == ucRxPocHeadInfo[ucLen])//strlen(ucRxPocHeadInfo)); //+POC://一直判斷，當頭與"+POC"一致時，進入，去頭
        {
          if(DrvGD83DrvObj.TxRxCmd.cRxLen == 5)//strlen(ucRxPocHeadInfo))		//rx poc
          {
            PocNotify_Queue_Start(DrvGD83DrvObj.TxRxCmd.cRxBuf, 0);
            DrvGD83DrvObj.TxRxCmd.cHeadFlag = Head_PlusPoc;//Head_PlusPoc(+POC:)
          }
        }
        else
        {
          DrvGD83DrvObj.TxRxCmd.Msg.Bits.bPlusPoc = ON;//判斷+POC狀態
        }
      }
      if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bPlusPoc  != OFF)
      {
        DrvGD83DrvObj.TxRxCmd.cHeadFlag = Head_Plus;		//at+ head
        AtNotify_Queue_Start(DrvGD83DrvObj.TxRxCmd.cRxBuf+1, DrvGD83DrvObj.TxRxCmd.cRxLen-1);//-------------------------------------------------
      }
    }
    if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bAtPos == ON)//A
    {
      if(DrvGD83DrvObj.TxRxCmd.cRxBuf[ucLen] == ucTxPocHeadInfo[ucLen])//strlen(ucTxPocHeadInfo));//如果是AT+POC=
      {
        if(DrvGD83DrvObj.TxRxCmd.cRxLen == 7)//strlen(ucTxPocHeadInfo))       //at+poc=
        {
          DrvGD83DrvObj.TxRxCmd.cHeadFlag = Head_AtPoc;
          PocNotify_Queue_Start(DrvGD83DrvObj.TxRxCmd.cRxBuf, 0);
        }
      }
      else
      {
        if(DrvGD83DrvObj.TxRxCmd.cRxLen > 0x03)
        {
          DrvGD83DrvObj.TxRxCmd.cHeadFlag = Head_At;
          AtNotify_Queue_Start(DrvGD83DrvObj.TxRxCmd.cRxBuf+3,
                               DrvGD83DrvObj.TxRxCmd.cRxLen-3);
        }
      }
    }
    if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOErrorPos == ON)			//ok or error
    {
      if(DrvGD83DrvObj.TxRxCmd.cRxBuf[ucLen] == ucRxOkHeadInfo[ucLen])//strlen(ucRxOkHeadInfo));  //ok
      {
        if(DrvGD83DrvObj.TxRxCmd.cRxLen == 2)//strlen(ucRxOkHeadInfo))
        {
          DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError = Drv_Rx_Ok;
        }
      }
      else
      {
        if(DrvGD83DrvObj.TxRxCmd.cRxLen > 2)//strlen(ucRxOkHeadInfo))
        {
          DrvGD83DrvObj.TxRxCmd.Msg.Bits.bOkOrError = Drv_Rx_Error;
        }
      }
    }
  }
  else
  {
    switch(DrvGD83DrvObj.TxRxCmd.cHeadFlag)
    {
    case Head_At:
    case Head_Plus:
      AtNotify_Queue(buf);
      break;
    case Head_AtPoc:
    case Head_PlusPoc:
      PocNotify_Queue(buf);
      break;
    case Head_Caret:
      CaretNotify_Queue(buf);
      break;
    case Head_RecevTcp:
      GpsNotify_Queue(buf);
      break;
    default:
      break;
    }
  }
}

static void CaretNotify_Queue_Start(u8 *buf, u8 len)
{
  u8 i;
  if(DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead >= DrvMC8332_UART_NoMAX)
  {
    DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead = 0;
  }
  for(i = 0;i < len; i++)
  {
    DrvGD83DrvObj.RxCaretNotifyBuf.Notify[DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead][i] = buf[i];
  }
  DrvGD83DrvObj.RxCaretNotifyBuf.NotifyLen[DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead] = len;
}

static void AtNotify_Queue_Start(u8 *buf, u8 len)
{
  u8 i;
  if(DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead >= DrvMC8332_UART_NoMAX)
  {
    DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead = 0;
  }
  for(i = 0;i < len; i++)
  {
    DrvGD83DrvObj.RxAtNotifyBuf.Notify[DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead][i] = buf[i];
  }
  DrvGD83DrvObj.RxAtNotifyBuf.NotifyLen[DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead] = len;
}

static void PocNotify_Queue_Start(u8 *buf, u8 len)
{
  u8 i;
  if(DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead >= DrvMC8332Poc_UART_NoMAX)
  {
    DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead = 0;
  }
  for(i = 0;i < len; i++)
  {
    DrvGD83DrvObj.RxPocNotifyBuf.Notify[DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead][i] = buf[i];
  }
  DrvGD83DrvObj.RxPocNotifyBuf.NotifyLen[DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead] = len;
}

static void GpsNotify_Queue_Start(u8 *buf, u8 len)
{
  u8 i;
  if(DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead >= DrvMC8332_UART_NoMAX)
  {
    DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead = 0;
  }
  for(i = 0;i < len; i++)
  {
    DrvGD83DrvObj.RxGpsNotifyBuf.Notify[DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead][i] = buf[i];
    if(buf[i] == 0x2c)
    {
      switch(DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop)
      {
      case 0x00:
        DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop++;
        break;
      case 0x01:
        DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop = 2;
        break;
      case 0x02:
        break;
      default:
        break;
      }
    }
    else
    {
      switch(DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop)
      {
      case 0x01:
        DrvGD83DrvObj.RxGpsNotifyBuf.ucReceiveLen = buf[i]-0x30;
        break;
      case 0x00:
      case 0x02:
      default:
        break;
      }
    }
  }
  DrvGD83DrvObj.RxGpsNotifyBuf.NotifyLen[DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead] = len;
}

static void Notify_Queue_Stop(void)
{
  if(DrvGD83DrvObj.TxRxCmd.cHeadFlag == Head_Null)
  {
    if(DrvGD83DrvObj.TxRxCmd.cRxLen != 0x00)
    {
      if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bPlusPos == ON)
      {
        if(DrvGD83DrvObj.RxAtNotifyBuf.cNotifyLen < DrvMC8332_UART_NoMAX)
        {
          AtNotify_Queue_Start(DrvGD83DrvObj.TxRxCmd.cRxBuf+1, DrvGD83DrvObj.TxRxCmd.cRxLen-1);
          DrvGD83DrvObj.RxAtNotifyBuf.cNotifyLen++;
          DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead++;
        }
      }
      if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bCaretPos == ON)
      {
        if(DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyLen < DrvMC8332_UART_NoMAX)
        {
          CaretNotify_Queue_Start(DrvGD83DrvObj.TxRxCmd.cRxBuf, DrvGD83DrvObj.TxRxCmd.cRxLen);
          DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyLen++;
          DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead++;
        }
      }
      
      if(DrvGD83DrvObj.TxRxCmd.Msg.Bits.bAtPos == ON)
      {
        if((DrvGD83DrvObj.RxAtNotifyBuf.cNotifyLen < DrvMC8332_UART_NoMAX))
        {
          AtNotify_Queue_Start(DrvGD83DrvObj.TxRxCmd.cRxBuf+3, DrvGD83DrvObj.TxRxCmd.cRxLen-3);
          DrvGD83DrvObj.RxAtNotifyBuf.cNotifyLen++;
          DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead++;
        }
      }
    }
  }
  else
  {
    switch(DrvGD83DrvObj.TxRxCmd.cHeadFlag)
    {
    case Head_At:
    case Head_Plus:
      if(DrvGD83DrvObj.RxAtNotifyBuf.cNotifyLen < DrvMC8332_UART_NoMAX
         && DrvGD83DrvObj.RxAtNotifyBuf.NotifyLen[DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead] != 0x00)
      {
        DrvGD83DrvObj.RxAtNotifyBuf.cNotifyLen++;
        DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead++;
      }
      break;
    case Head_AtPoc:
    case Head_PlusPoc:
      if((DrvGD83DrvObj.RxPocNotifyBuf.cNotifyLen < DrvMC8332Poc_UART_NoMAX)
         && (DrvGD83DrvObj.RxPocNotifyBuf.NotifyLen[DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead] != 0x00))
      {
        DrvGD83DrvObj.RxPocNotifyBuf.cNotifyLen++;
        DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead++;
      }
      break;
    case Head_Caret:
      if(DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyLen < DrvMC8332_UART_NoMAX
         && DrvGD83DrvObj.RxCaretNotifyBuf.NotifyLen[DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead] != 0x00)
      {
        DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyLen++;
        DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead++;
      }
      break;
    case Head_RecevTcp:
      if((DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyLen < DrvMC8332_UART_NoMAX)
         && (DrvGD83DrvObj.RxGpsNotifyBuf.NotifyLen[DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead] != 0x00))
      {
        DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyLen++;
        DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead++;
      }
      break;
    default:
      break;
    }
  }
}

static void CaretNotify_Queue(u8 buf)
{
	u8 len;
	
	len = DrvGD83DrvObj.RxCaretNotifyBuf.NotifyLen[DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead];
	if(len < DrvMC8332At_UART_NoLEN)
	{
		DrvGD83DrvObj.RxCaretNotifyBuf.Notify[DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead][len] = buf;
		DrvGD83DrvObj.RxCaretNotifyBuf.NotifyLen[DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyHead]++;
	}
}

static void AtNotify_Queue(u8 buf)
{
	u8 len;
	
	len = DrvGD83DrvObj.RxAtNotifyBuf.NotifyLen[DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead];
	if(len < DrvMC8332At_UART_NoLEN)
	{
		DrvGD83DrvObj.RxAtNotifyBuf.Notify[DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead][len] = buf;
		DrvGD83DrvObj.RxAtNotifyBuf.NotifyLen[DrvGD83DrvObj.RxAtNotifyBuf.cNotifyHead]++;
	}
}
static void PocNotify_Queue(u8 buf)
{
	u8 len;
	
	len = DrvGD83DrvObj.RxPocNotifyBuf.NotifyLen[DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead];
	if(len < DrvMC8332Poc_UART_NoLEN)
	{
		DrvGD83DrvObj.RxPocNotifyBuf.Notify[DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead][len] = buf;
		DrvGD83DrvObj.RxPocNotifyBuf.NotifyLen[DrvGD83DrvObj.RxPocNotifyBuf.cNotifyHead]++;
	}
}

static void GpsNotify_Queue(u8 buf)
{
  u8 len;
  len = DrvGD83DrvObj.RxGpsNotifyBuf.NotifyLen[DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead];
  if(len < DrvMC8332_UART_NoLEN)
  {
    DrvGD83DrvObj.RxGpsNotifyBuf.Notify[DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead][len] = buf;
    DrvGD83DrvObj.RxGpsNotifyBuf.NotifyLen[DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyHead]++;
    if(buf == 0x2c)
    {
      switch(DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop)
      {
      case 0x00:
        DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop++;
        break;
      case 0x01:
        DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop = 2;
        break;
      case 0x02:
      default:
        break;
      }
    }
    else
    {
      switch(DrvGD83DrvObj.RxGpsNotifyBuf.ucStartStop)
      {
      case 0x01:
        DrvGD83DrvObj.RxGpsNotifyBuf.ucReceiveLen = buf-0x30;
        break;
      case 0x00:
      case 0x02:
      default:
        break;
      }
    }
  }
}

u8 DrvMC8332_CaretNotify_Queue_front(u8 **pBuf)
{
  u8 r=0;
  if(DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyLen > 0)
  {
    *pBuf = DrvGD83DrvObj.RxCaretNotifyBuf.Notify[DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyTail];
    r=DrvGD83DrvObj.RxCaretNotifyBuf.NotifyLen[DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyTail];
    if(++DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyTail >= DrvMC8332_UART_NoMAX)
    {
      DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyTail = 0;
    }
    DrvGD83DrvObj.RxCaretNotifyBuf.cNotifyLen--;
  }
  return r;
}

u8 DrvMC8332_AtNotify_Queue_front(u8 **pBuf)
{
  u8 r=0;
  if(DrvGD83DrvObj.RxAtNotifyBuf.cNotifyLen > 0)
  {
    *pBuf = DrvGD83DrvObj.RxAtNotifyBuf.Notify[DrvGD83DrvObj.RxAtNotifyBuf.cNotifyTail];
    r=DrvGD83DrvObj.RxAtNotifyBuf.NotifyLen[DrvGD83DrvObj.RxAtNotifyBuf.cNotifyTail];
    if(++DrvGD83DrvObj.RxAtNotifyBuf.cNotifyTail >= DrvMC8332_UART_NoMAX)
    {
      DrvGD83DrvObj.RxAtNotifyBuf.cNotifyTail = 0;
    }
    DrvGD83DrvObj.RxAtNotifyBuf.cNotifyLen--;
  }
  return r;
}

u8 DrvMC8332_PocNotify_Queue_front(u8 **pBuf)
{
	u8 r=0;
	
	if(DrvGD83DrvObj.RxPocNotifyBuf.cNotifyLen > 0)
	{
		*pBuf = DrvGD83DrvObj.RxPocNotifyBuf.Notify[DrvGD83DrvObj.RxPocNotifyBuf.cNotifyTail];
		r=DrvGD83DrvObj.RxPocNotifyBuf.NotifyLen[DrvGD83DrvObj.RxPocNotifyBuf.cNotifyTail];
		if(++DrvGD83DrvObj.RxPocNotifyBuf.cNotifyTail >= DrvMC8332Poc_UART_NoMAX)
		{
			DrvGD83DrvObj.RxPocNotifyBuf.cNotifyTail = 0;
		}
		DrvGD83DrvObj.RxPocNotifyBuf.cNotifyLen--;
	}
	return r;
}
u8 DrvMC8332_GpsNotify_Queue_front(u8 **pBuf)
{
  u8 r=0x00;
  if(DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyLen > 0)
  {
    *pBuf = DrvGD83DrvObj.RxGpsNotifyBuf.Notify[DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyTail];
    r=DrvGD83DrvObj.RxGpsNotifyBuf.NotifyLen[DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyTail];
    if(++DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyTail >= DrvMC8332_UART_NoMAX)
    {
      DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyTail = 0;
    }
    DrvGD83DrvObj.RxGpsNotifyBuf.cNotifyLen--;
  }
  return r;
}