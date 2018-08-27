#include "AllHead.h"
#include <string.h>
#include <stdio.h>
#include<stdlib.h>
u8 ReportTimerRead[1];
u8 ReportTimerSET=0;
u8 Key3_PlayValue=0;
u8 key_top_value=0;
u8 Test1=0;
u8 SendGpsLoginInfoCount=0;
u8 TcpIpBuf[15];
u8 TcpPortBuf[5];
GpsServerType ApiGpsServerType;
typedef enum{
	GPSREV_Ack		= 0x8001,//平台通用应答
	GPSREV_Puls		= 0x8002,//终端心跳
	GPSREV_Login		= 0x8100,//终端注册应答
	GPSREV_Logout		= 0x0101,//终端注销
	GPSREV_Authentication	= 0x0102,//终端鉴权
	GPSREV_SetParam		= 0x8103,//设置终端参数
	GPSREV_GetParam		= 0x8104,//查询终端参数
	GPSREV_Position		= 0x8201,//位置信息查询
	GPSREV_Photo		= 0x8801//摄像头立即拍照命令
}GpsRevCommType;

typedef enum{
	GPSCOMM_Ack		= 0x00,
	GPSCOMM_Puls		= 0x01,
	GPSCOMM_Login		= 0x02,
	GPSCOMM_Logout		= 0x03,
	GPSCOMM_Authentication	= 0x04,
	GPSCOMM_Position	= 0x05,
	GPSCOMM_PositionAck     = 0x06,
	GPSCOMM_PhotoAck	= 0x07,
	GPSCOMM_PhotoData	= 0x08,
	GPSCOMM_CheckTcp	= 0x09,
	GPSCOMM_SetIp		= 0x0A,
	GPSCOMM_CheckUdp	= 0x0B,
	GPSCOMM_SetUpu		= 0x0C,
	GPSCOMM_SendTcp		= 0x0D,
	GPSCOMM_SendUdp		= 0x0E,
	GPSCOMM_PlayZtts	= 0x0F,
	GPSCOMM_ZGSN		= 0x10
}GpsCommType;

#define GPS_GB_INFO_LEN			64+20//+512
#define GPS_GB_HEAD_LEN			12//消息头长度
#define GPS_GB_POSITION_LEN		28
#define GPS_GB_POSITION_ACK_LEN	28+2

typedef union{
  u8 ucData[GPS_GB_HEAD_LEN];//消息头全部数据
  struct{
    union{
      u8 ucData[2];
      u16 usData;
    }ucMsgID;//消息ID
    union{
      u8 ucData[2];
      struct{
        u16 Len		: 10;//消息体长度
        u16 Encryption	: 3;//加密
        u16 Subpackage	: 1;//分包
        u16		: 2;
      }Bits;
    }MsgProperty;//消息体属性
    union{
      u8 ucData[6];
      struct{
        u8	Num0	: 4;
        u8	Num1	: 4;
        u8	Num2	: 4;
        u8	Num3	: 4;
        u8	Num4	: 4;
        u8	Num5	: 4;
        u8	Num6	: 4;
        u8	Num7	: 4;
        u8	Num8	: 4;
        u8	Num9	: 4;
        u8	Num10	: 4;
        u8	Num11	: 4;
      }Bits;
    }ucTerminalNo;//终端手机号/设备ID
    union{
      u8 ucData[2];
      u16 usData;
    }SerialNo;//流水号
  }stParam;
}_Gb_GpsHeadInfo;//消息头

typedef union{
  u32 ucData;
  struct{
    u32 bEmergency	: 1;
    u32 b1		: 1;
    u32 b2		: 1;
    u32 bEarlyWarning	: 1;
    u32 b4		: 1;
    u32 b5		: 1;
    u32 b6		: 1;
    u32 b7		: 1;
    u32 b8		: 1;
    u32 b9		: 1;
    u32 b10		: 1;
    u32 b11		: 1;
    u32 		: 6;
    u32 b18		: 1;
    u32 b19		: 1;
    u32 b20		: 1;
    u32 b21		: 1;
    u32 b22		: 1;
    u32 b23		: 1;
    u32 b24		: 1;
    u32 b25		: 1;
    u32 b26		: 1;
    u32 bIgnitionWarn	: 1;
    u32 b28		: 1;
    u32 b29		: 1;
    u32			: 2;
  }Bits;
}_Gb_AlarmState;//位置基本信息数据格式中的报警标志

typedef union{
  u32 ucData;
  struct{
    u32 bAcc		: 1;
    u32 bGpsVolid	: 1;
    u32 bNorthOrSouth	: 1;
    u32 bEastOrWest	: 1;
    u32 bWorking	: 1;
    u32 bEncryption	: 1;
    u32 		: 4;
    u32 bOilWay		: 1;
    u32 bPowerSupply	: 1;
    u32 bLock		: 2;
    u32			: 18;
  }Bits;
}_Gb_WorkState;//位置基本信息数据格式中的状态位


typedef union{
  u8 ucData[GPS_GB_POSITION_LEN+3+16];//3为附加消息长度
  struct{
    _Gb_AlarmState Warning;//报警标志
    _Gb_WorkState WorkStatus;//定位状态
    u32 ulLatitude;//纬度
    u32 ulLongitude;//经度
    u16 usAltitude;// 高度Geoid Separation
    u16 usSpeed;//速度
    u16 usDirection;//方向
    struct{
      u8 ucData[3];
    }Date;//日期
    struct{
      u8 ucData[3];
    }Time;//时间
    struct{
      u8 AddInfoID;
      u8 AddInfoLen;
#if 1 //f3 部标平台
      u8 AddInfo[18];
#else
      u8 AddInfo;
#endif
    }AdditionPositInfo;//附加消息+3
  }stParam;
}_PositInfo;

