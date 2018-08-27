#ifndef __APIMENU_H
#define __APIMENU_H

typedef enum{
  Menu0             = 0x00,//菜单标志位=0
  Menu1             = 0x01,
  Menu2             = 0x02,
  Menu3             = 0x03,
  Menu4             = 0x04,
  Menu5             = 0x05,
  Menu6             = 0x06,
  Menu7             = 0x07,
  Menu8             = 0x08,
  Menu_Locking_NoOperation    =0x09,
  Menu_unLocking              =0x0A,
  Menu_RefreshAllIco          =0x0B,
  Menu_UnlockStep1_Ok         = 0x0F,
}MenuDisplayType;

typedef enum{
  GpsInfoMenu                           =0x00,
  BacklightTimeSet                      =0x01,
  KeylockTimeSet                        =0x02,
  NativeInfoMenu                        =0x03,
  GroupSwitch                           =0x04,
  BeiDouOrWritingFrequencySwitch        =0x05,
}SubmenuMenuDisplayType;

typedef enum{
  BacklightTimeSet_10s          =0x01,
  BacklightTimeSet_20s          =0x02,
  BacklightTimeSet_30s          =0x03,
  BacklightTimeSet_40s          =0x04,
  BacklightTimeSet_50s          =0x05,
  BacklightTimeSet_60s          =0x06,
  BacklightTimeSet_0s           =0x07,
  KeylockTimeSet_0s             =0x10,
  KeylockTimeSet_30s            =0x11,
  KeylockTimeSet_60s            =0x12,
  KeylockTimeSet_90s            =0x13,
  KeylockTimeSet_120s           =0x14,
  KeylockTimeSet_150s           =0x15,
  KeylockTimeSet_180s           =0x16,
  network_mode_2g               =0x17,
  network_mode_3g               =0x18
}Level3MenuDisplayType;


extern u8 network_mode_2gor3g_SetCount;
extern u8 ApiMenu_SwitchGroup_Flag;
extern u8 ApiMenu_SwitchCallUser_Flag;
extern u8 ApiMenu_SwitchOnlineUser_Flag;
extern u8 ApiMenu_GpsInfo_Flag;
extern u8 ApiMenu_NativeInfo_Flag;
extern u8 ApiMenu_BacklightTimeSet_Flag;
extern u8 ApiMenu_KeylockTimeSet_Flag;
extern u8 ApiMenu_BeiDouOrWritingFrequency_Flag;

extern void MenuDisplay(MenuDisplayType id);
extern void SubmenuMenuDisplay(SubmenuMenuDisplayType id);
extern void Level3MenuDisplay(Level3MenuDisplayType id);
#endif