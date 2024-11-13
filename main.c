#include "main.h"
#include "mcp4725.h"
#include "Lcd.h"
#include "LcdApp.h"
#include "ModBusDev.h"
#include "ModBusHost.h"
#include "system.h"
#include "CalcDoseRate.h"
#include "i2c.h"
#include "uart.h"
#include "EepRom.h"
#include "MwPro.h"
#include "ModBus.h"
#include "PD_Probe.h"
#include "Sensor.h"
#include "cmd.h"

BYTE code VERSION = 101;  // V1.0.0

BYTE xdata StrTmp[64] = {0};
BYTE ChannelError[FLOW_METER_CNT] ={0};

#define Log //((CSampDemoDlg *)theApp.m_pMainWnd)->AddLog

#define PARAM_SIGN  0x3132
SYS_PARAM xdata SysParam;
RUN_STATUS xdata RunStatus;
PROBE_REGADDR xdata ProbeRegAddr;

BYTE ProbeSwitch = 0;               //0:GM  1:NaI   2:PD
BYTE ProbeSwitch_State = 0;

LP_SYSTEM_STTAE SysRunState={0};
BYTE Input_Status = 0;
u16 Eth_InitTim = 0;
bool Eth_Initflag = true;

u16 SendPDTim = 0;
BYTE SendPDFlag = 0;

BYTE MP3_StopFlag = 0;
BOOL AlarmConfirm_flag = false;
char Unit[8];
float My_PDDoserate = 0;
float My_Doserate = 0;


alt_u8 g_Output[OUT_IO_COUNT]      = {0,0,0,0};   // 上电绿灯亮 // 
alt_u8 g_OutStatus[OUT_IO_COUNT]   = {0,0,0,0};


extern u8  TX1_Cnt;    //发送计数
extern u8  RX1_Cnt;    //接收计数
extern bit B_TX1_Busy; //发送忙标志
extern u16 Rx1_Timer;

extern u8  TX2_Cnt;    //发送计数
extern u8  RX2_Cnt;    //接收计数
extern bit B_TX2_Busy; //发送忙标志
extern u16 Rx2_Timer;

extern u8  RX3_Cnt;    //接收计数
extern u8  TX3_Cnt;    //发送计数
extern bit B_TX3_Busy; //发送忙标志
extern u16 Rx3_Timer;

extern u8  RX4_Cnt;    //接收计数
extern u8  TX4_Cnt;    //发送计数
extern bit B_TX4_Busy; //发送忙标志
extern u16 Rx4_Timer;


extern u8  xdata RX2_Buffer[]; //接收缓冲
extern u8  xdata RX3_Buffer[]; //接收缓冲

extern const NH_PARAM NhDef[NH_COUNT];
extern char code UnitText[3][8];
extern WORD xdata ScaleBuf[SCALE_NUM];


void DebugMsg(char *msg)
{
    BYTE len = (BYTE)strlen(msg);
    //Uart1Send((BYTE *)msg,len);
}

void DebugInt(int msg)
{
    memset(StrTmp,0,64);
    sprintf(StrTmp,"%x\r\n",msg);
    DebugMsg(StrTmp);
}

void DumpCmd(BYTE *dat, BYTE len)
{
    BYTE i;
    memset(StrTmp,0,64);
    for (i=0;i<len;i++)
    {
        if (strlen(StrTmp) >= 60)
        {
            break;
        }
        sprintf(&StrTmp[i*3], "%02X ", dat[i]);
    }
    sprintf(&StrTmp[i*3], "\r\n");
    DebugMsg(StrTmp);
}

void Error()
{
    while(1)
    {
        RUN_LED(1);
        delay_ms(50);
        RUN_LED(0);
        delay_ms(50);
    }
    
}

void IoInit()
{
    EAXFR = 1;
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快

    P0M1 = 0x50;      P0M0 = 0x00;  // P0.0 P0.1 P0.4 推挽输出
    P1M1 = 0x00;      P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;      P2M0 = 0x0F;   // P2.2 推挽输出       P2M0 = 0xCF
    P3M1 = 0x24;      P3M0 = 0x18;   //设置为准双向口
    P4M1 = 0x00;      P4M0 = 0x01;   //设置为准双向口          P4M0 = 0x1F
    P5M1 = 0x00;      P5M0 = 0x05;   //设置为准双向口
    P6M1 = 0xF0;      P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;      P7M0 = 0x01;   //设置为准双向口          P7M0 = 0x1D;
}

