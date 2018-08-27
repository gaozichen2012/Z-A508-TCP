#ifndef __FLASH_EEPROM_H
#define __FLASH_EEPROM_H

#include "stm8s.h"
#include "stm8s_flash.h"

extern void WriteMultiBlockByte(uint8_t block_count,FLASH_MemType_TypeDef FLASH_MemType, 
                FLASH_ProgramMode_TypeDef FLASH_ProgMode, uint8_t *Buffer,uint8_t BlockNum);
extern void Flash_WriteDataBlock(uint8_t block_count, uint8_t *Buffer);
extern void ReadMultiBlockByte(uint8_t BlockNum,uint8_t ReadBlockByte[]);
extern void WriteEEPROMByte(uint32_t Address, uint8_t Data);
extern bool FILE_Write(u16 iAdr,u16 iLen,u8 *pBuf);
extern bool FILE_Write2(u16 iAdr,u16 iLen,u8 *pBuf);//如不确定修改是否会对账号读写有影响，直接新建一个函数
extern bool FILE_Read(u16 iAdr,u16 iLen,u8 *pBuf);//如果pBuf是字符串,则最后一位为\0
extern bool FILE_Read2(u16 iAdr,u16 iLen,u8 *pBuf);//如不确定修改是否会对账号读写有影响，直接新建一个函数


extern bool file_flash_write(u16 iAdr,u16 iLen,u8 *pBuf);//如果pBuf是字符串,则最后一位为\0
extern bool file_flash_read(u16 iAdr,u16 iLen,u8 *pBuf);//如果pBuf是字符串,则最后一位为\0
extern void WriteFlashByte(uint32_t Address, uint8_t Data);
#endif
