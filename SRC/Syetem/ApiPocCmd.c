#include "ALLHead.h"
//u8 group_num_temp_count=0;


#define poc_login_info_len  100
#define poc_user_name_len   16*4  //16*4+2=66
#define poc_group_name_len  8*4  //16*4+2=66
#define poc_user_num        5
#define poc_group_num       50


u8 refresh_users_list_count=0;
u8 refresh_users_list_count_max=0;
u8 group_num_count=0;
u8 get_group_list_loss_flag=FALSE;
u8 GetMemberCount=0;
bool ApiPocCmd_PersonalCallingMode=FALSE;
u8 PresentGroupNum=0;
u8 ReadBuffer[80];//Test 存EEPROM读取的数据使用
u8 InvalidCallCount=0;
u8 ASCII_ActiveUserID[22];//Test 存EEPROM读取的数据使用
u8 Get_Unicode_ActiveUserIDBuf[45];//
u8 Get_0X_ActiveUserIDBuf[11];//
u8 UnicodeForGbk_AllUserNameBuf[poc_user_name_len];
u8 UnicodeForGbk_AllGrounpNameBuf[25];
u8 UnicodeForGbk_MainWorkNameBuf[15];
u8 UnicodeForGbk_MainUserNameBuf[25];
u8 UnicodeForGbk_SpeakerRightnowNameBuf[64];
const u8 *ucAtPocHead   = "AT+POC=";
const u8 *ucTingEnd   = "0B0000";
const u8 *ucTingStart   = "0B0001";
const u8 *ucSetIPAndID    = "010000";

u8 POC_GetAllGroupNameDone_Flag=FALSE; 
u8 POC_EnterPersonalCalling_Flag=0;
u8 POC_QuitPersonalCalling_Flag=0;
u8 POC_AtEnterPersonalCalling_Flag=0;
u8 POC_AtQuitPersonalCalling_Flag=0;
u8 POC_EnterGroupCalling_Flag=0;
u8 POC_QuitGroupCalling_Flag=0;
bool POC_ReceivedVoice_Flag=FALSE;
bool ApiPocCmd_Tone_Flag=FALSE;
bool ApiPocCmd_PlayReceivedVoice_Flag=FALSE;//开关喇叭使用
bool SwitchGroupBUG=FALSE;
//bool POC_ReceivedNoVoice_Flag=FALSE;
bool EnterPersonalCalling=FALSE;
u8 POC_ReceivedVoiceStart_Flag=0;
u8 POC_ReceivedVoiceEnd_Flag=0;
u8 POC_ReceivedVoiceEndForXTSF_Flag=0;
bool POC_Receive86_Flag=FALSE;
u8 OffLineCount=0;
u8 OnlineMembership=0;
u8 KeyPttState=0;
bool PocNoOnlineMember_Flag=FALSE;
bool PocNoOnlineMember_Flag2=FALSE;
bool GettheOnlineMembersDone=FALSE;


typedef struct{
  struct{
    union{
      struct{
        u16 bUserInfo	: 3;
        u16 bUserWrite	: 1;
        u16 bPocReset	: 1;
        u16 bPocOpen	: 1;
        u16 bModeChange	: 1;
        u16	 bMode	: 3;
        u16 bNetStat	: 2;
        u16 bUnline	: 1;
        u16             : 1;
        u16             : 2;
      }Bits;
      u16 Byte;
    }Msg;
    //u8 Buf[10];
    u8 Buf2[10];
    u8 Buf3[3];
    u8 Buf4[3];
    u8 Buf5[3];
    u8 Buf6[3];
    u8 Buf7[3];
    u8 Buf8[9];
    struct{
      struct{
        u8 bSet	: 1;
        u8 Len	: 7;
      }Msg;
      u8 Buf[poc_login_info_len];
    }LoginInfo;
  }NetState;
  struct{
    struct{
      union{
        struct{
          u16 bInitial		        : 1;
          u16 bPttState		        : 1;			//0: ptt involide; 1 :ptt volide
          u16 bPttUser		        : 1;			//ptt operrtor 0: oneself; 1: other
          u16 bWorkGrpVolide	        : 3;
          u16 bEnterGroup		: 1;
          u16 bCallFail	                : 1;
          u16 bLogin                    : 1;
          u16 AlarmMode            	: 1;
          u16 PersonalCallingMode 	: 1;
          u16 			        : 5;
        }Bits;
        u16 Byte;
      }Msg;
      
      struct{
        u8 PresentGroupId;
        u8 GroupNum;
        u8 Id[8];
        u8 Name[poc_group_name_len+1];
        u8 NameLen;
      }MainWorkGroup;//当前所在的群组
      
      struct{
        u8 Id[8];
        u8 Name[poc_group_name_len+1];
        u8 NameLen;
      }WorkGroup;//中间变量，无具体作用
      
      struct{
        //u8 Id[8];
        u8 Name[poc_group_name_len+1];
        u8 NameLen;
      }Group[poc_group_num];//群组列表
      
      struct{
        bool UnicodeForGbk_SpeakerRightnowName_english;
        u8 PresentUserId;
        u8 Name[poc_user_name_len+1];
        u8 NameLen;
      }SpeakerRightnow;//群组模式下，当前说话的用户
      
      struct{
        bool UnicodeForGbk_english;
        u8 PresentUserId;
        u8 UserNum;
        //u8 Id[8];
        u8 Name[poc_user_name_len+1];
        u8 NameLen;
      }PttUserName;//单呼模式下，当前呼叫的用户名
      
      struct{
        //u8 Id[8];
        u8 Name[poc_user_name_len+1];//11*4+1=45
        u8 NameLen;
      }WorkUserName;//中间变量
      struct{
        bool UnicodeForGbk_AllUserName_english;
        //u8 Id[8];
        u8 Name[poc_user_name_len+1];//11*4+1=45
        u8 NameLen;
      }UserName[poc_user_num];//用户列表
      
    }UseState;
  }WorkState;
}PocCmdDrv;

static PocCmdDrv PocCmdDrvobj;
void ApiPocCmd_PowerOnInitial(void)
{
  PocCmdDrvobj.NetState.Msg.Byte = 0x00;
  PocCmdDrvobj.WorkState.UseState.Msg.Byte = 0x00;
  PocCmdDrvobj.WorkState.UseState.Msg.Bits.bInitial = 0x01;
  PocCmdDrvobj.WorkState.UseState.Msg.Bits.bLogin=0;
}

