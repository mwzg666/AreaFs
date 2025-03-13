#include "uart.h"
//#include "MwPro.h"
#include "cmd.h"
#include "i2c.h"
#include "mcp4725.h"
#include "ads1110.h"
#include "sensor.h"
#include "Lcd.h"


extern const BYTE VERSION[];
extern alt_u8 g_Output[];
extern SYS_PARAM xdata SysParam;

STU_CMD s_Head={0x02};
u8 sendbuf[200];

// 51单片机是大端的，通过结构体发送的数据要转换为小端
DWORD DwordToSmall(DWORD dat)
{
	BYTE buf[4];
    BYTE t;
    DWORD ret;
    
    memcpy(buf, &dat, 4);
	t = buf[3];
	buf[3] = buf[0];
	buf[0] = t;
	t = buf[2];
	buf[2] = buf[1];
	buf[1] = t;

    memcpy(&ret, buf, 4);
    return ret;
}

WORD WordToSmall(WORD dat)
{
	BYTE buf[2];
    BYTE t;
    WORD ret;
    
    memcpy(buf, &dat, 2);
	t = buf[1];
	buf[0] = buf[1];
	buf[1] = t;
	
    memcpy(&ret, buf, 2);
    return ret;
}

float FloatToSmall(float dat)
{
	BYTE buf[4];
    BYTE t;
    float ret;
    
    memcpy(buf, &dat, 4);
	t = buf[3];
	buf[3] = buf[0];
	buf[0] = t;
	t = buf[2];
	buf[2] = buf[1];
	buf[1] = t;

    memcpy(&ret, buf, 4);
    return ret;
}

//向上位机发送命令
void SendData(u8 cmd, u8 *cdata, u8 length)
{
    u8 sumCheck=0,i;
    s_Head.head = 0x02;
    s_Head.length = length + 8;
    switch(ProbeSwitch)
    {
        case 0:s_Head.type = 0x21;  break;      //0x21<->GM管; 0x22<->NaI; 0x23<->中子
        case 1:s_Head.type = 0x22;  break;
        case 2:s_Head.type = 0x23;  break;
    }               
    s_Head.Id = SysParam.s_General_Info.Address;
    s_Head.cmd = cmd;
    memcpy(sendbuf,(u8*)&s_Head,sizeof(STU_CMD));
    if(length>0)
    {
        memcpy(&sendbuf[sizeof(STU_CMD)],cdata,length);
    }
    for(i=0;i<length+4;i++)
    {
          sumCheck += sendbuf[i+1];
    }
    sprintf((char*)&sendbuf[length+sizeof(STU_CMD)],"%02X",sumCheck);
    sendbuf[length+sizeof(STU_CMD)+2] = 3;
    Uart2Send(sendbuf,(u8)(length+8));

}

void AlarmConfirm()
{
    SysRunState.s_DoseMSG.Dose = 0;   // 清累计剂量
    SysRunState.s_DoseMSG.State = 0; // 清报警状态
    SysParam.Pd_param.s_DoseRate.DevSt = 0;
    SysParam.Pd_param.s_DoseRate.DoseTotal = 0;
    
    SendData('E',NULL,0);
}


