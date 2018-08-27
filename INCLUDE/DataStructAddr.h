#ifndef __DATASTRUCT_ADDRESS_H
#define __DATASTRUCT_ADDRESS_H

#include "AllHead.h"
                               	
#define GROUP_MAX_NUM      32   
#define USER_MAX_NUM       64  

#define  nLenErpromChk    16

/*起始地址*/
#define  nAddrTemp    0x4000 /* start address */
#define  nLenTemp     16

#define LCD_DISP_LEN_MAX 16 //16
#define CH_ALIAS_LEN_MAX 10

/* PC */
#define nLenEepromID	16
#define nLenIDSeriesNo	16
#define nLenDealerID	16
#define nLenMaxUserSum  1
#define nLenMaxGrpSum   1
#define nLenSysMode	1
#define nLenRadioInfo	176
#define nLenPCRes1	100

#define nGrpEepromID	1
#define nGrpIDSeriesNo	1
#define nGrpDealerID	1
#define nGrpMaxUserSum  1
#define nGrpMaxGrpSum   1
#define nGrpSysMode	1
#define nGrpRadioInfo	1
#define nGrpPCRes1	1

#define nAddrEepromID	 nAddrTemp + nLenTemp 
#define nAddrIDSeriesNo	 nAddrEepromID + nLenEepromID 
#define nAddrDealerID	 nAddrIDSeriesNo + nLenIDSeriesNo
#define nAddrMaxUserSum	 nAddrDealerID + nLenDealerID 
#define nAddrMaxGrpSum	 nAddrMaxUserSum + nLenMaxUserSum
#define nAddrSysMode	 nAddrMaxGrpSum + nLenMaxGrpSum
#define nAddrRadioInfo	 nAddrSysMode + nLenSysMode 
#define nAddrPCRes1      nAddrRadioInfo + nLenRadioInfo
/*以上定义地址 0x0000-0x0156*/

#define nLenUserID       4
#define nLenRadioMode    12
#define nLenEepromVer	 4
#define nLenSoftPowerOn  1
#define nLenBatteryCfg	 13
#define nLenOtherRes1    23

#define nGrpUserID       1
#define nGrpRadioMode    1
#define nGrpEepromVer	 1
#define nGrpSoftPowerOn  1
#define nGrpBatteryCfg   1
#define nGrpOtherRes1    1

#define nAddrUserID      nAddrPCRes1 + nGrpPCRes1*nLenPCRes1
#define nAddrRadioMode   nAddrUserID + nGrpUserID*nLenUserID
#define nAddrEepromVer	 nAddrRadioMode + nGrpRadioMode*nLenRadioMode
#define nAddrSoftPowerOn nAddrEepromVer + nGrpEepromVer*nLenEepromVer
#define nAddrBatteryCfg  nAddrSoftPowerOn + nGrpSoftPowerOn*nLenSoftPowerOn
#define nAddrOtherRes1   nAddrBatteryCfg + nGrpBatteryCfg*nLenBatteryCfg
/* 以上为0x0157-0x018F */

#define nLenBatteryDbg  		1
#define nLenOilLoDbg			2
#define nLenOilHiDbg			2
#define nLenDbgRes1			5

#define nGrpBatteryDbg		        1
#define nGrpOilLoDbg			1
#define nGrpOilHiDbg			1
#define nGrpDbgRes1			1

#define nAddrBatteryDbg 		nAddrOtherRes1 + nGrpOtherRes1*nLenOtherRes1 
#define nAddrOilLoDbg			nAddrBatteryDbg+ nGrpBatteryDbg*nLenBatteryDbg
#define nAddrOilHiDbg			nAddrOilLoDbg+ nGrpOilLoDbg*nLenOilLoDbg
#define nAddrDbgRes1			nAddrOilHiDbg+ nGrpOilHiDbg*nLenOilHiDbg
/* 以上为0x0190-0x0199 */


#define nLenCardUserPassword	50
#define nLenLocalUserInfo	100//0x022f
#define nLenWorkMode		5//0x0234
#define nLenWarningTone		5//0x4235 (5)提示音音量【秒】
#define nLenSpeech		5//0x0244
#define nLenDisplay		5//
#define nLenPowerManage		9//0x4244 (9)低电提示间隔【秒】
#define nLenTOT			5
#define nLenKeyProg		14//0x25f
#define nLenGpsFun		109//部标0x260-0x2cc

/* data group */
#define nGrpLocalUserInfo	1
#define nGrpCardUserPassword	1
#define nGrpWorkMode		1
#define nGrpWarningTone		1
#define nGrpSpeech		1
#define nGrpDisplay		1
#define nGrpPowerManage		1
#define nGrpTOT			1
#define nGrpKeyProg		1
#define nGrpGpsFun		1

/* data address */
#define nAddrCardUserPassword	nAddrDbgRes1 + nGrpDbgRes1*nLenDbgRes1 //(0x0199+0x05)=0x019E
#define nAddrLocalUserInfo	nAddrCardUserPassword + nGrpCardUserPassword*nLenCardUserPassword//(0x019E+50)=0x01D0
#define nAddrWorkMode		nAddrLocalUserInfo + nGrpLocalUserInfo*nLenLocalUserInfo//(0x01D1+100)=0x234
#define nAddrWarningTone	nAddrWorkMode + nGrpWorkMode*nLenWorkMode//(0x234+5)=0x239
#define nAddrSpeech		nAddrWarningTone + nGrpWarningTone*nLenWarningTone//(0x239+5)=0x23E
#define nAddrDisplay		nAddrSpeech + nGrpSpeech*nLenSpeech//(0x23E+5)=0x243
#define nAddrPowerManage	nAddrDisplay + nGrpDisplay*nLenDisplay//(0x243+5)=0x248
#define nAddrTOT           	nAddrPowerManage + nGrpPowerManage*nLenPowerManage//(0x248+9)=0x251
#define nAddrKeyProg       	nAddrTOT + nGrpTOT*nLenTOT//(0x251+5)=0x256
#define nAddrGpsFun		nAddrKeyProg + nGrpKeyProg*nLenKeyProg//(0x256+14)=0x264
/* 0x019A-0x2cc */

#define nAddrAudioParam         0x0A62

#define nAddrGpsInfoHead        0x1DF0
#define nLenGpsInfoHead		16
#define nGrpGpsInfoHead		1
#define nAddrGpsInfo		0x1E00
#define nLenGpsInfo		32
#define nGrpGpsInfo		16
/* --------------------------------------------------------------------
  Variable To External(globle/external variables defined in this file, can be used externally)
  Example: extern u8 XxxXxx;
---------------------------------------------------------------------- */
#define nLenGpsRam				1500
#define nAddrGpsRam			0x3000
#define nAddrSdRam				nAddrGpsRam + nLenGpsRam


#endif

