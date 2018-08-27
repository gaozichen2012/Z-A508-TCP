#include "AllHead.h"
u8 BeidouRxData[75];
u8 BeidouRxDataLen=0;
u8 RxStartFlag[6];
//北斗消息体
#if 1
u8  BDValid=0;
u32 BDLongitude_Degree;//度
u32 BDLongitude_Minute;//分
u32 BDLongitude_Second;//小数点后的数
u32 BDLatitude_Degree;//度
u32 BDLatitude_Minute;//分
u32 BDLatitude_Second;//小数点后的数
u16 BDSpeed;//速度
u16 BDDirection;//方向
#endif

void ApiBeidou_Get_location_Information(void)
{
  u8 *pBuf;
  u8 i=0,Ccomma=0,cDot=0,cHead=0;
/***************获取经纬度数据*********************************************************************************************************/
  ////$GNRMC,000829.799,V,,,,,0.00,0.00,060180,,,N*58 \r
 // $GNRMC,025357.000,A,2231.0305,N,11355.0570,E,0.44,273.66,310118,,,A*7F
  //      0          1 2         3 4          5 6    7      8      
  //if(UART_BeidouGNRMCCommand()==TRUE)
  pBuf=BeidouRxData;
  for(i=0;i<BeidouRxDataLen;i++)
  {
      if(',' == pBuf[i])//2
      {
        switch(Ccomma)
        {
        case 0:
          break;
        case 1:
          if(pBuf[i+1]=='A')
            BDValid=1;
          else
            BDValid=0;
          break;
        case 2:
          break;
        case 3:
          if((i - cHead) >= 4)
          {
           BDLatitude_Second  = CHAR_TO_Digital(&pBuf[cHead], i-cHead);//纬度后四位
          }
          break;
        case 4://N
          break;
        case 5:
          if((i - cHead) >= 4)
          {
            BDLongitude_Second = CHAR_TO_Digital(&pBuf[cHead], i-cHead);//经度后四位
          }
          break;
        case 6:
          break;
        case 7:
          break;
        default:
          break;
        }
        cHead = i+1;
        Ccomma++;
      }
    else
    {
      if('.' == pBuf[i])//2
      {
        switch(cDot)
        {
        case 0:
          break;
        case 1://纬度22
          if((i - cHead) >= 4)
          {
            BDLatitude_Degree  = CHAR_TO_Digital(&pBuf[cHead],2);//纬度前2位
            BDLatitude_Minute  = CHAR_TO_Digital(&pBuf[cHead+2],2);//纬度中2位   
          }
          break;
        case 2:
          if((i - cHead) >= 5)
          {
            BDLongitude_Degree = CHAR_TO_Digital(&pBuf[cHead], 3);//经度前2位
            BDLongitude_Minute = CHAR_TO_Digital(&pBuf[cHead+3],2);//经度中2位
          }
          break;
        case 3:
          BDSpeed = (u16)(1.85*CHAR_TO_Digital(&pBuf[cHead], i-cHead));//速度（1.85*海里/小时）
          //BDSpeed = 20;//速度（1.85*海里/小时）
          break;
        case 4:
          BDDirection=CHAR_TO_Digital(&pBuf[cHead], i-cHead);
          break;
        default:
          break;
        }
        cHead = i+1;
        cDot++;
      }
    }
  }
}