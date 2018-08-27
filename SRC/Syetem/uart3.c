#include "AllHead.h"
#include "uart3.h"
#include <string.h>

#define UART_IDLE	0x00		//define UART constant value flag
#define UART_RUN	0x01
#define UART_RXBUFLEN	16//16			//define UART Rx buffer length value
#define UART_TXBUFLEN	32			//define UART Tx buffer length value
#define PASSWORD_ADR	CFG_IDPASS	//define password address value
#define PASSWORD_LEN	0x0010		//define password length 
#define SERIAL_NO_ADR	CFG_IDENTIFY//define serial no address value
#define SERIAL_NO_LEN	0x0010		//define serial no length
#define PASMAX_ERR		0x03		//define password error value
#define UART_RXMAXDEL	0x2000		//define UART Rx MAX softwave delay length
#define UART_RXDELAY	0x0800		//define UART Rx delay softwave delay length	
#define UART_TXMAXDEL	0x0800		//define UART Tx MAX softwave delay length value
#define UART_TXBUSY	0			//define UART Tx busy delay length value

typedef enum {						//password string code
	PAS_NULL   = 0x00,				//MCU passwrod null code	
	PAS_ENCODE = 0x01,				//MCU password encode code
	PAS_DECODE = 0x02,				//MCU password decode code
	PAS_MAX	   = 0x03				//MCU password max error code
}PAS_ERROR;

typedef struct {							//define UART drive data type 
	u8 bRunPos 	: 1;
	u8 bProtect	: 1;
        u8 bBDRunPos 	: 1;
	u8 		: 5;
	u8 cData;
	struct {
		u8 cTxBuf[UART_TXBUFLEN+18];
		u8 cRxBuf[UART_RXBUFLEN];
	}TxRxBuf;
}UART_DRV;

typedef struct {							//define password drive data type
	bool bPasOk;
	PAS_ERROR cErrCode;
	u8 cErrNum;
	u8 ComanType;
}PAS_TYPE;
bool WritingFrequency=FALSE;
bool WriteFreq_Flag=FALSE;
static UART_DRV	UartDrvObj;	//define UART drive data
static PAS_TYPE PasObj;	//define password data

static const u8 KEY_TABLE[]   = {'k','e','y'};		//key
static const u8 DBG_TABLE[]   = {'d','b','g'};		//dbg
static const u8 ADS_TABLE[]   = {'a','d','s'};		//ads
static const u8 READ_TABLE[]	 = {'r','e','a','d'};	//read
static const u8 WRITE_TABLE[] = {'w','r','i','t'};	//writ
static const u8 POC_TABLE[]   = {'p','o','c'};		//ads

