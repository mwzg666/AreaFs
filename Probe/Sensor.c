#include "main.h"
#include "Sensor.h"
#include "CalcDoseRate.h"
#include "system.h"

extern LP_SYSTEM_STTAE SysRunState;
extern SYS_PARAM xdata SysParam;
extern float My_Doserate;


u8 GDoseSeg = LOW_SEG;       //当前处在的段

const NH_PARAM NhDef[NH_COUNT] =
{
    {0,         0  },
    {1.85,      0.8},
    {14.71,     8.0},
    {144.8,     80},
    {817.14,    500},
    {1220.08,   800},
    {3173.77,   3000},
    {8561.06,   8000},
    {10165.63,  30000},
    {11310.53,  42000},
    {17409.7,   130000},
        
    {30000.0,   300000},
    {50000.0,   500000},
    {80000.0,   800000},
    {200000.0,  2000000},
    {500000.0,  10000000},    // 10Sv/h  
};



#ifdef POE_SEN
#define USE_MID_FIX  80     // 分段拟合
#define USE_LOW_USV 1000        //1mSv
#define USE_HIGH_USV 2000        //1.2mSv
#else
#define USE_LOW_USV   1000        //1mSv
#define USE_HIGH_USV 2000        //2mSv
#endif
extern u32 GetCounter(void);

u32 Low_CPS;

static float LowSmothCPS;
static float LowNOSmothCPS;
static float LowSumCPS;

static float HighSmothCPS;
static float HighNOSmothCPS;
static float HighSumCPS;


float parama,paramb,paramc,paramd;

void SensorInit(void)
{
    InitArr();
    memset((void*)&SysRunState.s_DoseMSG,0,sizeof(STU_DOSERATE));
    SysRunState.RealRange = LOW_SEG;
    //UseLowParam(0);
    RefreshParam();
    SensorMeasureBegin();//开始测量 
}

void RefreshParam(void)
{
    if(SysRunState.RealRange == HIG_SEG)
    {
//        parama = SysParam.Lp_Param.s_Jiaozhun.GAO_A;//33.6
//        paramb = SysParam.Lp_Param.s_Jiaozhun.GAO_B;//0.000023;
//        paramc = SysParam.Lp_Param.s_Jiaozhun.GAO_C;
//        #ifdef POE_SEN
//        paramd = SysParam.Lp_Param.s_Jiaozhun.GAO_D;
//        #endif
		UseHighParam(0);
    }
    else
    {
        UseLowParam(0);
//        parama = SysParam.Lp_Param.s_Jiaozhun.DI_A;//0.63;
//        paramb = SysParam.Lp_Param.s_Jiaozhun.DI_B;//0.00019;
//        paramc = SysParam.Lp_Param.s_Jiaozhun.DI_C;
//        #ifdef POE_SEN
//        paramd = SysParam.Lp_Param.s_Jiaozhun.DI_D;
//        #endif
    }
}

void SensorMeasureBegin(void)
{ 
    switch(ProbeSwitch)
    {
        case 0: GetCounter();
                GetHightCounter();
                break;
        case 1: GetNaICounter();
                break;
    }
}

