#include "AllHead.h"

bool mcu_key_pad_read(u8 index);
void mcu_key_pad_set(bool inout, u8 index, bool up);
void mcu_key_pad_out(u8 index,bool state);

void mcu_key_initial(void)
{
    //GPIO_Init(GPIO_Key_11,GPIO_PIN_Key_11,GPIO_MODE_OUT_PP_HIGH_FAST);
}

bool mcu_key_pad_read(u8 index)
{
  bool value = TRUE;
  u16 PIN;
  switch (index)
  {
  case 0:
    PIN = GPIO_PIN_Key_5;// K5:PC5
    value = (bool)GPIO_ReadInputPin(GPIO_Key_5, PIN);
    break;
  case 1:
    PIN = GPIO_PIN_Key_6;// K6:PC6
    value = (bool)GPIO_ReadInputPin(GPIO_Key_6, PIN);
    break;
  case 2:
    PIN = GPIO_PIN_Key_7;// K7:PC7
    value = (bool)GPIO_ReadInputPin(GPIO_Key_7, PIN);
    break;
  case 3:
    PIN = GPIO_PIN_Key_8;// K8:PG0
    value = (bool)GPIO_ReadInputPin(GPIO_Key_8, PIN);
    break;
  case 4:
    PIN = GPIO_PIN_Key_9;// K9:PG1
    value = (bool)GPIO_ReadInputPin(GPIO_Key_9, PIN);
    break;
  case 5:
    PIN = GPIO_PIN_Key_10;// K10:PG3
    value = (bool)GPIO_ReadInputPin(GPIO_Key_10, PIN);
    break;
  default:
    break;
  }
  return value;
}

void mcu_key_pad_set(bool inout, u8 index, bool up)
{
  u16 PIN;
  if (inout)
  {
    if (up)
    {
      switch (index)
      {
      case 0:
        PIN = GPIO_PIN_Key_5;
        GPIO_Init(GPIO_Key_5,PIN,GPIO_MODE_IN_PU_NO_IT);
        break;
      case 1:
        PIN = GPIO_PIN_Key_6;
        GPIO_Init(GPIO_Key_6,PIN,GPIO_MODE_IN_PU_NO_IT);
        break;
      case 2:
        PIN = GPIO_PIN_Key_7;
        GPIO_Init(GPIO_Key_7,PIN,GPIO_MODE_IN_PU_NO_IT);
        break;
      case 3:
        PIN = GPIO_PIN_Key_8;
        GPIO_Init(GPIO_Key_8,PIN,GPIO_MODE_IN_PU_NO_IT);
        break;
      case 4:
        PIN = GPIO_PIN_Key_9;
        GPIO_Init(GPIO_Key_9,PIN,GPIO_MODE_IN_PU_NO_IT);
        break;
      case 5:
        PIN = GPIO_PIN_Key_10;
        GPIO_Init(GPIO_Key_10,PIN,GPIO_MODE_IN_PU_NO_IT);
        break;
      default:
        break;
      }
    }
    else
    {
      switch (index)
      {
      case 0:
        PIN = GPIO_PIN_Key_5;
        GPIO_Init(GPIO_Key_5,PIN,GPIO_MODE_IN_FL_NO_IT);
        break;
      case 1:
        PIN = GPIO_PIN_Key_6;
        GPIO_Init(GPIO_Key_6,PIN,GPIO_MODE_IN_FL_NO_IT);
        break;
      case 2:
        PIN = GPIO_PIN_Key_7;
        GPIO_Init(GPIO_Key_7,PIN,GPIO_MODE_IN_FL_NO_IT);
        break;
      case 3:
        PIN = GPIO_PIN_Key_8;
        GPIO_Init(GPIO_Key_8,PIN,GPIO_MODE_IN_FL_NO_IT);
        break;
      case 4:
        PIN = GPIO_PIN_Key_9;
        GPIO_Init(GPIO_Key_9,PIN,GPIO_MODE_IN_FL_NO_IT);
        break;
      case 5:
        PIN = GPIO_PIN_Key_10;
        GPIO_Init(GPIO_Key_10,PIN,GPIO_MODE_IN_FL_NO_IT);
        break;
      default:
        break;
      }
    }
  }
  else
  {
    switch (index)
    {
    case 0:
      PIN = GPIO_PIN_Key_5;
      GPIO_Init(GPIO_Key_5,PIN,GPIO_MODE_OUT_OD_LOW_FAST);
      break;
    case 1:
      PIN = GPIO_PIN_Key_6;
      GPIO_Init(GPIO_Key_6,PIN,GPIO_MODE_OUT_OD_LOW_FAST);
      break;
    case 2:
      PIN = GPIO_PIN_Key_7;
      GPIO_Init(GPIO_Key_7,PIN,GPIO_MODE_OUT_OD_LOW_FAST);
      break;
    case 3:
      PIN = GPIO_PIN_Key_8;
      GPIO_Init(GPIO_Key_8,PIN,GPIO_MODE_OUT_OD_LOW_FAST);
      break;
    case 4:
      PIN = GPIO_PIN_Key_9;
      GPIO_Init(GPIO_Key_9,PIN,GPIO_MODE_OUT_OD_LOW_FAST);
      break;
    case 5:
      PIN = GPIO_PIN_Key_10;
      GPIO_Init(GPIO_Key_10,PIN,GPIO_MODE_OUT_OD_LOW_FAST);
      break;
    default:
      break;
    }
  }
}

