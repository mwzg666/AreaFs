//#include "pch.h"

#include "main.h"
#include "Lcd.h"
#include "LcdApp.h"
#include "cmd.h"


//#include "SerialPort.h"
//#include "SampDemo.h"
//#include "SampDemoDlg.h"

#define Log //((CSampDemoDlg *)theApp.m_pMainWnd)->AddLog

#define TEMP_BUF_LEN  128
BYTE xdata TempBuf[TEMP_BUF_LEN] = {0};

WORD xdata ScaleBuf[SCALE_NUM] = {SPIN_SCALE0_VAL,SPIN_SCALE1_VAL,SPIN_SCALE2_VAL,SPIN_SCALE3_VAL,
                                    SPIN_SCALE4_VAL,SPIN_SCALE5_VAL,SPIN_SCALE6_VAL,SPIN_SCALE7_VAL,
                                    SPIN_SCALE8_VAL,SPIN_SCALE9_VAL,SPIN_SCALE10_VAL};

SHOW_PARAM xdata ShowParam;
SHOW_CPS xdata ShowCpsMode;

RUN_INFO xdata RunInfo;
DEV_INFO xdata DevInfo;

bool NeedGetRet = false;
BYTE DoseRateAlarm = 0; // 默认状态正常
BYTE HisAlarm = 0;

extern BYTE code VERSION;
extern SYS_PARAM xdata SysParam;
extern LP_SYSTEM_STTAE SysRunState;

WORD code StatusPoint[3] = 
{
    MP_DOSERATE_END,MP_SCALE_UNIT_END,MP_UNIT_END//0xA300,0xA400,0xA500,0xA600,0xA700,0xA100
};

WORD code TxtColor[3] = 
{
    //正常     低剂量率       高剂量率                    //禁用 
    //绿色     黄色         红色                      //白色 
    0x07E8,0xFFE0,0xF800              //0x7E80,0xFFFF,
};

char code UnitText[3][8] =
{
    "μSv/h",
    "mSv/h",
    "Sv/h"
};

void ShowDevInfo()
{
    memset(&DevInfo, 0, sizeof(DEV_INFO));

    sprintf(DevInfo.ID,"ID:%d", SysParam.s_General_Info.Address);
    sprintf(DevInfo.SoftVer,"%s", SysParam.VerSion);
    LcdCmd(LCD_CMD_WRITE, REG_DEV_ID, (BYTE *)&DevInfo, 22);
}


void GetRetCode()
{
    BYTE len = 1;

    if (NeedGetRet)
    {
        LcdCmd(LCD_CMD_READ, REG_RETRUN, &len, 1);
    }
}


// 调整状态文字的颜色
void StatusColor(bool force)
{
    BYTE i;
    for (i=0;i<3;i++)
    {
        if ((DoseRateAlarm != HisAlarm) || force)
        {
            delay_ms(200);
            if((DoseRateAlarm == ALM_DOSERATE_PRE)||(DoseRateAlarm == ALM_DOSE_PRE))
            {
                SetTextColor(StatusPoint[i], SwWord(TxtColor[1]));
            }
            else if((DoseRateAlarm == ALM_DOSERATE_ERROR) ||(DoseRateAlarm == ALM_DOSE_ERROR)
                ||(DoseRateAlarm == ALM_DOSE_OVER) )
            {
                SetTextColor(StatusPoint[i], SwWord(TxtColor[2]));
            }
            else
            {
                SetTextColor(StatusPoint[i], SwWord(TxtColor[0]));
            }
        }
    }
}

void SetRunTime(DWORD tm)
{
    WORD h,m,s;
    h = (WORD)(tm/3600);
    m = (tm%3600)/60;
    s = tm%60;
    //memset(RunInfo.RunTime,0,16);
    //sprintf((char *)RunInfo.RunTime, "%04d:%02d:%02d",h,m,s);
    //WriteParam();
}

