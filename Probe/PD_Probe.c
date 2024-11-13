#include "main.h"
#include "ModBusHost.h"
#include "ModBusDev.h"
#include "PD_Probe.h"
#include "ModBus.h"

extern HOST_SEND_FRAME xdata  HostSendFrame;   // 发送帧
extern DEVICE_READ_ACK xdata  ReadAckFrame;    // 读接收帧
extern DEVICE_WRITE_ACK xdata WriteAckFrame;  // 写接收帧
extern MODBUS_PD_PARAM xdata ModBusPD_Param;

extern SYS_PARAM xdata SysParam;
extern PROBE_REGADDR xdata ProbeRegAddr;
extern float My_PDDoserate;



// Count :  寄存器个数
void SendReadPD(void)
{
    WORD RegCnt = 12;
    HostSendCmd(RS485, (BYTE)SysParam.Pd_param.Address,CMD_READ_REG, MODBUS_DOSERATE_ADD, RegCnt, NULL);
}

void SendReadPD_DoseRate(void)
{
    WORD RegCnt = 12;
    HostSendCmd(RS485, (BYTE)SysParam.Pd_param.Address,CMD_READ_REG, MODBUS_DOSERATE_ADD, RegCnt, NULL);
}
void SendReadPD_Alarm(void)
{
    WORD RegCnt = 8;
    HostSendCmd(RS485, (BYTE)SysParam.Pd_param.Address,CMD_READ_REG, MODBUS_ALARM_ADD, RegCnt, NULL);
}
void SendReadPD_Thr(void)
{
    WORD RegCnt = 4;
    HostSendCmd(RS485, (BYTE)SysParam.Pd_param.Address,CMD_READ_REG, MODBUS_THR_ADD, RegCnt, NULL);
}
void SendReadPD_fix(void)
{
    WORD RegCnt = 32;
    HostSendCmd(RS485, (BYTE)SysParam.Pd_param.Address,CMD_READ_REG, MODBUS_FACTOR_ADD, RegCnt, NULL);
}
void SendReadPD_Info(void)
{
    WORD RegCnt = 48;
    HostSendCmd(RS485, (BYTE)SysParam.Pd_param.Address,CMD_READ_REG, MODBUS_DEV_INFO, RegCnt, NULL);
}
void SendReadPD_Address(void)
{
    WORD RegCnt = 1;
    HostSendCmd(RS485, (BYTE)SysParam.Pd_param.Address,CMD_READ_REG, MODBUS_DEV_ADD, RegCnt, NULL);
}
void SendReadPD_Ver(void)
{
    WORD RegCnt = 1;
    HostSendCmd(RS485, (BYTE)SysParam.Pd_param.Address,CMD_READ_REG, MODBUS_DEV_VER, RegCnt, NULL);
}

void WritePD_Alarm()
{
    WORD RegCnt = 8;
    SyncModBusDev();
    HostSendCmd(RS485,1, CMD_WRITE_REG, MODBUS_ALARM_ADD, RegCnt, (BYTE *)&ModBusPD_Param.s_Alarm);
}

void WritePD_Thr()
{
    WORD RegCnt = 4;
    SyncModBusDev();
    HostSendCmd(RS485,1, CMD_WRITE_REG, MODBUS_THR_ADD, RegCnt, (BYTE *)&ModBusPD_Param.s_Thr);
}

void WritePD_fix()
{
    WORD RegCnt = 32;
    SyncModBusDev();
    HostSendCmd(RS485,1, CMD_WRITE_REG, MODBUS_FACTOR_ADD, RegCnt, (BYTE *)&ModBusPD_Param.fix);
}

void WritePD_Info()
{
    WORD RegCnt = 48;
    SyncModBusDev();
    HostSendCmd(RS485,1, CMD_WRITE_REG, MODBUS_DEV_INFO, RegCnt, (BYTE *)&ModBusPD_Param.s_PdInfo);
}

void WritePD_Address()
{
    WORD RegCnt = 1;
    SyncModBusDev();
    HostSendCmd(RS485,1, CMD_WRITE_REG, MODBUS_DEV_ADD, RegCnt, (BYTE *)&ModBusPD_Param.Address);
}


