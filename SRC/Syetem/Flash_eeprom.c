#include "Flash_eeprom.h"
  u32 AdrSumTest=0x00;

/***官方例程-FLASH_Config**/
void FLASH_Config(void)
{
 /* Define flash programming Time*/
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);

  FLASH_Unlock(FLASH_MEMTYPE_PROG);
  /* Wait until Flash Program area unlocked flag is set*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_PUL) == RESET)
  {}

  /* Unlock flash data eeprom memory */
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  /* Wait until Data EEPROM area unlocked flag is set*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
  {}
}



/*******************************************************************************
          每种型号的EEPROM的大小不一样，调用此函数的时候要注意将要写进的字节数组
         的大小是否超过该型号的EEPROM的地址。
         大容量的EEPROM的型号是STM8S208, STM8S207, STM8S007, STM8AF52Ax, STM8AF62Ax 
         EEPROM的地址是从0x004000到0x0047ff，共2048Byte,每个Block是128Byte,一共16个Block.
         中容量的EEPROM的型号是STM8S105, STM8S005, STM8AF626x
         EEPROM的地址是从0x004000到0x0043ff，共1024Byte,每个Block是128Byte,一共8个Block.
         小容量的EEPROM的型号是STM8S103, STM8S003, STM8S903 
         EEPROM的地址是从0x004000到0x00427f，共1024Byte,每个Block是64Byte,一共10个Block.

 ******************************************************************************/
void WriteMultiBlockByte(uint8_t block_count,FLASH_MemType_TypeDef FLASH_MemType, 
                FLASH_ProgramMode_TypeDef FLASH_ProgMode, uint8_t *Buffer,uint8_t BlockNum)
{
  uint8_t  BlockNum_Temp;
  /* 解锁 flash data eeprom memory */
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  /* 等待 Data EEPROM area 解锁标志位置位*/
  while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
        ;
  for(BlockNum_Temp=block_count;BlockNum_Temp<BlockNum;BlockNum_Temp++)
  {
    if(BlockNum_Temp>FLASH_DATA_BLOCKS_NUMBER)
      break;
    FLASH_ProgramBlock(BlockNum_Temp, FLASH_MemType, FLASH_ProgMode,Buffer+BlockNum_Temp*FLASH_BLOCK_SIZE);
    FLASH_WaitForLastOperation(FLASH_MemType);
  }
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);/*操作完要加锁*/

}

void Flash_WriteDataBlock(uint8_t block_count, uint8_t *Buffer)
{
    FLASH_Unlock(FLASH_MEMTYPE_DATA);//可以擦写EEPROM或Flash:FLASH_Unlock(FLASH_MemType_Program);
    while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
    {}
    FLASH_ProgramBlock(block_count-1, FLASH_MEMTYPE_DATA, FLASH_PROGRAMMODE_STANDARD, Buffer);

    while (FLASH_GetFlagStatus(FLASH_FLAG_HVOFF) == RESET)
    {}
    //FLASH_WaitForLastOperation();
    FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void ReadMultiBlockByte(uint8_t BlockNum,uint8_t ReadBlockByte[])
{
  uint32_t add, start_add, stop_add;
  start_add = FLASH_DATA_START_PHYSICAL_ADDRESS+(u32)((BlockNum-1)*FLASH_BLOCK_SIZE);
  stop_add = FLASH_DATA_START_PHYSICAL_ADDRESS + (u32)(BlockNum*FLASH_BLOCK_SIZE);
 
  for (add = start_add; add < stop_add; add++)
      ReadBlockByte[add-FLASH_DATA_START_PHYSICAL_ADDRESS]=FLASH_ReadByte(add);
 
  
}
void WriteEEPROMByte(uint32_t Address, uint8_t Data)
{
  FLASH_Unlock(FLASH_MEMTYPE_DATA);//可以擦写EEPROM或Flash:FLASH_Unlock(FLASH_MemType_Program);
  while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);//等待解锁成功
  FLASH_ProgramByte(Address,Data);//向指定地址写入数据Data
  while (FLASH_GetFlagStatus(FLASH_FLAG_HVOFF) == RESET);//写入完成
  FLASH_Lock(FLASH_MEMTYPE_DATA);//上锁
}


