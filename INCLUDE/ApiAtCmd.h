#ifndef __APIATCMD_H
#define __APIATCMD_H

#include "AllHead.h"


typedef enum{
  ATCOMM0_OSSYSHWID             = 0x00,
  ATCOMM1_PPPCFG                = 0x01,
  ATCOMM2_ZTTS_Abell            = 0x02,
  ATCOMM3_GD83Reset             = 0x03,
  ATCOMM4_GD83Mode              = 0x04,
  ATCOMM5_CODECCTL              = 0x05,
  ATCOMM6_CSQ                   = 0x06,
  ATCOMM7_VGR                   = 0x07,
  ATCOMM8_CheckTcp              = 0x08,
  ATCOMM9_SetIp                 = 0x09,
  ATCOMM10_SendTcp              = 0x0A,
  ATCOMM11_ZpppOpen             = 0x0B,
  ATCOMM12_CheckPPP             = 0x0C,
  ATCOMM13_CheckRssi            = 0x0D,
  ATCOMM14_CheckCard            = 0x0E,
  ATCOMM3_GD83StartupReset      = 0x0F,
  ATCOMM15_HDRCSQ               = 0x10,
}AtCommType;
extern u8 HDRCSQValue;//HDRCSQ的值
extern u8 BootProcess_SIMST_Flag;
extern u8 BootProcess_PPPCFG_Flag;
extern u8 ApiAtCmd_TrumpetVoicePlay_Flag;
extern bool ApiAtCmd_ZTTS_Flag;
extern u8 KeyDownUpChoose_GroupOrUser_Flag;
extern u8 CSQ_Flag;
extern bool PositionInformationSendToATPORT_Flag;
extern bool PositionInfoSendToATPORT_RedLed_Flag;
extern bool PositionInfoSendToATPORT_InfoDisplay_Flag;
extern bool ApiAtCmd_WritCommand(AtCommType id, u8 *buf, u16 len);
extern bool ApiAtCmd_PlayVoice(AtVoiceType id, u8 *buf, u8 len);

extern void csq_value_for_display(void);
extern void sou_xing_count_for_display(void);

extern void HDRCSQSignalIcons(void);
extern void ApiAtCmd_100msRenew(void);
extern void ApiCaretCmd_10msRenew(void);
extern void ApiAtCmd_10msRenew(void);
extern u8 ApiAtCmd_GetIccId(u8 **pBuf);
extern void ApiGetIccidBuf(void);
extern u16 ApiAtCmd_tcp_state(void);
extern u16 ApiAtCmd_Ppp_state(void);
extern u32  CHAR_TO_Digital(u8 * pBuf, u8 Len);
extern void ApiAtCmd_Get_location_Information(void);
extern void ApiAtCmd_Get_DateTime_Information(void);
extern u8 Data_Longitude_Minute(void);//经度整数位
extern u32 Data_Longitude_Second(void);//经度小数位
extern u8 Data_Latitude_Minute(void);//纬度整数位
extern u32 Data_Latitude_Second(void);//纬度小数位
extern u8 Data_Time0(void);//时
extern u8 Data_Time1(void);//分
extern u8 Data_Time2(void);//秒
extern u8 Data_Date0(void);//年
extern u8 Data_Date1(void);//月
extern u8 Data_Date2(void);//日
extern u16 Data_Speed(void);//Speed
#endif