static const u8 PAS_NULLTBL[PASSWORD_LEN] = {	//null password table
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

static bool UART_Link(u8 cId);
static bool UART_PasCommand(void);
static bool UART_KeyCommand(void);//判断开始链接的密码
static bool UART_WriteCommand(void);//PC写入MCU识别
static bool UART_ReadCommand(void);
static bool UART_PocCommand(void);
static bool api_poc_command_set(u8 cId, u8 *pBuf);
static u16   api_poc_command_get(u8 cId, u8 *pBuf);
static void UART_TxSend(u16 len);
static void UART_RightAck(u16 len);
static void UART_ErrorAck(UART_ERROR err);
static bool UART_GetCheckSum(void);
static bool UART_RxFrame(u16 len);
static void UART_Delay(void);

void Uart3_Init(void)
{
    UART3_DeInit();
	
	/*
	 * 将UART1配置为：
	 * 波特率 = 9600
	 * 数据位 = 8
	 * 1位停止位
	 * 无校验位
	 * 使能接收和发送
	 * 使能接收中断
	 */
	
    UART3_Init((u32)(345600/12), UART3_WORDLENGTH_8D, UART3_STOPBITS_1, \
    UART3_PARITY_NO , UART3_MODE_TXRX_ENABLE);
    UART3_ITConfig(UART3_IT_RXNE_OR,ENABLE  );
    UART3_Cmd(ENABLE);
    UartDrvObj.bRunPos	= UART_IDLE;
    PasObj.bPasOk 	    = FALSE;
    PasObj.cErrNum      = 0x00;
    PasObj.cErrCode     = PAS_NULL;
}

void UART3_Interrupt(void)
{
  u8 Res;


  if(UART3_GetITStatus(UART3_IT_RXNE )!= RESET)
  {
    Res = UART3_ReceiveData8();
    UartDrvObj.cData= (u8)Res;
    UartDrvObj.bRunPos   = UART_RUN;
#if 1//beidou
    switch(UartDrvObj.cData)
    {
          case '$':
            RxStartFlag[0]=1;
            break;
          case 'G':
            RxStartFlag[1]=1;
            break;
          case 'N':
            RxStartFlag[2]=1;
            break;
          case 'R':
            RxStartFlag[3]=1;
            break;
          case 'M':
            RxStartFlag[4]=1;
            break;
          case 'C':
            RxStartFlag[5]=1;
            break;
          default:
            break;
    }
        if(RxStartFlag[0]==1&&RxStartFlag[1]==1&&RxStartFlag[2]==1&&
           RxStartFlag[3]==1&&RxStartFlag[4]==1&&RxStartFlag[5]==1)
        {
          if(UartDrvObj.cData=='$')
          {
            BeidouRxDataLen=0;
            RxStartFlag[0]=0;
            RxStartFlag[1]=0;
            RxStartFlag[2]=0;
            RxStartFlag[3]=0;
            RxStartFlag[4]=0;
            RxStartFlag[5]=0;
            return;
          }
          BeidouRxData[BeidouRxDataLen]=UartDrvObj.cData;
          BeidouRxDataLen++;
        }
                
#endif
  }
}

void UART3_ToMcuMain(void)
{
	u8 cId;
	if (UartDrvObj.bRunPos == UART_RUN) 			//UART meassgae process
	{
		UartDrvObj.bRunPos = UART_IDLE;
		cId = UartDrvObj.cData;//UartDrvObj.cData为串口接收到的数据，数据传递到cId
		if ((cId == 0x5A) || (cId == 0xA5) || (cId == 0xAA))		//UART cId process process //判断接收到的数据是否是：5A、A5、AA
		{
                  WriteFreq_Flag=TRUE;
			if (UART_Link(cId) == TRUE)								//UART link process
			{			
				if (UART_KeyCommand() == FALSE)						//key command process
				{		
					//if (UART_AdsCommand() == FALSE)					//adj command process
					{
						//if (UART_DbgCommand() == FALSE)				//dad command process	
						{
							if (UART_ReadCommand() == FALSE)		//read command process
							{
								if (UART_WriteCommand() == FALSE)	//write command process	
								{
									if(UART_PocCommand() == FALSE)
									{
										UART_ErrorAck(UART_FAILI);
									}
								}
                                                                else
                                                                {
                                                                  Set_GreenLed(LED_ON);
                                                                  Set_RedLed(LED_OFF);
                                                                  
                                                                }
							}
                                                        else
                                                        {
                                                          Set_GreenLed(LED_OFF);
                                                          Set_RedLed(LED_ON);
                                                        }
						}
					}
				}
			}
		}
		UartDrvObj.bRunPos = UART_IDLE;
	}
	return;
}
/*******************************************************************************
 * 名称: UART3_SendByte
 * 功能: UART3发送一个字节
 * 形参: data -> 要发送的字节
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void UART3_SendByte(u8 data)
{
	UART3_SendData8((unsigned char)data);
	
	/* Loop until the end of transmission */  
	while (UART3_GetFlagStatus(UART3_FLAG_TXE) == RESET);
}

