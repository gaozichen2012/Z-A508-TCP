#include "AllHead.h"
u8 LED_Conut=0;
void LED_IntOutputRenew(void)
{

  if( GetTaskId()==Task_Start)
  {
    LED_Conut++;
    if(LED_Conut==5)
    {
      Set_GreenLed(LED_ON);
    }
    if(LED_Conut==8)
    {
      Set_GreenLed(LED_OFF);
      LED_Conut=0;
    }
  }
  else
  {
    if(POC_ReceivedVoice_Flag==TRUE)
    {
      Set_GreenLed(LED_ON);
    }
    else
    {
      LED_Conut++;
      if(LED_Conut>=49)
      {
#ifdef BEIDOU//使用外置北斗
        if(BDValid==1)
#else//使用内置GPS
        if(PositionInfoSendToATPORT_RedLed_Flag==TRUE)
#endif
        {
          Set_RedLed(LED_ON);
        }
        else
        {
          Set_GreenLed(LED_ON);
        }
        
      }
      if(LED_Conut>=51)
      {
        PositionInfoSendToATPORT_RedLed_Flag=FALSE;
        Set_GreenLed(LED_OFF);
        Set_RedLed(LED_OFF);
        LED_Conut=0;
      }
      if(LED_Conut<=48)
      {
        Set_GreenLed(LED_OFF);
        if(WriteFreq_Flag==TRUE)//解决读频红灯常亮的问题，应该是闪红灯
        {
          Set_RedLed(LED_OFF);
        }
        else
        {
        }
        
        //Set_RedLed(LED_OFF);
      }
    }
  }
}