/*******************************************************************************
功能：命令分析与执行
输入：U32 id:指令ID号
      unsigned char dlc: 数据长度
      unsigned char *pdata:数据指针
输出：无
*******************************************************************************/
STU_CMD gs_CMD={0};
void DataPro(u8 *cdata, u8 length)
{  
    u16 i,j;
    u8 checkOut = 0;
    char checkOutbuf[3] = {0};
    
    //for(i=0;i<length; i++)
    i = 0;
    {
        if(cdata[i] == 0x02)
        {
            memcpy(&gs_CMD,&cdata[i],sizeof(STU_CMD));
            if((gs_CMD.length > length)||(cdata[gs_CMD.length-1] != 0x03)
               ||(!(gs_CMD.Id == SysParam.s_General_Info.Address)))
               // ||(!((gs_CMD.type == SysRunState.SensorType+0x11)||(gs_CMD.type == 0xff))    
            {
                if(gs_CMD.Id != 0xff)
                {
                    return;  
                }              
            }
            
            for(j=0; j< gs_CMD.length-4; j++)
            {
                  checkOut += cdata[i+j+1];
            }
            sprintf(checkOutbuf,"%02X",checkOut);
            if(memcmp(checkOutbuf,&cdata[i+gs_CMD.length]-3,2) == 0)
            {
                switch(gs_CMD.cmd)
                {
                    case 'C':  ACK_CMD_C();    break;                   //联络命令
                    case 'V':  ACK_CMD_V();    break;               //读计数
                    case 'P':  ACK_CMD_P();    break;               //读脉冲
                    case 'H':  ACK_CMD_H(cdata[i+5]);    break;     //设置探测器地址
                    case 'B':  ACK_CMD_B(&cdata[i+5]);break;        //写报警参数
                    case 'W':  ACK_CMD_W(&cdata[i+5]); break;       //写参数
                    case 'F':  ACK_CMD_F();break;                   //读报警参数
                    case 'R':  ACK_CMD_R();break;                   //读参数
                    case 'T':  ACK_CMD_T();break;                   //测温
                    case 'Q':  ACK_CMD_Q(cdata[i+5]);break;         //量程查询或者设置
                    case 'M':  ACK_CMD_M();break;                   //软件版本o
                    case 'J':  ACK_CMD_J();break;                   //读校准因子
                    case 'N':  ACK_CMD_N(&cdata[i+5]);break;        //修改校准因子
                    case 'X':  ReadCS(); break;                     //读Co/Cs比
                    case 'Y':  SetCS(&cdata[i+5]); break;           //写Co/Cs比
                    case 'E':  AlarmConfirm();    break;           //报警确认
                    case CMD_WRITE_TIME:ACK_CMD_TIME(&cdata[i+5]);break;
                    //case 0x28:  asm(" mov &0xFFBE, PC;");  break;  //远程升级  //跳转到升级代码

                    #ifdef P2P_NH
                    // 点对点拟合 add by pgy 2024-2-20  
                    case CMD_READ_NH:  ReadNh();   break;               // 读拟合数据
                    case CMD_WRITE_NH: WriteNh(&cdata[i+5]);    break; // 写拟合数据
                    #endif

                    case CMD_READ_FIX:  ReadFix();   break; 
                    case CMD_WRITE_FIX:  WriteFix(&cdata[i+5]);   break; 
                    case CMD_READ_INFO: ACK_CMD_RINFO();break;
                    case CMD_WRITE_INFO: ACK_CMD_WINFO(&cdata[i+5]);break;
                    default:
                    break;
                }
                i += (gs_CMD.length+4);
            }
        }
    }
}


void ReadCS()
{
    float Cocs;
    if(ProbeSwitch !=2)
    {
        Cocs = FloatToSmall(SysParam.Lp_Param.CoCsRate);
        SendData('X',(u8*)&Cocs,4);
    }
}

void SetCS(u8 *cs)
{
    float Cocs;
    if(ProbeSwitch !=2)
    {
        memcpy(&Cocs,cs, 4);  
        SysParam.Lp_Param.CoCsRate = FloatToSmall(Cocs);
        SendData('Y',NULL,0);
    }
    SaveParam();
}  

void ReadNh()
{
    #ifdef P2P_NH
    BYTE i = 0;
    if(ProbeSwitch !=2)
    {
        for(i = 0;i < NH_COUNT;i++)
        {
           SysParam.Lp_Param.NhParam[i] =  FloatToSmall(SysParam.Lp_Param.NhParam[i]);
        }
        SendData(CMD_READ_NH,(u8*)&SysParam.Lp_Param.NhParam,sizeof(NH_PARAM)*NH_COUNT);
    }
    #endif
}

void WriteNh(u8 *dat)
{
    #ifdef P2P_NH
    BYTE i = 0;
    if(ProbeSwitch !=2)
    {
        memcpy((u8*)&SysParam.Lp_Param.NhParam,dat,sizeof(NH_PARAM)*NH_COUNT);
        for(i = 0;i < NH_COUNT;i++)
        {
           SysParam.Lp_Param.NhParam[i] =  FloatToSmall(SysParam.Lp_Param.NhParam[i]);
        }
        
        SendData(CMD_WRITE_NH,NULL,0);
    }
    #endif
    SaveParam();
    
}

void ReadFix()
{
    BYTE i = 0;
    switch(ProbeSwitch)
    {
        case 0:
            for(i = 0;i < FIX_COUNT;i++)
            {
               SysParam.Lp_Param.Fix[i] =  FloatToSmall(SysParam.Lp_Param.Fix[i]);
            }
            SendData(CMD_READ_FIX,(u8*)&SysParam.Lp_Param.Fix,sizeof(float)*FIX_COUNT);
            break;
        case 1:
            for(i = 0;i < FIX_COUNT;i++)
            {
               SysParam.Lp_Param.Fix[i] =  FloatToSmall(SysParam.Lp_Param.Fix[i]);
            }
            SendData(CMD_READ_FIX,(u8*)&SysParam.Lp_Param.Fix,sizeof(float)*FIX_COUNT);
            break;
        case 2:
            for(i = 0;i < PD_FIXNUM;i++)
            {
               SysParam.Pd_param.fix[i] =  FloatToSmall(SysParam.Pd_param.fix[i]);
            }
            SendData(CMD_READ_FIX,(u8*)&SysParam.Pd_param.fix ,sizeof(float)*PD_FIXNUM);
            break;
    }
    
}