void ShowDoseRate()
{
    memcpy((char *)RunInfo.Name,(char *)SysParam.s_General_Info.Name,32);
    switch(ProbeSwitch)
    {
        case 0:
                RunInfo.Cps1 = (DWORD)SysRunState.s_CPS.CPS1;
                RunInfo.Cps2 = (DWORD)SysRunState.s_CPS.CPS2;
                break;
        case 1:   
                RunInfo.Cps1 = (DWORD)SysRunState.s_CPS.CPS1;
                break;
        case 2:
                RunInfo.Cps1 = (DWORD)SysParam.Pd_param.s_DoseRate.Cps1;
                break;
    }         
    LcdCmd(LCD_CMD_WRITE, REG_DOSERATE, (BYTE *)&RunInfo, sizeof(RUN_INFO));
}

//CPS数据显示
void ShowCps()
{
    if(SysParam.Cps_Enable)
    {
        switch(ProbeSwitch)
        {
            case 0:
                ShowModule(MP_CPS1_END,REG_CPS1);
                SetTextIntegerNum(MP_CPS1_END,0x0207);
                ShowModule(MP_CPS2_END,REG_CPS2);
				SetTextIntegerNum(MP_CPS2_END,0x0207);
                break;
            case 1:
                ShowModule(MP_CPS1_END,REG_CPS1);
                SetTextIntegerNum(MP_CPS1_END,0x0009);
                break;
            case 2:
                ShowModule(MP_CPS1_END,REG_CPS1);
                SetTextIntegerNum(MP_CPS1_END,0x0206);
                break;
        }
    }
    else
    {
        HideModule(MP_CPS1_END);
        HideModule(MP_CPS2_END);
    }
}

//边框黑黄闪烁
void ShowBorderY()
{
    static BYTE t = 0;
    
    ShowIcon(REG_TOP_BORDER,1);
    ShowIcon(REG_LEFT_BORDER,1);   
    ShowIcon(REG_BELOW_BORDER,1);
    ShowIcon(REG_RIGHT_BORDER,1);
    t++;
    if(t>1)
    {   t = 0;
        ShowIcon(REG_TOP_BORDER,2);
        ShowIcon(REG_LEFT_BORDER,2);   
        ShowIcon(REG_BELOW_BORDER,2);
        ShowIcon(REG_RIGHT_BORDER,2);
    }
}

//边框黑红闪烁
void ShowBorderR()
{
    static BYTE t = 0;
    ShowIcon(REG_TOP_BORDER,1);
    ShowIcon(REG_LEFT_BORDER,1);   
    ShowIcon(REG_BELOW_BORDER,1);
    ShowIcon(REG_RIGHT_BORDER,1);
    t++;
    if(t>1)
    {   t = 0;
        ShowIcon(REG_TOP_BORDER,3);
        ShowIcon(REG_LEFT_BORDER,3);   
        ShowIcon(REG_BELOW_BORDER,3);
        ShowIcon(REG_RIGHT_BORDER,3);
        
    }
}

//隐藏边框
void HideBorder()
{
    ShowIcon(REG_TOP_BORDER,0);
    ShowIcon(REG_LEFT_BORDER,0);   
    ShowIcon(REG_BELOW_BORDER,0);
    ShowIcon(REG_RIGHT_BORDER,0);
}

void SendCps()
{  
    ShowCpsMode.Cps_Check = SwWord((WORD)SysParam.Cps_Check);
    ShowCpsMode.Cps_Enable = SwWord((WORD)SysParam.Cps_Enable);
    LcdCmd(LCD_CMD_WRITE, REG_CPS_CHECK, (BYTE *)&ShowCpsMode, sizeof(SHOW_CPS));
}