typedef union{
  u8 ucData[50+20];
  struct{
    _Gb_GpsHeadInfo HeadInfo;//消息头
    union{
      struct{
        u16 SerialNo;
        u16 Command;
        u8	Result;
      }Ack;
      struct{
        union{
           u8 ucData[2];
           u16 usData;
        }Province;//省域ID
        union{
           u8 ucData[2];
           u16 usData;
        }County;//市县域ID
         u8 Manufacturer[5];//制造商ID
         u8 TerminalType[8];//终端型号
         u8 TerminalID[7];//终端ID
         u8 TerminalColor;//车牌颜色
         u8 LicensePlate[15];//车牌号
      }LoginInfo;
      struct{
        u8 ucData[20];
      }Authentication;
      _PositInfo PositInfo;
      struct{
        u8 ucAckNo[2];
        _PositInfo PositInfo;
      }PositInfoAck;
      struct{
        u32 MediaId;
        u8 MediaMode;
        u8 MediaFormat;
        u8 MediaEven;
        u8 Number;
      }PhotoAck;
    }Message;
  }Param;
}_Gb_MsgBody;


typedef struct{
  struct{
    union{
      struct{
        u8 Enable	: 1;
        u8 Protocal	: 2;
        u8 Servicer	: 4;
        u8 WorkMode	: 1;
      }Bits;
      u8 Byte;
    }Cfg;
    u8 PulseTime;
    u8 ID[14];
    union{
      u8 ucData[19];//+512];
      struct{
        u8 IP[15];
        u8 Port[4];
      }Param;
    }NetParam;
  }Gps;
  u8 Vehicle;
  struct{
            union{
           u8 ucData[2];
           u16 usData;
        }Province;//省域ID
        union{
           u8 ucData[2];
           u16 usData;
        }County;//市县域ID
         u8 Manufacturer[5];//制造商ID
         u8 TerminalType[8];//终端型号
         u8 TerminalID[7];//终端ID
         u8 TerminalColor;//车牌颜色
         u8 LicensePlate[15];//车牌号
  }LoginInfo;
  struct{
    u8 OnReportTime;
    u8 OffReportTime;
    u8 CountdownTimer;
    u8 Res[3];
  }Acc;
}_GpsPar;

typedef struct{
  struct{
    u8 bAcc		: 1;
    u8 bRotot		: 2;
    u8 Protocol	        : 3;
    u8			: 2;
  }Msg;
  struct{
    struct{
      u8 bGpsVolid	: 1;
      u8 bNorthOrSouth	: 1;
      u8 bEastOrWest	: 1;
      u8		: 5;
    }Msg;
    u32 ulLatitude;
    u32 ulLongitude;
    u16 usAltitude;// Geoid Separation
    u16 usSpeed;
    u16 usDirection;
    u8 ucDate[3];
    u8 ucTime[3];
  }Position;
  u8 rev[6];
}_InfoRecord;

typedef struct{
  struct{
    u8 LoadCount;
  }CountdownTimer;
  u8 ucWorkState;
  u16 usPulseTimer;
  u16 usReportTimer;
  //_GpsPar GpsPar2;
  _GpsPar GpsPar;//存放送EEPROM中读取的数
  _InfoRecord InfoRecord;
  union{
    struct{
      struct{
        union{
          struct{
            u8 bGeneralAck		: 1;//通用应答
            u8 bSpecificAck		: 1;//特殊应答
            u8 bAuthenticatied	        : 1;//认证
            u8 bLen			: 5;
          }Bits;
          u8 ucData;
        }Msg;
        u16 ulAckStep;
        u8 ucStep;
        u32 MediaId;
      }State;
      union{
        u8 ucData[21];
        struct{
          union{
            struct{
              u8 bLoginSuccess	: 1;
              u8 bLen		: 7;
            }Bits;
            u8 ucData;
          }Msg;
          u8 AuthenticationId[20];
        }ucParam;
      }LoginInfo;
      _Gb_MsgBody MsgBody;//终端注册
      struct{
        u16 Len;
        u16 TestLen;
        u8 ucData[GPS_GB_INFO_LEN];
        u8 ucTestData[2*GPS_GB_INFO_LEN];//test
      }TempBuf;//打包好的数据包（即将发送）
    }GbSys;
  }PositionSystem;
}GpsFunDrv;

static GpsFunDrv GpsFunDrvObj;
static bool GpsCmd_GbWritCommand(GpsCommType id, u8 *buf, u8 len);
static void GpsCmd_GbAnalytical(u8 *pBuf, u8 len);//收到的数据进行分析
static void GpsCmd_GbDataTransave(GpsCommType GpsComm);//定位信息转换，等会要用到，POC获取GPS信息转换后发送TCP
 