void ETH_Init()
{ 
    ES0_M(1);
    delay_ms(200);
    ES0_M(0);

    //网络参数。   
    //STA 
    Uart3Send("at+Netmode=2\r\n",14);                   // sta模式
    delay_ms(100);
    Uart3Send("at+LANIp=192.168.16.254\r\n",25);        // 设备ip
    delay_ms(100);
    Uart3Send("at+LANIpMask=255.255.255.0\r\n",28);     // 设备网关
    delay_ms(100);
    Uart3Send("at+Dhcpd=0\r\n",12);     
    delay_ms(100);
    Uart3Send("at+STASsid=TheOne\r\n",19);              // 远端名称
    delay_ms(100);
    Uart3Send("at+STAPasswd=20251111\r\n",23);          // 远端密码
    delay_ms(100);
    Uart3Send("at+STAEncType=6\r\n",17);                // 密码保护方式
    delay_ms(100);
    Uart3Send("at+Dhcpc=1\r\n",12);                     // 动态匹配IP
    delay_ms(100);

    //静态IP
//    Uart3Send("at+WANIp=192.168.0.49\r\n",23);
//    delay_ms(50);
//    Uart3Send("at+WANIpMask=255.255.255.0\r\n",28);
//    delay_ms(50);
//    Uart3Send("at+SGw=192.168.0.1\r\n",20);
//    delay_ms(50);
//    Uart3Send("at+SDnsF=114.114.144.114\r\n",27);
//    delay_ms(50);

    //AP
//    Uart3Send("at+Netmode=3\r\n",14);
//    delay_ms(100);
//    Uart3Send("at+Dhcpd=1\r\n",12);
//    delay_ms(100);
//    Uart3Send("at+LANIp=192.168.10.1\r\n",23);
//    delay_ms(100);
//    Uart3Send("at+LANIpMask=255.255.255.0\r\n",28);
//    delay_ms(100);
//    Uart3Send("at+DhcpdIpStart=192.168.10.100\r\n",32);
//    delay_ms(100);
//    //Uart3Send("at+DhcpdMask=255.255.255.0\r\n",28);
//    //delay_ms(200);
//    Uart3Send("at+DhcpdIpStop=192.168.10.250\r\n",31);
//    delay_ms(100);
//    Uart3Send("at+DhcpdTime=86400\r\n",20);
//    delay_ms(100);
//    Uart3Send("at+APEncType=6\r\n",16);
//    delay_ms(100);
//    Uart3Send("at+APSsid=HLKWIFI\r\n",19);
//    delay_ms(100);
//    Uart3Send("at+APPasswd=12345678\r\n",22);
//    delay_ms(100);
    
    //模块串口 0 配置指令
    Uart3Send("at+NProType0=2\r\n",16);
    delay_ms(100);
    Uart3Send("at+NDomain0=192.168.74.204\r\n",28);     // 主机IP
    delay_ms(100);
    Uart3Send("at+NRPort0=8020\r\n",17);                // 远端端口
    delay_ms(100);
    Uart3Send("at+NLPort0=8080\r\n",17);                // 设备端口
    delay_ms(100);
    Uart3Send("at+NTcpTo0=0\r\n",14);
    delay_ms(100);
    CLR_WDT = 1;  // 喂狗
    Uart3Send("at+NProType1=0\r\n",16);
    delay_ms(100);
    Uart3Send("at+SBaud0=9600\r\n",16);
    delay_ms(100);
    Uart3Send("at+SWidth0=8\r\n",14);
    delay_ms(100);
    Uart3Send("at+SPari0=0\r\n",13);
    delay_ms(100);
    Uart3Send("at+SPari1=0\r\n",13);
    delay_ms(100);
    Uart3Send("at+SStop0=1\r\n",13);
    delay_ms(100);
    //保存，提交指令。
    Uart3Send("at+Save=1\r\n",11);
    delay_ms(100);
    Uart3Send("at+Apply=1\r\n",12);
    delay_ms(100);    
    Uart3Send("at+Reboot=1\r\n",13);
    delay_ms(100);
}

void LX_MP3_Init(void)
{
    A1(0);
    A2(0);
    A3(0);
    A4(0);
    A5(0);
    A6(0);
    A7(0);
    A8(0);
    A9(0);
    PAUSE_M(0);
    
}

void ProbeSelect()      
{
    if((D1()&&D2()))
    {
        ProbeSwitch = 0;
    }
    else
    {
        if(!D1())
        {
            ProbeSwitch = 1;
        }
        else if(!D2())
        {
            ProbeSwitch = 2;
        }
    }
    if(ProbeSwitch_State != ProbeSwitch)
    { 
        DevParamDef();
        SendCps();
        if(ProbeSwitch != 2)
        {
            MCP4725_OutVol(MCP4725_S1_ADDR,(char *)SysParam.Lp_Param.s_SysParam.yuzhi1);
        }
       
        ProbeSwitch_State = ProbeSwitch;
    }
    
   
}

void OutCtl(alt_u8 id, alt_u8 st)
{
    if (g_OutStatus[id] == st)
    {
        return;
    }

    g_OutStatus[id] = st;
    
    switch(id)
    {   
        case LIGHT_GREEN: 
        {
            (st)? GRE_LIGHT(1):GRE_LIGHT(0);
            break;
        }

        case LIGHT_BLUE: 
        {
            (st)? BLU_LIGHT(1)   :BLU_LIGHT(0); 
            break;
        }
        
        case LIGHT_RED:      
        {
            (st)? RED_LIGHT(1) : RED_LIGHT(0);
            break;
        }


        case ALARM_SOUND:   //蜂鸣器
        {
            (st)? ALARM(1) : ALARM(0);       
            break;
        }
    }

    
}

void OutFlash(alt_u8 id)
{
    static alt_u16 OutTimer[OUT_IO_COUNT] = {0,0,0,0};
    if (OutTimer[id] ++ > LED_FLASH_TIME/10)
    {
        OutTimer[id] = 0;
        if (g_OutStatus[id] == 1)
        {
            OutCtl(id, 0);
        }
        else
        {
            OutCtl(id, 1);
        }
    }
}