void ApiPocCmd_WritCommand(PocCommType id, u8 *buf, u16 len)
{
  u8 i;
  u8 NetStateBuf[5]={0,0,0,0,0};
  u8 testData=0;
  DrvMC8332_TxPort_SetValidable(ON);
  DrvGD83_UART_TxCommand((u8 *)ucAtPocHead,strlen((char const *)ucAtPocHead));
  switch(id)
  {
  case PocComm_OpenPOC://1
    DrvGD83_UART_TxCommand(buf, len);
    break;
  case PocComm_SetParam://设置账号密码
    DrvGD83_UART_TxCommand((u8 *)ucSetIPAndID,strlen((char const *)ucSetIPAndID));
    DrvGD83_UART_TxCommand(ReadBuffer, strlen((char const *)ReadBuffer));
    break;
  case PocComm_GetParam:
  case PocComm_Login:
    break;
  case PocComm_Logout:
  case PocComm_Cancel:
    DrvGD83_UART_TxCommand(buf, len);
    break;
  case PocComm_ModifyPwd:
    break;
  case PocComm_EnterGroup:
    DrvGD83_UART_TxCommand("090000000000", 12);
    COML_HexToAsc(GroupCallingNum, NetStateBuf);
    if(strlen((char const*)NetStateBuf)==1)
    {
      NetStateBuf[1]=NetStateBuf[0];
      NetStateBuf[0]=0x30;
    }
    else
    {
      testData          =NetStateBuf[0];
      NetStateBuf[0]    =NetStateBuf[1];
      NetStateBuf[1]    =testData;
    }
    DrvGD83_UART_TxCommand(NetStateBuf, 2);
    break;
  case PocComm_Invite:
    DrvGD83_UART_TxCommand("0A0000000000", 12);
    COML_HexToAsc(PersonalCallingNum+0x64,PocCmdDrvobj.NetState.Buf2);
    COML_StringReverse(2,PocCmdDrvobj.NetState.Buf2);
    DrvGD83_UART_TxCommand(PocCmdDrvobj.NetState.Buf2, 2);
    break;
  case PocComm_StartPTT://3
    DrvGD83_UART_TxCommand(buf, len);
    break;
  case PocComm_EndPTT://4
    DrvGD83_UART_TxCommand(buf, len);
    break;
  case PocComm_GroupListInfo://5
    DrvGD83_UART_TxCommand("0D0000", 6);
    break;
  case PocComm_UserListInfo://6
    for(i=0;i<=20;i++)
    {
      //memset(PocCmdDrvobj.WorkState.UseState.UserName[i].Name,0,
             //sizeof(PocCmdDrvobj.WorkState.UseState.UserName[i].Name));//每次收到0e将所有群组清空
      PocCmdDrvobj.WorkState.UseState.UserName[i].NameLen=0;
    }
    GetMemberCount=0;
    GettheOnlineMembersDone=FALSE;
    //get_online_user_list_num_flag=FALSE;
    DrvGD83_UART_TxCommand("0E0000000000", 12);
    COML_HexToAsc(PresentGroupNum, NetStateBuf);
    if(strlen((char const*)NetStateBuf)==1)
    {
      NetStateBuf[1]=NetStateBuf[0];
      NetStateBuf[0]=0x30;
    }
    else
    {
      testData          =NetStateBuf[0];
      NetStateBuf[0]    =NetStateBuf[1];
      NetStateBuf[1]    =testData;
    }
    DrvGD83_UART_TxCommand(NetStateBuf, 2);
    break;
  case PocComm_Key://7
    DrvGD83_UART_TxCommand(buf, len);
    break;
  default:
    break;
  }
  DrvMc8332_UART_TxTail();
  DrvMC8332_TxPort_SetValidable(ON);
}

//从EEPROM中读取数据传给写频软件
u8 ApiPocCmd_user_info_get(u8 ** pBuf)
{
  u8 Len=0;
  Len = Combine2Hex(ReadBuffer, strlen((char const *)ReadBuffer), ReadBuffer);
  *pBuf = ReadBuffer;
  return (strlen((char const *)ReadBuffer))/2;
}

//写频写入数据存入EEPROM
bool ApiPocCmd_user_info_set(u8 *pBuf, u8 len)//cTxBuf为存放ip账号密码的信息
{
	bool r;
	u8 i, uRet = 0;
	//ADRLEN_INF	adr;

	for(i = 0; i < len; i++)
	{
		if(pBuf[i] == ';')
		{
			uRet++;
		}
	}
	if(uRet >= 2)
	{
		Dec2Hex(pBuf, len, PocCmdDrvobj.NetState.LoginInfo.Buf);//将收到的数转化为字符串//LoginInfo.Buf为存放
		PocCmdDrvobj.NetState.LoginInfo.Msg.Len = len << 0x01;//为什么要len*2？
		PocCmdDrvobj.NetState.LoginInfo.Msg.bSet = ON;
		//adr = CFG_GetCurAdr(ADR_IDLocalUserInfo);
		//FILE_Write(adr.Adr,adr.Len,(u8*)(&PocCmdDrvobj.NetState.LoginInfo));
                FILE_Write(0,PocCmdDrvobj.NetState.LoginInfo.Msg.Len,(u8*)(&PocCmdDrvobj.NetState.LoginInfo));
                
		for(i = 0; i < len; i++)
		{
			PocCmdDrvobj.NetState.LoginInfo.Buf[i] = pBuf[i];
		}
		PocCmdDrvobj.NetState.LoginInfo.Msg.Len = len;
		
		//SYS_BufReset();
		PocCmdDrvobj.WorkState.UseState.WorkUserName.NameLen = 0;
		PocCmdDrvobj.NetState.Msg.Bits.bUserInfo = 3;
		PocCmdDrvobj.NetState.Msg.Bits.bUserWrite = ON;
		r = TRUE;
	}
	else
	{
		r = FALSE;
	}
	return r;
}