static void pack_data(u8 *pBuf, u16 ucLen);//消息包封装
static u8 COMLHexArray2String(u8 * buf1,u8 Len1,u8 * buf2);//16进制数组转字符串
void ApiGpsCmd_PowerOnInitial(void)//bubiao
{
  u16 i;
  u8 j,k;
  u8 TerminalIDLen=0;
  u8 LicensePlateLen=0;
  u8 ucIndex0 = 0, ucIndex1 = 0;
  ADRLEN_INF	adr;
  adr = CFG_GetCurAdr(ADR_IDGpsFun);//部标注册信息获取
  FILE_Read2(adr.Adr,adr.Len-16,(u8*)(&GpsFunDrvObj.GpsPar));
  FILE_Read2(adr.Adr+87,1,ReportTimerRead);//GPS上报时间位
  ReportTimerSET=(ReportTimerRead[0]-0x0e)*5-1;//当设置成5的时候，上报时间为6s，故减1
  FILE_Read(0,80,ReadBuffer);//80位
  FILE_Read(598,1,&Key3_PlayValue);//80位
  FILE_Read(602,1,&key_top_value);//顶部键类型的值，现改为报警时间设置
  
#if 1//侧键1播报语音类型
  switch(Key3_PlayValue)
  {
  case 0x00:
    Key3Option=Key3_OptionZero;
    break;
  case 0x01:
    Key3Option=Key3_OptionOne;
    break;
  case 0x02:
    Key3Option=Key3_OptionTwo;
    break;
  case 0x03:
    Key3Option=Key3_OptionThree;
    break;
  case 0x04:
    Key3Option=Key3_OptionFour;
    break;
  default:
    break;
  }
#endif
    
#if 1//获取参数决定上报哪个平台
  if(GpsFunDrvObj.GpsPar.Gps.Cfg.Byte==0x09)//部标平台
  {
    ApiGpsServerType=GpsServerType_BuBiao;
  }
  else if(GpsFunDrvObj.GpsPar.Gps.Cfg.Byte==0x11)//中兴平台
  {
    ApiGpsServerType=GpsServerType_ZTE;
  }
  else if(GpsFunDrvObj.GpsPar.Gps.Cfg.Byte==0x19)//部标&中兴平台
  {
    ApiGpsServerType=GpsServerType_BuBiaoAndZTE;
  }
  else
  {
    ApiGpsServerType=GpsServerType_ZTE;
  }
#endif
  
    GpsFunDrvObj.usReportTimer = 0;//添加：报告时间设置为0
    GpsFunDrvObj.PositionSystem.GbSys.State.ucStep=0x00;
    //Tset
    GpsFunDrvObj.PositionSystem.GbSys.State.MediaId = 0x01;
    GpsFunDrvObj.PositionSystem.GbSys.State.ulAckStep=0x00;
    GpsFunDrvObj.PositionSystem.GbSys.State.ucStep=0x00;
    GpsFunDrvObj.PositionSystem.GbSys.LoginInfo.ucParam.Msg.Bits.bLoginSuccess = OFF;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Encryption=0;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Subpackage=0;
#if 1//从EEPROM中读取设备ID
    ucIndex0 = strlen((char const *)GpsFunDrvObj.GpsPar.Gps.ID)-1;//ucIndex0为设备ID长度-1
    if(ucIndex0 > 11)
    {
      ucIndex0 = 11;
    }
    for(i = 0; i < 12; i++)
    {
      if(i < strlen((char const*)GpsFunDrvObj.GpsPar.Gps.ID))
      {
        if(i % 2 == 0x00)
        {
          GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[ucIndex1]
            =GpsFunDrvObj.GpsPar.Gps.ID[ucIndex0] - 0x30;
        }
        else
        {
          GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[ucIndex1]
            +=(GpsFunDrvObj.GpsPar.Gps.ID[ucIndex0] - 0x30) << 0x04;
          ucIndex1++;
        }
        ucIndex0--;
      }
      else//
      {
        if(i % 2 == 0x00)
        {
          GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[ucIndex1]=0;
        }
        else
        {
          GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[ucIndex1]+=0;
          ucIndex1++;
        }
      }
    }
#else
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[5]=0x00;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[4]=0x98;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[3]=0x00;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[2]=0x30;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[1]=0x53;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData[0]=0x88;
#endif
    
#if 1//客户要求需要添加终端ID及车牌号信息以及里程
    TerminalIDLen=strlen((char const *)GpsFunDrvObj.GpsPar.LoginInfo.TerminalID);
    LicensePlateLen=strlen((char const *)GpsFunDrvObj.GpsPar.LoginInfo.LicensePlate);
    for(j=0;j<TerminalIDLen;j++)
    {
      GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.TerminalID[j]=GpsFunDrvObj.GpsPar.LoginInfo.TerminalID[j];
    }
    for(k=0;k<LicensePlateLen;k++)
    {
      GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[k]=GpsFunDrvObj.GpsPar.LoginInfo.LicensePlate[k];
    }
#else
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.TerminalID[0]='6';//终端ID
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.TerminalID[1]='6';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.TerminalID[2]='6';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.TerminalID[3]='6';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.TerminalID[4]='6';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.TerminalID[5]='6';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.TerminalID[6]='6';
    
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[0]='7';//车牌号
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[1]='7';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[2]='7';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[3]='7';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[4]='7';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[5]='7';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[6]='7';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[7]='7';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[8]='7';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[9]='7';
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate[10]=0x00;
#endif
    COML_StringReverse(0x06,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucTerminalNo.ucData);
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.SerialNo.usData=0x0000;
    		//memcpy((u8*)&(GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo),
			//(u8*)&(GpsFunDrvObj.GpsPar2.LoginInfo), sizeof(GpsFunDrvObj.GpsPar2.LoginInfo));

}