#ifdef POE_SEN
void UseLowParam(float cps)
{
    if (cps < 101.23)			//40
    {
        parama = SysParam.Lp_Param.s_Jiaozhun.DI_A;		// 0.0552364572191522;   
        paramb = SysParam.Lp_Param.s_Jiaozhun.DI_B;		// 0.325776472823953; 	
        paramc = SysParam.Lp_Param.s_Jiaozhun.DI_C;		// 0.00103232522627559;	
        paramd = SysParam.Lp_Param.s_Jiaozhun.DI_D;		// -3.46972129850684E-06;
    }
    else if(cps < 1776.69)		//800
    {
        parama =  SysParam.Lp_Param.s_Jiaozhun.MID_A;
        paramb =  SysParam.Lp_Param.s_Jiaozhun.MID_B;
        paramc =  SysParam.Lp_Param.s_Jiaozhun.MID_C;
        paramd =  SysParam.Lp_Param.s_Jiaozhun.MID_D;
    }
	else						//8m
	{
		parama = 59.4796602798306;
		paramb = 0.303667994175407;
		paramc = 6.68351922872401E-05;
		paramd = 1.04497920295577E-09;
	}
}
void UseHighParam(float cps)
{
    if (cps < 595.92)				//4m
    {
        parama = SysParam.Lp_Param.s_Jiaozhun.GAO_A;	//3.62608397252143;		
        paramb = SysParam.Lp_Param.s_Jiaozhun.GAO_B;	//6.3652408143604;		
        paramc = SysParam.Lp_Param.s_Jiaozhun.GAO_C;	//0.00160225788737462;	
        paramd = SysParam.Lp_Param.s_Jiaozhun.GAO_D;	//-1.72769678770162E-06;	
    }
    else if(cps < 11328.44)			//80m
    {
        parama  = 236.789864654085;
        paramb  = 6.22196259158567;
        paramc  = 0.000183022470193148;
        paramd  = -9.77290205077047E-09;
    }
	else if(cps < 40742.44)			//400m
    {
        parama  = 3233.46176012245;
        paramb  = 6.14745265519994;
        paramc  = 4.29620223191468E-05;
        paramd  = 1.10881806696143E-09;
    }
	else							//2Sv
	{
		parama  = 16403.0066342549;
        paramb  = 4.9781477886218;
        paramc  = 7.64639328950134E-05;
        paramd  = 7.962274359201E-10;
	}
}

#endif


// 单低量程GM管
void UseSLParam(float cps)
{
    if (cps <= 14.71)
    {
        parama = 9.76996261670138E-15;
        paramb = 0.416404571866349;
        paramc = 0.00866370841409324;
        paramd = 0;
    }
    else if (cps <= 1220.8)
    {
        parama = 0.0167463070762324;
        paramb = 0.541654614387292;
        paramc = 7.14371777215714E-05;
        paramd = 1.77334918007136E-08;
    }
    else if (cps <= 8561.06)
    {
        parama = -679.93211770926;
        paramb = 1.24525771071468;
        paramc = -2.70261510263897E-05;
        paramd = 0;
    }
    else if (cps <= 11310.53)
    {
        parama = -211604.529633801;
        paramb = 35.7075232732049;
        paramc = -0.00117461680924023;
        paramd = 0;
    }
    else
    {
        parama = -121190.506249211;
        paramb = 14.4281926885134;
        paramc = 0;
        paramd = 0;
    }
}


float DrFix(float dr)
{
    if (dr < 1.0)
    {
        return (dr*SysParam.Lp_Param.Fix[0]);
    }
    else if (dr < 10.0)
    {
        return (dr*SysParam.Lp_Param.Fix[1]);
    }
    else if (dr < 100.0)
    {
        return (dr*SysParam.Lp_Param.Fix[2]);
    }
    else if (dr < 1000.0)
    {
        return (dr*SysParam.Lp_Param.Fix[3]);
    }
    else if (dr < 10000.0)
    {
        return (dr*SysParam.Lp_Param.Fix[4]);
    }
    else if (dr < 100000.0)
    {
        return (dr*SysParam.Lp_Param.Fix[5]);
    }
    else if (dr < 1000000.0)
    {
        return (dr*SysParam.Lp_Param.Fix[6]);
    }
    else if (dr < 10000000.0)
    {
        return (dr*SysParam.Lp_Param.Fix[7]);
    }
    else if (dr < 100000000.0)
    {
        return (dr*SysParam.Lp_Param.Fix[8]);
    }
    else if (dr < 1000000000.0)
    {
        return (dr*SysParam.Lp_Param.Fix[9]);
    }
    else if (dr < 10000000000.0)
    {
        return (dr*SysParam.Lp_Param.Fix[10]);
    }

    return dr;
}