void WriteFix(u8 *dat)
{
    BYTE i = 0;
    switch(ProbeSwitch)
    {
        case 0: 
            memcpy((u8*)&SysParam.Lp_Param.Fix,dat,sizeof(float)*FIX_COUNT);
            for(i = 0;i < FIX_COUNT;i++)
            {
               SysParam.Lp_Param.Fix[i] =  FloatToSmall(SysParam.Lp_Param.Fix[i]);
            }
           
            break;
        case 1:
            memcpy((u8*)&SysParam.Lp_Param.Fix,dat,sizeof(float)*FIX_COUNT);
            for(i = 0;i < FIX_COUNT;i++)
            {
               SysParam.Lp_Param.Fix[i] =  FloatToSmall(SysParam.Lp_Param.Fix[i]);
            }
            
            break;
        case 2:
            memcpy((u8*)&SysParam.Pd_param.fix,dat,sizeof(float)*PD_FIXNUM);
            for(i = 0;i < PD_FIXNUM;i++)
            {
               SysParam.Lp_Param.Fix[i] =  FloatToSmall(SysParam.Lp_Param.Fix[i]);
            }
            
            break;
    }    
    SendData(CMD_WRITE_FIX,NULL,0);
    SaveParam();
}

/*******************************************************************************
功能：联络命令响应
输入：unsigned char SensorType:传感器类型
输出：无
*******************************************************************************/
void ACK_CMD_C()
{
    SendData('C',NULL,0);
}


/*******************************************************************************
功能：设置时间响应
输入：unsigned char SensorType:传感器类型
输出：无
*******************************************************************************/
void ACK_CMD_TIME(unsigned char *cdata)
{
    SetLcdTime(cdata);
    SendData(CMD_WRITE_TIME,NULL,0);
}

/*******************************************************************************
功能：设置探测器号
输入：
输出：
*******************************************************************************/

void ACK_CMD_H(u8 Address)
{
    SysParam.s_General_Info.Address = Address;
    SendData('H',NULL,0);
    SaveParam();
}