void ApiGpsCmd_100msRenew(void)//决定什么时候发送什么数据
{
  if(GetTaskId()==Task_Start)//处于初始状态
  {
    if(OFF != ApiAtCmd_tcp_state())
    {
      if(GpsCmd_GbWritCommand(GPSCOMM_Logout, (void *)0, 0) == TRUE)
      {
      }
    }
  }
  else//登录成功状态
  {
    if(GetTaskId()==Task_NormalOperation)
    {
      if(++Test1 == 10)//GPS定时1S
      {
        Test1 = 0x00;//1s进一次
        if((GpsFunDrvObj.usPulseTimer) == 0x00)//当脉冲定时=0
        {
          if(GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bSpecificAck == 0x00//收到特殊指令标志位=0
             &&(GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bGeneralAck == 0x00))//如果没有收到命令
          {
            GpsFunDrvObj.usPulseTimer = (GpsFunDrvObj.GpsPar.Gps.PulseTime * 10);//将脉冲定时设为一个值
            if(OFF != ApiAtCmd_tcp_state())//疑问，当TCP连接正常工作时
            {
              if(GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bAuthenticatied != OFF)//认证正确时
              {
                GpsCmd_GbWritCommand(GPSCOMM_Puls, (void*)0, 0);//设置终端心跳
              }
            }
          }
        }
        else//当脉冲定时≠0
        {
          GpsFunDrvObj.usPulseTimer--;
        }
        if((GpsFunDrvObj.usReportTimer) == 0x00)//报告时间=0时
        {
          //GpsFunDrvObj.usReportTimer =(GpsFunDrvObj.GpsPar.Acc.OnReportTime);//将报告时间设为一个值
          if(OFF != ApiAtCmd_tcp_state())//疑问，当TCP连接正常工作时
          {
            if(GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bGeneralAck != 0x00)//如果是通用应答
            {
              GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bGeneralAck = 0x00;
              GpsCmd_GbWritCommand(GPSCOMM_Ack, (void*)0, 0);
              GpsFunDrvObj.usReportTimer = 3;//报告时间设为3
            }
            else
            {
              
              if(GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bSpecificAck != 0x00)//如果是特殊应答
              {
                GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bSpecificAck = 0x00;
                switch(GpsFunDrvObj.PositionSystem.GbSys.State.ulAckStep)//AckStep为判断收到的指令类型
                {
                case GPSREV_Ack://通用应答
                case GPSREV_Puls://心跳应答
                case GPSREV_Login://登录应答
                case GPSREV_Logout://注销应答
                case GPSREV_Authentication://鉴权应答
                  break;
                case GPSREV_Position://定位应答
                  GpsCmd_GbWritCommand(GPSCOMM_PositionAck, (void*)0, 0);
                  break;
                case GPSREV_Photo:
                  break;
                default:
                  break;
                }
              }
              else//如果既不是普通指令也不是特殊指令
              {
                switch(GpsFunDrvObj.PositionSystem.GbSys.State.ucStep)
                {
                case 0x00://ucStep=0表示设备登录
                  if(GpsFunDrvObj.PositionSystem.GbSys.LoginInfo.ucParam.Msg.Bits.bLoginSuccess==OFF&&ApiGpsServerType!=GpsServerType_ZTE)//
                  {
#if 1//测试不注册鉴权，直接发送位置信息
#ifdef BEIDOU//使用外置北斗
                    if(BDValid==1)
#else//使用内置GPS
                    if(PositionInformationSendToATPORT_Flag==TRUE)
#endif
                    {
                      if(SendGpsLoginInfoCount==0)
                      {
                        GpsCmd_GbWritCommand(GPSCOMM_Login, (void*)0, 0);
                        SendGpsLoginInfoCount=1;
                      }
                      //SendGpsLoginInfoCount++;
                      GpsCmd_GbWritCommand(GPSCOMM_Position, (void*)0, 0);
                      PositionInformationSendToATPORT_Flag=FALSE;
                    }
                    else
                    {
                      GpsCmd_GbWritCommand(GPSCOMM_Puls, (void*)0, 0);//设置终端心跳
                       //GpsCmd_GbWritCommand(GPSCOMM_Position, (void*)0, 0);
                    }
#else
                    GpsCmd_GbWritCommand(GPSCOMM_Login, (void*)0, 0);
#endif
                    GpsFunDrvObj.usReportTimer = ReportTimerSET;
                  }
                  else//若收到平台发送8100，表示终端登录成功，usReportTimer=0
                  {
                    GpsFunDrvObj.PositionSystem.GbSys.State.ucStep++;
                    GpsFunDrvObj.usReportTimer = 0x00;
                  }
                  break;
                case 0x01://ucStep=1表示终端鉴权
                  if(GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bAuthenticatied == OFF)
                  {
                    GpsCmd_GbWritCommand(GPSCOMM_Authentication, (void*)0, 0);
                    GpsFunDrvObj.usReportTimer = ReportTimerSET;
                  }
                  else
                  {
                    GpsFunDrvObj.PositionSystem.GbSys.State.ucStep++;
                    GpsFunDrvObj.usReportTimer = 0x00;
                  }
                  break;
                case 0x02://ucStep=2表示终端定位
                  if(GpsFunDrvObj.CountdownTimer.LoadCount < ReportTimerSET)
                  {
                      GpsFunDrvObj.CountdownTimer.LoadCount++;
                      GpsFunDrvObj.usReportTimer =(GpsFunDrvObj.GpsPar.Acc.OnReportTime);
                  }
                  else
                  {
                    GpsFunDrvObj.CountdownTimer.LoadCount = 0x00;
                    GpsFunDrvObj.usReportTimer =(GpsFunDrvObj.GpsPar.Acc.OnReportTime);
                  }
                  GpsCmd_GbWritCommand(GPSCOMM_Position, (void*)0, 0);//--------------------------------位置信息发送
                  break;
                case 0x03://ucStep=3表示发送图片
                  GpsCmd_GbWritCommand(GPSCOMM_PhotoAck, (void*)0, 0);
                  GpsFunDrvObj.PositionSystem.GbSys.State.ucStep = 0x04;
                  GpsFunDrvObj.usReportTimer = 2;
                  break;
                case 0x04:
                  break;
                default:
                  break;
                }
              }
            }
          }
          else
          {
            GpsFunDrvObj.PositionSystem.GbSys.State.ucStep = 0x00;
            //GpsCmd_InfoRecordSave();
          }
        }
        else//如果usReportTimer报告时间≠0，usReportTimer减减
        {
          if(OFF != ApiAtCmd_tcp_state())
          {
            if(GpsFunDrvObj.PositionSystem.GbSys.State.ucStep == 0x00)
            {
              if(GpsFunDrvObj.usReportTimer > ReportTimerSET)
              {
                GpsFunDrvObj.usReportTimer = ReportTimerSET;
              }
            }
          }
          GpsFunDrvObj.usReportTimer--;
          if(GpsFunDrvObj.usReportTimer == 0x01)
          {
            //GpsCmd_InfoRecordPack();//从外部GPS模块获取定位信息，并解析
          }
        }
      }
    }
  }
}

