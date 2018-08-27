#include "AllHead.h"

#define KeyCountNum 50//数字键盘消抖-解决了上下键误读数字键问题
u8 AkeyvolumeCount=7;
u8 *ucVGR1                       = "AT+VGR=1";//音量增益1
u8 *ucVGR7                       = "AT+VGR=7";//音量增益默认7
u8 *ucKeyUp                = "10000003";
u8 *ucKeyDown              = "10000004";
u8 *ucQuitPersonalCalling  = "0A0000ffffffff";
s8 GroupCallingNum=1;
s8 PersonalCallingNum=0;
s8 KeyUpDownCount=0;//组呼上下键计数
s8 KeyPersonalCallingCount=0;//个呼上下键计数
u32 get_key_value(u8 scan_value);
u8 Key_Flag_1=0;
u8 MenuMode_Flag=0;
u8 MenuModeCount=1;
u8 BacklightTimeSetCount=1;//默认进选择体1
u8 KeylockTimeSetCount=0x11;//默认进选择体1
u8 TheMenuLayer_Flag=0;//所处菜单层级；默认状态：1 一级菜单：1 二级菜单：2
bool NumberKeyboardPressDown_flag=FALSE;
bool LockingState_EnterOK_Flag=FALSE;
u8 VoiceType_FreehandOrHandset_Flag=0;
u8 TestNum1,TestNum2,TestNum3,TestNum4,TestNum5,TestNum6;
u8 TestNum7,TestNum8,TestNum9,TestNum10,TestNum11,TestNum12;
u8 TestBuf1[6];//测试显示屏短号号码使用
  u8 num1=0;//测试显示屏短号号码使用
  u8 num2=0;//测试显示屏短号号码使用
  u8 num3=0;//测试显示屏短号号码使用
  u8 num4=0;//测试显示屏短号号码使用
  u8 num5=0;//测试显示屏短号号码使用
  u8 num6=0;//测试显示屏短号号码使用
  u8 numCount=0;//测试显示屏短号号码使用
bool PressButton;//测试短号功能使用
bool KeyBoardState;//测试短号功能使用
bool UpDownSwitching_Flag=FALSE;
bool get_online_user_list_num_flag=FALSE;
bool get_online_user_list_num_for_key_flag=FALSE;
bool huo_qu_zhong_flag=FALSE;
u8 xinhao_test_flag=0;
static void GeHuTest(u32 KeyID);

