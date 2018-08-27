#ifndef __APIBEIDOU_H
#define __APIBEIDOU_H
#define COML_FUNENABLE

//#define BEIDOU//默认内置GPS，注释去掉为外置北斗，测试正常工作
#define BEIDOU_FUNENABLE

#ifdef	BEIDOU_FUNENABLE
  #define BEIDOUAPI extern
#else
  #define BEIDOUAPI 
#endif

#if 1
BEIDOUAPI u8  BDValid;
BEIDOUAPI u32 BDLongitude_Degree;//度
BEIDOUAPI u32 BDLongitude_Minute;//分
BEIDOUAPI u32 BDLongitude_Second;//小数点后的数
BEIDOUAPI u32 BDLatitude_Degree;//度
BEIDOUAPI u32 BDLatitude_Minute;//分
BEIDOUAPI u32 BDLatitude_Second;//小数点后的数
BEIDOUAPI u16 BDSpeed;//速度
BEIDOUAPI u16 BDDirection;//方向
#endif

BEIDOUAPI u8 BeidouRxData[75];
BEIDOUAPI u8 BeidouRxDataLen;
BEIDOUAPI u8 RxStartFlag[6];
BEIDOUAPI void ApiBeidou_Get_location_Information(void);
#endif