void ApGpsCmd_10msRenew(void)
{
  u8 * pBuf, Len;
  if(GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bGeneralAck != 0x00
     || GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bSpecificAck != 0x00)//如果收到普通和特殊指令
  {
    return;
  }
  while((Len = DrvMC8332_GpsNotify_Queue_front(&pBuf)) != 0)//
  {
    GpsCmd_GbAnalytical(pBuf, Len);
  }
}

static void GpsCmd_GbAnalytical(u8 *pBuf, u8 len)//收到的数据进行分析
{
  u8 i, j, k;
  s16 Len;
  u16 usSeralNo;
  u32 ulParamId, ulParamData;
  pBuf+=0x05;
  GpsFunDrvObj.PositionSystem.GbSys.State.ulAckStep = (u16)(pBuf[1]) << 0x08 | pBuf[2];
  Len= (u16)(pBuf[3]) << 0x08 | pBuf[4];
  switch(GpsFunDrvObj.PositionSystem.GbSys.State.ulAckStep)
  {
  case GPSREV_Ack:
    if(pBuf[17] == 0x00)
    {
      usSeralNo = (u16)(pBuf[15]) << 0x08 | pBuf[16];
      switch(usSeralNo)
      {
      case 0x0000:
      case 0x0001:
        break;
      case 0x0102:
        GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bAuthenticatied = ON;
        break;
      case 0x0801:
        break;
      default:
        break;
      }
    }
    break;
  case GPSREV_Puls:
    break;
  case GPSREV_Login:
    if(pBuf[15] == 0x00)
    {
      pBuf+=16;
      Len-=0x03;//若Len=0，减3后就会变成负数，则异常
      for(i = 0x00; i < Len; i++)
      {
        GpsFunDrvObj.PositionSystem.GbSys.LoginInfo.ucParam.AuthenticationId[i] = pBuf[i];
      }
      GpsFunDrvObj.PositionSystem.GbSys.LoginInfo.ucParam.Msg.Bits.bLen = Len;
      //GpsFunDrvObj.PositionSystem.GbSys.LoginInfo.ucParam.Msg.Bits.bLoginSuccess = ON;
    }
    break;
  case GPSREV_Logout:
    break;
  case GPSREV_Authentication:
    break;
  case GPSREV_SetParam:
    for(i = 0x00, j = 14; i < pBuf[13]; i++)
    {
      ulParamId = 0;
      for(k = 0x00; k < 4;k++, j++)
      {
        ulParamId <<= 0x08;
        ulParamId += pBuf[j];
      }
      len = pBuf[j++];
      for(k = 0x00; k < len;k++, j++)
      {
        ulParamData <<= 0x08;
        ulParamData += pBuf[j];
      }
      switch(ulParamId)
      {
      case 0x00000001:
        GpsFunDrvObj.GpsPar.Gps.PulseTime = ulParamData/10;
        break;
      case 0x00000027:
        GpsFunDrvObj.GpsPar.Acc.OffReportTime = ulParamData;
        break;
      case 0x00000029:
        GpsFunDrvObj.GpsPar.Acc.OnReportTime = ulParamData;
        break;
      default:
        break;
      }
    }
    break;
  case GPSREV_GetParam:
    break;
  case GPSREV_Position:
    GpsFunDrvObj.PositionSystem.GbSys.State.Msg.Bits.bSpecificAck = ON;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.PositInfoAck.ucAckNo[0] = pBuf[11];
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.PositInfoAck.ucAckNo[1] = pBuf[12];
    GpsFunDrvObj.usReportTimer = 0x00;
    break;
  case GPSREV_Photo:
    break;
  default:
    break;
  }
}

