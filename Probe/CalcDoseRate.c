#include <math.h>
#include "system.h"
#include "CalcDoseRate.h"
#include "Sensor.h"

extern float parama,paramb,paramc,paramd;
extern LP_SYSTEM_STTAE SysRunState;
extern SYS_PARAM xdata SysParam;



// 取平均值
#define CT 60   
DWORD CpsArr[CT];
static BOOL Full = FALSE;
static BYTE ArrCnt = 0;
static float CpsHis = 0;

BYTE BackTimer = 0;
float BackAv[10] = {0.0};    //  本底平滑值
BYTE BackCnt = 0;

void InitArr()
{
    BYTE i;
    // 初始状态填15 个  1
    memset(CpsArr,0,sizeof(CpsArr));
    for (i=0;i<CT;i+=4)
    {
        CpsArr[i] = 1;
    }
    Full = TRUE;
    ArrCnt = CT;
    CpsHis = 15.0/CT;
}

// 将计数压入堆栈
void PushCps(DWORD cps)
{
    BYTE i;
    if (ArrCnt >= CT)
    {
        Full = TRUE;
        for (i=1;i<CT;i++)
        {
            CpsArr[i-1] = CpsArr[i];
        }
        CpsArr[CT-1] = cps;
    }
    else
    {
        CpsArr[ArrCnt++] = cps;
    }
}

void ClearCounter()
{
    memset(CpsArr,0,sizeof(CpsArr));
    Full = FALSE;
    ArrCnt = 0;
    CpsHis = 0;
}

// 本底平滑10分钟
float BackPH()
{
    float r = 0;
    BYTE i;
    
    if (++BackTimer >= CT)
    {
        BackTimer = 0;
        BackAv[BackCnt++] = CounterPH();
        if (BackCnt >= 10)
        {
            // 把后面的往后移
            for (i=1;i<10;i++)
            {
                BackAv[i-1] = BackAv[i];
            }
            
            BackCnt = 9;
        }
    }

    if (BackCnt > 0)
    {
        r = 0;
        for (i=0;i<BackCnt;i++)
        {
            r += BackAv[i];
        }
        r /= BackCnt;
    }
    else
    {
        r = CounterPH();
    }

    CpsHis = r;
    return r;
}


// 计数平滑
float CounterPH()
{
    #define BK_CPS  0.2
    float Val = 0.0;
    BYTE i;
    
    Val = 0.0;
    if (Full)
    {
        for (i=0;i<CT;i++)
        {
            Val += CpsArr[i];
        }
        Val = Val/CT;
    }
    else
    {
        if (ArrCnt == 0)
        {
            CpsHis = BK_CPS;
            return -1;
        }
        else
        {
            for (i=0;i<ArrCnt;i++)
            {
                Val += CpsArr[i];
            }
            Val = Val/ArrCnt;
        }
    }

    if (Val < BK_CPS)
    {
        Val = BK_CPS;
    }
    
    CpsHis = Val;
    return Val;
}


float GetHis(BYTE time)
{
    BYTE i;
    float ret = 0.0;
    if (Full)
    {
        for (i=CT-time;i<CT;i++)
        {
            ret += CpsArr[i];
        }

        return (ret/time);
    }
    else
    {
        if (time > ArrCnt)
        {
            for (i=0;i<ArrCnt;i++)
            {
                ret += CpsArr[i];
            }

            return (ret/ArrCnt);
        }
        else
        {
            for (i=ArrCnt-time;i<ArrCnt;i++)
            {
                ret += CpsArr[i];
            }

            return (ret/time);
        }
    }

    //return 0.0;
}


// time :  保留最后计数的时间
void ResvCps(BYTE time)
{
    BYTE i;
    if (Full)
    {
        for (i=0;i<CT;i++)
        {
            if (i<time)
            {
                CpsArr[i] = CpsArr[CT-time+i];
            }
            else
            {
                CpsArr[i] = 0;
            }
        }
        ArrCnt = time;
        Full = FALSE;
    }
    else
    {
        if (time < ArrCnt)
        {
            for (i=0;i<ArrCnt;i++)
            {
                if (i<time)
                {
                    CpsArr[i] = CpsArr[ArrCnt-time+i];
                }
                else
                {
                    CpsArr[i] = 0;
                }
            }

            ArrCnt = time;
        }
        else
        {
            // 全部保留，就不做处理了
        }
    }
}

// 计数压入队列
void AddCounter(DWORD Cps)
{
    PushCps(Cps);   
}