void ModBusPDSave()
{
    BYTE i = 0;
    SysParam.Pd_param.s_DoseRate.DoseRate = ModBusPD_Param.s_DoseRate.DoseRate;
    SysParam.Pd_param.s_DoseRate.DoseTotal = ModBusPD_Param.s_DoseRate.DoseTotal;
    SysParam.Pd_param.s_DoseRate.Cps1 = ModBusPD_Param.s_DoseRate.Cps1;
    SysParam.Pd_param.s_DoseRate.Cps2 = ModBusPD_Param.s_DoseRate.Cps2;
    SysParam.Pd_param.s_DoseRate.Cps3 = ModBusPD_Param.s_DoseRate.Cps3;
    //SysParam.Pd_param.s_DoseRate.DevSt = ModBusPD_Param.s_DoseRate.DevSt;
    //SysParam.Pd_param.Address = ModBusPD_Param.Address; 

    #if 0
    memcpy(SysParam.Pd_param.s_PdInfo.Index,ModBusPD_Param.s_PdInfo.Index,32);
    memcpy(SysParam.Pd_param.s_PdInfo.Location,ModBusPD_Param.s_PdInfo.Location,32);
    memcpy(SysParam.Pd_param.s_PdInfo.Name,ModBusPD_Param.s_PdInfo.Name,32);
    
    
    SysParam.Pd_param.Ver = ModBusPD_Param.Ver; 
    
    
    SysParam.Pd_param.s_Alarm.DoseRatePreAlarm = ModBusPD_Param.s_Alarm.DoseRatePreAlarm;
    SysParam.Pd_param.s_Alarm.DoseRateAlarm = ModBusPD_Param.s_Alarm.DoseRateAlarm;
    SysParam.Pd_param.s_Alarm.DosePreAlarm = ModBusPD_Param.s_Alarm.DosePreAlarm;
    SysParam.Pd_param.s_Alarm.DoseAlarm = ModBusPD_Param.s_Alarm.DoseAlarm;
    for(i = 0;i<3;i++)
    {
        SysParam.Pd_param.s_Thr.Det_Thr[i] = ModBusPD_Param.s_Thr.Det_Thr[i];
    }
    SysParam.Pd_param.s_Thr.AnalogChannel = ModBusPD_Param.s_Thr.AnalogChannel;
    for(i = 0;i<16;i++)
    {
        SysParam.Pd_param.fix[i] = ModBusPD_Param.fix[i];
    }
    #endif
    
    WriteParam();
}

bool WritePD_Ack()
{
   return true;
}

bool Calc_PD_AlarmState()
{
    static u16 DrAlarmCnt = 0;
    static u16 DrPreAlarmCnt = 0;
	SysParam.Pd_param.s_DoseRate.DevSt = 0;
    #if 0
    /* 剂量当量报警检查 */    
    if ((SysParam.Pd_param.s_DoseRate.DoseTotal >= SysParam.Pd_param.s_Alarm.DoseAlarm)
        &&(SysParam.Pd_param.s_Alarm.DoseAlarm > 0)) 
    { 
        SysParam.Pd_param.s_DoseRate.DevSt |= DOSE_ALARM_BIT;
    } 
    else
    {
        SysParam.Pd_param.s_DoseRate.DevSt &= ~DOSE_ALARM_BIT;
    }
    
    /* 剂量当量预警检查 */    
    if( (SysParam.Pd_param.s_DoseRate.DoseTotal >= SysParam.Pd_param.s_Alarm.DosePreAlarm)
        &&(SysParam.Pd_param.s_Alarm.DosePreAlarm > 0)
        &&(SysParam.Pd_param.s_DoseRate.DoseTotal < SysParam.Pd_param.s_Alarm.DoseAlarm) ) 
    { 
        SysParam.Pd_param.s_DoseRate.DevSt |= DOSE_PRE_ALARM_BIT;
    } 
    else
    {
        SysParam.Pd_param.s_DoseRate.DevSt &= ~DOSE_PRE_ALARM_BIT;
    }
    #endif    
    
    /* 剂量当量率报警检查*/    
    if ( (SysParam.Pd_param.s_DoseRate.DoseRate >= SysParam.Pd_param.s_Alarm.DoseRateAlarm)
        &&(SysParam.Pd_param.s_Alarm.DoseRateAlarm > 0) ) 
    { 
        if((++DrAlarmCnt) >= MIB_CST_DOSERATE_THRESHOLD_ALARM) 
        {   
            //连续两次报警则认为报警
           SysParam.Pd_param.s_DoseRate.DevSt |= DOSERATE_ALARM_BIT;
        }
    } 
    else 
    {
        DrAlarmCnt= 0x0;
        SysParam.Pd_param.s_DoseRate.DevSt &= ~DOSERATE_ALARM_BIT;
    }
    
    /* 剂量当量率预警检查 */    
    if ( (SysParam.Pd_param.s_DoseRate.DoseRate >= SysParam.Pd_param.s_Alarm.DoseRatePreAlarm)
        &&(SysParam.Pd_param.s_DoseRate.DoseRate < SysParam.Pd_param.s_Alarm.DoseRateAlarm) )
    { 
        if((++DrPreAlarmCnt) >= MIB_CST_DOSERATE_THRESHOLD_WARNING) 
        {
            //连续两次报警则认为报警
            SysParam.Pd_param.s_DoseRate.DevSt |= DOSERATE_PRE_ALARM_BIT;
        }
    } 
    else 
    {
        DrPreAlarmCnt= 0x0;
        SysParam.Pd_param.s_DoseRate.DevSt &= ~DOSERATE_PRE_ALARM_BIT;
    }

    #if 0
    if (SysParam.Pd_param.s_DoseRate.DoseTotal >= 10.0*1000*1000)
    {
        SysParam.Pd_param.s_DoseRate.DevSt |= OVER_RANGE_BIT;
        SysParam.Pd_param.s_DoseRate.DoseTotal = 10.0*1000*1000;
    }
    else
    {
        SysParam.Pd_param.s_DoseRate.DevSt &= ~OVER_RANGE_BIT;
    }
    #endif
    return true;
}



