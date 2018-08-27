#ifndef __ALLHEAD_H
#define __ALLHEAD_H
//

#include "stm8s.h"


#include "sysclock.h"
#include "McuConfig.h"
#include "stm8s_it.h"
#include "CommonLib.h"
#include "Define.h"// 
#include "Delay.h"

#include "Voice.h"

//通讯模块
#include "DrvGD83.h"
#include <string.h>
#include "ApiPocCmd.h"
#include "ApiAtCmd.h"

//键盘
#include "DrvKeyboard.h"
#include "ApiKeyboard.h"

//显示屏
#include "ApiDisplay.h"
#include "DrvHTG12832.h"
#include "DrvGT20L16S1Y.h"

//BEEP
#include "Tone.h"
#include "Beep.h"

//任务
#include "Task.h"
#include "System.h"

//指示灯
#include "ApiLED.h"

//写频
#include "uart3.h"
#include "Flash_eeprom.h"
#include "Config.h"//部标强行添加
#include "DataStructAddr.h"
//部标
#include "ApiGpsCmd.h"

//电池电量检测
#include "ADC.h"
//字库IC
#include "GT20L16P1Y.h"
#include "GT20L16P1Y_D.h"//字库补丁

//菜单显示
#include "ApiMenu.h"

//北斗
#include "ApiBeidou.h"

//软件版本设置
#include "KeyCmd.h"
#endif