/*******************************************************************************
功能：读命令(R)响应
输入：无
输出：无
*******************************************************************************/
void ACK_CMD_R(void)
{
    SEND_PD_THR Pd_thr;
	HOST_SENSOR_PARAM Hs_ps;
	char temp[5] = {0};
    switch(ProbeSwitch)
    {
        case 0: 
			memset(&Hs_ps,0,sizeof(HOST_SENSOR_PARAM));
		
            Hs_ps.Canshu1 = FloatToSmall(SysParam.Lp_Param.s_SysParam.Canshu1);
            Hs_ps.Canshu2 = FloatToSmall(SysParam.Lp_Param.s_SysParam.Canshu2);
			memcpy(temp,SysParam.Lp_Param.s_SysParam.yuzhi1,4);
			Hs_ps.Thr1 = atoi(temp);
			Hs_ps.Thr1 = DwordToSmall(Hs_ps.Thr1);

			memcpy(temp,SysParam.Lp_Param.s_SysParam.yuzhi2,4);
			Hs_ps.Thr2 = atoi(temp);
			Hs_ps.Thr2 = DwordToSmall(Hs_ps.Thr2);

			memcpy(temp,SysParam.Lp_Param.s_SysParam.yuzhi3,4);
			Hs_ps.Thr3 = atoi(temp);
			Hs_ps.Thr3 = DwordToSmall(Hs_ps.Thr3);

			memcpy(temp,SysParam.Lp_Param.s_SysParam.PingHuaShiJian,4);
			Hs_ps.Time = atoi(temp);
			Hs_ps.Time = DwordToSmall(Hs_ps.Time);
			
            //memcpy(ps.yuzhi1,SysParam.Lp_Param.s_SysParam.yuzhi1,4);
            //memcpy(ps.yuzhi2,SysParam.Lp_Param.s_SysParam.yuzhi2,4);
            //memcpy(ps.PingHuaShiJian,SysParam.Lp_Param.s_SysParam.PingHuaShiJian,4);  

            SendData('R',(u8*)&Hs_ps,sizeof(HOST_SENSOR_PARAM));
            break;
            
        case 1:  
			memset(&Hs_ps,0,sizeof(HOST_SENSOR_PARAM));
		
            Hs_ps.Canshu1 = FloatToSmall(SysParam.Lp_Param.s_SysParam.Canshu1);
            Hs_ps.Canshu2 = FloatToSmall(SysParam.Lp_Param.s_SysParam.Canshu2);
			memcpy(temp,SysParam.Lp_Param.s_SysParam.yuzhi1,4);
			Hs_ps.Thr1 = atoi(temp);
			Hs_ps.Thr1 = DwordToSmall(Hs_ps.Thr1);

			memcpy(temp,SysParam.Lp_Param.s_SysParam.yuzhi2,4);
			Hs_ps.Thr2 = atoi(temp);
			Hs_ps.Thr2 = DwordToSmall(Hs_ps.Thr2);

			memcpy(temp,SysParam.Lp_Param.s_SysParam.yuzhi3,4);
			Hs_ps.Thr3 = atoi(temp);
			Hs_ps.Thr3 = DwordToSmall(Hs_ps.Thr3);

			memcpy(temp,SysParam.Lp_Param.s_SysParam.PingHuaShiJian,4);
			Hs_ps.Time = atoi(temp);
			Hs_ps.Time = DwordToSmall(Hs_ps.Time);

			
            //memcpy(ps.yuzhi1,SysParam.Lp_Param.s_SysParam.yuzhi1,4);
            //memcpy(ps.yuzhi2,SysParam.Lp_Param.s_SysParam.yuzhi2,4);
            //memcpy(ps.PingHuaShiJian,SysParam.Lp_Param.s_SysParam.PingHuaShiJian,4);  

            SendData('R',(u8*)&Hs_ps,sizeof(HOST_SENSOR_PARAM));
            break;
        case 2:
			memset(&Pd_thr,0,sizeof(SEND_PD_THR));
            Pd_thr.Det_Thr[0]= DwordToSmall((DWORD)SysParam.Pd_param.s_Thr.Det_Thr[0]);
            Pd_thr.Det_Thr[1] = DwordToSmall((DWORD)SysParam.Pd_param.s_Thr.Det_Thr[1]);
            Pd_thr.Det_Thr[2] = DwordToSmall((DWORD)SysParam.Pd_param.s_Thr.Det_Thr[2]);     
            Pd_thr.AnalogChannel = DwordToSmall((DWORD)SysParam.Pd_param.s_Thr.AnalogChannel);
            SendData('R',(u8*)&Pd_thr,sizeof(SEND_PD_THR));
            break;
    }
                
    
}
    

