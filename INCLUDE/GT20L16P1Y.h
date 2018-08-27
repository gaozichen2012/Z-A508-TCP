#ifndef _GT20L16P1Y_H_
#define _GT20L16P1Y_H_

//extern unsigned char r_dat_bat(unsigned long address,unsigned long byte_long,unsigned char *p_arr);

unsigned long WW_GetData(unsigned int UNICODE,unsigned char *DZ_Data);
unsigned long ALB_16_GetData(unsigned int UNICODE_alb,unsigned char *DZ_Data);
void GB2312_16_GetData (unsigned char MSB,unsigned char LSB,unsigned char *DZ_Data);
void UNICODE_16_GetData(unsigned int UNICODE,unsigned char *DZ_Data);

#endif

