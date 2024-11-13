#include <math.h>
#include "system.h"
#include "CalcDoseRate.h"
#include "Sensor.h"

extern float parama,paramb,paramc,paramd;
extern LP_SYSTEM_STTAE SysRunState;
extern SYS_PARAM xdata SysParam;



// ȡƽ��ֵ
#define CT 60   
DWORD CpsArr[CT];
static BOOL Full = FALSE;
static BYTE ArrCnt = 0;
static float CpsHis = 0;

BYTE BackTimer = 0;
float BackAv[10] = {0.0};    //  ����ƽ��ֵ
BYTE BackCnt = 0;

void InitArr()
{
    BYTE i;
    // ��ʼ״̬��15 ��  1
    memset(CpsArr,0,sizeof(CpsArr));
    for (i=0;i<CT;i+=4)
    {
        CpsArr[i] = 1;
    }
    Full = TRUE;
    ArrCnt = CT;
    CpsHis = 15.0/CT;
}

// ������ѹ���ջ
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

// ����ƽ��10����
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
            // �Ѻ����������
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


// ����ƽ��
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


// time :  ������������ʱ��
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
            // ȫ���������Ͳ���������
        }
    }
}

// ����ѹ�����
void AddCounter(DWORD Cps)
{
    PushCps(Cps);   
}


BOOL CpsFilter(DWORD Cps)
{
    BYTE i;
    
    // ������3-5����Ӧ
    static DWORD BigCpsErr[5] = {0};
    static BYTE BigErrCnt = 0;
    BYTE BigCpsTime = 0;
    
    static DWORD SmCpsErr[5] = {0};
    static BYTE SmErrCnt = 0;

    // ����2CPS(0.8uSv/h����) ���ж϶��� -- �Ǹ�ܵ���С����Ϊ 1uSv/h
    if ( (Cps>(CpsHis+3*sqrt(CpsHis)))  
          && (Cps >= 2) )
    {
        if (Cps >= 4.0)   
        {
            BigCpsTime = 3;  // 3����Ӧ
        }
        else        
        {
            BigCpsTime = 5;  // 5����Ӧ
        }
        

        SmErrCnt = 0;
        BigCpsErr[BigErrCnt++] = Cps;
        if (BigErrCnt >= BigCpsTime)
        {
            // ����3����������㶸��
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
    
    // С����
    if (CpsHis > 9)  // >4uSv/h 
    {
        if (Cps < (CpsHis-3*sqrt(CpsHis))) 
        {
            BigErrCnt = 0;
            SmCpsErr[SmErrCnt++] = Cps;
            if (SmErrCnt == 3)
            {
                // ����3��С�������㶸��
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
    else if (CpsHis > 4)   // 2.5uSv/h ����
    {
        if (Cps < (CpsHis-2*sqrt(CpsHis))) 
        {
            BigErrCnt = 0;
            SmCpsErr[SmErrCnt++] = Cps;
            if (SmErrCnt == 5)
            {
                // ����5��С�������㶸��
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


    // ������ͻ��������ֱ�ӽ������
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

    return dr;   // ��λ uSv/h
}


#ifdef P2P_NH
float CpsToUsv_h(char mode, float count)
{
    float ret;
    
    BYTE i;
    float a,b;
    
    if (mode == 1)
    {
        // ��Ե��������
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
            // �����������һ����ϵ㣬�������һ��
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
    else   // ˫GM����ʱ���� pgy 2024.2.22
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

// ÿһ�뱻����һ��,�������������CPS������������
float SmothCpsCalcRate(float RealCPS, float OlduSvh, float *NewuSvh)
{
    float CPS = 0;
    BOOL cf;
    
    cf = CpsFilter((DWORD)RealCPS);    
    //CPS = CounterPH(); //����ƽ��
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