/*******************************************************************************
功能：写参数命令(W)响应
输入：unsigned char *pdata:参数
输出：无
*******************************************************************************/
void ACK_CMD_W(unsigned char *cdata)
{
	HOST_SENSOR_PARAM Hs_ps;
    SEND_PD_THR pd_thr;
	char temp[5] = {0};
    switch(ProbeSwitch)
    {
        case 0: 
            memcpy((u8*)&Hs_ps,cdata,sizeof(HOST_SENSOR_PARAM));
              
            SysParam.Lp_Param.s_SysParam.Canshu1 = FloatToSmall(Hs_ps.Canshu1);
            SysParam.Lp_Param.s_SysParam.Canshu2 = FloatToSmall(Hs_ps.Canshu2);
			Hs_ps.Thr1 = DwordToSmall(Hs_ps.Thr1);
			sprintf(temp,"%04u",(unsigned int)Hs_ps.Thr1);
			memcpy(SysParam.Lp_Param.s_SysParam.yuzhi1,temp,4);

			Hs_ps.Thr2 = DwordToSmall(Hs_ps.Thr2);
			sprintf(temp,"%04u",(unsigned int)Hs_ps.Thr2);
			memcpy(SysParam.Lp_Param.s_SysParam.yuzhi2,temp,4);

		    Hs_ps.Thr3 = DwordToSmall(Hs_ps.Thr3);
			sprintf(temp,"%04u",(unsigned int)Hs_ps.Thr3);
			memcpy(SysParam.Lp_Param.s_SysParam.yuzhi3,temp,4);

		    Hs_ps.Time = DwordToSmall(Hs_ps.Time);
			sprintf(temp,"%04u",(unsigned int)Hs_ps.Time);
			memcpy(SysParam.Lp_Param.s_SysParam.PingHuaShiJian,temp,4);

		   
           //memcpy(SysParam.Lp_Param.s_SysParam.yuzhi1,wcm.yuzhi1,4);
           //memcpy(SysParam.Lp_Param.s_SysParam.yuzhi2,wcm.yuzhi2,4);
           //memcpy(SysParam.Lp_Param.s_SysParam.PingHuaShiJian,wcm.PingHuaShiJian,4);
        
           #ifndef POE_SEN
                   //SysRunState.stParam.s_Jiaozhun.DI_C = SysRunState.stParam.s_SysParam.Canshu1;
                   //SysRunState.stParam.s_Jiaozhun.GAO_C = SysRunState.stParam.s_SysParam.Canshu2;
                   RefreshParam();
           #endif
           MCP4725_OutVol(MCP4725_S1_ADDR,SysParam.Lp_Param.s_SysParam.yuzhi1);
           break;
        case 1:  
            memcpy((u8*)&Hs_ps,cdata,sizeof(LP_SYS_PRAM));
              
           SysParam.Lp_Param.s_SysParam.Canshu1 = FloatToSmall(Hs_ps.Canshu1);
           SysParam.Lp_Param.s_SysParam.Canshu2 = FloatToSmall(Hs_ps.Canshu2);
		   Hs_ps.Thr1 = DwordToSmall(Hs_ps.Thr1);
			sprintf(temp,"%04u",(unsigned int)Hs_ps.Thr1);
			memcpy(SysParam.Lp_Param.s_SysParam.yuzhi1,temp,4);

			Hs_ps.Thr2 = DwordToSmall(Hs_ps.Thr2);
			sprintf(temp,"%04u",(unsigned int)Hs_ps.Thr2);
			memcpy(SysParam.Lp_Param.s_SysParam.yuzhi2,temp,4);

		    Hs_ps.Thr3 = DwordToSmall(Hs_ps.Thr3);
			sprintf(temp,"%04u",(unsigned int)Hs_ps.Thr3);
			memcpy(SysParam.Lp_Param.s_SysParam.yuzhi3,temp,4);

		    Hs_ps.Time = DwordToSmall(Hs_ps.Time);
			sprintf(temp,"%04u",(unsigned int)Hs_ps.Time);
			memcpy(SysParam.Lp_Param.s_SysParam.PingHuaShiJian,temp,4);

		   
           //memcpy(SysParam.Lp_Param.s_SysParam.yuzhi1,wcm.yuzhi1,4);
           //memcpy(SysParam.Lp_Param.s_SysParam.yuzhi2,wcm.yuzhi2,4);
           //memcpy(SysParam.Lp_Param.s_SysParam.PingHuaShiJian,wcm.PingHuaShiJian,4);
        
           #ifndef POE_SEN
                   //SysRunState.stParam.s_Jiaozhun.DI_C = SysRunState.stParam.s_SysParam.Canshu1;
                   //SysRunState.stParam.s_Jiaozhun.GAO_C = SysRunState.stParam.s_SysParam.Canshu2;
                   RefreshParam();
           #endif
           MCP4725_OutVol(MCP4725_S1_ADDR,SysParam.Lp_Param.s_SysParam.yuzhi1);
           break;
        case 2:
            memcpy((u8*)&pd_thr,cdata,sizeof(SEND_PD_THR));
            SysParam.Pd_param.s_Thr.Det_Thr[0] = (WORD)DwordToSmall(pd_thr.Det_Thr[0]);
            SysParam.Pd_param.s_Thr.Det_Thr[1] = (WORD)DwordToSmall(pd_thr.Det_Thr[1]);
            SysParam.Pd_param.s_Thr.Det_Thr[2] = (WORD)DwordToSmall(pd_thr.Det_Thr[2]);
            SysParam.Pd_param.s_Thr.AnalogChannel = (WORD)DwordToSmall(pd_thr.AnalogChannel);
            break;
    }
   
    SendData('W',NULL,0);
    SaveParam();
}



/*******************************************************************************
功能：写校准因子(M)响应
输入：unsigned char SensorType:传感器类型
输出：无
*******************************************************************************/
void ACK_CMD_N(unsigned char *cdata)
{
    SYS_JIAOZHUNYINZI Jzyz_w;   
    if(ProbeSwitch !=2)
    {
        memcpy((u8*)&Jzyz_w,cdata,sizeof(SYS_JIAOZHUNYINZI));
        SysParam.Lp_Param.s_Jiaozhun.DI_A = FloatToSmall(Jzyz_w.DI_A);
        SysParam.Lp_Param.s_Jiaozhun.DI_B = FloatToSmall(Jzyz_w.DI_B);
        SysParam.Lp_Param.s_Jiaozhun.DI_C = FloatToSmall(Jzyz_w.DI_C);
        SysParam.Lp_Param.s_Jiaozhun.GAO_A = FloatToSmall(Jzyz_w.GAO_A);
        SysParam.Lp_Param.s_Jiaozhun.GAO_B = FloatToSmall(Jzyz_w.GAO_B);
        SysParam.Lp_Param.s_Jiaozhun.GAO_C = FloatToSmall(Jzyz_w.GAO_C);

        SendData('N',NULL,0);
    }
    SaveParam();
    RefreshParam();
}