void IoCtlTask()
{
    alt_u8 i;
    for (i=0;i<OUT_IO_COUNT;i++)
    {
        if (g_Output[i] == 2)
        {
            OutFlash(i);
        }
        else
        {
            OutCtl(i, g_Output[i]);
        }
    }
}

// 板载指示灯
void RunLed(u16 dt)
{   
    static u16 tm = 0;
    u16 to = 3000;
    tm += dt;

    if (tm > to)
    {
        tm = 0;
        RUN_LED(0);
    }
    else if (tm > (to-100))
    {
        RUN_LED(1);
    }
}



void Task1s()
{
    static BYTE tm = 0;

    //CLR_WDT = 1;  // 喂狗
    
    tm++;
    if((tm == 3))
    {
        //GetRetCode();
    }

    if((tm == 6))
    {
         if(ProbeSwitch == 2)
         {
            SendReadPD();
         }
        //GetRetCode();
    }
    
    if(tm == 9)
    {
        DevRun();
    }
    
    if(tm == 10)
    {
        CLR_WDT = 1;  // 喂狗
        SysRunState.isCanReadSensor = 1;      
        tm = 0;
        Eth_InitTim ++;
        if(Eth_InitTim > 8)
        {
            Eth_InitTim = 0;
            if(!Eth_Initflag)
            {
                return;
            }
            
            ETH_Init();
            Eth_Initflag = false;
        }

    } 
}


void AlmConfirm()
{
//    if(SysRunState.keydownTime >= 300)//超过3秒,长按，关机
//    {
//      
//        SysRunState.keydownTime = 0;
//        PowerOff();
//        //PW_MAIN(0);
//    }
    
    if((SysRunState.keydownFlag == 1)&&(POWER_LOCK()))
   {
       if((SysRunState.keydownTime > 10)&&(SysRunState.keydownTime < 300))//按键时间超过100ms，不超过3秒都认为短按
       {
            //SysRunState.keyValue = 1; 
            g_Output[ALARM_SOUND] = 0; 
			if(MP3_StopFlag)
			{
			
				if(!AlarmConfirm_flag)
				{
		            StopMP3(MP3_2);
	    	        StopMP3(MP3_3);
	        	    StartMP3(MP3_STOP);
				}
			}
			AlarmConfirm_flag = true;
            //StopAlmLight();
       }
       else if(SysRunState.keydownTime >= 300)//超过2秒,长按，关机
       {
            PowerOff();
       }
       SysRunState.keydownFlag = 0;
       SysRunState.keydownTime = 0;
   }
}


void TimerTask()
{
    u16 delta = 0;
    static u16 Time1s = 0;
    
    if (Timer0Cnt)
    {
        delta = Timer0Cnt * 10;
        Timer0Cnt = 0;

        if (RX1_Cnt > 0)
       {
           Rx1_Timer += delta;
       }

        if (RX2_Cnt > 0)
        {
            Rx2_Timer += delta;
        }

        if(RX3_Cnt > 0)
        {
            Rx3_Timer += delta;
        }
        
        if(RX4_Cnt > 0)
        {
            Rx4_Timer += delta;
        }
        
        SendPDTim += delta;
        if(SendPDTim > 400)
        {
            SendPDTim = 0;
            SendPDFlag = 1;
        }
        
        RunLed(delta);
        IoCtlTask();
        ProbeSelect();
        
        Time1s += delta;
        if (Time1s >= 100)
        {
            Time1s = 0;
            Task1s();
        }
    }
}

void Delay(WORD ms)
{
    WORD t = 1000;
    while(ms--)
    {
        for (t=0;t<1000;t++) ;
    }
}

void Delayms(WORD ms)	//@11.0592MHz
{
	unsigned long edata i;
    while(ms--)
    {
    	_nop_();
    	_nop_();
    	i = 2763UL;
    	while (i) i--;
    }
}

void delay_ms(unsigned int ms)
{
    unsigned int i;
    do{
        i = MAIN_Fosc / 6030;
        while(--i);
    }while(--ms);
}


WORD ParamCheck(BYTE *buf, WORD len)
{
    WORD dwSum = 0;
    WORD i;

    for (i = 0; i < len; i++)
    {
        dwSum += buf[i];
    }

    return dwSum;
}

void ReadParam()
{
    EEPROM_read(0, (BYTE *)&SysParam, sizeof(SYS_PARAM));
    if ((SysParam.Sum != ParamCheck((BYTE *)&SysParam, sizeof(SYS_PARAM)-2)))
    {
        GeneralParam();
        DevParamDef();
        memcpy(SysParam.VerSion,DEV_VER,6);
        WriteParam();
    }
}


void WriteParam()
{
    EA = 0;    
    
    EEPROM_SectorErase(0);
    EEPROM_SectorErase(512);
    SysParam.Sum = ParamCheck((BYTE *)&SysParam, sizeof(SYS_PARAM)-2);
    if (!EEPROM_write(0, (BYTE *)&SysParam, sizeof(SYS_PARAM)))
    {
        Error();
    }

    EA = 1;     //打开总中断
}




void GetInput()
{
    if(POWER_LOCK() == 0)
    {
        delay_ms(30);
        while(POWER_LOCK() == 0)
        {
            SysRunState.keydownFlag = 1; 
        }
         AlmConfirm();
    }
}


