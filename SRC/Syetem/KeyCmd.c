#include "AllHead.h"

typedef struct {							//mcu version information data struct
	u8 cMode[12];
	u8 cVersion[4];
	u8 cDate[4];
	u8 cEepVersion[4];						//eeprom version
	u8 cRes[8];
}MCU_VERSION;

static const MCU_VERSION Mcu_Version = {	//mcu vwersion information
	{"A508"},				//product version
	{0x20,0x10,0x10,0x04},
	{0x20,0x18,0x08,0x27},
	{"res"},				//4//4byte eeprom version
	{"res"}
};
//修改版本时必须也修改此处版本
void MCU_VERSIONForMenu(void)
{
  api_lcd_pwr_on_hint3("软件版本:TCP02  ");//清屏
   api_lcd_pwr_on_hint("V2.01.01.004    ");//清屏
}
static u8 KCMD_GetMcuVersion(u8* pBuf);
u8 KCMD_KeyCmdGet(u8 cId,u8 *pBuf)
{
  u8 i;
  i = 0;
  switch (cId & 0x7F)
  {
  case KCMD_IDNULL://undefine id code 
    break;
  case KCMD_IDKEY://get mcu version information//0x81&7F=0x01
    i =  KCMD_GetMcuVersion(pBuf);
    break;
  default:
    break;
  }
  return i;
}
static u8 KCMD_GetMcuVersion(u8 *pBuf)
{
  u8  i;
  u8 * p;
  //ADRLEN_INF adr;
  MCU_VERSION stMcuVersion;
  p = (u8 *)(&Mcu_Version);
  for(i = 0 ; i < sizeof(MCU_VERSION); i++) //get mcu version and date
  {
    *((u8*)(&stMcuVersion) + i) = p[i];
  }
  //adr  = CFG_GetCurAdr(ADR_IDRadioMode);
  //FILE_Read(adr.Adr, adr.Len, stMcuVersion.cMode);  //get product name
  //adr  = CFG_GetCurAdr(ADR_IDEEPROMVER);
  //FILE_Read(adr.Adr, adr.Len, stMcuVersion.cEepVersion);  //get product name
  for(i = 0 ; i < sizeof(MCU_VERSION); i++) //get mcu version and date
  {
    pBuf[i] = *((u8*)(&stMcuVersion) + i);
  }
  return sizeof(MCU_VERSION);
}