/*******************************************************************************
功能： 读校准因子(J)响应
输入：unsigned char SensorType:传感器类型
输出：无
*******************************************************************************/
void ACK_CMD_J(void)
{
    
    SYS_JIAOZHUNYINZI Jzyz_r;
    if(ProbeSwitch !=2)
    {
        Jzyz_r.DI_A = FloatToSmall(SysParam.Lp_Param.s_Jiaozhun.DI_A);
        Jzyz_r.DI_B = FloatToSmall(SysParam.Lp_Param.s_Jiaozhun.DI_B);
        Jzyz_r.DI_C = FloatToSmall(SysParam.Lp_Param.s_Jiaozhun.DI_C);
        Jzyz_r.GAO_A = FloatToSmall(SysParam.Lp_Param.s_Jiaozhun.GAO_A);
        Jzyz_r.GAO_B = FloatToSmall(SysParam.Lp_Param.s_Jiaozhun.GAO_B);
        Jzyz_r.GAO_C = FloatToSmall(SysParam.Lp_Param.s_Jiaozhun.GAO_C);

        SendData('J',(u8*)&Jzyz_r,sizeof(SYS_JIAOZHUNYINZI));
    }
}

/*******************************************************************************
功能：读计数命令(V)响应
输入：unsigned char SensorType:传感器类型
输出：无
*******************************************************************************/

void ACK_CMD_V(void)
{
    STU_DOSERATE dr;
    //PD_DOSERATE Pd_dr;
    SEND_PD_DOSERATE Pd_dr;
    switch(ProbeSwitch)
    {
        case 0: 
            dr.DoseRate = FloatToSmall(SysRunState.s_DoseMSG.DoseRate);
            dr.Dose     = FloatToSmall(SysRunState.s_DoseMSG.Dose);
            dr.State    = SysRunState.s_DoseMSG.State;

            dr.PULSE1 = DwordToSmall(SysRunState.s_CPS.CPS1);
            dr.PULSE2 = DwordToSmall(SysRunState.s_CPS.CPS2);
            dr.PULSE3 = 0;

//            #ifdef POE_SEN
//            if (SysRunState.RealRange == LOW_SEG)
//            {
//                dr.DoseRate *= SysParam.Lp_Param.CoCsRate;
//            }
//            #else
//           
//            #endif
            SendData('V',(u8*)&dr,sizeof(STU_DOSERATE));
            break;
        case 1: 
            
            dr.DoseRate = FloatToSmall(SysRunState.s_DoseMSG.DoseRate);
            dr.Dose     = FloatToSmall(SysRunState.s_DoseMSG.Dose);
            dr.State    = SysRunState.s_DoseMSG.State;

            dr.PULSE1 = DwordToSmall(SysRunState.s_CPS.CPS1);
            dr.PULSE2 = DwordToSmall(SysRunState.s_CPS.CPS2);
            dr.PULSE3 = 0;

//            #ifdef POE_SEN
//            if (SysRunState.RealRange == LOW_SEG)
//            {
//                dr.DoseRate *= SysParam.Lp_Param.CoCsRate;
//            }
//            #else
//           
//            #endif
            SendData('V',(u8*)&dr,sizeof(STU_DOSERATE));
            break;
        case 2: 
            
            Pd_dr.DoseRate  = FloatToSmall(SysParam.Pd_param.s_DoseRate.DoseRate);
            Pd_dr.DoseTotal = FloatToSmall(SysParam.Pd_param.s_DoseRate.DoseTotal);
            Pd_dr.DevSt     = (BYTE)SysParam.Pd_param.s_DoseRate.DevSt;
			

            Pd_dr.Cps1 = FloatToSmall(SysParam.Pd_param.s_DoseRate.Cps1);
            Pd_dr.Cps2 = FloatToSmall(SysParam.Pd_param.s_DoseRate.Cps2);
            Pd_dr.Cps3 = FloatToSmall(SysParam.Pd_param.s_DoseRate.Cps3);
            SendData('V',(u8*)&Pd_dr,sizeof(PD_DOSERATE));
            break;
    }
}