void CaptureSensorPluseCounter(void)
{
    float uSvh = 0;
    static float OldDr = 0;
    float nSmothCPS = 0;
    BYTE i = 0;
	
    switch(ProbeSwitch)
    {
        case 0:
                LowSumCPS = GetCounter();
                HighSumCPS = GetHightCounter();

                if(LowSumCPS == 0)
                {
                    SysRunState.LChannelNoCountTime++;
                }
                else
                {
                    SysRunState.LChannelNoCountTime = 0;
                }
                
                SysRunState.s_CPS.CPS1 = LowSumCPS;
                SysRunState.s_CPS.CPS2 = HighSumCPS;
                if(SysRunState.RealRange == LOW_SEG)
                {
                    nSmothCPS = SmothCpsCalcRate(LowSumCPS, OldDr, &uSvh);
                }
                else
                {
                    nSmothCPS = SmothCpsCalcRate(HighSumCPS, OldDr, &uSvh);
                }
               
                if((uSvh > USE_HIGH_USV)&&(SysRunState.RealRange == LOW_SEG))
                {
                    SysRunState.RealRange = HIG_SEG;
					
					//UseHighParam(nSmothCPS);
//                    parama = SysParam.Lp_Param.s_Jiaozhun.GAO_A;
//                    paramb = SysParam.Lp_Param.s_Jiaozhun.GAO_B;
//                    paramc = SysParam.Lp_Param.s_Jiaozhun.GAO_C;
//                    #ifdef POE_SEN
//                    paramd = SysParam.Lp_Param.s_Jiaozhun.GAO_D;
//                    #endif
                    ClearCounter();
                }
                else if((uSvh < USE_LOW_USV)&&(SysRunState.RealRange == HIG_SEG))
                {
                    SysRunState.RealRange = LOW_SEG;
                    #ifdef POE_SEN
                    //UseLowParam(nSmothCPS);
                    #else
                    parama = SysParam.Lp_Param.s_Jiaozhun.DI_A;
                    paramb = SysParam.Lp_Param.s_Jiaozhun.DI_B;
                    paramc = SysParam.Lp_Param.s_Jiaozhun.DI_C;
                    paramd = SysParam.Lp_Param.s_Jiaozhun.DI_D;
                    #endif
                    
                    ClearCounter();
                }
                OldDr = uSvh;
                uSvh = DrFix(uSvh);
                if (SysRunState.RealRange == LOW_SEG)
                {
                    SysRunState.s_DoseMSG.DoseRate = uSvh*SysParam.Lp_Param.s_SysParam.Canshu1;
                }
                else
                {
                    SysRunState.s_DoseMSG.DoseRate = uSvh*SysParam.Lp_Param.s_SysParam.Canshu2;
                }
                
            break;
        case 1:
                LowSumCPS = GetNaICounter();
                if(LowSumCPS == 0)
                {
                    SysRunState.LChannelNoCountTime++;
                }
                else
                {
                    SysRunState.LChannelNoCountTime = 0;
                }
                
                SysRunState.s_CPS.CPS1 = LowSumCPS;
                SysRunState.s_CPS.CPS2 = LowSumCPS;
                
               
                if(SysRunState.RealRange == LOW_SEG)
                { 
                    nSmothCPS = SmothCpsCalcRate(LowSumCPS, OldDr, &uSvh);

                    #ifdef POE_SEN
                    //UseLowParam(uSvh);
                    #else
                    parama = SysParam.Lp_Param.s_Jiaozhun.DI_A;
                    paramb = SysParam.Lp_Param.s_Jiaozhun.DI_B;
                    paramc = SysParam.Lp_Param.s_Jiaozhun.DI_C;
                    #endif
                }
                OldDr = uSvh;
                uSvh = DrFix(uSvh);
                SysRunState.s_DoseMSG.DoseRate = uSvh*SysParam.Lp_Param.s_SysParam.Canshu1;
            break;
     }
    if(SysRunState.s_DoseMSG.DoseRate < 0)
    { 
		SysRunState.s_DoseMSG.DoseRate = 0;
    }
    CalcAlarmState(&SysRunState);
    My_Doserate = SysRunState.s_DoseMSG.DoseRate;
    Set_LP_DoseRateUnit();   
    SysRunState.s_DoseMSG.Dose += SysRunState.s_DoseMSG.DoseRate/3600.0f;
}

float Get_Low_Counter(void)
{
    return LowNOSmothCPS;
}

float Get_Low_Smooth_Counter(void)
{
    return LowSmothCPS;
}


void Set_LP_DoseRateUnit()
{
    if(My_Doserate < 1000)
    {
        if(My_Doserate <= 100)
        {
            RunStatus.Unit = Unit_uSv;
        }
        else
        {
            RunStatus.Unit = Unit_mSv;
            My_Doserate = My_Doserate / 1000;
        }
    }
    else if(My_Doserate < 1000000)
    {
        My_Doserate = My_Doserate / 1000;
        if(My_Doserate <= 100)
        {
            RunStatus.Unit = Unit_mSv;
        }
        else
        {
            RunStatus.Unit = Unit_Sv;
            My_Doserate = My_Doserate / 1000;
        }
        
    }
    else
    {
        RunStatus.Unit = Unit_Sv;
        if(My_Doserate > 100000000)
        {
            My_Doserate = 100000000;
        }
        My_Doserate = My_Doserate / 1000000;
        
    }
}