void Set_PD_DoseRateUnit()
{
    if(My_PDDoserate < 1000)
    {
          if(My_PDDoserate < 100)
        {
            RunStatus.Unit = Unit_uSv;
        }
        else
        {
            RunStatus.Unit = Unit_mSv;
            My_PDDoserate = My_PDDoserate / 1000;
        }
    }
    else if(My_PDDoserate < 1000000)
    {
        My_PDDoserate= My_PDDoserate / 1000;
        if(My_PDDoserate < 100)
        {
            RunStatus.Unit = Unit_mSv;
        }
        else
        {
            RunStatus.Unit = Unit_Sv;
            My_PDDoserate = My_PDDoserate / 1000;
        }
    }
    else
    {
        RunStatus.Unit = Unit_Sv;
        if(My_PDDoserate > 100000000)
        {
            My_PDDoserate = 100000000;
        }
        My_PDDoserate = My_PDDoserate / 1000000;
    }
}



BYTE ReadPD_Ack()
{
    WORD i = 0;
    WORD dat = 0;   
    if(ReadAckFrame.DataLen <= 0)
    {
        return false;
    }
   #if 0
    if(ReadAckFrame.DataLen == 2)
    { 
        memcpy(&dat, ReadAckFrame.Data, 2);
        if(dat >= 100)
        {
            memcpy(&ModBusPD_Param.Ver, ReadAckFrame.Data, 2);
        }
        else
        {
            memcpy(&ModBusPD_Param.Address, ReadAckFrame.Data, 2);
        }
    } 
    
    else if(ReadAckFrame.DataLen == sizeof(PD_INFO))
    {
        memcpy(&ModBusPD_Param.s_PdInfo, ReadAckFrame.Data,sizeof(PD_INFO));
    } 
    #endif
    else if(ReadAckFrame.DataLen == sizeof(ModBusPD_Param.fix))
    {
        memcpy(&ModBusPD_Param.fix, ReadAckFrame.Data,sizeof(ModBusPD_Param.fix));
    }
    else if(ReadAckFrame.DataLen == sizeof(PD_THR))
    {
        memcpy(&ModBusPD_Param.s_Thr, ReadAckFrame.Data,sizeof(PD_THR));
    }
    else if(ReadAckFrame.DataLen == sizeof(PD_ALARM))
    {
        memcpy(&ModBusPD_Param.s_Alarm, ReadAckFrame.Data,sizeof(PD_ALARM));
    }
  
    if(ReadAckFrame.DataLen == sizeof(PD_DOSERATE))
    {
        memcpy(&ModBusPD_Param.s_DoseRate, ReadAckFrame.Data,sizeof(PD_DOSERATE));
    }
    ModBusPDSave();
    return true;
}


void PDProbe_Ack(BYTE *Buf, BYTE Len)
{
    if (!ValidRtuFrame(Buf, Len))
    {
        //DebugMsg("Comm err\r\n",10);
        return;
    }

    memset(&ReadAckFrame, 0, sizeof(DEVICE_READ_ACK));
    memcpy(&ReadAckFrame, Buf, Len);
   
   if(ReadAckFrame.Address != SysParam.Pd_param.Address)
    {
        return;
    }
    switch(ReadAckFrame.FunctionCode)
    {      
        case CMD_READ_REG: 
        {
            ReadPD_Ack(); 
            Calc_PD_AlarmState();
            My_PDDoserate = SysParam.Pd_param.s_DoseRate.DoseRate;
            Set_PD_DoseRateUnit();
            break;
        }
        case CMD_WRITE_REG:  WritePD_Ack() ;  break;
    }
}