void PowerOff()
{
    RED_LIGHT(0);  // 红灯
    GRE_LIGHT(0);  // 黄灯
    BLU_LIGHT(0);  // 蓝灯
    ALARM(0);      // 报警音
    ShowLight(0);
    PW_MAIN(0);

    while(1)
    {
        ;
    }
}

void HndInput()
{
    #if 0
    static bool em = false;
    if(STOP_M() == 0)
    {
        delay_ms(10);
        if (STOP_M() == 0)
        {
            if (RunStatus.Running)
            {
                StopSamp(false);
            }
        }

        if (em == false)
        {
            em = true;
            ShowEmStop(em);
        }
    }
    else
    {
        if (em)
        {
            em = false;
            ShowEmStop(em);
        }
    }
    #endif
}

/*
void ReportInput()
{
    BYTE PwOff = POWER_OFF;
    
    if (g_CommIdleTime > 200)
    {
        if (g_Key_Confrom)
        {
            g_Key_Confrom = 0;
            SendPcCmd(0, CMD_CERTAINKEY, NULL, 0);
            return;
        }

        if (g_Key_Power)
        {
            g_Key_Power = 0;
            SendPcCmd(0, CMD_POWER, &PwOff, 1);
            return;
        }

        #if 0
        if (g_Key_Input)
        {
            g_Key_Input = 0;
            SendPcCmd(0, CMD_INPUT, &Input_Status, 1);
        }
        #endif
    }
}
*/

void LedInit()
{
    // 指示灯
    RED_LIGHT(0);  // 红灯
    GRE_LIGHT(0);  // 绿灯
    BLU_LIGHT(0);  // 蓝灯
    ALARM(0);      // 报警音
}


void GeneralParam()
{
    SysParam.Sign     = PARAM_SIGN;
    RunStatus.Unit = Unit_uSv;
    SysParam.Cps_Enable = 0;
    SysParam.Cps_Check = 0;

    SysParam.s_General_Info.Address = 1;
    memset(&SysParam.s_General_Info.Index,0,32); 
    memset(&SysParam.s_General_Info.Location,0,32); 
    memset(&SysParam.s_General_Info.Name,0,32); 
    sprintf((char*) SysParam.s_General_Info.Name,DEV_NAME);  
    WriteParam();
}