bool FILE_Write(u16 iAdr,u16 iLen,u8 *pBuf)//如果pBuf是字符串,则最后一位为\0
{
  u16 EEPROM_WriteCount;
  u32 AdrSum=0x00;
  for(EEPROM_WriteCount=0x01;EEPROM_WriteCount<iLen+1;EEPROM_WriteCount++)
  {
    AdrSum=0x4000+iAdr+EEPROM_WriteCount-1;
    WriteEEPROMByte(AdrSum, pBuf[EEPROM_WriteCount]);
  }
  return TRUE;
}
bool FILE_Write2(u16 iAdr,u16 iLen,u8 *pBuf)//如不确定修改是否会对账号读写有影响，直接新建一个函数
{
  u16 EEPROM_WriteCount;
  u32 AdrSum=0x00;
  for(EEPROM_WriteCount=0x01;EEPROM_WriteCount<iLen+1;EEPROM_WriteCount++)
  {
    AdrSum=0x4000+iAdr+EEPROM_WriteCount-1;
    WriteEEPROMByte(AdrSum, pBuf[EEPROM_WriteCount-1]);
  }
  return TRUE;
}
bool FILE_Read(u16 iAdr,u16 iLen,u8 *pBuf)//如果pBuf是字符串,则最后一位为\0
{
  u16 i;
  u32 AdrSum=0x00;
  for(i=0x00;i<iLen;i++)
  {
    AdrSum=0x4000+iAdr+i;
    pBuf[i]=FLASH_ReadByte(AdrSum);
  }
  return TRUE;
}

bool FILE_Read2(u16 iAdr,u16 iLen,u8 *pBuf)//如不确定修改是否会对账号读写有影响，直接新建一个函数
{
  u16 i;
  u32 AdrSum=0x00;
  for(i=0x00;i<iLen;i++)
  {
    AdrSum=iAdr+i;
    pBuf[i]=FLASH_ReadByte(AdrSum);
  }
  return TRUE;
}

#if 1//FLASH读写
bool file_flash_write(u16 iAdr,u16 iLen,u8 *pBuf)//如果pBuf是字符串,则最后一位为\0
{
#if 0
  
  FLASH_ProgramBlock(1, FLASH_MEMTYPE_DATA, FLASH_PROGRAMMODE_STANDARD, GBuffer);
    startadd = FLASH_PROG_START_PHYSICAL_ADDRESS + ((uint16_t)BLOCK_OPERATION * (uint16_t)FLASH_BLOCK_SIZE);
  stopadd = startadd + (uint16_t)FLASH_BLOCK_SIZE;
#else
  u16 EEPROM_WriteCount;
  u32 AdrSum=0x00;
  for(EEPROM_WriteCount=0x00;EEPROM_WriteCount<iLen;EEPROM_WriteCount++)
  {
    AdrSum=0x10000+iAdr+EEPROM_WriteCount;
    WriteFlashByte(AdrSum, pBuf[EEPROM_WriteCount]);
  }
#endif
  return TRUE;
  
}
bool file_flash_read(u16 iAdr,u16 iLen,u8 *pBuf)//如果pBuf是字符串,则最后一位为\0
{
  u16 i;
  u32 AdrSum=0x00;
  for(i=0x00;i<iLen;i++)
  {
    AdrSum=0x10000+iAdr+i;
    pBuf[i]=FLASH_ReadByte(AdrSum);
  }
  return TRUE;
}
void WriteFlashByte(uint32_t Address, uint8_t Data)
{
  
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
  
  /* Unlock Data memory */
  FLASH_Unlock(FLASH_MEMTYPE_PROG);//可以擦写EEPROM或Flash:FLASH_Unlock(FLASH_MemType_Program);
  //while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);//等待解锁成功
  FLASH_ProgramByte(Address,Data);//向指定地址写入数据Data
  
/*

  FLASH_ProgramBlock(BLOCK_OPERATION, FLASH_MEMTYPE_DATA, FLASH_PROGRAMMODE_STANDARD, GBuffer);
  

  while (FLASH_GetFlagStatus(FLASH_FLAG_HVOFF) == RESET)
  {}
  */
}
#endif