/*******************************************************************************
功能：读计数命令(V)响应
输入：unsigned char SensorType:传感器类型
输出：无
*******************************************************************************/
void ACK_CMD_P(void)
{
    char buf1[13],buf2[19];
    SEND_PD_DOSERATE Pd_dr;
    switch(ProbeSwitch)
    {
        case 0: 
            sprintf(buf1,"%06ld",(long)SysRunState.s_CPS.CPS1);
            sprintf(&buf1[6],"%06ld",(long)SysRunState.s_CPS.CPS2);
            buf1[12] = SysRunState.s_CPS.State;

            SendData('P',(u8*)buf1,13);
            break;
        case 1:  
            sprintf(buf1,"%06ld",(long)SysRunState.s_CPS.CPS1);
            sprintf(&buf1[6],"%06ld",(long)SysRunState.s_CPS.CPS2);
            buf1[12] = SysRunState.s_CPS.State;

            SendData('P',(u8*)buf1,13);
            break;
        case 2:
            Pd_dr.DevSt = (BYTE)SysParam.Pd_param.s_DoseRate.DevSt;
            sprintf(buf2,"%06ld",(long)SysParam.Pd_param.s_DoseRate.Cps1);
            sprintf(&buf2[6],"%06ld",(long)SysParam.Pd_param.s_DoseRate.Cps2);
            sprintf(&buf2[12],"%06ld",(long)SysParam.Pd_param.s_DoseRate.Cps3);
            buf2[18] = Pd_dr.DevSt;

            SendData('P',(u8*)buf2,19);
            break;
    }
   
}




/*******************************************************************************
功能：写报警参数命令(WF)响应
输入：unsigned char *pdata
输出：无
*******************************************************************************/
void ACK_CMD_B(u8 *cdata)
{
    SYS_ALARM wal;   
    PD_ALARM Pd_alm;
    switch(ProbeSwitch)
    {
        case 0: 
             memcpy((u8*)&wal,cdata,sizeof(SYS_ALARM));

            SysParam.Lp_Param.s_Alarm.DoseAlarm = FloatToSmall(wal.DoseAlarm);
            SysParam.Lp_Param.s_Alarm.DosePreAlarm = FloatToSmall(wal.DosePreAlarm);
            SysParam.Lp_Param.s_Alarm.DoseRateAlarm = FloatToSmall(wal.DoseRateAlarm);
            SysParam.Lp_Param.s_Alarm.DoseRatePreAlarm = FloatToSmall(wal.DoseRatePreAlarm);
            break;
        case 1:  
            memcpy((u8*)&wal,cdata,sizeof(SYS_ALARM));

            SysParam.Lp_Param.s_Alarm.DoseAlarm = FloatToSmall(wal.DoseAlarm);
            SysParam.Lp_Param.s_Alarm.DosePreAlarm = FloatToSmall(wal.DosePreAlarm);
            SysParam.Lp_Param.s_Alarm.DoseRateAlarm = FloatToSmall(wal.DoseRateAlarm);
            SysParam.Lp_Param.s_Alarm.DoseRatePreAlarm = FloatToSmall(wal.DoseRatePreAlarm);
            break;
        case 2:
            memcpy((u8*)&Pd_alm,cdata,sizeof(PD_ALARM));

            SysParam.Pd_param.s_Alarm.DoseAlarm = FloatToSmall(Pd_alm.DoseAlarm);
            SysParam.Pd_param.s_Alarm.DosePreAlarm = FloatToSmall(Pd_alm.DosePreAlarm);
            SysParam.Pd_param.s_Alarm.DoseRateAlarm = FloatToSmall(Pd_alm.DoseRateAlarm);
            SysParam.Pd_param.s_Alarm.DoseRatePreAlarm = FloatToSmall(Pd_alm.DoseRatePreAlarm);
            break;
    }  
    SendData('B',NULL,0);
    SaveParam();
} 