void DevParamDef()
{
    BYTE i;
    switch(ProbeSwitch)
    {
        case 0:
                memset((void*)&SysParam.Lp_Param,0,sizeof(LP_PARAM));
                SysParam.Lp_Param.Sign = 0x4142;

                SysParam.Lp_Param.s_Alarm.DosePreAlarm = 300;     //300uSv
                SysParam.Lp_Param.s_Alarm.DoseAlarm = 400;        //400uSv
                SysParam.Lp_Param.s_Alarm.DoseRatePreAlarm = 300;//300uSv/h
                SysParam.Lp_Param.s_Alarm.DoseRateAlarm = 400;//400uSv/h

                SysParam.Lp_Param.s_SysParam.Canshu1 = 1;
                SysParam.Lp_Param.s_SysParam.Canshu2 = 1;

                #ifdef POE_SEN
                // 低量程用Co60拟合，最后剂量率要乘Co/Cs比
                SysParam.Lp_Param.s_Jiaozhun.DI_A = 0.071749524255754;   		// 0.0552364572191522;    				//a = 1.84773623191768
                SysParam.Lp_Param.s_Jiaozhun.DI_B = 0.321361930688732; 			// 0.325776472823953; 					//b = 0.362602496332558
                SysParam.Lp_Param.s_Jiaozhun.DI_C = 0.00117277837260376;		// 0.00103232522627559;					//c = 5.23843587599517E-05
                SysParam.Lp_Param.s_Jiaozhun.DI_D = -4.45221185949912E-06;		// -3.46972129850684E-06;				//d = 2.02801464142905E-09

                SysParam.Lp_Param.s_Jiaozhun.MID_A = 5.1735799060138;    
                SysParam.Lp_Param.s_Jiaozhun.MID_B = 0.314778799356561; 
                SysParam.Lp_Param.s_Jiaozhun.MID_C = 0.000188389561373155;
                SysParam.Lp_Param.s_Jiaozhun.MID_D = -6.40204890256851E-08;
                
                // Cs137 > 1.2m
                SysParam.Lp_Param.s_Jiaozhun.GAO_A = 3.62608397252143;			//684.399049164029; 
                SysParam.Lp_Param.s_Jiaozhun.GAO_B = 6.3652408143604;			//6.14300026842352; 
                SysParam.Lp_Param.s_Jiaozhun.GAO_C = 0.00160225788737462;		//5.3028356929535E-05;  
                SysParam.Lp_Param.s_Jiaozhun.GAO_D = -1.72769678770162E-06;		//9.30859286124393E-10;
                #else
                SysParam.Lp_Param.s_Jiaozhun.DI_A = 0.5593;    // 0.56;
                SysParam.Lp_Param.s_Jiaozhun.DI_B = 0.000119; // 0.00017;
                SysParam.Lp_Param.s_Jiaozhun.DI_C = 1;
                SysParam.Lp_Param.s_Jiaozhun.GAO_A = 31.92; // 33.6;
                SysParam.Lp_Param.s_Jiaozhun.GAO_B = 0.000018;  // 0.000023;
                SysParam.Lp_Param.s_Jiaozhun.GAO_C = 1;
                #endif
                sprintf((char*)SysParam.Lp_Param.s_SysParam.yuzhi1,"160");
                SysParam.Lp_Param.CoCsRate = 1.19;

                //memcpy((char *)&SysParam.Lp_Param.NhParam, (char *)&NhDef, sizeof(NH_PARAM)*NH_COUNT);

                for (i=0;i<FIX_COUNT;i++)
                {
                    SysParam.Lp_Param.Fix[i] = 1.0;
                }
                break;
                
        case 1: 
                memset((void*)&SysParam.Lp_Param,0,sizeof(LP_PARAM));
                SysParam.Lp_Param.Sign = 0x4142;

                SysParam.Lp_Param.s_Alarm.DosePreAlarm = 300;       //300uSv
                SysParam.Lp_Param.s_Alarm.DoseAlarm = 400;          //400uSv
                SysParam.Lp_Param.s_Alarm.DoseRatePreAlarm = 30;    //30uSv/h
                SysParam.Lp_Param.s_Alarm.DoseRateAlarm = 40;       //40uSv/h

                SysParam.Lp_Param.s_SysParam.Canshu1 = 1;
                SysParam.Lp_Param.s_SysParam.Canshu2 = 1;

                #ifdef POE_SEN
                // 低量程用Co60拟合，最后剂量率要乘Co/Cs比
                SysParam.Lp_Param.s_Jiaozhun.DI_A = -0.119135107108512;    
                SysParam.Lp_Param.s_Jiaozhun.DI_B = 0.406688356856999; 
                SysParam.Lp_Param.s_Jiaozhun.DI_C = -0.000795882360575705;
                SysParam.Lp_Param.s_Jiaozhun.DI_D = 8.30493670182536E-06;

                SysParam.Lp_Param.s_Jiaozhun.MID_A = 25.2437957332788;    
                SysParam.Lp_Param.s_Jiaozhun.MID_B = 0.352560222117065; 
                SysParam.Lp_Param.s_Jiaozhun.MID_C = 6.84378068779986E-05;
                SysParam.Lp_Param.s_Jiaozhun.MID_D = 0;
                
                // Cs137 > 1.2m
                SysParam.Lp_Param.s_Jiaozhun.GAO_A = -32435.201473664; 
                SysParam.Lp_Param.s_Jiaozhun.GAO_B = 35.1613941353375; 
                SysParam.Lp_Param.s_Jiaozhun.GAO_C = -6.81597532235814E-4;  
                SysParam.Lp_Param.s_Jiaozhun.GAO_D = 2.46287003496529E-08;
                #else
                SysParam.Lp_Param.s_Jiaozhun.DI_A = 0.5593;    // 0.56;
                SysParam.Lp_Param.s_Jiaozhun.DI_B = 0.000119; // 0.00017;
                SysParam.Lp_Param.s_Jiaozhun.DI_C = 1;
                SysParam.Lp_Param.s_Jiaozhun.GAO_A = 31.92; // 33.6;
                SysParam.Lp_Param.s_Jiaozhun.GAO_B = 0.000018;  // 0.000023;
                SysParam.Lp_Param.s_Jiaozhun.GAO_C = 1;
                #endif
                sprintf((char*)SysParam.Lp_Param.s_SysParam.yuzhi1,"140");

                SysParam.Lp_Param.CoCsRate = 1.19;
                
                #ifdef P2P_NH
                memcpy((char *)&SysParam.Lp_Param.NhParam, (char *)&NhDef, sizeof(NH_PARAM)*NH_COUNT);
                #endif
                
                for (i=0;i<FIX_COUNT;i++)
                {
                    SysParam.Lp_Param.Fix[i] = 1.0;
                }
                break;
        case 2: 
                SysParam.Pd_param.Address = PD_MODBUS_ADDR;
                SysParam.Pd_param.s_Alarm.DosePreAlarm = 300;     //300uSv
                SysParam.Pd_param.s_Alarm.DoseAlarm = 400;        //400uSv
                SysParam.Pd_param.s_Alarm.DoseRatePreAlarm = 300;//300uSv/h
                SysParam.Pd_param.s_Alarm.DoseRateAlarm = 400;    //400uSv/h
                SysParam.Pd_param.s_Thr.Det_Thr[0] = 255-30;
                SysParam.Pd_param.s_Thr.Det_Thr[1] = 10;
                SysParam.Pd_param.s_Thr.Det_Thr[2] = 150;
                for(i = 0;i<PD_FIXNUM;i++)
                {
                    SysParam.Pd_param.fix[i] = 1.0;
                }      
                break;
    }
    SysParam.Cps_Enable = 0;
    SysParam.Cps_Check = 0;

    WriteParam();

}

void SaveParam()
{
    WriteParam();
}


void UpdataUI()
{

    //ShowStatus();
    ShowIcon(REG_STATUS,0);
    delay_ms(200);
    StatusColor(true);
}