void ApiPocCmd_10msRenew(void)
{
  u8 ucId,user_list_ucId,i, Len;
  u8 * pBuf, ucRet;
  u8 NumCount;
  u8 i_count1=0;
  while((Len = DrvMC8332_PocNotify_Queue_front(&pBuf)) != 0x00)
  {
    ucId = COML_AscToHex(pBuf, 0x02);
    switch(ucId)
    {
    case 0x0A://判断讲话状态
      ucId = COML_AscToHex(pBuf+2, 0x02);
      if(ucId==0x00)
      {
        if(TASK_Ptt_StartPersonCalling_Flag==TRUE)//如果按下PTT键单呼某用户
        {
          EnterPersonalCalling=TRUE;
        }
      }
      break;
    case 0x0B://判断按下PTT
      ucId = COML_AscToHex(pBuf+2, 0x02);
      if(ucId==0x00)
      {
        KeyPttState=1;//KeyPttState 0：未按PTT 1:按下ptt瞬间  2：按住PTT状态 3：松开PTT瞬间
      }
      break;
    case 0x0C://判断松开PTT
      ucId = COML_AscToHex(pBuf+2, 0x02);
      if(ucId==0x00)
      {
        KeyPttState=3;//KeyPttState 0：未按PTT 1:按下ptt瞬间  2：按住PTT状态 3：松开PTT瞬间
      }
      break;  
    case 0x0e://在线用户个数
      //0E000000000001000066745186963e6dfa51406236003
      //0E000000002a000000671152a
      ucId = COML_AscToHex(pBuf+8, 0x04);//
      PocCmdDrvobj.WorkState.UseState.PttUserName.UserNum = ucId;//
      refresh_users_list_count_max=(u8)(PocCmdDrvobj.WorkState.UseState.PttUserName.UserNum/poc_get_once_group_and_user_num);
      if(Len==12)
      {
        if(PocCmdDrvobj.WorkState.UseState.PttUserName.UserNum==0)
        {
          ApiPocCmd_PersonalCallingMode=FALSE;
          PocNoOnlineMember_Flag=TRUE;
        }
        else
        { 
          PocCmdDrvobj.WorkState.UseState.PttUserName.UserNum = ucId;
          PocNoOnlineMember_Flag=FALSE;
        }
      }
      break;
    case 0x0d://群组个数
      group_num_count=0;
      ucId = COML_AscToHex(pBuf+10, 0x02);
      PocCmdDrvobj.WorkState.UseState.MainWorkGroup.GroupNum = ucId;
      if(PocCmdDrvobj.WorkState.UseState.MainWorkGroup.GroupNum>=poc_group_num)
      {
        PocCmdDrvobj.WorkState.UseState.MainWorkGroup.GroupNum=poc_group_num;
      }
      break;
    case 0x80://获取工作组列表
      ucId = COML_AscToHex(pBuf+10, 0x02);
      if(ucId<=PocCmdDrvobj.WorkState.UseState.MainWorkGroup.GroupNum)//群组数大于最大群组数则不存
      {
        if(Len >= 24)//如果群组id后面还有群组名
        {
          PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen = Len - 24;
          if(PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen>=poc_group_name_len)
          {
            PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen=poc_group_name_len;
          }
        }
        else//无群组名
        {
          PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen = 0x00;
        }
        for(i = 0x00; i < PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen; i++)
        {
          PocCmdDrvobj.WorkState.UseState.WorkGroup.Name[i] = pBuf[i+24];//存入获取的群组名
          PocCmdDrvobj.WorkState.UseState.Group[ucId-1].Name[i] = 
            PocCmdDrvobj.WorkState.UseState.WorkGroup.Name[i];
        }
        PocCmdDrvobj.WorkState.UseState.Group[ucId-1].NameLen = PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen;
        group_num_count++;
        if(group_num_count<PocCmdDrvobj.WorkState.UseState.MainWorkGroup.GroupNum)
        {
          get_group_list_loss_flag=TRUE;
        }
        else
        {
          get_group_list_loss_flag=FALSE;
        }
      }
      break;
    case 0x81://获取组内成员列表
      ucId = COML_AscToHex(pBuf+10, 0x02);//
      refresh_users_list_count=(u8)(PersonalCallingNum/poc_get_once_group_and_user_num);
      user_list_ucId=ucId-refresh_users_list_count*poc_get_once_group_and_user_num;
      if(user_list_ucId<poc_get_once_group_and_user_num)
      {
        if(Len >= 20)//如果用户ID后面还有用户名
        {
          PocCmdDrvobj.WorkState.UseState.WorkUserName.NameLen = Len - 20;
          if(PocCmdDrvobj.WorkState.UseState.WorkUserName.NameLen>=poc_user_name_len)
          {
            PocCmdDrvobj.WorkState.UseState.WorkUserName.NameLen = poc_user_name_len;
          }
        }
        else//无用户名
        {
          PocCmdDrvobj.WorkState.UseState.WorkUserName.NameLen = 0x00;
        }
        for(i = 0x00; i < PocCmdDrvobj.WorkState.UseState.WorkUserName.NameLen; i++)
        {
          PocCmdDrvobj.WorkState.UseState.WorkUserName.Name[i] = pBuf[i+20];//存入获取的群组名
          PocCmdDrvobj.WorkState.UseState.UserName[user_list_ucId].Name[i]=PocCmdDrvobj.WorkState.UseState.WorkUserName.Name[i];
        }
        PocCmdDrvobj.WorkState.UseState.UserName[user_list_ucId].NameLen = PocCmdDrvobj.WorkState.UseState.WorkUserName.NameLen;

        
        
        i_count1=0;
        NumCount=0;
        while(1)
        {
            if(4*i_count1<PocCmdDrvobj.WorkState.UseState.UserName[user_list_ucId].NameLen)
            {
              if(PocCmdDrvobj.WorkState.UseState.WorkUserName.Name[4*i_count1+2]==0x30&&
                 PocCmdDrvobj.WorkState.UseState.WorkUserName.Name[4*i_count1+3]==0x30)
              {
                NumCount++;
              }
              i_count1++;
            }
            else
            {
              if(NumCount*4>=PocCmdDrvobj.WorkState.UseState.UserName[user_list_ucId].NameLen)
              {
                PocCmdDrvobj.WorkState.UseState.UserName[user_list_ucId].UnicodeForGbk_AllUserName_english=TRUE;
              }
              else
              {
                PocCmdDrvobj.WorkState.UseState.UserName[user_list_ucId].UnicodeForGbk_AllUserName_english=FALSE;
              }
              break;
            }
        }
        GetMemberCount++;
        if(refresh_users_list_count==refresh_users_list_count_max)//如果是最后一列信息
        {
          if(PocCmdDrvobj.WorkState.UseState.PttUserName.UserNum!=0
             &&GetMemberCount!=0
             &&GetMemberCount>=(PocCmdDrvobj.WorkState.UseState.PttUserName.UserNum%poc_get_once_group_and_user_num))
          {
            GetMemberCount=0;
            GettheOnlineMembersDone=TRUE;
          }
        }
        else//如果是一列完整的数据
        {
          if(GetMemberCount>=poc_get_once_group_and_user_num)
          {
            GetMemberCount=0;
            GettheOnlineMembersDone=TRUE;
          }
        }
        
        if(GettheOnlineMembersDone==TRUE&&huo_qu_zhong_flag==TRUE)//上下键查询到边界的时候再发一次0e指令，显示并播报当前选中群组
        {
          huo_qu_zhong_flag=FALSE;
          
          api_lcd_pwr_on_hint4("                ");//清屏
          if(UnicodeForGbk_AllUserName_english_flag(PersonalCallingNum%poc_get_once_group_and_user_num)==TRUE)
          {
            api_lcd_pwr_on_hint(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
          else
          {
            api_lcd_pwr_on_hint4(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
          VOICE_SetOutput(ATVOICE_FreePlay,ApiAtCmd_GetUserName(PersonalCallingNum%poc_get_once_group_and_user_num),ApiAtCmd_GetUserNameLen(PersonalCallingNum%poc_get_once_group_and_user_num));//播报按上键之后对应的用户名
        }
        
        if(get_online_user_list_num_flag==TRUE)
        {
          api_lcd_pwr_on_hint2(HexToChar_AllOnlineMemberNum());
          
          if(GettheOnlineMembersDone==TRUE
             ||GetMemberCount>=poc_get_once_group_and_user_num-1
             ||GetMemberCount>=PocCmdDrvobj.WorkState.UseState.PttUserName.UserNum)//解决在菜单在线成员列表状态下获取第一列成员列表总是少第二组的问题
          {
            get_online_user_list_num_flag=FALSE;
                    PersonalCallingNum=0;//解决按单呼键直接选中，单呼用户并不是播报的用户
                    VOICE_SetOutput(ATVOICE_FreePlay,"2857bf7e106258547065",20);//在线成员数
                    Delay_100ms(15);//DEL_SetTimer(0,150);
                    VOICE_SetOutput(ATVOICE_FreePlay,VoiceAllOnlineMemberNum(),8);//
                    Delay_100ms(10);//DEL_SetTimer(0,100);
                    VOICE_SetOutput(ATVOICE_FreePlay,ApiAtCmd_GetUserName(0),ApiAtCmd_GetUserNameLen(0));//首次获取组内成员播报第一个成员
                    api_lcd_pwr_on_hint3("在线成员列表  ");
                    api_lcd_pwr_on_hint4("                ");//清屏
                    if(UnicodeForGbk_AllUserName_english_flag(0)==TRUE)
                    {
                      api_lcd_pwr_on_hint(UnicodeForGbk_AllUserName(0));//显示当前选中的群组名
                    }
                    else
                    {
                      api_lcd_pwr_on_hint4(UnicodeForGbk_AllUserName(0));//显示当前选中的群组名
                    }
                    TheMenuLayer_Flag=2;
                    KeyPersonalCallingCount=0;//解决单呼模式，上下键成员非正常顺序，第一个成员在切换时会第二、第三个碰到
          }
        }
      }
      break;
    case 0x82://判断是否登录成功
      ucId = COML_AscToHex(pBuf+3, 0x01);
      if(ucId == 0x02)
      {
        PocCmdDrvobj.WorkState.UseState.Msg.Bits.bLogin = 0x01;
        OffLineCount=0;
      }
      else if(ucId == 0x00)
      {
        OffLineCount++;
        if(OffLineCount>=5)
        {
          ApiAtCmd_WritCommand(ATCOMM3_GD83Reset,(void*)0, 0);
          OffLineCount=0;
        }
        PocCmdDrvobj.WorkState.UseState.Msg.Bits.bLogin = 0x00;
      }
      else
      {
        PocCmdDrvobj.WorkState.UseState.Msg.Bits.bLogin = 0x00;
      }
      break;
/********判断是否是被呼状态******************************/
    case 0x83:
      ucId = COML_AscToHex(pBuf+2, 0x02);
      if(ucId == 0x00)
      {
        if(Len >= 12)
        {
          PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen = Len - 12;
          if(InvalidCallCount==1)
          {
            InvalidCallCount=0;
          }
          else
          {
            if(PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen > poc_user_name_len)
            {
              PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen = poc_user_name_len;
            }
            POC_ReceivedVoice_Flag=TRUE;
            POC_ReceivedVoiceStart_Flag=2;//0:正常 1：收到语音 2：刚开始语音
            POC_ReceivedVoiceEnd_Flag=1;//0:正常 1：收到语音 2：刚结束语音
            POC_ReceivedVoiceEndForXTSF_Flag=1;
          }
        }
        for(i = 0x00; i < PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen; i++)
        {
          PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name[i] = pBuf[i+12];//存入当前说话人的名字
        }
        i_count1=0;
        NumCount=0;
        while(1)
        {
          if(4*i_count1<PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen)
          {
            if(PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name[4*i_count1+2]==0x30&&
               PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name[4*i_count1+3]==0x30)
            {
              NumCount++;
            }
            i_count1++;
          }
          else
          {
            if(NumCount*4>=PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen)
            {
              PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.UnicodeForGbk_SpeakerRightnowName_english=TRUE;
            }
            else
            {
              PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.UnicodeForGbk_SpeakerRightnowName_english=FALSE;
            }
            break;
          }
        }
        }
      if(ucId == 0xff)
      {
        if(SwitchGroupBUG==TRUE)
        {
        }
        else
        {
          POC_ReceivedVoice_Flag=FALSE;
          POC_ReceivedVoiceEnd_Flag=2;//0:正常 1：收到语音 2：刚结束语音
          POC_ReceivedVoiceEndForXTSF_Flag=2;
        }
      }
      break;
/**************************************/
    case 0x8B:
      ucId = COML_AscToHex(pBuf+4, 0x02);
      if(ucId==0x00)//语音通话结束
      {
        ApiPocCmd_PlayReceivedVoice_Flag=FALSE;
      }
      if(ucId==0x01)//语音通话开始
      {
        ApiPocCmd_PlayReceivedVoice_Flag=TRUE;
      }
      if(ucId==0x03)//tone音
      {
        ApiPocCmd_Tone_Flag=TRUE;
        AUDIO_IOAFPOW(ON);
#if 1//当收到Tone音，将ZTTS至0
        ApiAtCmd_ZTTS_Flag=FALSE;
#endif
      }

      break;
    case 0x86:
      ucId = COML_AscToHex(pBuf+10, 0x02);
      if(ucId!=0x00&&ucId!=0xff)
      {
        PresentGroupNum=ucId;
      }
      POC_GetAllGroupNameDone_Flag=TRUE;
      POC_Receive86_Flag=TRUE;
/****************判断接入单呼**************************************************************/
      ucId = COML_AscToHex(pBuf+4, 0x02);
      if(ucId==0x0a)//接入单呼
      { 
        POC_EnterPersonalCalling_Flag=2;//用于判断进入单呼,0:正常 2：正在进入单呼 1：单呼状态
        POC_QuitPersonalCalling_Flag=1;//用于判断退出单呼
      }
      else
      {
        if(POC_QuitPersonalCalling_Flag==1)//收到则退出单呼（退出单呼、进组状态）
        {
          POC_EnterPersonalCalling_Flag=0;
          POC_QuitPersonalCalling_Flag=2;
        }
      }
/****************群组状态判断及群组信息获取**************************************************************/
      ucId = COML_AscToHex(pBuf+10, 0x02);
      if(ucId==0xff)
      {
        if(POC_EnterPersonalCalling_Flag==1)
        {
          POC_EnterPersonalCalling_Flag=0;
          POC_QuitPersonalCalling_Flag=2;
          POC_AtEnterPersonalCalling_Flag=0;
          POC_AtQuitPersonalCalling_Flag=0;
        }
        if(POC_AtEnterPersonalCalling_Flag==1)
        {
          POC_AtEnterPersonalCalling_Flag=0;
          POC_AtQuitPersonalCalling_Flag=2;
          POC_EnterPersonalCalling_Flag=0;
          POC_QuitPersonalCalling_Flag=0;
        }
        POC_EnterGroupCalling_Flag=0;//0默认 1在群组内 2正在进入
        POC_QuitGroupCalling_Flag=2;//0默认 1在群组内 2正在退出
      }
      else//正在进入群组或单呼
      {
          if(EnterPersonalCalling==TRUE)
          {
            POC_AtEnterPersonalCalling_Flag=2;
            POC_AtQuitPersonalCalling_Flag=1;
            EnterPersonalCalling=FALSE;
            TASK_Ptt_StartPersonCalling_Flag=FALSE;
          }
#if 1//解决被呼状态下换组后按PTT提示禁发，绿灯亮
          if(POC_AtEnterPersonalCalling_Flag==0)//解决单呼模式下显示个呼名前还显示一下群组名的BUG
          {
            POC_ReceivedVoice_Flag=FALSE;
            POC_ReceivedVoiceEnd_Flag=2;//0:正常 1：收到语音 2：刚结束语音
            POC_ReceivedVoiceEndForXTSF_Flag=2;
            POC_ReceivedVoiceStart_Flag=0;//0:正常 1：收到语音 2：刚开始语音
          }

#endif
        if(PocCmdDrvobj.WorkState.UseState.Msg.Bits.PersonalCallingMode == 0x01)//如果是进入单呼模式则86存入单呼名
        {
          POC_EnterGroupCalling_Flag=2;
          POC_QuitGroupCalling_Flag=1;
            if(Len >= 12)//如果群组id后面还有群组名
            {
              PocCmdDrvobj.WorkState.UseState.PttUserName.NameLen = Len - 12;
              if(PocCmdDrvobj.WorkState.UseState.PttUserName.NameLen > poc_user_name_len)
              {
                PocCmdDrvobj.WorkState.UseState.PttUserName.NameLen = poc_user_name_len;
              }
            }
            else//无群组名
            {
              PocCmdDrvobj.WorkState.UseState.PttUserName.NameLen = 0x00;
            }
            for(i = 0x00; i < PocCmdDrvobj.WorkState.UseState.PttUserName.NameLen; i++)
            {
              PocCmdDrvobj.WorkState.UseState.PttUserName.Name[i] = pBuf[i+12];//存入获取的群组名
            }
            i_count1=0;
            NumCount=0;
            while(1)
            {
              if(4*i_count1<PocCmdDrvobj.WorkState.UseState.PttUserName.NameLen)
              {
                if(PocCmdDrvobj.WorkState.UseState.PttUserName.Name[4*i_count1+2]==0x30&&
                   PocCmdDrvobj.WorkState.UseState.PttUserName.Name[4*i_count1+3]==0x30)
                {
                  NumCount++;
                }
                i_count1++;
              }
              else
              {
                if(NumCount*4>=PocCmdDrvobj.WorkState.UseState.PttUserName.NameLen)
                {
                  PocCmdDrvobj.WorkState.UseState.PttUserName.UnicodeForGbk_english=TRUE;
                }
                else
                {
                  PocCmdDrvobj.WorkState.UseState.PttUserName.UnicodeForGbk_english=FALSE;
                }
                break;
              }
            }
        }
        else//进组存组名
        {
          ApiPocCmd_PersonalCallingMode=FALSE;
          POC_EnterGroupCalling_Flag=2;
          POC_QuitGroupCalling_Flag=1;
          PocCmdDrvobj.WorkState.UseState.MainWorkGroup.PresentGroupId = ucId;
          ucId = 0x00;
          for(i = 0x00; i < 0x08; i++)
          {
            PocCmdDrvobj.WorkState.UseState.WorkGroup.Id[i] = pBuf[i+4];
            PocCmdDrvobj.WorkState.UseState.MainWorkGroup.Id[i] = PocCmdDrvobj.WorkState.UseState.WorkGroup.Id[i];
            if(PocCmdDrvobj.WorkState.UseState.WorkGroup.Id[i] != 'f') //=f为离开群组
              ucId++;
          }
          if(ucId==0x00)//如果为指令代表离开群组
          {}
          else//r如果为在群组内
          {
            if(Len >= 12)//如果群组id后面还有群组名
            {
              PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen = Len - 12;
              if(PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen > poc_group_name_len)
              {
                PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen = poc_group_name_len;
              }
            }
            else//无群组名
            {
              PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen = 0x00;
            }
            for(i = 0x00; i < PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen; i++)
            {
              PocCmdDrvobj.WorkState.UseState.WorkGroup.Name[i] = pBuf[i+12];//存入获取的群组名
              PocCmdDrvobj.WorkState.UseState.MainWorkGroup.Name[i] = PocCmdDrvobj.WorkState.UseState.WorkGroup.Name[i];
            }
            PocCmdDrvobj.WorkState.UseState.MainWorkGroup.NameLen = PocCmdDrvobj.WorkState.UseState.WorkGroup.NameLen;
          }
        }
      }
      break;
    case 0x91://通知进入某种模式（进入退出一键告警、单呼模式）
      ucId = COML_AscToHex(pBuf+2, 0x02);
      if(ucId == 0x00)
      {
        PocCmdDrvobj.WorkState.UseState.Msg.Bits.AlarmMode = 0x00;//退出一键告警
      }
      if(ucId == 0x01)
      {
        PocCmdDrvobj.WorkState.UseState.Msg.Bits.AlarmMode = 0x01;//进入一键告警
      }
      if(ucId == 0x02)
      {
        PocCmdDrvobj.WorkState.UseState.Msg.Bits.PersonalCallingMode = 0x01;//进入单呼
      }
      if(ucId == 0x03)
      {
        PocCmdDrvobj.WorkState.UseState.Msg.Bits.PersonalCallingMode = 0x00;//退出单呼 
        //AUDIO_IOAFPOW(ON);//打开功放，解决'组呼模式'播报问题

      }
      break;
    default:
      break;
    }
  }
}

u8 *ApiAtCmd_GetGroupName(u8 n)//获取所有群组名
{
  return PocCmdDrvobj.WorkState.UseState.Group[n-1].Name;
}
u8 ApiAtCmd_GetGroupNameLen(u8 n)
{
  return PocCmdDrvobj.WorkState.UseState.Group[n-1].NameLen;
}
/*********************************/
//换组换呼上下键显示屏显示选中群组名
u8 *UnicodeForGbk_AllGrounpName(u8 n)
{
  u8 *GrounpBuf1;
  u8 Buf2[poc_user_name_len];

  u8 GrounpLen=0;
  u8 i=0;
  GrounpBuf1=PocCmdDrvobj.WorkState.UseState.Group[n-1].Name;
  GrounpLen=PocCmdDrvobj.WorkState.UseState.Group[n-1].NameLen;
  while(1)
  {
    if(4*i<=GrounpLen)
    {
      Buf2[4*i+0]=GrounpBuf1[4*i+2];
      Buf2[4*i+1]=GrounpBuf1[4*i+3];
      Buf2[4*i+2]=GrounpBuf1[4*i+0];
      Buf2[4*i+3]=GrounpBuf1[4*i+1];
      UnicodeForGbk_AllGrounpNameBuf[2*i+0]=COML_AscToHex(Buf2+(4*i), 0x02);
      UnicodeForGbk_AllGrounpNameBuf[2*i+1]=COML_AscToHex(Buf2+(4*i)+2, 0x02);
      i++;
    }
    else
    {
      Buf2[GrounpLen]='\0';
      return UnicodeForGbk_AllGrounpNameBuf;
    }
  }
}
/********************************/

u8 *ApiAtCmd_GetUserName(u8 n)//获取所有在线用户名（个呼）
{
  return PocCmdDrvobj.WorkState.UseState.UserName[n].Name;
}
u8 ApiAtCmd_GetUserNameLen(u8 n)
{
  return PocCmdDrvobj.WorkState.UseState.UserName[n].NameLen;
}

/*********************************/
//换组换呼上下键显示屏显示选中用户名
u8 *UnicodeForGbk_AllUserName(u8 n)
{
  u8 *UserBuf1;
  u8 Buf3[poc_user_name_len];

  u8 UserLen2=0;
  u8 i=0;
  u8 j=0;
  u8 NumCount=0;
  UserBuf1=PocCmdDrvobj.WorkState.UseState.UserName[n].Name;//read_all_user_name_from_flash(n);
  UserLen2=strlen((char const *)PocCmdDrvobj.WorkState.UseState.UserName[n].Name);
  while(1)
  {
    if(4*i<UserLen2)
    {
/****解决用户名为纯数字时，显示位数只有8位的问题****************/
      if(UserBuf1[4*i+2]==0x30&&UserBuf1[4*i+3]==0x30)
      {
        NumCount++;
      }
/**************************************************************/
      Buf3[4*i+0]=UserBuf1[4*i+2];
      Buf3[4*i+1]=UserBuf1[4*i+3];
      Buf3[4*i+2]=UserBuf1[4*i+0];
      Buf3[4*i+3]=UserBuf1[4*i+1];
      UnicodeForGbk_AllUserNameBuf[2*i+0]=COML_AscToHex(Buf3+(4*i), 0x02);
      UnicodeForGbk_AllUserNameBuf[2*i+1]=COML_AscToHex(Buf3+(4*i)+2, 0x02);
      i++;
    }
    else
    {
      Buf3[UserLen2]='\0';
      if(NumCount*4>=UserLen2)
      {
        for(j=0;4*j<UserLen2;j++)
        {
          UnicodeForGbk_AllUserNameBuf[j]=COML_AscToHex(UserBuf1+(4*j), 0x02);
        }
        return UnicodeForGbk_AllUserNameBuf;
      }
      else
      {
        return UnicodeForGbk_AllUserNameBuf;
      }
    }
      
  }
}
/********************************/


u8 *ApiAtCmd_GetMainWorkName(void)//获取工作群组名
{
  return PocCmdDrvobj.WorkState.UseState.MainWorkGroup.Name;
}
u8 ApiAtCmd_GetMainWorkNameLen(void)
{
  return PocCmdDrvobj.WorkState.UseState.MainWorkGroup.NameLen;
}

u8 *ApiAtCmd_GetMainUserName(void)//获取当前用户名（个呼）
{
  return PocCmdDrvobj.WorkState.UseState.PttUserName.Name;
}
u8 ApiAtCmd_GetMainUserNameLen(void)
{
  return PocCmdDrvobj.WorkState.UseState.PttUserName.NameLen;
}

u8 ApiAtCmd_GetGroupNum(void)//获取群组数
{
  return PocCmdDrvobj.WorkState.UseState.MainWorkGroup.GroupNum;
}
u8 ApiAtCmd_GetUserNum(void)//获取在线成员数（个呼）
{
  return PocCmdDrvobj.WorkState.UseState.PttUserName.UserNum;
}
u8 ApiAtCmd_GetMainGroupId(void)//获取当前群组id
{
  return PocCmdDrvobj.WorkState.UseState.MainWorkGroup.PresentGroupId;
}
u8 ApiAtCmd_GetMainUserId(void)//获取当前用户id（个呼）
{
  return PocCmdDrvobj.WorkState.UseState.PttUserName.PresentUserId;
}

bool ApiPocCmd_GetPttState(void)//判断PPT状态，是否有话权
{
  return (bool)PocCmdDrvobj.WorkState.UseState.Msg.Bits.bPttState;
}
bool ApiAtCmd_GetLoginState(void)
{
  return (bool)PocCmdDrvobj.WorkState.UseState.Msg.Bits.bLogin;
}
void ApiAtCmd_SetLoginState(void)
{
  PocCmdDrvobj.WorkState.UseState.Msg.Bits.bLogin=0;
}
/*
void ApiGetPocBuf(void)
{
  DrvGD83_UART_TxCommand((u8 *)"AT+Printf=",strlen((char const *)"AT+Printf="));
  DrvGD83_UART_TxCommand((u8 *)PocCmdDrvobj.PocDateTest.Buf,strlen((char const *)PocCmdDrvobj.PocDateTest.Buf));
  DrvGD83_UART_TxCommand((u8 *)"\r\n",strlen((char const *)"\r\n"));                      
}
*/


bool GetPersonalCallingMode(void)
{
  return (bool)PocCmdDrvobj.WorkState.UseState.Msg.Bits.PersonalCallingMode;
}

u8 *HexToChar_MainGroupId(void)//16进制转字符串 当前群组ID 显示屏数据使用
{
  u8 i;
  i=ApiAtCmd_GetMainGroupId();
  PocCmdDrvobj.NetState.Buf3[0]=((i&0xf0)>>4)+0x30;
  PocCmdDrvobj.NetState.Buf3[1]=(i&0x0f)+0x30;
  PocCmdDrvobj.NetState.Buf3[2]='\0';
  return PocCmdDrvobj.NetState.Buf3;
}
u8 *HexToChar_GroupCallingNum(void)//16进制转字符串 按键播报当前群组ID 显示屏数据使用
{
  u8 i;
  i=GroupCallingNum;
  PocCmdDrvobj.NetState.Buf4[0]=((i&0xf0)>>4)+0x30;
  PocCmdDrvobj.NetState.Buf4[1]=(i&0x0f)+0x30;
  PocCmdDrvobj.NetState.Buf4[2]='\0';
  return PocCmdDrvobj.NetState.Buf4;
}

u8 *HexToChar_MainUserId(void)//16进制转字符串 当前用户ID 显示屏数据使用
{
  u8 i;
  i=ApiAtCmd_GetMainUserId()+0x01;
  PocCmdDrvobj.NetState.Buf5[0]=((i&0xf0)>>4)+0x30;
  PocCmdDrvobj.NetState.Buf5[1]=(i&0x0f)+0x30;
  PocCmdDrvobj.NetState.Buf5[2]='\0';
  return PocCmdDrvobj.NetState.Buf5;
}
u8 *HexToChar_PersonalCallingNum(void)//16进制转字符串 按键播报当前用户ID 显示屏数据使用
{
  u8 i;
  i=PersonalCallingNum+0x01;
  PocCmdDrvobj.NetState.Buf6[0]=((i&0xf0)>>4)+0x30;
  PocCmdDrvobj.NetState.Buf6[1]=(i&0x0f)+0x30;
  PocCmdDrvobj.NetState.Buf6[2]='\0';
  return PocCmdDrvobj.NetState.Buf6;
}

u8 *HexToChar_AllOnlineMemberNum(void)//16进制转字符串 显示屏显示在线成员个数
{
  u8 i;
  i=PocCmdDrvobj.WorkState.UseState.PttUserName.UserNum;
  COML_DecToAsc(i,PocCmdDrvobj.NetState.Buf7);
  COML_StringReverse(strlen((char const *)PocCmdDrvobj.NetState.Buf7),PocCmdDrvobj.NetState.Buf7);
  return PocCmdDrvobj.NetState.Buf7;
}

u8 *VoiceAllOnlineMemberNum(void)// 语音播报在线成员个数
{
  PocCmdDrvobj.NetState.Buf8[0]='3';
  PocCmdDrvobj.NetState.Buf8[1]=PocCmdDrvobj.NetState.Buf7[0];
  PocCmdDrvobj.NetState.Buf8[2]='0';
  PocCmdDrvobj.NetState.Buf8[3]='0';
  PocCmdDrvobj.NetState.Buf8[4]='3';
  PocCmdDrvobj.NetState.Buf8[5]=PocCmdDrvobj.NetState.Buf7[1];
  PocCmdDrvobj.NetState.Buf8[6]='0';
  PocCmdDrvobj.NetState.Buf8[7]='0';
  PocCmdDrvobj.NetState.Buf8[8]='\0';
  return PocCmdDrvobj.NetState.Buf8;
}

//显示屏显示当前群组名
u8 *UnicodeForGbk_MainWorkName(void)
{
  u8 *Buf1;
  u8 Buf2[poc_group_name_len];

  u8 Len=0;
  u8 i=0;
  Buf1=ApiAtCmd_GetMainWorkName();
  Len=strlen((char const *)ApiAtCmd_GetMainWorkName());
  while(1)
  {
    if(4*i<=Len)
    {
      Buf2[4*i+0]=Buf1[4*i+2];
      Buf2[4*i+1]=Buf1[4*i+3];
      Buf2[4*i+2]=Buf1[4*i+0];
      Buf2[4*i+3]=Buf1[4*i+1];
      UnicodeForGbk_AllGrounpNameBuf[2*i+0]=COML_AscToHex(Buf2+(4*i), 0x02);
      UnicodeForGbk_AllGrounpNameBuf[2*i+1]=COML_AscToHex(Buf2+(4*i)+2, 0x02);
      i++;
    }
    else
    {
      Buf2[Len]='\0';

      return UnicodeForGbk_AllGrounpNameBuf;
    }
  }
}


//显示屏显示当前用户昵称
u8 *UnicodeForGbk_MainUserName(void)
{
  u8 *Buf0;
  u8 Buf2[poc_user_name_len];

  u8 Len=0,i=0;
  u8 j=0;
  u8 NumCount=0;
  Buf0=PocCmdDrvobj.WorkState.UseState.PttUserName.Name;
  Len=PocCmdDrvobj.WorkState.UseState.PttUserName.NameLen;
  while(1)
  {
    if(4*i<Len)
    {
/****解决用户名为纯数字时，显示位数只有8位的问题****************/
      if(Buf0[4*i+2]==0x30&&Buf0[4*i+3]==0x30)
      {
        NumCount++;
      }
/**************************************************************/
      Buf2[4*i+0]=Buf0[4*i+2];
      Buf2[4*i+1]=Buf0[4*i+3];
      Buf2[4*i+2]=Buf0[4*i+0];
      Buf2[4*i+3]=Buf0[4*i+1];
      UnicodeForGbk_MainUserNameBuf[2*i+0]=COML_AscToHex(Buf2+(4*i), 0x02);
      UnicodeForGbk_MainUserNameBuf[2*i+1]=COML_AscToHex(Buf2+(4*i)+2, 0x02);
      i++;
    }
    else
    {
      Buf2[Len]='\0';
      if(NumCount*4>=Len)
      {
        for(j=0;4*j<Len;j++)
        {
          UnicodeForGbk_MainUserNameBuf[j]=COML_AscToHex(Buf0+(4*j), 0x02);
        }
        return UnicodeForGbk_MainUserNameBuf;
      }
      else
      {
        return UnicodeForGbk_MainUserNameBuf;
      }
    }
  }
}

//显示屏显示组呼模式下当前说话人的昵称
u8 *UnicodeForGbk_SpeakerRightnowName(void)
{
  u8 Buf2[poc_user_name_len+4];
  u8 i=0;
  u8 j=0;
  u8 NumCount=0;
  memset(UnicodeForGbk_SpeakerRightnowNameBuf,0,sizeof(UnicodeForGbk_SpeakerRightnowNameBuf));
  while(1)
  {
    if(4*i<PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen)
    {
/****解决用户名为纯数字时，显示位数只有8位的问题****************/
      if(PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name[4*i+2]==0x30&&
         PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name[4*i+3]==0x30)
      {
        NumCount++;
      }
        Buf2[4*i+0]=PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name[4*i+2];
        Buf2[4*i+1]=PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name[4*i+3];
        Buf2[4*i+2]=PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name[4*i+0];
        Buf2[4*i+3]=PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name[4*i+1];

      UnicodeForGbk_SpeakerRightnowNameBuf[2*i+0]=COML_AscToHex(Buf2+(4*i), 0x02);
      UnicodeForGbk_SpeakerRightnowNameBuf[2*i+1]=COML_AscToHex(Buf2+(4*i)+2, 0x02);
      i++;
    }
    else
    {
      Buf2[PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen]='\0';
      if(NumCount*4>=PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen)
      {
        for(j=0;4*j<PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.NameLen;j++)
        {
          UnicodeForGbk_SpeakerRightnowNameBuf[j]=COML_AscToHex(PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.Name+(4*j), 0x02);
        }
        return UnicodeForGbk_SpeakerRightnowNameBuf;
      }
      else
      {
        return UnicodeForGbk_SpeakerRightnowNameBuf;
      }
    }
  }
}
   
//当前用户ID（播报使用）
u8 *Get_Unicode_ActiveUserID(void)
{
  u8 Len=0,i=0;
  FILE_Read(28,22,ASCII_ActiveUserID);//3139383030333037343732
  Len=strlen((char const *)ASCII_ActiveUserID);
  for(i=0;i<Len;i++)
  {
    if(i%2==0)//偶数
    {
      Get_Unicode_ActiveUserIDBuf[2*i]=ASCII_ActiveUserID[i];
    }
    else//奇数
    {
      Get_Unicode_ActiveUserIDBuf[2*i-1]=ASCII_ActiveUserID[i];
      Get_Unicode_ActiveUserIDBuf[2*i]=0x30;
      Get_Unicode_ActiveUserIDBuf[2*i+1]=0x30;
    }
  }
  //31003900380030003000330030003700340037003200
  return Get_Unicode_ActiveUserIDBuf;
}

//当前用户ID（显示屏使用）
u8 *Get_GBK_ActiveUserID(void)
{
  u8 i=0,Len=0;
  FILE_Read(28,22,ASCII_ActiveUserID);//3139383030333037343732
  Len=strlen((char const *)ASCII_ActiveUserID);
  for(i=0;i<Len;i++)
  {
    if(i%2!=0)
    {
      Get_0X_ActiveUserIDBuf[(i-1)/2]=ASCII_ActiveUserID[i];
    }
  }
  return Get_0X_ActiveUserIDBuf;
}

bool UnicodeForGbk_SpeakerRightnowName_english_flag(void)
{
  return PocCmdDrvobj.WorkState.UseState.SpeakerRightnow.UnicodeForGbk_SpeakerRightnowName_english;
}
bool UnicodeForGbk_MainUserName_english_flag(void)
{
  return PocCmdDrvobj.WorkState.UseState.PttUserName.UnicodeForGbk_english;
}
bool UnicodeForGbk_AllUserName_english_flag(u8 a)
{
  return PocCmdDrvobj.WorkState.UseState.UserName[a].UnicodeForGbk_AllUserName_english;
}

/*u8 *read_all_user_name_from_flash(u8 ucId)//从flash中读取用户名
{
  file_flash_read(0x6800+ucId*(poc_user_name_len+1),poc_user_name_len+1,read_all_user_name_from_flash_buf);
  return read_all_user_name_from_flash_buf;
}*/

/*void select_which_line_users_list(void)
{
  if(PersonalCallingNum%20==0)
  {
    //at+POC=0e00000000001
  }
  
  
  
}*/