u16 CalcAlarmState(LP_SYSTEM_STTAE *me)
{
    float ov;
    
    #if 0
    /* 剂量当量报警检查 */    
    if ((me->s_DoseMSG.Dose >= SysParam.Lp_Param.s_Alarm.DoseAlarm)
        &&(SysParam.Lp_Param.s_Alarm.DoseAlarm > 0)) 
    { 
        me->s_DoseMSG.State |= DOSE_ALARM_BIT;
    } 
    else
    {
        me->s_DoseMSG.State &= ~DOSE_ALARM_BIT;
    }
    
    /* 剂量当量预警检查 */    
    if( (me->s_DoseMSG.Dose >= SysParam.Lp_Param.s_Alarm.DosePreAlarm)
        &&(SysParam.Lp_Param.s_Alarm.DosePreAlarm > 0)
        &&(me->s_DoseMSG.Dose < SysParam.Lp_Param.s_Alarm.DoseAlarm) ) 
    { 
        me->s_DoseMSG.State |= DOSE_PRE_ALARM_BIT;
    } 
    else
    {
        me->s_DoseMSG.State &= ~DOSE_PRE_ALARM_BIT;
    }
    #endif    
    
    /* 剂量当量率报警检查 */    
    if ( (me->s_DoseMSG.DoseRate >= SysParam.Lp_Param.s_Alarm.DoseRateAlarm)
        &&(SysParam.Lp_Param.s_Alarm.DoseRateAlarm > 0) ) 
    { 
        if((++me->DoseRateAlarmCnt) >= MIB_CST_DOSERATE_THRESHOLD_ALARM) 
        {   
            //连续两次报警则认为报警
            me->s_DoseMSG.State |= DOSERATE_ALARM_BIT;
        }
    } 
    else 
    {
        me->DoseRateAlarmCnt= 0x0;
        me->s_DoseMSG.State &= ~DOSERATE_ALARM_BIT;
    }
    
    /* 剂量当量率预警检查 */    
    if ( (me->s_DoseMSG.DoseRate >= SysParam.Lp_Param.s_Alarm.DoseRatePreAlarm)
        &&(me->s_DoseMSG.DoseRate < SysParam.Lp_Param.s_Alarm.DoseRateAlarm) )
    { 
        if((++me->DoseRatePreAlarmCnt) >= MIB_CST_DOSERATE_THRESHOLD_WARNING) 
        {
            //连续两次报警则认为报警
            me->s_DoseMSG.State |= DOSERATE_PRE_ALARM_BIT;
        }
    } 
    else 
    {
        me->DoseRatePreAlarmCnt= 0x0;
        me->s_DoseMSG.State &= ~DOSERATE_PRE_ALARM_BIT;
    }

    
    // 过载
    if (SysRunState.RealRange == LOW_SEG) 
    {
        // 单GM管
        ov = 200.0*1000;
    }
    else
    {
        ov = 10.0*1000*1000;
    }
    
    if ((me->s_DoseMSG.Dose >= 10.0*1000*1000)||(me->s_DoseMSG.DoseRate >= ov))
    {
        me->s_DoseMSG.State |= OVER_RANGE_BIT;
        me->s_DoseMSG.DoseRate = ov;
    }
    else
    {
        me->s_DoseMSG.State &= ~OVER_RANGE_BIT;
    }
    
    if(((SysRunState.LChannelNoCountTime > 60)&&(SysRunState.RealRange == LOW_SEG))
      ||(SysRunState.LChannelNoCountTime > 600))//1分钟无数据,探测器异常
    {
        me->s_DoseMSG.State |= LOW_BACK_BIT;
    }
    else
    {
        me->s_DoseMSG.State &= ~LOW_BACK_BIT;
    }
    
    if(me->s_CPS.State != me->s_DoseMSG.State)//报警信息发生变化则，重启蜂鸣器报警
    {
        SysRunState.keyValue = 0;
    }
    
    me->s_CPS.State = me->s_DoseMSG.State;
    return true;
}