BOOL CpsFilter(DWORD Cps)
{
    BYTE i;
    
    // 剂量率3-5秒响应
    static DWORD BigCpsErr[5] = {0};
    static BYTE BigErrCnt = 0;
    BYTE BigCpsTime = 0;
    
    static DWORD SmCpsErr[5] = {0};
    static BYTE SmErrCnt = 0;

    // 大于2CPS(0.8uSv/h左右) 才判断陡升 -- 盖格管的最小剂量为 1uSv/h
    if ( (Cps>(CpsHis+3*sqrt(CpsHis)))  
          && (Cps >= 2) )
    {
        if (Cps >= 4.0)   
        {
            BigCpsTime = 3;  // 3秒响应
        }
        else        
        {
            BigCpsTime = 5;  // 5秒响应
        }
        

        SmErrCnt = 0;
        BigCpsErr[BigErrCnt++] = Cps;
        if (BigErrCnt >= BigCpsTime)
        {
            // 连续3个大计数则算陡升
            ClearCounter();
            for (i=0;i<BigErrCnt;i++)
            {
                AddCounter(BigCpsErr[i]);
            }
            
            BigErrCnt = 0; 
            return TRUE;
        }

        return FALSE;
    }
    
    // 小计数
    if (CpsHis > 9)  // >4uSv/h 
    {
        if (Cps < (CpsHis-3*sqrt(CpsHis))) 
        {
            BigErrCnt = 0;
            SmCpsErr[SmErrCnt++] = Cps;
            if (SmErrCnt == 3)
            {
                // 连续3个小计数则算陡降
                ClearCounter();
                for (i=0;i<SmErrCnt;i++)
                {
                    AddCounter(SmCpsErr[i]);
                }
                SmErrCnt = 0;
            }
            return TRUE;
        }
    }
    else if (CpsHis > 4)   // 2.5uSv/h 左右
    {
        if (Cps < (CpsHis-2*sqrt(CpsHis))) 
        {
            BigErrCnt = 0;
            SmCpsErr[SmErrCnt++] = Cps;
            if (SmErrCnt == 5)
            {
                // 连续5个小计数则算陡降
                ClearCounter();
                for (i=0;i<SmErrCnt;i++)
                {
                    AddCounter(SmCpsErr[i]);
                }
                SmErrCnt = 0;
            }
            return TRUE;
        }
    }


    // 不属于突变的情况就直接进入队列
    AddCounter(Cps);
    BigErrCnt = 0;
    SmErrCnt = 0;

    return FALSE;
}

float GetNaIDoseRate(float cps)
{
    float dr = 0;

    dr = NAI_K3 * cps * cps * cps + NAI_K2 * cps * cps + NAI_K1 * cps + NAI_K0;
    if(dr < 0)
    {
        dr = 0;
    }

    return dr;   // 单位 uSv/h
}


#ifdef P2P_NH
float CpsToUsv_h(char mode, float count)
{
    float ret;
    
    BYTE i;
    float a,b;
    
    if (mode == 1)
    {
        // 点对点数据拟合
        for (i=1;i<NH_COUNT;i++)
        {
            if (count < SysParam.Lp_Param.NhParam[i].cps)
            {
                a = (SysParam.Lp_Param.NhParam[i].dr - SysParam.Lp_Param.NhParam[i-1].dr) / 
                     (SysParam.Lp_Param.NhParam[i].cps - SysParam.Lp_Param.NhParam[i-1].cps);
                b = SysParam.Lp_Param.NhParam[i].dr - SysParam.Lp_Param.NhParam[i].cps * a;
                break;
            }
        }

        if (i == NH_COUNT) 
        {
            // 计数大于最后一个拟合点，就用最后一个
            a = (SysParam.Lp_Param.NhParam[NH_COUNT-1].dr - SysParam.Lp_Param.NhParam[NH_COUNT-2].dr) / 
                (SysParam.Lp_Param.NhParam[NH_COUNT-1].cps - SysParam.Lp_Param.NhParam[NH_COUNT-2].cps);
            b = SysParam.Lp_Param.NhParam[NH_COUNT-1].dr - SysParam.Lp_Param.NhParam[NH_COUNT-1].cps * a;
        }

        ret = a*count+b;
        if (ret < 0)
        {
            ret = 0;
        }
    }
    else   // 双GM管暂时不改 pgy 2024.2.22
    {
        ret = paramc * parama * count * exp(paramb*count);
    }
    
    return ret;
}
#else
float CpsToUsv_h(char mode, float count)
{   
    float ret;	
    if (mode == 1)
    {
        float c = count;
        float c2 = c*c;
        float c3 = c2*c;
        ret = parama + paramb*c + paramc*c2 + paramd*c3;
    }
    else
    {
        ret = paramc * parama * count * exp(paramb*count);
    }
    return ret;
}
#endif

// 每一秒被调用一次,输入参数，返回CPS，带出剂量率
float SmothCpsCalcRate(float RealCPS, float OlduSvh, float *NewuSvh)
{
    float CPS = 0;
    BOOL cf;
    
    cf = CpsFilter((DWORD)RealCPS);    
    //CPS = CounterPH(); //计数平滑
    if ((CpsHis < 1) && (cf == FALSE))
    {
        CPS = BackPH(); 
    }
    else
    {
        BackCnt = 0;
        BackTimer = 0;
        CPS = CounterPH();
    }
    
    if (CPS != -1)
    {
        #ifdef POE_SEN
        switch(ProbeSwitch)
        {
            case 0: 
                    if(SysRunState.RealRange == LOW_SEG)
                    {
                    	UseLowParam(CPS);
                    }
					else
					{
						UseHighParam(CPS);
					}
					*NewuSvh = CpsToUsv_h(1, CPS); 
                    break;
            case 1:
                    *NewuSvh =  GetNaIDoseRate(CPS);
                    break;
        }// uSv/h
        #else
        switch(ProbeSwitch)
        {
            case 0: 
                    *NewuSvh = CpsToUsv_h(0, CPS);   // uSv/h
                    break;
            case 1: //printf("CPS2 = %f\r\n",CPS);
                    *NewuSvh =  GetNaIDoseRate(CPS);
                    break;
        }
        #endif
    }
    else
    {
        *NewuSvh = OlduSvh;
    }

    return CPS;
}