/*******************************************************************************
功能：读报警参数命令(F)响应
输入：无
输出：无
*******************************************************************************/
void ACK_CMD_F(void)
{
    SYS_ALARM ral;
    PD_ALARM Pd_ral;
        switch(ProbeSwitch)
        {
            case 0: 
                ral.DoseAlarm = FloatToSmall(SysParam.Lp_Param.s_Alarm.DoseAlarm);
                ral.DosePreAlarm = FloatToSmall(SysParam.Lp_Param.s_Alarm.DosePreAlarm);
                ral.DoseRateAlarm = FloatToSmall(SysParam.Lp_Param.s_Alarm.DoseRateAlarm);
                ral.DoseRatePreAlarm = FloatToSmall(SysParam.Lp_Param.s_Alarm.DoseRatePreAlarm);
                SendData('F',(u8*)&ral,sizeof(SYS_ALARM));
                break;
            case 1:  
                ral.DoseAlarm = FloatToSmall(SysParam.Lp_Param.s_Alarm.DoseAlarm);
                ral.DosePreAlarm = FloatToSmall(SysParam.Lp_Param.s_Alarm.DosePreAlarm);
                ral.DoseRateAlarm = FloatToSmall(SysParam.Lp_Param.s_Alarm.DoseRateAlarm);
                ral.DoseRatePreAlarm = FloatToSmall(SysParam.Lp_Param.s_Alarm.DoseRatePreAlarm);
                SendData('F',(u8*)&ral,sizeof(SYS_ALARM));
                break;
            case 2:
                Pd_ral.DoseAlarm=FloatToSmall(SysParam.Pd_param.s_Alarm.DoseAlarm);
                Pd_ral.DosePreAlarm=FloatToSmall(SysParam.Pd_param.s_Alarm.DosePreAlarm);
                Pd_ral.DoseRateAlarm=FloatToSmall(SysParam.Pd_param.s_Alarm.DoseRateAlarm);
                Pd_ral.DoseRatePreAlarm=FloatToSmall(SysParam.Pd_param.s_Alarm.DoseRatePreAlarm);
                SendData('F',(u8*)&Pd_ral,sizeof(PD_ALARM));
                break;
        }
} 


/*******************************************************************************
功能：测温
输入：
输出：
*******************************************************************************/
void ACK_CMD_T(void)
{
    SendData('T',NULL,0);
}

/*******************************************************************************
功能：量程查询或者设置
输入：
输出：
*******************************************************************************/
void ACK_CMD_Q(u8 range)
{
    if(ProbeSwitch == 0)
    {
        if (range == 1)
        {
            SysRunState.RealRange = HIG_SEG;
    	}
        else
        {
            SysRunState.RealRange = LOW_SEG;
        }
    
        //SendData('Q',(u8*)&SysRunState.range,1);
        SendData('Q',NULL,0);
    }
}

/*******************************************************************************
功能：版本
输入：
输出：
*******************************************************************************/
void ACK_CMD_M(void)
{
     SendData('M',(u8*)SysParam.VerSion,6);
}

void ACK_CMD_WINFO(unsigned char *cdata)
{
    SYS_GENERAL_INFO Dev_Winfo;  
   
    memcpy((u8*)&Dev_Winfo,cdata,sizeof(SYS_GENERAL_INFO)-1);
    memcpy(SysParam.s_General_Info.Index,Dev_Winfo.Index,32);
    memcpy(SysParam.s_General_Info.Location,Dev_Winfo.Location,32);
    memcpy(SysParam.s_General_Info.Name,Dev_Winfo.Name,32);
    SendData(CMD_WRITE_INFO,NULL,0);
    SaveParam();
}

void ACK_CMD_RINFO()
{
    SYS_GENERAL_INFO Dev_Rinfo;
    memcpy(Dev_Rinfo.Index,SysParam.s_General_Info.Index,32);  
    memcpy(Dev_Rinfo.Location,SysParam.s_General_Info.Location,32);  
    memcpy(Dev_Rinfo.Name,SysParam.s_General_Info.Name,32); 
    SendData(CMD_READ_INFO,(u8*)&Dev_Rinfo,sizeof(SYS_GENERAL_INFO)-1);
}


/*******************************************************************************
功能：电压
输入：
输出：
*******************************************************************************/
bool ReadBatVol()
{
    BAT_INFO bi;
    int Voltage = 0;
    GetAds1110(I2C_BAT_VOL, ADS110_BAT_VOL);

    bi.Vol = (WORD)Voltage;
    bi.Charging = (BAT_CHARGE() != 0);
   SendData('G', (BYTE *)&bi, 3);
    return true;
}




/*
函数名称:CheckSum
函数描述:校验和函数
输    入:数据、长度
输    出:校验结果
作    者:张红丽
日    期:2012.9.10

*/
unsigned char CheckSum(char *buf,int len)
{
    //unsigned long Sum=0;
    unsigned char verify=0;
    unsigned int i=0;
    for(i=0;i<len;i++)
    {
        verify+=*(buf+i);
    }
    //verify=(char)(Sum%256);
    return verify;
}