/*******************************************************************************
 * 名称: UART3_SendString
 * 功能: UART3发送字节串
 * 形参: data -> 要发送的字节
 * 		 len -> 字节长度
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void UART3_SendString(u8* Data,u16 len)
{
	u16 i=0;
	
	for(;i<len;i++)
		UART3_SendByte(Data[i]);
}

/*******************************************************************************
 * 名称: UART3_ReceiveByte
 * 功能: UART3接收字节串
 * 形参: 无
 * 返回: 接收到的字符串
 * 说明: 无 
 ******************************************************************************/
u8 UART3_ReceiveByte(void)
{
	u8 USART3_RX_BUF; 
	
	while (UART3_GetFlagStatus(UART3_FLAG_RXNE) == RESET);
	
	USART3_RX_BUF=UART3_ReceiveData8();
	return  USART3_RX_BUF;
}



static bool UART_Link(u8 cId)
{
	bool r = FALSE;
	
	if (cId == 0x5A)									//0x5A null password 
	{
          
		UartDrvObj.TxRxBuf.cTxBuf[0] = UART_START;
		UART_TxSend(1);
	}
	else												//0xA5, and 0xAA passwrod process
	{
		if (PASMAX_ERR < PasObj.cErrNum)				//check password error number
		{
			UartDrvObj.TxRxBuf.cTxBuf[0] = UART_START;
			UartDrvObj.TxRxBuf.cTxBuf[1] = PAS_MAX;
			UART_TxSend(2);
			UART_TxSend(16);
			return FALSE;
		}
		PasObj.ComanType = cId;
		if (cId == 0xAA)								//0xAA clr password process
		{
			PasObj.bPasOk = FALSE;
		}
		if (UART_PasCommand() == FALSE)	//password link process
		{
			return FALSE;
		}
	}
	if(r != PasObj.bPasOk)
	{
		//r = PasObj.bPasOk;
		//return FALSE;
	}
	if(PasObj.ComanType == 0xAA)
	{
		return FALSE;
	}
	if (UART_RxFrame(9) == FALSE)						//rx next command process
	{
		return FALSE;
	}
	return UART_GetCheckSum();							//next data check
}