void SendParam()
{
    switch(ProbeSwitch)
    {
        case 0:
        case 1: //memcpy(SysParam.s_General_Info.Name,SysParam.Lp_Param.Name,32);
                //SysParam.s_General_Info.Address = SysParam.Lp_Param.address;
                ShowParam.PreAlarmThres = SwDWord((DWORD)SysParam.Lp_Param.s_Alarm.DoseRatePreAlarm);
                ShowParam.AlarmThres = SwDWord((DWORD)SysParam.Lp_Param.s_Alarm.DoseRateAlarm);
                break;
        case 2: //memcpy(SysParam.s_General_Info.Name,SysParam.Pd_param.s_PdInfo.Name,32);    
                //SysParam.s_General_Info.Address = SysParam.Pd_param.Address;
                ShowParam.PreAlarmThres = SwDWord((DWORD)SysParam.Pd_param.s_Alarm.DoseRatePreAlarm);
                ShowParam.AlarmThres = SwDWord((DWORD)SysParam.Pd_param.s_Alarm.DoseRateAlarm);
                break;
    }
    
    //ShowParam.BkLight = SwWord((WORD)SysParam.BkLight);
    memcpy(ShowParam.Name,SysParam.s_General_Info.Name,32);
    ShowParam.Address = SwWord((WORD)SysParam.s_General_Info.Address);
    //memcpy(ShowParam.MP3Txt,SysParam.MP3Txt,112);
    
    LcdCmd(LCD_CMD_WRITE, REG_SYS_PARAM, (BYTE *)&ShowParam, sizeof(SHOW_PARAM));
}


// 获取修改后的时间
void GetInputTime()
{
    BYTE len = 7;
    NeedGetRet = true;
    //delay_ms(300);
    LcdCmd(LCD_CMD_READ, REG_DATE_TIME, &len, 1);
}

// 在时间设置界面写入当前时间
void SetCurTime()
{
    LCD_FRAME *pFrame;
    LCD_DATE_TIME  ldt;
    
    BYTE dt[8] = {0};
    pFrame = (LCD_FRAME *)&RecvBuf1[LcdFrameloc];
    if (!CheckDataLen(4))
    {
        return;
    }
    
    memcpy(dt, &pFrame->Data[1], 8);
    
    ldt.Year = SwWord((WORD)(dt[0]/10));
    //ldt.YearL = SwWord((WORD)(dt[0]%10));
    ldt.Month = SwWord((WORD)dt[1]);
    ldt.Day   = SwWord((WORD)dt[2]);

    ldt.Hour   = SwWord((WORD)dt[4]);
    ldt.Minute = SwWord((WORD)dt[5]);
    ldt.Secend = SwWord((WORD)dt[6]);

    LcdCmd(LCD_CMD_WRITE, REG_DATE_TIME, (BYTE *)&ldt, sizeof(LCD_DATE_TIME));
    
}

void Cps_Check()
{
    if(SysParam.Cps_Check)
    {
        ShowModule(MP_CPS_ONOFF_END,REG_CPS_ONOFF);  
        ShowControl();
    }
    else
    {
        HideModule(MP_CPS_ONOFF_END);
        HideControl();
    }
}

void ReadReg()
{
    LCD_FRAME *pFrame = (LCD_FRAME *)&RecvBuf1[LcdFrameloc];
    WORD addr = SwWord(pFrame->Addr);
    //BYTE res;
    DebugInt(addr);
    
    switch (addr)
    {
        // 主界面
        //case REG_TIME_BTN: GetLcdTime();                            break;  // 点击时间

        // 修改时间
        //case REG_ADJ_TIME:   GetInputTime();                        break;    // 时间修改完成，点击了返回按钮
        //case REG_DATE_TIME:  SetLcdTime();                          break;    // 修改屏幕RTC      
        //case LCD_REG_RTC:    SetCurTime();                          break;   // 获取屏幕时间返回     
        //CPS显示
        case REG_CPS_CHECK:  SysParam.Cps_Check = (BYTE)PopWord(); Cps_Check(); break;   // CPS通道显示按钮
        case REG_CPS_ONOFF:  SysParam.Cps_Enable = (BYTE)PopWord();   break;   // CPS通道按钮
    }

}


void WriteReg()
{
    #if 0
    LCD_FRAME *pFrame = (LCD_FRAME *)&RecvBuf1[LcdFrameloc];
    CString str;
    str.Format(_T("Write:0x%04X\r\n"), SwWord(pFrame->Addr));
    Log(str);
    #endif
}