void mcu_key_pad_out(u8 index,bool state)
{
  if(state==FALSE)
  {
    switch (index)
    {
		case 0:
			GPIO_WriteLow(GPIO_Key_5, GPIO_PIN_Key_5);
			break;
		case 1:
			GPIO_WriteLow(GPIO_Key_6, GPIO_PIN_Key_6);
			break;
		case 2:
			GPIO_WriteLow(GPIO_Key_7, GPIO_PIN_Key_7);
			break;
		case 3:
			GPIO_WriteLow(GPIO_Key_8, GPIO_PIN_Key_8);
			break;
		case 4:
			GPIO_WriteLow(GPIO_Key_9, GPIO_PIN_Key_9);
			break;
		case 5:
			GPIO_WriteLow(GPIO_Key_10, GPIO_PIN_Key_10);
			break;
		default:
			break;
    }
  }
  else
  {
    switch (index)
    {
		case 0:
			GPIO_WriteHigh(GPIO_Key_5, GPIO_PIN_Key_5);
			break;
		case 1:
			GPIO_WriteHigh(GPIO_Key_6, GPIO_PIN_Key_6);
			break;
		case 2:
			GPIO_WriteHigh(GPIO_Key_7, GPIO_PIN_Key_7);
			break;
		case 3:
			GPIO_WriteHigh(GPIO_Key_8, GPIO_PIN_Key_8);
			break;
		case 4:
			GPIO_WriteHigh(GPIO_Key_9, GPIO_PIN_Key_9);
			break;
		case 5:
			GPIO_WriteHigh(GPIO_Key_10, GPIO_PIN_Key_10);
			break;
		default:
			break;
    }
  }
	
}

/**********************************************************************
Description     :  none
Input           :  none
Output          :  none
Return          :  none
Others          :  
					OK	DN	UP Cancel
					14	24	34	44

					1	2	3	*
					13	23 33	43

					4	5	6	0
					12	22	32	42

					7	8	9	#
					11	21	31	41
**********************************************************************/
u8 key_scan_board(u8 keycnt)
{
	u8 value = 0,i;

	switch (keycnt)
	{
		case 0:
			for (i=0; i<5; i++)
			{
				if (mcu_key_pad_read(i) == FALSE)
				{
					switch (i)
					{
						case 0:
							value = 0x14;
							break;
						case 1:
							value = 0x24;
							break;
						case 2:
							value = 0x34;
							break;
						case 3:
							value = 0x44;
							break;
						case 4:
							value = 0x54;
							break;
						default:
							break;
					}
				}
			}		
			break;
		case 1:
			for (i=0; i<5; i++)
			{
				if (mcu_key_pad_read(i) == FALSE)
				{
					switch (i)
					{
						case 0:
							value = 0x11;
							break;
						case 1:
							value = 0x21;
							break;
						case 2:
							value = 0x31;
							break;
						case 3:
							value = 0x41;
							break;
						case 4:
							value = 0x42;
							break;
						default:
							break;
					}
				}
			}	
			break;
		case 2:
			for (i=0; i<4; i++)
			{
				if (mcu_key_pad_read(i) == FALSE)
				{
					switch (i)
					{
						case 0:
							value = 0x12;
							break;
						case 1:
							value = 0x22;
							break;
						case 2:
							value = 0x32;
							break;
						case 3:
							value = 0x43;
							break;
						default:
							break;
					}
					
					break;
				}
			}
			break;
		case 3:
			for (i=0; i<3; i++)
			{
				if (mcu_key_pad_read(i) == FALSE)
				{
					switch (i)
					{
						case 0:
							value = 0x13;// 
							break;
						case 1:
							value = 0x23;
							break;
						case 2:
							value = 0x33;
							break;
						default:
							break;
					}
				}
			}	
			break;
		case 4:
			
			break;
		case 5:
			
			break;
		default:
			break;
	}

	return value;
}