static bool UART_PasCommand(void)
{
	u8 buf[16];
	u8 i;
	bool r;

	r = FALSE;
	//if (FILE_Read(PASSWORD_ADR,PASSWORD_LEN,buf) == TRUE) 			//read paasword
	{
		PasObj.cErrCode = PAS_NULL;
		for(i = 0; i < PASSWORD_LEN; i++)							//password process
		{
			if (PAS_NULLTBL[i] != buf[i])
			{
				if (PasObj.bPasOk == TRUE)							//null password process OK
				{
					PasObj.cErrCode = PAS_ENCODE;
				}
				else
				{
					PasObj.cErrCode = PAS_DECODE;
				}
				break;
			}
		}
#if 0//暂时不从EEPROM中读取UartDrvObj.TxRxBuf.cTxBuf[2]值，直接赋值即可
		if (FILE_Read(SERIAL_NO_ADR,SERIAL_NO_LEN,&UartDrvObj.TxRxBuf.cTxBuf[2]) == TRUE)	//read serial no 

#else
               UartDrvObj.TxRxBuf.cTxBuf[2] =0x31;
               UartDrvObj.TxRxBuf.cTxBuf[3] =0x32;
               UartDrvObj.TxRxBuf.cTxBuf[4] =0x33;
               UartDrvObj.TxRxBuf.cTxBuf[5] =0x34;
               UartDrvObj.TxRxBuf.cTxBuf[6] =0x35;
               UartDrvObj.TxRxBuf.cTxBuf[7] =0x36;
               UartDrvObj.TxRxBuf.cTxBuf[8] =0x37;
               UartDrvObj.TxRxBuf.cTxBuf[9] =0x38;
               UartDrvObj.TxRxBuf.cTxBuf[10]=0xFF;
               UartDrvObj.TxRxBuf.cTxBuf[11]=0xFF;
               UartDrvObj.TxRxBuf.cTxBuf[12]=0xFF;
               UartDrvObj.TxRxBuf.cTxBuf[13]=0xFF;
               UartDrvObj.TxRxBuf.cTxBuf[14]=0xFF;
               UartDrvObj.TxRxBuf.cTxBuf[15]=0xFF;
               UartDrvObj.TxRxBuf.cTxBuf[16]=0xFF;
               UartDrvObj.TxRxBuf.cTxBuf[17]=0xFF;
#endif
		{
			UartDrvObj.TxRxBuf.cTxBuf[0] = UART_START;
#if 0
                        UartDrvObj.TxRxBuf.cTxBuf[1] = (u8)PasObj.cErrCode;//PasObj.cErrCode=PAS_DECODE，暂时屏蔽
#else
                        PasObj.cErrCode = PAS_NULL;
                        UartDrvObj.TxRxBuf.cTxBuf[1] = (u8)PasObj.cErrCode;

#endif
			for(i = 0; i < SERIAL_NO_LEN; i++)						//password not process
			{
				UartDrvObj.TxRxBuf.cTxBuf[2+i] = ~UartDrvObj.TxRxBuf.cTxBuf[2+i];//~取反//CE CD CC CB CA C9 C8 C7 00取反：31 32 33 34 35 36 37 38 FF FF
			}			
			UART_TxSend(2 + SERIAL_NO_LEN);
			if ((PasObj.cErrCode == PAS_NULL) || (PasObj.cErrCode == PAS_ENCODE) || PasObj.bPasOk == TRUE)
			{
				r = TRUE;
			}
			else
			{
				if(PasObj.ComanType == 0xAA)
				{
					return FALSE;
				}
				if(PasObj.bPasOk == TRUE)
				{
					return TRUE;
				}
				if (UART_RxFrame(0x09) == TRUE)						//rx password 
				{
					if (UART_GetCheckSum() == TRUE)					//check data
					{
						for(i = 0; i < 8; i++)						//save passwrod 8 byte
						{
							UartDrvObj.TxRxBuf.cTxBuf[i] = UartDrvObj.TxRxBuf.cRxBuf[i];
						}
						UART_RightAck(0x08);						//ack data 
						if (UART_RxFrame(0x09) == TRUE)
						{
							if (UART_GetCheckSum() == TRUE)			//check data process
							{
								for(i = 0; i < 8; i++)				//save passwrod 8 byte
								{
									UartDrvObj.TxRxBuf.cTxBuf[i+8] = UartDrvObj.TxRxBuf.cRxBuf[i];
								}	
								for(i = 0; i < 16; i++)				//password comp
								{
									if (buf[i] != UartDrvObj.TxRxBuf.cTxBuf[i]) 
									{
										PasObj.cErrNum++;
										UART_ErrorAck(UART_PASSWORD);
										return FALSE;
									}
								}
								PasObj.cErrNum = 0;
								PasObj.bPasOk  = TRUE;
								UART_RightAck(0x00);
								r = FALSE;
							}
						}
					}
				}
			}
		}
	}
	return r;
}