void Keyboard_Test(void)
{
  u8 scanvalue = 0;
  u32 ulAllKeyID = 0x00000000;

  scanvalue = drv_keypad_scan();
  ulAllKeyID = get_key_value(scanvalue);
  switch(ulAllKeyID)
  {
  case 0x00000002://1
    TestNum1++;
    if(TestNum1>=KeyCountNum)
    {
      TestNum1=0;
    NumberKeyboardPressDown_flag=TRUE;
    }
    break; 
  case 0x00000008://3
    TestNum2++;
    if(TestNum2>=KeyCountNum)
    {
      TestNum2=0;
    NumberKeyboardPressDown_flag=TRUE;
    }
    break;
  case 0x00000080://4
    NumberKeyboardPressDown_flag=TRUE;
    if(xinhao_test_flag==0x09)//按294进入测试信号模式
    {
      xinhao_test_flag=0x04;
    }
    break;
  case 0x00000200://6
    TestNum4++;
    if(TestNum4>=KeyCountNum)
    {
      TestNum4=0;
    NumberKeyboardPressDown_flag=TRUE;
    }
    break;
  case 0x00002000://7
    TestNum5++;
    if(TestNum5>=KeyCountNum)
    {
      TestNum5=0;
    NumberKeyboardPressDown_flag=TRUE;
    }
     break;
  case 0x00008000://9
    NumberKeyboardPressDown_flag=TRUE;
    if(xinhao_test_flag==0x02)//按294进入测试信号模式
    {
      xinhao_test_flag=0x09;
    }
    break;
  case 0x00010000://dn
    NumberKeyboardPressDown_flag=TRUE;
    if(LockingState_Flag==TRUE)
    {}
    else
    {
    if(MenuMode_Flag==1)
    {
      if(ApiMenu_BacklightTimeSet_Flag==1)//如果是设置背光灯二级菜单
      {
        BacklightTimeSetCount=BacklightTimeSetCount-1;
        if(BacklightTimeSetCount<1) {BacklightTimeSetCount=7;}
        Level3MenuDisplay(BacklightTimeSetCount);
      }
      else if(ApiMenu_KeylockTimeSet_Flag==1)//如果是设置键盘锁二级菜单
      {
        KeylockTimeSetCount=KeylockTimeSetCount-1;
        if(KeylockTimeSetCount<0x10) {KeylockTimeSetCount=0x16;}
        Level3MenuDisplay(KeylockTimeSetCount);
      }
      else if(ApiMenu_BeiDouOrWritingFrequency_Flag==1)//如果是设置网络模式
      {
        if(network_mode_2gor3g_SetCount==0x17)
        {
          network_mode_2gor3g_SetCount=0x18;
        }
        else
        {
          network_mode_2gor3g_SetCount=0x17;
        }
        Level3MenuDisplay(network_mode_2gor3g_SetCount);
      }
      else if(ApiMenu_SwitchOnlineUser_Flag==1)//如果是显示在线用户名二级菜单---------------------------------------------------------------
      {
        KeyPersonalCallingCount--;
        PersonalCallingNum=KeyPersonalCallingCount;//个呼计数从零开始
        if(PersonalCallingNum<0)
        {
          PersonalCallingNum=ApiAtCmd_GetUserNum()-1;
          KeyPersonalCallingCount=ApiAtCmd_GetUserNum()-1;
        }
        if((PersonalCallingNum==ApiAtCmd_GetUserNum()-1)||PersonalCallingNum%poc_get_once_group_and_user_num==poc_get_once_group_and_user_num-1)
        {
          //ApiPocCmd_WritCommand(PocComm_UserListInfo,0,0);
          api_lcd_pwr_on_hint("获取中...       ");//清屏
          huo_qu_zhong_flag=TRUE;
        }
        else
        {
          VOICE_SetOutput(ATVOICE_FreePlay,ApiAtCmd_GetUserName(PersonalCallingNum%poc_get_once_group_and_user_num),ApiAtCmd_GetUserNameLen(PersonalCallingNum%poc_get_once_group_and_user_num));//播报按上键之后对应的用户名
          api_lcd_pwr_on_hint4("                ");//清屏
          if(UnicodeForGbk_AllUserName_english_flag(PersonalCallingNum%poc_get_once_group_and_user_num)==TRUE)
          {
            api_lcd_pwr_on_hint(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
          else
          {
            api_lcd_pwr_on_hint4(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
        }
        UpDownSwitching_Flag=TRUE;
        UpDownSwitchingCount=0;
      }
      else if(ApiMenu_GpsInfo_Flag==1||ApiMenu_NativeInfo_Flag==1)//如果是GPS信息、本机信息、北斗写频切换二级菜单
      {}
      else
      {
        MenuModeCount=MenuModeCount-1;
        if(MenuModeCount<1) {MenuModeCount=8;}
        MenuDisplay(MenuModeCount);
      }
    }
    else
    {
    if(Key_PersonalCalling_Flag==1)//如果按下个呼键
    {
      if(GettheOnlineMembersDone==TRUE)
      {
        KeyPersonalCallingCount--;
        PersonalCallingNum=KeyPersonalCallingCount;//个呼计数从零开始
        if(PersonalCallingNum<0)
        {
          PersonalCallingNum=ApiAtCmd_GetUserNum()-1;
          KeyPersonalCallingCount=ApiAtCmd_GetUserNum()-1;
        }
        
        if((PersonalCallingNum==ApiAtCmd_GetUserNum()-1)||PersonalCallingNum%poc_get_once_group_and_user_num==poc_get_once_group_and_user_num-1)
        {
          //ApiPocCmd_WritCommand(PocComm_UserListInfo,0,0);
          api_lcd_pwr_on_hint("获取中...       ");//清屏
          huo_qu_zhong_flag=TRUE;
        }
        else
        {
          VOICE_SetOutput(ATVOICE_FreePlay,ApiAtCmd_GetUserName(PersonalCallingNum%poc_get_once_group_and_user_num),ApiAtCmd_GetUserNameLen(PersonalCallingNum%poc_get_once_group_and_user_num));//播报按上键之后对应的用户名
          api_lcd_pwr_on_hint4("                ");//清屏
          if(UnicodeForGbk_AllUserName_english_flag(PersonalCallingNum%poc_get_once_group_and_user_num)==TRUE)
          {
            api_lcd_pwr_on_hint(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
          else
          {
            api_lcd_pwr_on_hint4(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
        }
        UpDownSwitching_Flag=TRUE;
        UpDownSwitchingCount=0;
        /*api_lcd_pwr_on_hint("对象:   个呼选择");
        api_lcd_pwr_on_hint2(HexToChar_PersonalCallingNum());*/
        KeyDownUpChoose_GroupOrUser_Flag=2;
      }
    }
    else
    {
      KeyUpDownCount--;
      GroupCallingNum=ApiAtCmd_GetMainGroupId()+KeyUpDownCount;
      if(GroupCallingNum<=0)
      {
        GroupCallingNum=ApiAtCmd_GetGroupNum();
        KeyUpDownCount=ApiAtCmd_GetGroupNum()-ApiAtCmd_GetMainGroupId();//
      }
      //api_disp_icoid_output( eICO_IDLOCKED, TRUE, TRUE);//选
      VOICE_SetOutput(ATVOICE_FreePlay,ApiAtCmd_GetGroupName( GroupCallingNum),ApiAtCmd_GetGroupNameLen(GroupCallingNum));
      api_lcd_pwr_on_hint4("                ");//清屏
      api_lcd_pwr_on_hint4(UnicodeForGbk_AllGrounpName(GroupCallingNum));//显示当前选中的群组名
      
      UpDownSwitching_Flag=TRUE;
      UpDownSwitchingCount=0;
      //api_lcd_pwr_on_hint("群组:   选择群组");//显示汉字
      //api_lcd_pwr_on_hint2(HexToChar_GroupCallingNum());//显示数据
      KeyDownUpChoose_GroupOrUser_Flag=1;
    }
    }
    Key_Flag_1=1;
    }
    //api_lcd_pwr_on_hint("欧标按键:Down  ");
    break;  
  case 0x00000010://ok
    if(LockingState_Flag==TRUE)
    {
      MenuDisplay(Menu_UnlockStep1_Ok);
      LockingState_EnterOK_Flag=TRUE;
    }
    else
    { 
      if(ApiPocCmd_PersonalCallingMode==TRUE)//解决按个呼键后同时按下键和OK出现死机情况
      {
        if(KeyDownUpChoose_GroupOrUser_Flag!=0)//如果处于换组或者选择个呼状态
        {
          switch(KeyDownUpChoose_GroupOrUser_Flag)
          {
          case 0://默认PTT状态
          break;
          case 1://=1，进入某群组
          break;
          case 2://=2,呼叫某用户
            if(GettheOnlineMembersDone==TRUE)
            {
              //GettheOnlineMembersDone=FALSE;
              VOICE_SetOutput(ATVOICE_FreePlay,"f25d09902d4e",12);//播报已选中
              UpDownSwitchingCount=0;//解决选中单呼后切换群组，语音中断的问题
              Delay_100ms(6);//无影响
              ApiPocCmd_WritCommand(PocComm_Invite,"0000000101",strlen((char const *)"0000000101"));
              KeyDownUpChoose_GroupOrUser_Flag=0;
              TASK_Ptt_StartPersonCalling_Flag=TRUE;//判断主动单呼状态（0a）
              EnterKeyTimeCount=0;
            }
            break;
          case 3:
            break;
          default:
            break;
          }
        }
      }
      else
      {
      NumberKeyboardPressDown_flag=TRUE;
      if(KeyDownUpChoose_GroupOrUser_Flag!=0)//如果处于换组或者选择个呼状态
      {
        switch(KeyDownUpChoose_GroupOrUser_Flag)
        {
        case 0://默认PTT状态
          break;
        case 1://=1，进入某群组
          VOICE_SetOutput(ATVOICE_FreePlay,"f25d09902d4e",12);//播报已选中
          UpDownSwitchingCount=0;//解决选中单呼后切换群组，语音中断的问题
          Delay_100ms(4);//DEL_SetTimer(0,40);
          //while(1){if(DEL_GetTimer(0) == TRUE) {break;}}
          ApiPocCmd_WritCommand(PocComm_EnterGroup,"0000000101",strlen((char const *)"0000000101"));
          KeyDownUpChoose_GroupOrUser_Flag=0;
          EnterKeyTimeCount=0;
          KeyUpDownCount=0;
          break;
        case 2://=2,呼叫某用户
          if(GettheOnlineMembersDone==TRUE)
          {
            //GettheOnlineMembersDone=FALSE;
            VOICE_SetOutput(ATVOICE_FreePlay,"f25d09902d4e",12);//播报已选中
            UpDownSwitchingCount=0;//解决选中单呼后切换群组，语音中断的问题
            Delay_100ms(6);//DEL_SetTimer(0,60);
            //while(1){if(DEL_GetTimer(0) == TRUE) {break;}}
            ApiPocCmd_WritCommand(PocComm_Invite,"0000000101",strlen((char const *)"0000000101"));
            KeyDownUpChoose_GroupOrUser_Flag=0;
            TASK_Ptt_StartPersonCalling_Flag=TRUE;//判断主动单呼状态（0a）
            EnterKeyTimeCount=0;
            
          }
          break;
        case 3:
          break;
        default:
          break;
        }
      }
      else//否则就进入菜单模式
      {
        switch(MenuModeCount)//默认按ok键进入一级菜单
        {
        case 1://群组选择
          Key_PersonalCalling_Flag=0;//进入组呼标志位
          switch(ApiMenu_SwitchGroup_Flag)
          {
          case 0://默认模式按OK键进入一级菜单
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            ApiMenu_SwitchGroup_Flag=1;
            TheMenuLayer_Flag=1;//处于一级菜单
            break;
          case 1://一级菜单再按ok键默认模式
            SubmenuMenuDisplay(GroupSwitch);
            VOICE_SetOutput(ATVOICE_FreePlay,"07526263A47FC47E",16);//切换群组
            ApiMenu_SwitchGroup_Flag=0;
            TheMenuLayer_Flag=0;//处于0级菜单，进入换组为菜单外功能
            MenuMode_Flag=0;
            break;
          }

          break;
        case 2://成员选择
          switch(ApiMenu_SwitchCallUser_Flag)
          {
          case 1://默认菜单按OK键进入一级菜单
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            ApiMenu_SwitchCallUser_Flag=0;
            TheMenuLayer_Flag=1;//处于一级菜单
            break;
          case 0://一级菜单按ok键进入单呼模式
            MenuDisplay(Menu_RefreshAllIco);
            ApiMenu_SwitchCallUser_Flag=1;
            MenuMode_Flag=0;
            /*******直接搬个呼键状态检测的程序***************************************************************************************************************************************/
            if(POC_EnterPersonalCalling_Flag==1)//解决被呼状态下，按个呼键无效（应该是被呼状态下，让个呼键失效）
            {
              VOICE_SetOutput(ATVOICE_FreePlay,"ab887c542d4e",12);//个呼中
              Delay_100ms(5);//无影响
              api_lcd_pwr_on_hint("                ");//清屏
              
              if(UnicodeForGbk_MainUserName_english_flag()==TRUE)
              {
                api_lcd_pwr_on_hint(UnicodeForGbk_MainUserName());//显示当前用户昵称
              }
              else
              {
                api_lcd_pwr_on_hint4(UnicodeForGbk_MainUserName());//显示当前用户昵称
              }
              
            }
            else
            {
              api_lcd_pwr_on_hint("    单呼模式    ");
              PersonalCallingNum=0;//解决按单呼键直接选中，单呼用户并不是播报的用户
              Key_PersonalCalling_Flag=1;
              VOICE_SetOutput(ATVOICE_FreePlay,"C5627C54216A0F5F",16);//单呼模式
              Delay_100ms(6);//无影响
              get_online_user_list_num_for_key_flag=TRUE;
              KeyDownUpChoose_GroupOrUser_Flag=2;
              TheMenuLayer_Flag=0;//处于0级菜单，进入单呼模式为菜单外功能
              KeyPersonalCallingCount=0;//解决单呼模式，上下键成员非正常顺序，第一个成员在切换时会第二、第三个碰到
            }
            break;
          }
          break;
        case 3://在线成员列表
          switch(ApiMenu_SwitchOnlineUser_Flag)
          {
          case 2:
            ApiMenu_SwitchOnlineUser_Flag=0;
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            break;
          case 1://默认菜单按OK键进入一级菜单
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            ApiMenu_SwitchOnlineUser_Flag=2;
            TheMenuLayer_Flag=1;//处于一级菜单
            break;
          case 0://一级菜单按ok键进入单呼模式
            ApiMenu_SwitchOnlineUser_Flag=1;
            MenuMode_Flag=1;
            /*******直接搬个呼键状态检测的程序***************************************************************************************************************************************/
            api_lcd_pwr_on_hint3("在线成员数:     ");
            api_lcd_pwr_on_hint("                ");
            get_online_user_list_num_flag=TRUE;
            get_online_user_list_num_for_key_flag=TRUE;
            break;
          }
          break;
        case 4://GPS设置
              switch(ApiMenu_GpsInfo_Flag)
              {
               //客户要求GPS经纬度及本机信息界面按上下键和OK键无效，只有按退出键退出，故屏蔽以下
              /*case 1://二级菜单按OK键进入一级菜单
                MenuDisplay(MenuModeCount);
                MenuMode_Flag=1;
                ApiMenu_GpsInfo_Flag=0;
                TheMenuLayer_Flag=1;//处于一级菜单
                break;*/
              case 0://一级菜单按ok键进入二级菜单
                SubmenuMenuDisplay(GpsInfoMenu);
                ApiMenu_GpsInfo_Flag=1;
                TheMenuLayer_Flag=2;//处于二级菜单
                break;
              }
          break;
        case 5://背光灯设置
              switch(ApiMenu_BacklightTimeSet_Flag)
              {
              case 2:
                ApiMenu_BacklightTimeSet_Flag=0;
                MenuDisplay(MenuModeCount);
                MenuMode_Flag=1;
                break;
              case 0://在一级菜单按ok键进入二级菜单
                ApiMenu_BacklightTimeSet_Flag=1;//在上下键中处理
                SubmenuMenuDisplay(BacklightTimeSet);
                TheMenuLayer_Flag=2;//处于二级菜单
                break;
              case 1://二级菜单按ok键进入一级菜单
                ApiMenu_BacklightTimeSet_Flag=2;
                MenuDisplay(MenuModeCount);
                MenuMode_Flag=1;
                TheMenuLayer_Flag=1;//处于一级菜单
                break;
              }
          break;
        case 6://键盘锁定
              switch(ApiMenu_KeylockTimeSet_Flag)
              {
              case 2://默认状态按OK键进入一级菜单
                ApiMenu_KeylockTimeSet_Flag=0;
                MenuDisplay(MenuModeCount);
                MenuMode_Flag=1;
                break;
              case 0://在一级菜单按ok键进入二级菜单
                ApiMenu_KeylockTimeSet_Flag=1;//在上下键中处理
                SubmenuMenuDisplay(KeylockTimeSet);
                TheMenuLayer_Flag=2;//处于二级菜单
                break;
              case 1:
                ApiMenu_KeylockTimeSet_Flag=2;
                MenuDisplay(MenuModeCount);
                MenuMode_Flag=1;
                TheMenuLayer_Flag=1;//处于一级菜单
                break;
              }
          break;
        case 7://本机信息
              switch(ApiMenu_NativeInfo_Flag)
              {
                //客户要求GPS经纬度及本机信息界面按上下键和OK键无效，只有按退出键退出，故屏蔽以下
              /*case 1://默认状态按OK键进入一级菜单
                MenuDisplay(MenuModeCount);
                MenuMode_Flag=1;
                ApiMenu_NativeInfo_Flag=0;
                TheMenuLayer_Flag=1;//处于一级菜单
                break;*/
              case 0://在gps信息一级菜单按ok键进入二级菜单
                SubmenuMenuDisplay(NativeInfoMenu);
                ApiMenu_NativeInfo_Flag=1;
                TheMenuLayer_Flag=2;//处于二级菜单
                break;
              }
          break;
        case 8://北斗/写频切换
          switch(ApiMenu_BeiDouOrWritingFrequency_Flag)
          {
          case 2://默认状态按OK键进入一级菜单
            ApiMenu_BeiDouOrWritingFrequency_Flag=0;
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            break;
          case 0://在一级菜单按ok键进入二级菜单
            ApiMenu_BeiDouOrWritingFrequency_Flag=1;//在上下键中处理
            SubmenuMenuDisplay(BeiDouOrWritingFrequencySwitch);
            TheMenuLayer_Flag=2;//处于二级菜单
            break;
          case 1://二级菜单确认返回一级菜单
            ApiMenu_BeiDouOrWritingFrequency_Flag=2;
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            TheMenuLayer_Flag=1;//处于一级菜单
            break;
          }
          break;
        default:
          break;
        }
    }
    }
    }
    Key_Flag_1=1;//按键延时标志位
    break;
  case 0x00800000://menu
    NumberKeyboardPressDown_flag=TRUE;
    if(LockingState_Flag==TRUE)
    {}
    else
    {
      KEY_4_Flag=TRUE;
    if(AkeyvolumeCount==7)
    {
      
      VOICE_SetOutput(ATVOICE_FreePlay,"2c54527b216a0f5f",16);//听筒模式
      if(MenuMode_Flag==0)
      {
        api_disp_icoid_output( eICO_IDMONITER, TRUE, TRUE);//听筒模式图标
      }
      VoiceType_FreehandOrHandset_Flag=1;
      api_disp_all_screen_refresh();// 全屏统一刷新
      Delay_100ms(3);//DEL_SetTimer(0,30);
      //while(1){if(DEL_GetTimer(0) == TRUE) {break;}}
      NoUseNum=ApiAtCmd_WritCommand(ATCOMM7_VGR,(u8 *)ucVGR1,strlen((char const *)ucVGR1));//
      AkeyvolumeCount=1;
    }
    else
    {
      if(AkeyvolumeCount==1)
      {
        VOICE_SetOutput(ATVOICE_FreePlay,"4d51d063216a0f5f",16);//免提模式
        if(MenuMode_Flag==0)
        {
          api_disp_icoid_output( eICO_IDTemper, TRUE, TRUE);//免提模式图标          
        }
        VoiceType_FreehandOrHandset_Flag=0;
        api_disp_all_screen_refresh();// 全屏统一刷新
        Delay_100ms(3);//DEL_SetTimer(0,30);
        //while(1){if(DEL_GetTimer(0) == TRUE) {break;}}
        NoUseNum=ApiAtCmd_WritCommand(ATCOMM7_VGR,(u8 *)ucVGR7,strlen((char const *)ucVGR7));//
        AkeyvolumeCount=7;
      }
    }
    }

    break;   
  case 0x00000004://2
    NumberKeyboardPressDown_flag=TRUE;
    if(LockingState_Flag==TRUE)
    {}
    else
    {
      if(xinhao_test_flag==0x00)//按294进入测试信号模式
      {
        xinhao_test_flag=0x02;
      }
    }
    break;  
  case 0x00080000://*
    TestNum8++;
    if(TestNum8>=KeyCountNum)
    {
      TestNum8=0;
    }
    break;  
  case 0x00000100://5
    TestNum9++;
    if(TestNum9>=KeyCountNum)
    {
      TestNum9=0;
    NumberKeyboardPressDown_flag=TRUE;
    }
    break;  
  case 0x00100000://0
    TestNum10++;
    if(TestNum10>=KeyCountNum)
    {
      TestNum10=0;
    NumberKeyboardPressDown_flag=TRUE;
    }
    break;  
  case 0x00004000://8
    TestNum11++;
    if(TestNum11>=KeyCountNum)
    {
      TestNum11=0;
    NumberKeyboardPressDown_flag=TRUE;
    }
    break;  
  case 0x00200000://#
    TestNum12++;
    if(TestNum12>=KeyCountNum)
    {
      if(LockingState_EnterOK_Flag==TRUE)//如果锁定状态下按了OK键然后按了#键
      {
        LockingState_EnterOK_Flag=FALSE;
        LockingState_Flag=FALSE;
        TimeCount=0;
        TimeCount3=0;//解决锁屏键偶尔失效的问题
        MenuDisplay(Menu_unLocking);
        api_disp_icoid_output(eICO_IDBANDWIDTHN, TRUE, TRUE);//无锁屏空图标
        api_disp_all_screen_refresh();// 全屏统一刷新
      }
      else
      {
        NumberKeyboardPressDown_flag=TRUE;
        if(xinhao_test_flag==0x04)//按294进入测试信号模式
        {
          xinhao_test_flag=0xFF;
        }
      }
      TestNum12=0;
    }
    break;  
  case 0x00000400://up
    NumberKeyboardPressDown_flag=TRUE;
    if(LockingState_Flag==TRUE)
    {}
    else
    {
      if(MenuMode_Flag==1)
      {
        if(ApiMenu_BacklightTimeSet_Flag==1)//如果是设置背光灯二级菜单
        {
          BacklightTimeSetCount=BacklightTimeSetCount+1;
          if(BacklightTimeSetCount>7) {BacklightTimeSetCount=1;}
          Level3MenuDisplay(BacklightTimeSetCount);
        }
        else if(ApiMenu_KeylockTimeSet_Flag==1)//如果是设置键盘锁二级菜单
        {
          KeylockTimeSetCount=KeylockTimeSetCount+1;
          if(KeylockTimeSetCount>0x16) {KeylockTimeSetCount=0x10;}
          Level3MenuDisplay(KeylockTimeSetCount);
        }
        else if(ApiMenu_BeiDouOrWritingFrequency_Flag==1)//如果是设置网络模式
        {
          if(network_mode_2gor3g_SetCount==0x17)
          {
            network_mode_2gor3g_SetCount=0x18;
          }
          else
          {
            network_mode_2gor3g_SetCount=0x17;
          }
          Level3MenuDisplay(network_mode_2gor3g_SetCount);
        }
        else if(ApiMenu_SwitchOnlineUser_Flag==1)//如果是显示在线用户名二级菜单---------------------------------------------------------------
        {
          KeyPersonalCallingCount++;
          PersonalCallingNum=KeyPersonalCallingCount;//个呼计数从零开始
          if(PersonalCallingNum>ApiAtCmd_GetUserNum()-1)
          {
            KeyPersonalCallingCount=0;
            PersonalCallingNum=0;
          }
          
        if(PersonalCallingNum%poc_get_once_group_and_user_num==0)
        {
          //ApiPocCmd_WritCommand(PocComm_UserListInfo,0,0);
          api_lcd_pwr_on_hint("获取中...       ");//清屏
          huo_qu_zhong_flag=TRUE;
        }
        else
        {
          VOICE_SetOutput(ATVOICE_FreePlay,ApiAtCmd_GetUserName(PersonalCallingNum%poc_get_once_group_and_user_num),ApiAtCmd_GetUserNameLen(PersonalCallingNum%poc_get_once_group_and_user_num));//播报按上键之后对应的用户名
          api_lcd_pwr_on_hint4("                ");//清屏
          if(UnicodeForGbk_AllUserName_english_flag(PersonalCallingNum%poc_get_once_group_and_user_num)==TRUE)
          {
            api_lcd_pwr_on_hint(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
          else
          {
            api_lcd_pwr_on_hint4(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
        }

          UpDownSwitching_Flag=TRUE;
          UpDownSwitchingCount=0;
        }
        else if(ApiMenu_GpsInfo_Flag==1||ApiMenu_NativeInfo_Flag==1)//如果是GPS信息、本机信息、北斗写频切换二级菜单
        {}
        else
        {
          MenuModeCount=MenuModeCount+1;
          if(MenuModeCount>8) {MenuModeCount=1;}
          MenuDisplay(MenuModeCount);
        }
      }
      else
      {
        if(Key_PersonalCalling_Flag==1)//如果按下个呼键
        {
          if(GettheOnlineMembersDone==TRUE)
          {
            KeyPersonalCallingCount++;
            PersonalCallingNum=KeyPersonalCallingCount;//个呼计数从零开始
            if(PersonalCallingNum>ApiAtCmd_GetUserNum()-1)
            {
              KeyPersonalCallingCount=0;
              PersonalCallingNum=0;
            }
        if(PersonalCallingNum%poc_get_once_group_and_user_num==0)
        {
          //ApiPocCmd_WritCommand(PocComm_UserListInfo,0,0);
          api_lcd_pwr_on_hint("获取中...       ");//清屏
          huo_qu_zhong_flag=TRUE;
        }
        else
        {
          VOICE_SetOutput(ATVOICE_FreePlay,ApiAtCmd_GetUserName(PersonalCallingNum%poc_get_once_group_and_user_num),ApiAtCmd_GetUserNameLen(PersonalCallingNum%poc_get_once_group_and_user_num));//播报按上键之后对应的用户名
          api_lcd_pwr_on_hint4("                ");//清屏
          if(UnicodeForGbk_AllUserName_english_flag(PersonalCallingNum%poc_get_once_group_and_user_num)==TRUE)
          {
            api_lcd_pwr_on_hint(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
          else
          {
            api_lcd_pwr_on_hint4(UnicodeForGbk_AllUserName(PersonalCallingNum%poc_get_once_group_and_user_num));//显示当前选中的群组名
          }
        }
            UpDownSwitching_Flag=TRUE;
            UpDownSwitchingCount=0;
            /*api_lcd_pwr_on_hint("对象:   个呼选择");
            api_lcd_pwr_on_hint2(HexToChar_PersonalCallingNum());*/
            KeyDownUpChoose_GroupOrUser_Flag=2;
          }
        }
        else
        {
          KeyUpDownCount++;
          GroupCallingNum=ApiAtCmd_GetMainGroupId()+KeyUpDownCount;
          if(GroupCallingNum>ApiAtCmd_GetGroupNum())
          {
            GroupCallingNum=1;
            KeyUpDownCount=1-ApiAtCmd_GetMainGroupId();
          }
          //api_disp_icoid_output( eICO_IDLOCKED, TRUE, TRUE);//选
          VOICE_SetOutput(ATVOICE_FreePlay,ApiAtCmd_GetGroupName(GroupCallingNum),ApiAtCmd_GetGroupNameLen(GroupCallingNum));
          api_lcd_pwr_on_hint4("                ");//清屏
          api_lcd_pwr_on_hint4(UnicodeForGbk_AllGrounpName(GroupCallingNum));//显示当前选中的群组名
          UpDownSwitching_Flag=TRUE;
          UpDownSwitchingCount=0;
          //api_lcd_pwr_on_hint("群组:   群组选择");//显示汉字
          //api_lcd_pwr_on_hint2(HexToChar_GroupCallingNum());//显示数据
          KeyDownUpChoose_GroupOrUser_Flag=1;
        }
      }
      Key_Flag_1=1;
    }
   
    break;
  case 0x00400000://cancel
    if(xinhao_test_flag==0xFF)
    {
      api_lcd_pwr_on_hint("                ");
      api_lcd_pwr_on_hint3("                ");
      if(NetworkType_2Gor3G_Flag==3)
        api_disp_icoid_output( eICO_IDEmergency, TRUE, TRUE);//3G图标
      else
        api_disp_icoid_output( eICO_IDPOWERL, TRUE, TRUE);//图标：2G
      if(VoiceType_FreehandOrHandset_Flag==0)
        api_disp_icoid_output( eICO_IDTemper, TRUE, TRUE);//免提模式
      else
        api_disp_icoid_output( eICO_IDMONITER, TRUE, TRUE);//听筒模式图标
      if(PersonCallIco_Flag==0)
        api_disp_icoid_output( eICO_IDPOWERM, TRUE, TRUE);//显示组呼图标
      else
        api_disp_icoid_output( eICO_IDPOWERH, TRUE, TRUE);//显示个呼图标
      if(KeyDownUpChoose_GroupOrUser_Flag==0)
        api_disp_icoid_output( eICO_IDMESSAGEOff, TRUE, TRUE);//空图标-与选对应
      else
        api_disp_icoid_output( eICO_IDLOCKED, TRUE, TRUE);//选
    }
    xinhao_test_flag=0x00;
    NumberKeyboardPressDown_flag=TRUE;
    if(LockingState_Flag==TRUE)
    {}
    else
    {
      if(MenuMode_Flag==1)
      {
        if(TheMenuLayer_Flag==2)//二级菜单按返回键返回1级菜单
        {
          if(ApiMenu_GpsInfo_Flag==1)
          {
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            ApiMenu_GpsInfo_Flag=0;
            TheMenuLayer_Flag=1;//处于一级菜单
          }
          else if(ApiMenu_SwitchOnlineUser_Flag==1)
          {
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            ApiMenu_SwitchOnlineUser_Flag=0;
            TheMenuLayer_Flag=1;//处于一级菜单
          }
          else if(ApiMenu_NativeInfo_Flag==1)
          {
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            ApiMenu_NativeInfo_Flag=0;
            TheMenuLayer_Flag=1;//处于一级菜单
          }
          else if(ApiMenu_BeiDouOrWritingFrequency_Flag==1)
          {
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            ApiMenu_BeiDouOrWritingFrequency_Flag=0;
            TheMenuLayer_Flag=1;//处于一级菜单
          }
          else if(ApiMenu_BacklightTimeSet_Flag==1)
          {
            ApiMenu_BacklightTimeSet_Flag=0;
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            TheMenuLayer_Flag=1;//处于一级菜单
          }
          else if(ApiMenu_KeylockTimeSet_Flag==1)
          {
            ApiMenu_KeylockTimeSet_Flag=0;
            MenuDisplay(MenuModeCount);
            MenuMode_Flag=1;
            TheMenuLayer_Flag=1;//处于一级菜单
          }
          else
          {}
        }
        else if(TheMenuLayer_Flag==1)
        {
#if 1
          MenuDisplay(Menu_RefreshAllIco);
          api_lcd_pwr_on_hint("                ");//清屏
          //api_lcd_pwr_on_hint(HexToChar_MainGroupId());//显示当前群组ID
          api_lcd_pwr_on_hint4(UnicodeForGbk_MainWorkName());//显示当前群组昵称
          MenuModeCount=1;
          TheMenuLayer_Flag=0;
          MenuMode_Flag=0;
          ApiMenu_SwitchGroup_Flag=0;
          ApiMenu_SwitchCallUser_Flag=0;
          ApiMenu_SwitchOnlineUser_Flag=0;
          ApiMenu_GpsInfo_Flag=0;
          ApiMenu_BacklightTimeSet_Flag=0;
          ApiMenu_KeylockTimeSet_Flag=0;
          ApiMenu_NativeInfo_Flag=0;
          ApiMenu_BeiDouOrWritingFrequency_Flag=0;
#else
          switch(MenuModeCount)//默认按ok键进入一级菜单
          {
          case 1://群组选择
            if(ApiMenu_SwitchGroup_Flag==1)//以下为一级菜单按返回键进入默认界面
            {
              TheMenuLayer_Flag=0;//处于一级菜单
              MenuMode_Flag=0;
              ApiMenu_SwitchGroup_Flag=0;
              MenuDisplay(Menu_RefreshAllIco);
              api_lcd_pwr_on_hint("                ");//清屏
              api_lcd_pwr_on_hint(HexToChar_MainGroupId());//显示当前群组ID
              api_lcd_pwr_on_hint4(UnicodeForGbk_MainWorkName());//显示当前群组昵称
            }
            break;
          case 2://成员选择
            if(ApiMenu_SwitchCallUser_Flag==0)//以下为一级菜单按返回键进入默认界面
            {
              TheMenuLayer_Flag=0;//处于一级菜单
              MenuMode_Flag=0;
              ApiMenu_SwitchCallUser_Flag=1;
              MenuDisplay(Menu_RefreshAllIco);
              api_lcd_pwr_on_hint("                ");//清屏
              api_lcd_pwr_on_hint(HexToChar_MainGroupId());//显示当前群组ID
              api_lcd_pwr_on_hint4(UnicodeForGbk_MainWorkName());//显示当前群组昵称
            }
            break;
          case 3://GPS信息
            if(ApiMenu_GpsInfo_Flag==0)//以下为一级菜单按返回键进入默认界面
            {
              TheMenuLayer_Flag=0;//处于一级菜单
              MenuMode_Flag=0;
              ApiMenu_GpsInfo_Flag=1;
              MenuDisplay(Menu_RefreshAllIco);
              api_lcd_pwr_on_hint("                ");//清屏
              api_lcd_pwr_on_hint(HexToChar_MainGroupId());//显示当前群组ID
              api_lcd_pwr_on_hint4(UnicodeForGbk_MainWorkName());//显示当前群组昵称
            }
            break;
          case 4://背光灯设置
            if(ApiMenu_BacklightTimeSet_Flag==0)
            {
              TheMenuLayer_Flag=0;//处于一级菜单
              MenuMode_Flag=0;
              ApiMenu_BacklightTimeSet_Flag=1;
              MenuDisplay(Menu_RefreshAllIco);
              api_lcd_pwr_on_hint("                ");//清屏
              api_lcd_pwr_on_hint(HexToChar_MainGroupId());//显示当前群组ID
              api_lcd_pwr_on_hint4(UnicodeForGbk_MainWorkName());//显示当前群组昵称
            }
            break;
          case 5://键盘锁定
            if(ApiMenu_KeylockTimeSet_Flag==0)
            {
              TheMenuLayer_Flag=0;//处于一级菜单
              MenuMode_Flag=0;
              ApiMenu_KeylockTimeSet_Flag=1;
              MenuDisplay(Menu_RefreshAllIco);
              api_lcd_pwr_on_hint("                ");//清屏
              api_lcd_pwr_on_hint(HexToChar_MainGroupId());//显示当前群组ID
              api_lcd_pwr_on_hint4(UnicodeForGbk_MainWorkName());//显示当前群组昵称
            }
            break;
          case 6://本机信息
            if(ApiMenu_NativeInfo_Flag==0)
            {
              TheMenuLayer_Flag=0;//处于一级菜单
              MenuMode_Flag=0;
              ApiMenu_NativeInfo_Flag=1;
              MenuDisplay(Menu_RefreshAllIco);
              api_lcd_pwr_on_hint("                ");//清屏
              api_lcd_pwr_on_hint(HexToChar_MainGroupId());//显示当前群组ID
              api_lcd_pwr_on_hint4(UnicodeForGbk_MainWorkName());//显示当前群组昵称
            }
            break;
          case 7://北斗/写频切换
            if(ApiMenu_BeiDouOrWritingFrequency_Flag==0)
            {
              TheMenuLayer_Flag=0;//处于一级菜单
              MenuMode_Flag=0;
              ApiMenu_BeiDouOrWritingFrequency_Flag=1;
              MenuDisplay(Menu_RefreshAllIco);
              api_lcd_pwr_on_hint("                ");//清屏
              api_lcd_pwr_on_hint(HexToChar_MainGroupId());//显示当前群组ID
              api_lcd_pwr_on_hint4(UnicodeForGbk_MainWorkName());//显示当前群组昵称
            }
            break;
          }
#endif
        }
        else
        {}
        Key_Flag_1=1;//按键延时标志位
      }
      else
      {
        if(Key_PersonalCalling_Flag==1||POC_EnterPersonalCalling_Flag==1||POC_AtEnterPersonalCalling_Flag==1)//如果处于单呼模式，按返回键进入组呼
        {
          ApiPocCmd_WritCommand(PocComm_Cancel,(u8 *)ucQuitPersonalCalling,strlen((char const *)ucQuitPersonalCalling));
          Key_Flag_1=1;//按键延时标志位
          Key_PersonalCalling_Flag=0;//进入组呼标志位
          KeyDownUpChoose_GroupOrUser_Flag=0;//解决（个呼键→返回键→OK或PTT）屏幕显示错误的BUG
          huo_qu_zhong_flag=FALSE;
        }
        else//如果处于组呼模式则应该无变化
        {
          MenuMode_Flag=0;
          if(ShowTime_Flag==FALSE)
          {
            api_disp_icoid_output(eICO_IDMESSAGEOff, TRUE, TRUE);//空图标-与选对应
          }
          api_lcd_pwr_on_hint("                ");//清屏
          //api_lcd_pwr_on_hint(HexToChar_MainGroupId());//显示当前群组ID
          api_lcd_pwr_on_hint4(UnicodeForGbk_MainWorkName());//显示当前群组昵称
          Key_Flag_1=1;//按键延时标志位
          //用于PTT键及上下键返回默认状态
          KeyDownUpChoose_GroupOrUser_Flag=0;
          KeyUpDownCount=0;
        }
        key_warning_flag=FALSE;//按返回键报警标志位清零
        key_warning_bubiao_flag=FALSE;//按返回键部标报警标志位清零
      }

    }
    break;  
  default:
    break;
  }
  GeHuTest(ulAllKeyID);
  if(Key_Flag_1==1)//按下按键延迟1秒
  {
    Delay_100ms(1);//DEL_SetTimer(1,30);//influence get group name
    Key_Flag_1=0;
    //if(KeyDownUpChoose_GroupOrUser_Flag==2)
    {
      if(huo_qu_zhong_flag==TRUE)
      {
        ApiPocCmd_WritCommand(PocComm_UserListInfo,0,0);
      }
    }
    if(get_online_user_list_num_for_key_flag==TRUE)
    {
      ApiPocCmd_WritCommand(PocComm_UserListInfo,0,0);
      get_online_user_list_num_for_key_flag=FALSE;
    }
  }
}



u32 get_key_value(u8 scan_value)
{
  u32 value = 0;
  switch (scan_value)
  {
  case 0x11://"7"
    value = 0x00002000;
    break;
  case 0x12://"4"
    value = 0x00000080;
    break;
  case 0x13://"1"
    value = 0x00000002;
    break;
  case 0x14://OK
    value = 0x00000010;
    break;
  case 0x21://"8"
    value = 0x00004000;
    break;
  case 0x22://"5"
    value = 0x00000100;
    break;
  case 0x23://"2"
    value = 0x00000004;
    break;
  case 0x24://"DN"
    value = 0x00010000;
    break;
  case 0x31://"9"
    value = 0x00008000;
    break;
  case 0x32://"6"
    value = 0x00000200;
    break;
  case 0x33://"3"
    value = 0x00000008;
    break;
  case 0x34://"UP"
    value = 0x00000400;
    break;
  case 0x41://"#"
    value = 0x00200000;
    break;
  case 0x42://"0"
    value = 0x00100000;
    break;
  case 0x43://"*"
    value = 0x00080000;
    break;
  case 0x44://"Cancel"
    value = 0x00400000;
    break;
  case 0x54://"Menu"
    value = 0x00800000;
    break;
  default:
    break;
  }
  return value;
}

void GeHuTest(u32 KeyID)
{
#if 0
   switch(KeyID)
  {
  case 0x00000002://1√
    TestNum1++;
    if(TestNum1>=KeyCountNum)
    {
      TestNum1=0;
      num1=1;
      PressButton=TRUE;
    }
    break; 
  case 0x00000004://2√
    TestNum7++;
    if(TestNum7>=KeyCountNum)
    {
      TestNum7=0;
      num1=2;
      PressButton=TRUE;
    }
    break;  
  case 0x00000008://3√
    TestNum2++;
    if(TestNum2>=KeyCountNum)
    {
      TestNum2=0;
      num1=3;
      PressButton=TRUE;
    }
    break;
  case 0x00000080://4√
    TestNum3++;
    if(TestNum3>=KeyCountNum)
    {
      TestNum3=0;
      num1=4;
      PressButton=TRUE;
    }
    break;
  case 0x00000100://5√
    TestNum9++;
    if(TestNum9>=KeyCountNum)
    {
      TestNum9=0;
      num1=5;
      PressButton=TRUE;
    }
    break;  
  case 0x00000200://6√
    TestNum4++;
    if(TestNum4>=KeyCountNum)
    {
      TestNum4=0;
      num1=6;
      PressButton=TRUE;
    }
    break;
  case 0x00002000://7√
    TestNum5++;
    if(TestNum5>=KeyCountNum)
    {
      TestNum5=0;
      num1=7;
      PressButton=TRUE;
    }
     break;
  case 0x00004000://8√
    TestNum11++;
    if(TestNum11>=KeyCountNum)
    {
      TestNum11=0;
      num1=8;
      PressButton=TRUE;
    }
    break;  
  case 0x00008000://9√
    TestNum6++;
    if(TestNum6>=KeyCountNum)
    {
      TestNum6=0;
      num1=9;
      PressButton=TRUE;
    }
    break;
  case 0x00100000://0
    TestNum10++;
    if(TestNum10>=KeyCountNum)
    {
      TestNum10=0;
      num1=0;
      PressButton=TRUE;
    }
    break;
  case 0x00010000://dn
    break;  
  case 0x00000010://ok
    break;
  case 0x00800000://menu   
    break;   
  case 0x00080000://*
    break;  
  case 0x00200000://#
    break;  
  case 0x00000400://up
    break;
  case 0x00400000://cancel
    numCount=0;
    break;  
  default:
    if(PressButton==TRUE)
    {
      KeyBoardState=TRUE;
      PressButton  =FALSE;
    }
    else
    {
      KeyBoardState=FALSE;
    }
    break;
  }
  
if(KeyBoardState==TRUE)//识别按下按键到松开按键的过程
{
  numCount++;
  num6=num5;
  num5=num4;
  num4=num3;
  num3=num2;
  num2=num1;
    switch(numCount)
    {
    case 1:
      TestBuf1[0]=num2+0x30;
      TestBuf1[1]='\0';
      break;
    case 2:
      TestBuf1[0]=num3+0x30;
      TestBuf1[1]=num2+0x30;
      TestBuf1[2]='\0';
      break;
    case 3:
      TestBuf1[0]=num4+0x30;
      TestBuf1[1]=num3+0x30;
      TestBuf1[2]=num2+0x30;
      TestBuf1[3]='\0';
      break;
    case 4:
      TestBuf1[0]=num5+0x30;
      TestBuf1[1]=num4+0x30;
      TestBuf1[2]=num3+0x30;
      TestBuf1[3]=num2+0x30;
      TestBuf1[4]='\0';
      break;
    case 5:
      TestBuf1[0]=num6+0x30;
      TestBuf1[1]=num5+0x30;
      TestBuf1[2]=num4+0x30;
      TestBuf1[3]=num3+0x30;
      TestBuf1[4]=num2+0x30;
      TestBuf1[5]='\0';
      break;
    default:
      break;
    }
   //短号呼叫有问题，暂时屏蔽
    api_lcd_pwr_on_hint3("个呼短号        ");
     api_lcd_pwr_on_hint("                ");
     api_lcd_pwr_on_hint(TestBuf1);
 
}
  #endif

}