void InitLcd()
{   
    memset(&RunStatus, 0, sizeof(RUN_STATUS));
    memset(&RunInfo, 0, sizeof(RUN_INFO));
    ShowLightON();
    delay_ms(200);
    HideBorder();
    delay_ms(200);
    SendCps();
    delay_ms(200);
    ShowCps();
    delay_ms(200);
    Cps_Check();
    delay_ms(200);
    UpdataUI();    
    delay_ms(200);
    ShowIcon(MP_SCALE_END+1,36);
    delay_ms(200);
    HideControl();
    delay_ms(20);
    
    //SendParam();
    //delay_ms(200);
    //SetBkLight(false);
    //delay_ms(200);
    //ShowDevInfo();
    //delay_ms(200);
}


void Char2Unicode(char *buff,BYTE len)
{
    BYTE i = 0;
    WORD dat = 0;
    for(i = 0;i<len;i++)
    {
        RunInfo.DoseRate[i] = (dat|buff[i]);
    }
}

void GetDoseRate()
{
    char drbuff[5] = {0};
    static WORD time = 0;

    switch(ProbeSwitch)
    {
        case 0:
        case 1: 
                #if 0
                time++;
                if(time < 30)
                {
                    SysRunState.s_DoseMSG.DoseRate = 2.00; 
                }
                else if(time < 60)
                {
                    SysRunState.s_DoseMSG.DoseRate = 1386.8;
                   
                } 
                if(time > 60)
                {
                    time = 0;
                } 
                CalcAlarmState(&SysRunState);
				My_Doserate = SysRunState.s_DoseMSG.DoseRate;
                //printf("DoseRate1 = %f\r\n",SysRunState.s_DoseMSG.DoseRate);
                Set_LP_DoseRateUnit(); 
                #endif
                RunStatus.DoseRate = My_Doserate;               
                break;
        case 2: RunStatus.DoseRate = My_PDDoserate;     // 模拟
                break;
    }
    if(RunStatus.DoseRate < 10)
    {
        sprintf((char *)drbuff,"%.3f", RunStatus.DoseRate);
    }
    else if(RunStatus.DoseRate < 100)
    {
        sprintf((char *)drbuff,"%.2f", RunStatus.DoseRate);
    }
    else if(RunStatus.DoseRate == 100)
    {
        
        sprintf((char *)drbuff,"%.1f", RunStatus.DoseRate);
    }
    Char2Unicode(drbuff,5);
    
    memset(RunInfo.Unit,0,8);
    //sprintf(Unit, UnitText[RunStatus.Unit]);
    sprintf(RunInfo.Unit, UnitText[RunStatus.Unit]);
    LcdCmd(LCD_CMD_WRITE, REG_SCALE_UNIT, (BYTE *)RunInfo.Unit,8);
}

void CheckDoseRate()
{
    WORD deg = 0,i = 0;
    WORD buf[SCALE_NUM];
    if(RunStatus.DoseRate <= 10)
    {
        ShowVal(ScaleBuf);
        deg = RunStatus.DoseRate * 10;
    }
   else if(RunStatus.DoseRate <= 100)
    {
        buf[0] = SPIN_SCALE0_VAL;
        for(i = 0;i < SCALE_NUM-1;i++)
        {
            buf[i+1] = buf[i] + 10;
        }
        ShowVal((WORD *)buf);
        deg = RunStatus.DoseRate;
    }
//    else if(RunStatus.DoseRate <= 1000)
//    {
//        buf[0] = SPIN_SCALE10_VAL*5;
//        for(i = 0;i < SCALE_NUM-1;i++)
//        {
//            buf[i+1] = buf[i] + 50;
//        }        
//        ShowVal((WORD *)buf);
//        deg = (RunStatus.DoseRate-500)/5;
//
//       
//    }
    ShowSpin(deg);
}

void StartMP3(BYTE id)
{
    switch(id)
    {
        case MP3_1: A1(1);delay_ms(100);break;   

        case MP3_2: A2(1);delay_ms(100);break;   

        case MP3_3: A3(1);delay_ms(100);break;   

        case MP3_4: A4(1);delay_ms(100);break;   

        case MP3_5: A5(1);delay_ms(100);break;   

        case MP3_6: A6(1);delay_ms(100);break;   

        case MP3_7: A7(1);delay_ms(100);break;   

        case MP3_8: A8(1);delay_ms(100);break;   

        case MP3_9: A9(1);delay_ms(100);break;   

        case MP3_STOP: PAUSE_M(1);delay_ms(200);  PAUSE_M(0);      break;
    }
}

void StopMP3(BYTE id)
{
    switch(id)
    {
        case MP3_1: A1(0);delay_ms(100);break;

        case MP3_2: A2(0);delay_ms(100);break;

        case MP3_3: A3(0);delay_ms(100);break;

        case MP3_4: A4(0);delay_ms(100);break;

        case MP3_5: A5(0);delay_ms(100);break;

        case MP3_6: A6(0);delay_ms(100);break;

        case MP3_7: A7(0);delay_ms(100);break;

        case MP3_8: A8(0);delay_ms(100);break;

        case MP3_9: A9(0);delay_ms(100);break;

        case MP3_STOP: PAUSE_M(0);     break;

    }

}