static bool UART_KeyCommand(void)//判断开始链接的密码
{
	u8 i;

	for(i = 0; i < 3; i++)					//key command scan process
	{
		if (KEY_TABLE[i] != UartDrvObj.TxRxBuf.cRxBuf[i])
		{
			return FALSE;
		}
	}
	if ((UartDrvObj.TxRxBuf.cRxBuf[3] & 0x80) == 0x00) //UartDrvObj.TxRxBuf.cRxBuf[3]=0x81
	{
		//LED_SetOutput(LED_IDWRITE,ON,FALSE);		//write LED ON process		
		//if (KCMD_KeyCmdSet(UartDrvObj.TxRxBuf.cRxBuf[3],&UartDrvObj.TxRxBuf.cRxBuf[4]) == FALSE)
		{
			UART_ErrorAck(UART_KEYERR);
		}
		//else
		{
		//	UART_RightAck(UartDrvObj.TxRxBuf.cRxBuf[3]);
		}
	}
	else
	{
		//LED_SetOutput(LED_IDREAD,ON,FALSE);			//read LED on process
		for(i = 0; i < 4; i++) 
		{
			UartDrvObj.TxRxBuf.cTxBuf[i] = UartDrvObj.TxRxBuf.cRxBuf[4+i];//UartDrvObj.TxRxBuf.cTxBuf存入的数应该为：41 2D 35 31//6B 65 79 81 41 2D 35 31 9E
		}
#if 1//暂时屏蔽从EEPROM中获取软件版本，直接赋值
		i = KCMD_KeyCmdGet(UartDrvObj.TxRxBuf.cRxBuf[3],UartDrvObj.TxRxBuf.cTxBuf);//UartDrvObj.TxRxBuf.cRxBuf[3]=0x81

#else
                UartDrvObj.TxRxBuf.cTxBuf[0] ='A';
                UartDrvObj.TxRxBuf.cTxBuf[1] ='-';
                UartDrvObj.TxRxBuf.cTxBuf[2] ='5';
                UartDrvObj.TxRxBuf.cTxBuf[3] ='1';
                UartDrvObj.TxRxBuf.cTxBuf[4] ='0';
                UartDrvObj.TxRxBuf.cTxBuf[5] ='P';
                UartDrvObj.TxRxBuf.cTxBuf[6] ='+';
                UartDrvObj.TxRxBuf.cTxBuf[7] =0x00;
                UartDrvObj.TxRxBuf.cTxBuf[8] =0x00;
                UartDrvObj.TxRxBuf.cTxBuf[9] =0x00;
                UartDrvObj.TxRxBuf.cTxBuf[10]=0x00;
                UartDrvObj.TxRxBuf.cTxBuf[11]=0x00;
                UartDrvObj.TxRxBuf.cTxBuf[12]=0x30;
                UartDrvObj.TxRxBuf.cTxBuf[13]=0x30;
                UartDrvObj.TxRxBuf.cTxBuf[14]=0x10;
                UartDrvObj.TxRxBuf.cTxBuf[15]=0x01;
                UartDrvObj.TxRxBuf.cTxBuf[16]=0x20;
                UartDrvObj.TxRxBuf.cTxBuf[17]=0x15;
                UartDrvObj.TxRxBuf.cTxBuf[18]=0x07;
                UartDrvObj.TxRxBuf.cTxBuf[19]=0x07;
                UartDrvObj.TxRxBuf.cTxBuf[20]='r';
                UartDrvObj.TxRxBuf.cTxBuf[21]='e';
                UartDrvObj.TxRxBuf.cTxBuf[22]='s';
                UartDrvObj.TxRxBuf.cTxBuf[23]=0x00; 
                UartDrvObj.TxRxBuf.cTxBuf[24]='r';
                UartDrvObj.TxRxBuf.cTxBuf[25]='e';
                UartDrvObj.TxRxBuf.cTxBuf[26]='s';
                UartDrvObj.TxRxBuf.cTxBuf[27]=0x00;
                UartDrvObj.TxRxBuf.cTxBuf[28]=0x00;
                UartDrvObj.TxRxBuf.cTxBuf[39]=0x00;
                UartDrvObj.TxRxBuf.cTxBuf[30]=0x00;
                UartDrvObj.TxRxBuf.cTxBuf[31]=0x00;
                i=32;
#endif
		if (i == 0x00)
		{
			UART_ErrorAck(UART_KEYERR);
		}
		else
		{
			UART_RightAck(i);//MCU发送KEY后回复：06 00 20 00
			if (UART_RxFrame(1) == TRUE)	//UART synchronization process
			{
				UART_TxSend(i);//发送MCU版本信息
			}
		}
	}
	//LED_SetOutput(LED_IDREAD,OFF,FALSE);			//read LED on process
	//LED_SetOutput(LED_IDWRITE,OFF,FALSE);			//write LED ON process
	return TRUE;
}