u8 drv_keypad_scan(void)
{
	u8 key = 0;
	u8 scankeycnt;

	for (scankeycnt=0; scankeycnt<4; scankeycnt++)
	{
		switch (scankeycnt)
		{
			case 0://OK、L、R、C、MENU、
				mcu_key_pad_set(TRUE, 0, TRUE);//输入上拉 	//K5
				mcu_key_pad_set(TRUE, 1, TRUE);			//K6	
				mcu_key_pad_set(TRUE, 2, TRUE);			//K7
				mcu_key_pad_set(TRUE, 3, TRUE);			//K8
				mcu_key_pad_set(TRUE, 4, TRUE);			//K9		
				mcu_key_pad_set(TRUE, 5, TRUE);			//K10	
						
				break;
			case 1:// 扫描按键7 8 9 # 0
				mcu_key_pad_set(TRUE, 0, TRUE);//输入下拉
				mcu_key_pad_set(TRUE, 1, TRUE);			
				mcu_key_pad_set(TRUE, 2, TRUE);
				mcu_key_pad_set(TRUE, 3, TRUE);
				mcu_key_pad_set(TRUE, 4, TRUE);
				mcu_key_pad_set(FALSE,5, FALSE);//输出
				mcu_key_pad_out(5, FALSE);//输出高电平
			
				break;
			case 2:// 扫描按键4 5 6 *
				mcu_key_pad_set(TRUE, 0, TRUE);
				mcu_key_pad_set(TRUE, 1, TRUE);	
				mcu_key_pad_set(TRUE, 2, TRUE);
				mcu_key_pad_set(TRUE, 3, TRUE);
				mcu_key_pad_set(FALSE, 4, FALSE);//输出
				mcu_key_pad_out(4, FALSE);
				mcu_key_pad_set(FALSE,5, FALSE);
				break;
			case 3:// 扫描按键1 2 3
				mcu_key_pad_set(TRUE, 0, TRUE);
				mcu_key_pad_set(TRUE, 1, TRUE);
				mcu_key_pad_set(TRUE, 2, TRUE);
				mcu_key_pad_set(FALSE, 3, FALSE);//输出
				mcu_key_pad_out(3, FALSE);
				mcu_key_pad_set(FALSE,4, FALSE);
				mcu_key_pad_set(FALSE,5, FALSE);
				break;
			case 4:

				break;
			case 5:

				break;
			default:
				break;
		}
		key = key_scan_board(scankeycnt);
		mcu_key_pad_set(FALSE, 0, FALSE);	 	//K5
		mcu_key_pad_set(FALSE, 1, FALSE);			//K6	
		mcu_key_pad_set(FALSE, 2, FALSE);			//K7
		mcu_key_pad_set(FALSE, 3, FALSE);			//K8
		mcu_key_pad_set(FALSE, 4, FALSE);			//K9		
		mcu_key_pad_set(FALSE, 5, FALSE);			//K10	
		mcu_key_pad_out(0, FALSE);
		mcu_key_pad_out(1, FALSE);
		mcu_key_pad_out(2, FALSE);
		mcu_key_pad_out(3, FALSE);
		mcu_key_pad_out(4, FALSE);
		mcu_key_pad_out(5, FALSE);	
		if (key != 0)
		{
			if ( key != 0x14 && key != 0x24 && key != 0x34 && key != 0x44 && key != 0x54)
			{
				//scankeycnt = 0;
			}
			break;
		}
	}

	return key;
}