void StopAlmLight()
{
    g_Output[LIGHT_GREEN] = 0;
    g_Output[LIGHT_RED] = 0;
    g_Output[ALARM_SOUND] = 0; 

    HideBorder();
    StopMP3(MP3_2);
    StopMP3(MP3_3);

    if(MP3_StopFlag)
    {
        if(!AlarmConfirm_flag)
        {
            StartMP3(MP3_STOP);
        }
        MP3_StopFlag = 0;
    }
    AlarmConfirm_flag = false;
    
}
void AbnorAlarm()
{   
    bool HaveAlarm = false;
	
    if((DoseRateAlarm == ALM_DOSERATE_PRE)||(DoseRateAlarm == ALM_DOSE_PRE))
    {
        HaveAlarm = true;
		RunInfo.FsIcon = 1;
        ShowIcon(REG_STATUS,1);
        g_Output[LIGHT_RED] = 1;
		g_Output[LIGHT_GREEN] = 1;
        ShowIcon(MP_SCALE_END+1,37);
        ShowBorderY();
        if(!AlarmConfirm_flag)
        {
            StartMP3(MP3_2);
        }
        StopMP3(MP3_3);

    }
    else if((DoseRateAlarm == ALM_DOSERATE_ERROR)||(DoseRateAlarm == ALM_DOSE_ERROR)
        ||(DoseRateAlarm == ALM_DOSE_OVER))
    {
        HaveAlarm = true;
		RunInfo.FsIcon = 2;
        ShowIcon(REG_STATUS,2);
        g_Output[LIGHT_RED] = 1;
		g_Output[LIGHT_GREEN] = 0;
        ShowIcon(MP_SCALE_END+1,38);
        ShowBorderR();
        if(!AlarmConfirm_flag)
        {
            StartMP3(MP3_3);
        }
        StopMP3(MP3_2);
    }
    else
    {
        HaveAlarm = false;
	}
    if (HaveAlarm)
    {
        MP3_StopFlag = 1;
		if(!AlarmConfirm_flag)
        {
         	g_Output[ALARM_SOUND] = 1;
        }
    }
    else
    {
    	RunInfo.FsIcon = 0;
        ShowIcon(REG_STATUS,0);
        ShowIcon(MP_SCALE_END+1,36);
        StopAlmLight();
    }
}
void CheckAlarm()
{
    switch(ProbeSwitch)
    {
        case 0:
        case 1: 
                if((SysRunState.s_DoseMSG.State & (1<<7)))
                {
                    DoseRateAlarm = ALM_DOSERATE_PRE;
                }
                else if((SysRunState.s_DoseMSG.State & (1<<6)))
                {
                    DoseRateAlarm = ALM_DOSERATE_ERROR;
                }
                #if 0
                else if((SysRunState.s_DoseMSG.State & (1<<5)))
                {
                    DoseRateAlarm = ALM_DOSE_PRE;
                }
                else if((SysRunState.s_DoseMSG.State & (1<<4)))
                {
                    DoseRateAlarm = ALM_DOSE_ERROR;
                }
                else if((SysRunState.s_DoseMSG.State & (1<<3)))
                {
                    DoseRateAlarm = ALM_DOSE_OVER;
                }
                #endif
                else
                {
                    DoseRateAlarm = ALM_DOSERATE_NOR;
                }
                break;
        case 2: 
               //printf("DevSt = %d\r\n",SysParam.Pd_param.s_DoseRate.DevSt);
                if((SysParam.Pd_param.s_DoseRate.DevSt & (1<<7)))
                {
                    DoseRateAlarm = ALM_DOSERATE_PRE;
                }
                else if((SysParam.Pd_param.s_DoseRate.DevSt & (1<<6)))
                {
                    DoseRateAlarm = ALM_DOSERATE_ERROR;
                }
                #if 0
                else if((SysParam.Pd_param.s_DoseRate.DevSt & (1<<5)))
                {
                    DoseRateAlarm = ALM_DOSE_PRE;
                }
                else if((SysParam.Pd_param.s_DoseRate.DevSt & (1<<4)))
                {
                    DoseRateAlarm = ALM_DOSE_ERROR;
                }
                else if((SysParam.Pd_param.s_DoseRate.DevSt & (1<<3)))
                {
                    DoseRateAlarm = ALM_DOSE_OVER;
                }
                #endif
                else
                {
                    DoseRateAlarm = ALM_DOSERATE_NOR;
                }
                break; 
    }

    if (HisAlarm != DoseRateAlarm)
    {
        // 报警有变化才更新界面
        //delay_ms(200);
        //ShowStatus();
        //delay_ms(200);
        StatusColor(false);

        HisAlarm = DoseRateAlarm;
    }
    AbnorAlarm();
}

// 1秒运行一次
void DevRun()
{
    RunStatus.RunTime ++;
    //CPS显示
    ShowCps();
    
    GetDoseRate();
    
    //刻度盘指针显示
    CheckDoseRate();

    // 2. 显示剂量率和状态
    ShowDoseRate();
    //delay_ms(200);
    
    // 3. 检查报警状态  
    if (RunStatus.RunTime > 5)
    {
    	RunStatus.RunTime = 6;
        // 运行时间大于10秒才检测
        CheckAlarm();
    }

}