static bool UART_WriteCommand(void)
{
	u16 i;
	u16 adr,len;

	for(i = 0;i < 4;i++)							//write command scan process
	{
          if(WRITE_TABLE[i] != UartDrvObj.TxRxBuf.cRxBuf[i]) 
          {
            return FALSE;
          }
        }

	adr = UartDrvObj.TxRxBuf.cRxBuf[4];//0x02//cRxBuf[4]为地址高8位
	adr <<= 8;
	adr |= UartDrvObj.TxRxBuf.cRxBuf[5]; //0x30//35//3A//3F//44//4D//52//60//cRxBuf[5]为地址低8位
	len = UartDrvObj.TxRxBuf.cRxBuf[6];//00//cRxBuf[6]为长度高8位
	len <<= 8;
	len |= UartDrvObj.TxRxBuf.cRxBuf[7];//0x05//05//05//05//09//05//0E//6D//cRxBuf[7]为长度低8位
	UART_RightAck(len);//收到数据MCU向串口发送回复的指令
	while((len/8) != 0)//>8 or /8 process
	{
		if (UART_RxFrame(9) == FALSE)	//recv one frame data
		{
			goto UARTWriteCommand_Exit;
		}
		if (UART_GetCheckSum() == FALSE)//data check process
		{
			goto UARTWriteCommand_Exit;
		}
		if (FILE_Write2(adr,0x08,UartDrvObj.TxRxBuf.cRxBuf) == FALSE)
		{
			UART_ErrorAck(UART_READWRITE);
			goto UARTWriteCommand_Exit;
		}
		adr += 8;
		len -= 8;
		UART_RightAck(len);//right ack process
	}
	len %= 8;//<8 process
	if (len != 0)
	{
		if (UART_RxFrame(len) == FALSE)
		{
			goto UARTWriteCommand_Exit;
		}
		if (FILE_Write2(adr,len,UartDrvObj.TxRxBuf.cRxBuf) == FALSE) 
		{
			UART_ErrorAck(UART_READWRITE);
			goto UARTWriteCommand_Exit;
		}
		UART_RightAck(0x00);
	}

UARTWriteCommand_Exit:
  return TRUE;
}

/******************************************************************************
; Function Description	: UART read command process program
; Enter Parameter	: void
; Return Parameter	: TRUE 	work right return value
;			FALSE	work error return value
******************************************************************************/
static bool UART_ReadCommand(void)
{
  u16 adr,len;
  for(len = 0; len < 4; len++) 	//read command process
  {
    if(READ_TABLE[len] != UartDrvObj.TxRxBuf.cRxBuf[len]) //是否收到PC发送的READ指令
    {
      return FALSE;
    }
  }
  adr = UartDrvObj.TxRxBuf.cRxBuf[4];
  adr <<= 8;
  adr |= UartDrvObj.TxRxBuf.cRxBuf[5];
  len = UartDrvObj.TxRxBuf.cRxBuf[6];
  len <<= 8;
  len |= UartDrvObj.TxRxBuf.cRxBuf[7];
        if(adr==0x260)
        {
          adr=0x260;
        }
  /*if (FILE_InStatus(adr+len) == FALSE)	//file test process
  {
    UART_ErrorAck(UART_READWRITE);
  }
  else*/
  {
    UART_RightAck(len);
    if (UART_RxFrame(1) == TRUE)//UART synchronization process
    {
      while((len/UART_TXBUFLEN) != 0)
      {
        if (FILE_Read(adr,UART_TXBUFLEN,UartDrvObj.TxRxBuf.cTxBuf) == FALSE)
        {
          goto UARTReadCommand_Exit;
        }
        UART_TxSend(UART_TXBUFLEN);
        len -= UART_TXBUFLEN;
        adr += UART_TXBUFLEN;
      }
      len %= UART_TXBUFLEN;
      if (len != 0)
      {
        if (FILE_Read(adr,len,UartDrvObj.TxRxBuf.cTxBuf) == TRUE)
        {
          UART_TxSend(len);
        }
      }
      //Set_GreenLed(LED_OFF);
      //Set_RedLed(LED_ON);
    }
  }

UARTReadCommand_Exit:
  return TRUE;
}