static void GpsCmd_GbDataTransave(GpsCommType GpsComm)//定位信息转换，等会要用到，POC获取GPS信息转换后发送TCP
{
  u8 i = 0;
  u8 ucIndex = 0;
  _PositInfo *pPositInfo;
  
  switch(GpsComm)
  {
  case GPSCOMM_PositionAck://位置信息查询应答
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.usData = 0x0201;
    pPositInfo = &GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.PositInfoAck.PositInfo;
    break;
  case GPSCOMM_Position://位置信息汇报
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.usData = 0x0200;
    pPositInfo = &GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.PositInfo;
    break;
  default:
    break;
  }
  //COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.ucData);//原消息ID反序，屏蔽看是否正序
  if(key_warning_bubiao_flag==TRUE)
  {
    pPositInfo->stParam.Warning.ucData = 1;//报警标志位
  }
  else
  {
    pPositInfo->stParam.Warning.ucData = 0;//报警标志位
  }
  pPositInfo->stParam.WorkStatus.ucData = 0;
  pPositInfo->stParam.WorkStatus.Bits.bAcc = 1;//=GpsFunDrvObj.InfoRecord.Msg.bAcc;
  pPositInfo->stParam.WorkStatus.Bits.bLock= 1;//=1车门加锁
  GpsFunDrvObj.InfoRecord.Position.Msg.bGpsVolid=1;//手动赋值
  pPositInfo->stParam.WorkStatus.Bits.bGpsVolid = GpsFunDrvObj.InfoRecord.Position.Msg.bGpsVolid;
  pPositInfo->stParam.WorkStatus.Bits.bNorthOrSouth = 0;//GpsFunDrvObj.InfoRecord.Position.Msg.bNorthOrSouth;
  pPositInfo->stParam.WorkStatus.Bits.bEastOrWest = 0;//GpsFunDrvObj.InfoRecord.Position.Msg.bEastOrWest;
#ifdef BEIDOU//使用外置北斗
  GpsFunDrvObj.InfoRecord.Position.ulLongitude =(BDLongitude_Degree*1000000)+((BDLongitude_Minute*10000+BDLongitude_Second)*10/6);//经度Longitude
  GpsFunDrvObj.InfoRecord.Position.ulLatitude=(BDLatitude_Degree*1000000)+((BDLatitude_Minute*10000+BDLatitude_Second)*10/6);
  GpsFunDrvObj.InfoRecord.Position.usSpeed=BDSpeed*6;
  GpsFunDrvObj.InfoRecord.Position.usDirection=BDDirection;
#else//使用内置GPS
  GpsFunDrvObj.InfoRecord.Position.ulLatitude =Data_Longitude_Minute()*1000000+Data_Longitude_Second();
  GpsFunDrvObj.InfoRecord.Position.ulLongitude=Data_Latitude_Minute()*1000000+Data_Latitude_Second();
  GpsFunDrvObj.InfoRecord.Position.usSpeed=Data_Speed();//Data_Speed()*6;
#endif
  //经纬度成功存组，准备与平台连接，收到一次数据发送一次数据
  pPositInfo->stParam.ulLatitude = GpsFunDrvObj.InfoRecord.Position.ulLatitude;//经纬度在此存入（此时的GpsFunDrvObj.InfoRecord.Position.ulLatitude已经乘以1000000）
  pPositInfo->stParam.ulLongitude = GpsFunDrvObj.InfoRecord.Position.ulLongitude;//经纬度在此存入
  pPositInfo->stParam.usAltitude = GpsFunDrvObj.InfoRecord.Position.usAltitude;
  pPositInfo->stParam.usSpeed = GpsFunDrvObj.InfoRecord.Position.usSpeed;
  pPositInfo->stParam.usDirection = GpsFunDrvObj.InfoRecord.Position.usDirection;
  //位置附加消息添加里程值
  pPositInfo->stParam.AdditionPositInfo.AddInfoID=0x01;
  pPositInfo->stParam.AdditionPositInfo.AddInfoLen=0x01;

#if 1 //f3 部标平台
  pPositInfo->stParam.AdditionPositInfo.AddInfo[0]=0x88;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[1]=0xF3;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[2]=0x01;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[3]=0x08;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[4]=0x0C;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[5]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[6]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[7]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[8]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[9]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[10]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[11]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[12]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[13]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[14]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[15]=0x00;
  pPositInfo->stParam.AdditionPositInfo.AddInfo[16]=0x00;

#else
  pPositInfo->stParam.AdditionPositInfo.AddInfo=0x88;
#endif
  
  //日期手动赋值2018-1-18 17:28:20
  GpsFunDrvObj.InfoRecord.Position.ucTime[0]=Data_Time0();
  GpsFunDrvObj.InfoRecord.Position.ucTime[1]=Data_Time1();
  GpsFunDrvObj.InfoRecord.Position.ucTime[2]=Data_Time2();
  GpsFunDrvObj.InfoRecord.Position.ucDate[0]=Data_Date0();
  GpsFunDrvObj.InfoRecord.Position.ucDate[1]=Data_Date1();
  GpsFunDrvObj.InfoRecord.Position.ucDate[2]=Data_Date2();
  for(i = 0x00; i < 0x03; i++)
  {
    pPositInfo->stParam.Time.ucData[i] = GpsFunDrvObj.InfoRecord.Position.ucTime[i];
    pPositInfo->stParam.Date.ucData[i] = GpsFunDrvObj.InfoRecord.Position.ucDate[i];
  }
  //COML_StringReverse(0x04, (u8 *)(&pPositInfo->stParam.Warning));
  //COML_StringReverse(0x04, (u8 *)(&pPositInfo->stParam.WorkStatus));
  //COML_StringReverse(0x04, (u8 *)(&pPositInfo->stParam.ulLatitude));
  //COML_StringReverse(0x04, (u8 *)(&pPositInfo->stParam.ulLongitude));
  //COML_StringReverse(0x02, (u8 *)(&pPositInfo->stParam.usAltitude));
  //COML_StringReverse(0x02, (u8 *)(&pPositInfo->stParam.usSpeed));
  //COML_StringReverse(0x02, (u8 *)(&pPositInfo->stParam.usDirection));
  //COML_StringReverse(0x03, (u8 *)(&pPositInfo->stParam.Date));
  i = pPositInfo->stParam.Time.ucData[0];
  ucIndex = ((i >> 0x04) * 10);
  i &= 0x0F;
  i = (i + ucIndex + 0) % 24;//时区
  ucIndex = (i / 10) << 0x04;
  i %= 10; 
  pPositInfo->stParam.Time.ucData[0] = i | ucIndex; 
}