void SyncModBusDev()
{
    BYTE i;
    SysParam.Pd_param.Address =  SysParam.s_General_Info.Address;
    ModBusPD_Param.Address = SysParam.Pd_param.Address;
    //ModBusPD_Param.Ver = SysParam.Pd_param.Ver;
    //sprintf(ModBusAddr.Ver,"V%d.%d.%d", SysParam.Ver/100, SysParam.Ver%100/10, SysParam.Ver%10 );
//    memset(&ModBusPD_Param.s_DoseRate, 0, sizeof(PD_DOSERATE));
//    ModBusPD_Param.s_DoseRate.DoseRate = SysParam.Pd_param.s_DoseRate.DoseRate;
//    ModBusPD_Param.s_DoseRate.DoseTotal = SysParam.Pd_param.s_DoseRate.DoseTotal;
//    ModBusPD_Param.s_DoseRate.DevSt= SysParam.Pd_param.s_DoseRate.DevSt;
//    ModBusPD_Param.s_DoseRate.Cps1 = SysParam.Pd_param.s_DoseRate.Cps1;
//    ModBusPD_Param.s_DoseRate.Cps2 = SysParam.Pd_param.s_DoseRate.Cps2;
//    ModBusPD_Param.s_DoseRate.Cps3 = SysParam.Pd_param.s_DoseRate.Cps3;

    memset(&ModBusPD_Param.s_Alarm, 0, sizeof(PD_ALARM));
    ModBusPD_Param.s_Alarm.DoseRatePreAlarm = SysParam.Pd_param.s_Alarm.DoseRatePreAlarm;
    ModBusPD_Param.s_Alarm.DoseRateAlarm= SysParam.Pd_param.s_Alarm.DoseRateAlarm;
    ModBusPD_Param.s_Alarm.DosePreAlarm = SysParam.Pd_param.s_Alarm.DosePreAlarm;
    ModBusPD_Param.s_Alarm.DoseAlarm = SysParam.Pd_param.s_Alarm.DoseAlarm;

    memset(&ModBusPD_Param.s_Thr, 0, sizeof(PD_THR));
    for(i = 0;i<3;i++)
    {
        ModBusPD_Param.s_Thr.Det_Thr[i] = SysParam.Pd_param.s_Thr.Det_Thr[i];
    }
    ModBusPD_Param.s_Thr.AnalogChannel = SysParam.Pd_param.s_Thr.AnalogChannel;

    memset(&ModBusPD_Param.fix, 0, sizeof(ModBusPD_Param.fix));
    for(i = 0;i < 16;i++)
    {
        ModBusPD_Param.fix[i] = SysParam.Pd_param.fix[i];
    }

    memset(&ModBusPD_Param.s_PdInfo, 0, sizeof(PD_INFO));
        
    //memcpy(SysParam.Pd_param.s_PdInfo.Name,SysParam.s_General_Info.Name,32);
    //memcpy(ModBusPD_Param.s_PdInfo.Index,SysParam.Pd_param.s_PdInfo.Index,32);
    //memcpy(ModBusPD_Param.s_PdInfo.Location,SysParam.Pd_param.s_PdInfo.Location,32);
    //memcpy(ModBusPD_Param.s_PdInfo.Name,SysParam.Pd_param.s_PdInfo.Name,32);
    
}

void Dev_Init()
{
    ProbeSwitch = 0;
    if((D1()&&D2()))
    {
        ProbeSwitch = 0;
    }
    else
    {
        if(!D1())
        {
            ProbeSwitch = 1;
        }
        else if(!D2())
        {
            ProbeSwitch = 2;
        }
    }
    //ProbeSwitch_State = ProbeSwitch;
    RST(0);
    ES0_M(0);
    //ES1_M(0);

}

void main(void)
{
    SysInit();
    IoInit();
    PW_MAIN(1);  // 主电源
    delay_ms(500);
    
    RUN_LED(1);
    
    //ProbeSelect();
    Dev_Init(); 
    LX_MP3_Init();
    LedInit();
    delay_ms(500);
    
    Timer0_Init();
    Timer1_Init();
    Timer3_Init();
    Timer4_Init();
    delay_ms(500);

    UART1_config();
    UART2_config();
    UART3_config();
    UART4_config();
    ClearUart1Buf();
    ClearUart2Buf();
    ClearUart3Buf();
    ClearUart4Buf();
    // 待CPU稳定了再读参数
    delay_ms(500);
    ReadParam();
    delay_ms(500);

    RUN_LED(0);

    EA = 1;     //打开总中断
    SensorInit();
    delay_ms(500);
    InitLcd();
    delay_ms(500);
//    ETH_Init();
//    delay_ms(500);
    ClearCounter();
    if(ProbeSwitch != 2)
    {
        MCP4725_OutVol(MCP4725_S1_ADDR,(char *)SysParam.Lp_Param.s_SysParam.yuzhi1);
    }
    WDT_CONTR |= (1<<5) |  7;  // 启动开门狗，约8秒
    while(1)
    {
        TimerTask(); 
        GetInput();
        if(SysRunState.isCanReadSensor == 1)
        {
            SysRunState.isCanReadSensor = 0;
            if(ProbeSwitch != 2)
            {
                CaptureSensorPluseCounter();//捕获当前测量结果
            }
        }

        Uart1Hnd();
        Uart2Hnd();
        Uart3Hnd(); 
        Uart4Hnd();
    }
}