static bool UART_PocCommand(void)
{
	u16  i;
       // len;
	//u8 aBuffer[10];
	//u8 *pBuffer = 0;

	for(i = 0; i < 3; i++)								//dbg command scan process
	{
		if (POC_TABLE[i] != UartDrvObj.TxRxBuf.cRxBuf[i]) 
		{
			return FALSE;
		}
	}
	if ((UartDrvObj.TxRxBuf.cRxBuf[3] & 0x80) == 0x00) //写入对讲机
	{

		if (api_poc_command_set(UartDrvObj.TxRxBuf.cRxBuf[3],&UartDrvObj.TxRxBuf.cRxBuf[4]) == FALSE)//cRxBuf[4]=0x13
		{
			UART_ErrorAck(UART_DAERR);
		}
		else
		{
			
		}     
	}
	else//读取对讲机参数
	{

		for(i = 0 ; i < 4; i++) 
		{
			UartDrvObj.TxRxBuf.cTxBuf[i] = UartDrvObj.TxRxBuf.cRxBuf[4+i];
		}
		
		i = api_poc_command_get(UartDrvObj.TxRxBuf.cRxBuf[3],UartDrvObj.TxRxBuf.cTxBuf);
                
	}
	return TRUE;
}

static void UART_TxSend(u16 len)
{
  u8 i;
  
  if (UART_TXBUFLEN < len) 
  {
    len = UART_TXBUFLEN;
  }
  
  for(i=0;i<len;i++)
  {
    UART3_SendByte(UartDrvObj.TxRxBuf.cTxBuf[i]);
  }
}

bool api_poc_command_set(u8 cId, u8 *pBuf)
{
  u16 len, i=0;
  bool r = FALSE;
  
  len =  pBuf[1];
  len <<= 0x08;
  len |= pBuf[0];
  switch (cId & 0x7F)
  {
  case 0://set cdma use id
    if(len <= 50)
    {
      UART_RightAck(len);
      while((len-i) >= 8)//>8 or /8 process
      {
        if (UART_RxFrame(9) == FALSE)//1组信息9个16进制数，8个有效数//recv one frame data
        {
          goto PocCommandSet_Exit;
        }
        if (UART_GetCheckSum() == FALSE)//data check process
        {
          goto PocCommandSet_Exit;
        }
        memcpy(&UartDrvObj.TxRxBuf.cTxBuf[i], UartDrvObj.TxRxBuf.cRxBuf, 8);
        i += 8;
        UART_RightAck(len);//right ack process
      }
      if (len != i)//不足8位的数
      {
        if (UART_RxFrame(len-i) == FALSE)
        {
          goto PocCommandSet_Exit;
        }
        memcpy(&UartDrvObj.TxRxBuf.cTxBuf[i], UartDrvObj.TxRxBuf.cRxBuf, len-i);
      }
      r = ApiPocCmd_user_info_set(UartDrvObj.TxRxBuf.cTxBuf, len);//cTxBuf为存放ip账号密码的信息//ApiPocCmd_user_info_set为数据存入EEPROM的驱动函数
      r=TRUE;
      UART_RightAck(len-i);
    }
    //SYS_McuReset();
  PocCommandSet_Exit:

    break;
  default://error all D/A off
    break;
    //return FALSE;
  }
  return r;
}