static bool GpsCmd_GbWritCommand(GpsCommType id, u8 *buf, u8 len)
{
  bool r = TRUE;
  u16 i = 0;
  GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Encryption=0;
  GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Subpackage=0;
  switch(id)
  {
  case GPSCOMM_Ack:
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.usData = 0x0001;
    COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.ucData);
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.Ack.Result = 0;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Len = 0x0005;
    COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.ucData);
    pack_data(GpsFunDrvObj.PositionSystem.GbSys.MsgBody.ucData, GPS_GB_HEAD_LEN+5);
    disableInterrupts();
    ApiAtCmd_WritCommand(ATCOMM10_SendTcp, GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData,
    GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len);
    enableInterrupts();
    break;
  case GPSCOMM_Puls:
     GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.usData = 0x0002;
    //COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.ucData);
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Len = 0x0000;
    //COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.ucData);
    pack_data(GpsFunDrvObj.PositionSystem.GbSys.MsgBody.ucData, GPS_GB_HEAD_LEN);
    GpsFunDrvObj.PositionSystem.GbSys.TempBuf.TestLen=COMLHexArray2String(GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData,
                                                                          GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len,
                                                                          GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucTestData);
    ApiAtCmd_WritCommand(ATCOMM10_SendTcp, GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucTestData,GpsFunDrvObj.PositionSystem.GbSys.TempBuf.TestLen);//临时缓冲区存放加密后的设备注册消息包
    
    //ApiAtCmd_WritCommand(ATCOMM10_SendTcp, GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData,GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len);
    break;
  case GPSCOMM_Login:
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Encryption=0;
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Subpackage=0;
    
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.SerialNo.usData=0x0000;//流水号
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.usData=0x0100;//终端注册消息ID:0100
    //COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.ucData);//消息ID转化为字符串
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Len=0x23;//消息体长度
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.ucData[0]=0x23;//test
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.ucData[1]=0x00;//test
    COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.ucData);//消息体属性
    COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.SerialNo.ucData);//流水号
    i = strlen((char const *)GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate);//车牌号的长度
    //COML_StringReverse(i,GpsFunDrvObj->PositionSystem.GbSys.MsgBody.Param.Message.LoginInfo.LicensePlate);
    pack_data(GpsFunDrvObj.PositionSystem.GbSys.MsgBody.ucData, 37+i);//计算消息头和消息体的总长度len=12+25+车牌号长度，并将数据GpsFunDrvObj.PositionSystem.GbSys.MsgBody.ucData按照部标要求打包放入GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData
    
    GpsFunDrvObj.PositionSystem.GbSys.TempBuf.TestLen=COMLHexArray2String(GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData,
                                                                          GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len,
                                                                          GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucTestData);
    ApiAtCmd_WritCommand(ATCOMM10_SendTcp, GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucTestData,GpsFunDrvObj.PositionSystem.GbSys.TempBuf.TestLen);//临时缓冲区存放加密后的设备注册消息包
    for(i=0;i<84;i++)//不清除可能会对下条消息造成影响
    {
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucTestData[i]='\0';
    }
    break;
  case GPSCOMM_Logout:
    if(GpsFunDrvObj.ucWorkState != id)
    {
      GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.usData = 0x0003;
      COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.ucData);
      GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Len = 0;//GPS_GB_POSITION_LEN;
      COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.ucData);
      pack_data(GpsFunDrvObj.PositionSystem.GbSys.MsgBody.ucData, GPS_GB_HEAD_LEN);//+GPS_GB_POSITION_LEN);
      ApiAtCmd_WritCommand(ATCOMM10_SendTcp, GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData,GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len);
    }
    break;
  case GPSCOMM_Authentication:
  for(i = 0x00; i < GpsFunDrvObj.PositionSystem.GbSys.LoginInfo.ucParam.Msg.Bits.bLen; i++)
    {
      GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.Message.Authentication.ucData[i] =
        GpsFunDrvObj.PositionSystem.GbSys.LoginInfo.ucParam.AuthenticationId[i];
    }
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.usData = 0x0102;
    COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.ucMsgID.ucData);
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Len = 
      GpsFunDrvObj.PositionSystem.GbSys.LoginInfo.ucParam.Msg.Bits.bLen;
    COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.ucData);
    pack_data(GpsFunDrvObj.PositionSystem.GbSys.MsgBody.ucData, GPS_GB_HEAD_LEN+i);
    ApiAtCmd_WritCommand(ATCOMM10_SendTcp, GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData,
                         GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len);
    break;
  case GPSCOMM_Position:
    GpsCmd_GbDataTransave(GPSCOMM_Position);
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Len = GPS_GB_POSITION_LEN+3+16;//GPS_GB_POSITION_LEN+6+4;
    //COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.ucData);//原消息ID反序，屏蔽看是否正序
    //pack_data(GpsFunDrvObj.PositionSystem.GbSys.MsgBody.ucData, GPS_GB_HEAD_LEN+GPS_GB_POSITION_LEN+6+4);
    pack_data(GpsFunDrvObj.PositionSystem.GbSys.MsgBody.ucData, GPS_GB_HEAD_LEN+GPS_GB_POSITION_LEN+3+16);
    GpsFunDrvObj.PositionSystem.GbSys.TempBuf.TestLen=COMLHexArray2String(GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData,
                                                                          GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len,
                                                                          GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucTestData);

    ApiAtCmd_WritCommand(ATCOMM10_SendTcp, GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucTestData,GpsFunDrvObj.PositionSystem.GbSys.TempBuf.TestLen);//临时缓冲区存放加密后的设备注册消息包
    for(i=0;i<2*84;i++)
    {
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucTestData[i]='\0';
    }
    //ApiAtCmd_WritCommand(ATCOMM5_CODECCTL,(u8 *)"AT^CDMATIME",strlen((char const *)"AT^CDMATIME"));//发送获取CDMATIME获取时间
    break;
  case GPSCOMM_PositionAck:
    GpsCmd_GbDataTransave(GPSCOMM_PositionAck);
    GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.Bits.Len = GPS_GB_POSITION_LEN+6+4+2;
    COML_StringReverse(0x02,GpsFunDrvObj.PositionSystem.GbSys.MsgBody.Param.HeadInfo.stParam.MsgProperty.ucData);
    pack_data(GpsFunDrvObj.PositionSystem.GbSys.MsgBody.ucData, GPS_GB_HEAD_LEN+GPS_GB_POSITION_LEN+6+4+2);
    ApiAtCmd_WritCommand(ATCOMM10_SendTcp, GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData,
                         GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len);
    break;
  case GPSCOMM_PhotoAck:
    break;
  case GPSCOMM_PhotoData:
    break;
  default:
    break;
  }
  GpsFunDrvObj.ucWorkState = id;
  return r;
}

static void pack_data(u8 *pBuf, u16 ucLen)//消息包封装
{
  u16 i, ucTemp;
  GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len = 0x01;
  GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[0] = 0x7E;
  for(i = 0; i < ucLen; i++)//校验码、消息头、消息体中出现0x7e，则要进行转义处理
  {
    if(i != 0x00 && pBuf[i] == 0x7e)
    {
      ucTemp = pBuf[0];
    }
  }
  if(ucLen != 0)
  {
    for(i = 0; i < ucLen; i++)
    {
      if(i == 0x00)
      {
        ucTemp = pBuf[0];
      }
      else
      {
        ucTemp ^= pBuf[i];//异或 ucTemp为校验码
      }
      switch(pBuf[i])
      {
      case 0x7e://0x7e <————> 0x7d 后紧跟一个 0x02
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len] = 0x7d;
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len]= 0x02;
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
        break;
      case 0x7d://0x7d <————> 0x7d 后紧跟一个 0x01
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len]= 0x7d;
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len]= 0x01;
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
        break;
      default:
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len]= pBuf[i];
        GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
        break;
      }
    }
    switch(ucTemp)//如果校验码是0x7e或0x7d，则转义，否则存入校验码
    {
    case 0x7e:
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len] = 0x7d;
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len]= 0x02;
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
      break;
    case 0x7d:
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len]= 0x7d;
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len]= 0x01;
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
      break;
    default:
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len]= ucTemp;//存入校验码
      GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
      break;
    }
  }
  GpsFunDrvObj.PositionSystem.GbSys.TempBuf.ucData[GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len] = 0x7E;//最后加入一个0x7e标识位
  GpsFunDrvObj.PositionSystem.GbSys.TempBuf.Len++;
}


u8 COMLHexArray2String(u8 * buf1,u8 Len1,u8 * buf2)//16进制数组转字符串
{
  u8 i,Len2;
  for(i=0;i<Len1;i++)
  {
    COML_HexToAsc2(buf1[i],buf2+2*i);//在buf2中占两位
    COML_StringReverse(2,buf2+2*i);
    //可以转换了，不过还有问题，部分0未转换成0x30，而是0x00，（0x00为前面的0转化为00，后面的转化为30，找到原因）
  }
  buf2[2*Len1]='\0';
  Len2=2*Len1+1;
return Len2;
}

u8 *ApiGps_GetTcpIpAddress()
{
  return GpsFunDrvObj.GpsPar.Gps.NetParam.Param.IP;
}
u8 *ApiGps_GetTcpPortAddress()
{
  return GpsFunDrvObj.GpsPar.Gps.NetParam.Param.Port;
}
u8 ApiGps_GetTcpPortAddress_No5()
{
  return GpsFunDrvObj.GpsPar.LoginInfo.TerminalColor;//客户需求：端口5位
}
void GetIPPORT(void)
{
  u8 i;
  for(i=0;i<15;i++)
  {
    TcpIpBuf[i]=GpsFunDrvObj.GpsPar.Gps.NetParam.Param.IP[i];
  }
  
  for(i=0;i<4;i++)
  {
    TcpPortBuf[i]=GpsFunDrvObj.GpsPar.Gps.NetParam.Param.Port[i];
  }
}