u16 api_poc_command_get(u8 cId, u8 *pBuf)
{
	u16 len=0, i = 0;
	u8 * ptBuf;

	switch (cId & 0x7F)//0x80&0x7F=0x00
	{
		case 0x00://
			len = ApiPocCmd_user_info_get(&ptBuf);
			break;
			
		case 0x01:
			//len = ApiXM39_UART_GetReportPoint(&ptBuf);
			break;
			
		case 0x02://get ESN
			//len = ApiAtCmd_GetIccId(&ptBuf);
			break;	
		case 0x03:
			//len = ApiAtCmd_GetRssi(&ptBuf);
			break;
		default:						//error all D/A off
			break;
	}
	if(len != 0)
	{
		UART_RightAck(len);
		if (UART_RxFrame(1) == TRUE)
		{
			while((len - i) >= UART_TXBUFLEN)
			{
				memcpy(UartDrvObj.TxRxBuf.cTxBuf, ptBuf+i, UART_TXBUFLEN);
				i += UART_TXBUFLEN;
				UART_TxSend(UART_TXBUFLEN);
			}
			if(i != len)
			{
				memcpy(UartDrvObj.TxRxBuf.cTxBuf, ptBuf+i, len-i);
				UART_TxSend(len-i);
			}
		}
		else
		{
			UART_ErrorAck(UART_TIMEOVER);
		}
	}
	else
	{
		UART_ErrorAck(UART_ADERR);
	}
	return len;
}



static void UART_RightAck(u16 len)
{
	u8 i,buf[4];

	for(i = 0; i < 4; i++)							//back UART tx data process
	{
		buf[i] = UartDrvObj.TxRxBuf.cTxBuf[i];
	}
	UartDrvObj.TxRxBuf.cTxBuf[0] = UART_ACK;
	UartDrvObj.TxRxBuf.cTxBuf[1] = (u8)(len >> 8);
	UartDrvObj.TxRxBuf.cTxBuf[2] = (u8)(len & 0x00FF);
	UartDrvObj.TxRxBuf.cTxBuf[3] = 0x00;
	UART_TxSend(4);
	for(i = 0; i < 4; i++)							//renew UART rx data process
	{
		UartDrvObj.TxRxBuf.cTxBuf[i] = buf[i];
	}
	return;
}

static void UART_ErrorAck(UART_ERROR err)
{
	UartDrvObj.TxRxBuf.cTxBuf[0] = err;
	UartDrvObj.TxRxBuf.cTxBuf[1] = 0x00; 
	UartDrvObj.TxRxBuf.cTxBuf[2] = 0x00;
	UartDrvObj.TxRxBuf.cTxBuf[3] = (u8)(~err);
	UART_TxSend(4);	
	return;
}

/******************************************************************************
; Function Description	: UART check sum compare program (XOR)
; Enter Parameter		: void
; Return Parameter		: TRUE 		recever data right code 
;						  FALSE		recever data error code
******************************************************************************/
static bool UART_GetCheckSum(void)
{
	u8 i;

	for(i = 0;i < 8;i++)
	{ 
		UartDrvObj.TxRxBuf.cRxBuf[8] ^= UartDrvObj.TxRxBuf.cRxBuf[i];
	}
	if (UartDrvObj.TxRxBuf.cRxBuf[8] == 0x00)
	{
		return TRUE;
	}
	UART_ErrorAck(UART_RECEIVE);
	return FALSE;
}

static bool UART_RxFrame(u16 len)//关键函数：保存长度为Len的数据
{
	u8 i;
	u16  del;

	if ((len > UART_RXBUFLEN) || (len == 0)) 		//RX length process	
	{
		return FALSE;
	}
	for(i = 0; i < len; i++)
	{
		UartDrvObj.bRunPos = UART_IDLE;
		del = 0;
		while(UartDrvObj.bRunPos == UART_IDLE)
		{
			del++;
			UART_Delay();
			if (del	> UART_RXMAXDEL)
			{
				UART_ErrorAck(UART_TIMEOVER);
				return FALSE;
			}
		}
		UartDrvObj.TxRxBuf.cRxBuf[i] = UartDrvObj.cData;
	}
	UartDrvObj.bRunPos = UART_IDLE;
	return TRUE;
}

/******************************************************************************
; Function Description	: UART softwave delay process
; Enter Parameter		: void
; Return Parameter		: void
******************************************************************************/
static void UART_Delay(void)
{
	u8 i;

	for(i = 0; i < 64; i++) 
	{
		nop();
	}
	return;
}

