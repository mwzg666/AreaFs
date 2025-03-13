#ifndef __MAIN_H__
#define __MAIN_H__

#include "STC32G.h"

#include "stdio.h"
#include "intrins.h"
#include "string.h"
#include <stdlib.h>


//#define  FOR_DEBUG

typedef 	unsigned char	BOOL;
#define     TRUE    1
#define     FALSE   0

typedef 	unsigned char	BYTE;
typedef 	unsigned short	WORD;
typedef 	unsigned long	DWORD;

typedef 	unsigned char	u8;
typedef 	unsigned short	u16;
typedef 	unsigned long	u32;

typedef 	unsigned int	uint;


#define alt_u8   BYTE
#define alt_u16 WORD
#define alt_u32 DWORD  


#define bool BYTE
#define true 1
#define false 0

#define BOOL BYTE
#define TRUE 1
#define FALSE 0

#define SENSOR_COUNT 4
#define CHANNLE_NUM  8
#define FLOW_METER_CNT   9

#define Unit_uSv 0           // μSv/h 
#define Unit_mSv  1          // mSv/h 
#define Unit_Sv   2          // Sv/h

//预留选择端口
#define D4() (P6 & (1<<4))       
#define D3() (P6 & (1<<5)) 
#define D2() (P6 & (1<<6))    //PD     Switch_6
#define D1() (P6 & (1<<7))    //NaI   Switch_5


#define DEV_NAME "FS-1F-01"

#define POE_SEN     // POE探头盖格管 

#define MAIN_Fosc        11059200UL    // 11.0592M
//#define MAIN_Fosc        35000000UL    // 35M

#define SysTick     9216   // 10ms    // 次/秒, 系统滴答频率, 在4000~16000之间

#define Timer0_Reload   (65536UL - SysTick)     //Timer 0 中断频率

#define OFFLINE_TIME  600

#define OUT_IO_COUNT      4
#define OUT_MP3_COUNT      10

#define POWERON     1
#define POWEROFF    0

#define LIGHT_GREEN       0
#define LIGHT_BLUE        1
#define LIGHT_RED          2
#define ALARM_SOUND       3

typedef enum
{
    MP3_1,MP3_2,MP3_3,MP3_4,MP3_5,MP3_6,MP3_7,MP3_8,MP3_9,MP3_STOP
}MP3_INDEX;

// output port 

#define ALM_DOSERATE_NOR      0
#define ALM_DOSERATE_PRE    1
#define ALM_DOSERATE_ERROR    2
#define ALM_DOSE_PRE    3
#define ALM_DOSE_ERROR    4
#define ALM_DOSE_OVER   5


//#define ALM_FLOW_ABNOR    4

#define LED_FLASH_TIME    500   // ms

#define PW_MAIN(x)   (x)?(P7 |= (1<<5)):(P7 &= ~(1<<5))    // 主电源控制
#define RUN_LED(x) (x)?(P4 |= (1<<0)):(P4 &= ~(1<<0))    // 板载LED

// 指示灯
#define GRE_LIGHT(x) (x)?(P2 |= (1<<1)):(P2 &= ~(1<<1))      // 绿灯
#define RED_LIGHT(x) (x)?(P2 |= (1<<2)):(P2 &= ~(1<<2))      // 红灯
#define BLU_LIGHT(x) (x)?(P2 |= (1<<3)):(P2 &= ~(1<<3))      // 蓝灯
#define ALARM(x)      (x)?(P2 |= (1<<0)):(P2 &= ~(1<<0))      // 报警音

#define SINU_M(x)     (P3 & (1<<2))      

#define RS485_EN(x)  (x)?(P5 |= (1<<2)):(P5 &= ~(1<<2))     //串口4 485使能

#define POWER_LOCK()    (P4 & (1<<5))       // 开关机锁
#define LOCK_BIT()      (1<<5)

#define BAT_CHARGE()    (P5 & (1<<1))        //电池充电状态

//LX-MP3
#define A1(x)  (x)?(P7 |= (1<<4)):(P7 &= ~(1<<4))     
#define A2(x)  (x)?(P2 |= (1<<7)):(P2 &= ~(1<<7))
#define A3(x)  (x)?(P2 |= (1<<6)):(P2 &= ~(1<<6))
#define A4(x)  (x)?(P4 |= (1<<4)):(P4 &= ~(1<<4))
#define A5(x)  (x)?(P4 |= (1<<3)):(P4 &= ~(1<<3))     
#define A6(x)  (x)?(P4 |= (1<<2)):(P4 &= ~(1<<2))
#define A7(x)  (x)?(P4 |= (1<<1)):(P4 &= ~(1<<1))
#define A8(x)  (x)?(P7 |= (1<<3)):(P7 &= ~(1<<3))
#define A9(x)  (x)?(P7 |= (1<<2)):(P7 &= ~(1<<2))

#define PAUSE_M(x)   (x)?(P7 |= (1<<0)):(P7 &= ~(1<<0))     //语言暂停

//HLK_RM08K  ETH
#define ES0_M(x)    (x)?(P3 |= (1<<3)):(P3 &= ~(1<<3))     
#define RST(x)      (x)?(P3 |= (1<<4)):(P3 &= ~(1<<4))
#define ES1_M(x)    (x)?(P5 |= (1<<0)):(P5 &= ~(1<<0))


#define CMD_OUT_CTL      'O'  // 0x4F
#define CMD_IN_STATUE    'I'  // 0x49
#define CMD_GET_VER      'M'  // 0x56
#define CMD_DATA_TT      'T'  // 0x54 数据透传
#define CMD_SET_ADD      'H'  // 0x48


#define KEY_ALM_CFM      3
#define POWER_ON               0xAA
#define POWER_OFF              0x55


#define RED       0
#define YELLOW    1
#define BLUE      2

#define NAI_K0      0.0000124264931333855         //0.149434802297712    	    //0.149429067085229
#define NAI_K1      0.00467903650870694	          //0.00278922406291244    	    //0.00577694678584278
#define NAI_K2      9.635213697987E-09		      //3.84151853056591E-08    	//1.64786140335038E-07
#define NAI_K3      -1.72215004037806E-13         //2.43281836203998E-13    	//2.16163629786135E-12


typedef struct
{
    bool  Running;
    DWORD RunTime;

    float DoseRate;
    BYTE  Unit;
    char  Status[16];
}RUN_STATUS;

typedef struct
{
    WORD PdAddr;
    WORD GmAddr;
    WORD NaIAddr;
}
PROBE_REGADDR;


//PD
#define PD_PARAM_REG    0xA000
#define PD_MODBUS_ADDR 0x01
#define PD_VER  100 //"V:VER1.986"  
#define PD_FIXNUM  17

typedef struct
{
    float DoseRate;   
    float DoseTotal;
    DWORD DevSt;
    float Cps1;
    float Cps2;
    float Cps3;
}PD_DOSERATE;

typedef struct
{
    float DoseRate;   
    float DoseTotal;
    float res;
    float Cps1;
    float Cps2;
    BYTE DevSt;
    float Cps3;
}SEND_PD_DOSERATE;



typedef struct
{
    float Cps1;
    float Cps2;
    float Cps3;
    DWORD DevSt;
}PD_CPS;



typedef struct
{
    float  DoseRatePreAlarm;        //剂量率一级报警(uSv/h)
    float  DoseRateAlarm;              //剂量率二级报警(uSv/h)
    float  DosePreAlarm;              //累计剂量一级报警(uSv)
    float  DoseAlarm;                  //累计剂量二级报警(uSv)
}PD_ALARM;

typedef struct
{
	//float res[2];
    WORD Det_Thr[3];                 //探测器阈值(0-255)
    WORD AnalogChannel;             //模拟通道
}PD_THR;

typedef struct
{
	float res[2];
    DWORD Det_Thr[3];                 //探测器阈值(0-255)
    DWORD AnalogChannel;             //模拟通道
}SEND_PD_THR;


typedef struct
{
    char Index[32];
    char Location[32];
    char Name[32];
}PD_INFO;



typedef struct
{
    PD_DOSERATE s_DoseRate;
    PD_ALARM s_Alarm;
    PD_THR s_Thr;
    float fix[17];
    //PD_INFO s_PdInfo;
    
    WORD Address;
    //WORD Ver;
}PD_PARAM;

//TR3X0  

#ifdef POE_SEN
#define LP_VER "V2.0.0"    //最长6个字节
#else
#define LP_VER "V1.9.6"    //最长6个字节
#endif

#define DEV_VER "V1.0.5"    //最长6个字节
#define BD_BK   0.1

#ifndef POE_SEN
#define P2P_NH         // 点对点拟合 --   暂时只支持单GM管探头
#endif

#define NH_COUNT  16   // 拟合点数
#define FIX_COUNT 11  // 修正段数

#define MIB_CST_DOSERATE_THRESHOLD_ALARM    2    
#define MIB_CST_DOSERATE_THRESHOLD_WARNING    2

#define DOSERATE_PRE_ALARM_BIT (1<<7)        //剂量率预警位
#define DOSERATE_ALARM_BIT (1<<6)            //剂量率报警位
#define DOSE_PRE_ALARM_BIT (1<<5)            //剂量当量预警位
#define DOSE_ALARM_BIT (1<<4)                //剂量当量报警位
#define OVER_RANGE_BIT (1<<3)                //过载报警位
#define LOW_BACK_BIT (1<<2)                  //低本底报警位

enum SNESOR_TYPE{LOW,HIGH,WHOLE}; 
//↑切换临界切换值
#define LOW_SEG 0 //低量程段状态
#define HIG_SEG 1 //高量程段状态


typedef struct
{
    float Canshu1;    // 低量程通道校准因子C        
    float Canshu2;    // 高量程通道校准因子C     
    char yuzhi1[4];    // 
    char yuzhi2[4]; 
	char yuzhi3[4]; 
    char PingHuaShiJian[4]; // 平滑时间,发送的时候需要转换为4字节ASC字符发送
}LP_SYS_PRAM;

typedef struct
{
	float Canshu1;
	float Canshu2;
	DWORD Thr1;
	DWORD Thr2;
	DWORD Thr3;
	DWORD Time;
}HOST_SENSOR_PARAM;


typedef struct
{
    float cps;
    float dr;
}NH_PARAM;

//系统配置参数
typedef struct
{
    // < 80u
    float DI_A;    //低量程通道校准因子A        
    float DI_B;    // 低量程通道校准因子B     
    float DI_C;    //低量程通道校准因子C    
    #ifdef POE_SEN
    float DI_D;    //低量程通道校准因子D

    // 80 - 1300
    float MID_A;    //中量程通道校准因子A        
    float MID_B;    //中量程通道校准因子B     
    float MID_C;    //中量程通道校准因子C    
    float MID_D;    //中量程通道校准因子D  
    #endif
    float GAO_A;    // 高量程通道校准因子A        
    float GAO_B;    // 高量程通道校准因子B 
    float GAO_C;    // 高量程通道校准因子C     
    #ifdef POE_SEN
    float GAO_D;    //高量程通道校准因子D
    #endif
}SYS_JIAOZHUNYINZI;

//报警信息
typedef struct
{
    float  DoseRatePreAlarm;      //剂量率一级报警(uSv/h)
    float  DoseRateAlarm;          //剂量率二级报警(uSv/h)
    float  DosePreAlarm;          //累计剂量预警阀值
    float  DoseAlarm;              //累计剂量报警阀值
}SYS_ALARM;

//计数结构体    
typedef struct
{
    float  DoseRate;
    float  Dose;         //累积剂量
    
    float WAITINGFORUSE;//备用
    DWORD PULSE1;//脉冲1
    DWORD PULSE2;//脉冲2

    /*
    bit7   剂量率一级报警、
    bit6位 剂量率二级报警、
    bit5位 累计剂量率一级报警、
    bit4位 累计剂量率二级报警、
    bit3位 过载报警、       
    bit2位 失效一级报警、
    bit1位 失效二级报警、
    bit0位 低本底故障
    */
    u8 State;    //报警状态
    //INF_SOUND_CERTAIN CERTAIN_KEY;//按钮报
    DWORD PULSE3;//脉冲3 -- 临界探头专用
}STU_DOSERATE;

//脉冲计数结构体    
typedef struct
{
    float  CPS1;  //发送的时候要转为6字节asc编码发送
    float  CPS2;    //发送的时候要转为6字节asc编码发送
    u8 State;    //状态字‘１’为有效，‘０’为无效，７—2位分别表示剂量率一级报警、剂量率二级报警、累计剂量率一级报警、累计剂量率二级报警、过载报警、低本底故障，１、０位未定义
}STU_CPS;
#pragma pack()


typedef struct
{
    u16   Sign;                             // 0x4142             
    u8    address;                          // 探头类型
    //char       VerSion[6];                // 版本号     10
    LP_SYS_PRAM   s_SysParam;               //        20
    SYS_JIAOZHUNYINZI s_Jiaozhun;           //校准因子   24
    SYS_ALARM  s_Alarm;                     //        16
    float      CoCsRate;                    //        4
    
    #ifdef P2P_NH
    NH_PARAM   NhParam[NH_COUNT];         //   -- 128
    #endif

    float   Fix[FIX_COUNT];                  //  -- 40
    //char    Name[32];
    
    u16   ParaCheckSum;//参数校验和
}LP_PARAM;



typedef struct
{
    LP_PARAM    stParam;
    u8 SensorType;
    u8 keydownFlag;//按键按下标志
    u8 keyValue;//按键键值
    u32 keydownTime;//按键按下计数时间*10ms
    u8 isCanReadSensor;
    STU_DOSERATE s_DoseMSG;
    STU_CPS s_CPS;
    u8 range;    //设置的量程
    u8 RealRange;//实际使用的量程
    u8 isSwitching;    // >0正在切换档位，
    u16 DoseRatePreAlarmCnt;//剂量率预警次数，连续2次报警能报警
    u16 DoseRateAlarmCnt;//剂量率报警次数，连续2次报警能报警
    u32 LChannelNoCountTime;            //低通道无计数时间
}LP_SYSTEM_STTAE;

typedef struct
{
    char Index[32];
    char Location[32];
    char Name[32];
    BYTE Address;
    
}SYS_GENERAL_INFO;


typedef struct
{
    WORD  Sign;        // 0xA55A
    SYS_GENERAL_INFO s_General_Info;
    BYTE  res1;
    //HOST_SENSOR_PARAM SenParam[SENSOR_COUNT];   // 14*4 = 56 * 4 = 224
    //HOST_ALRAM_PARA   AlmParam[SENSOR_COUNT];   //  6*4 = 24 * 4 = 96
    PD_PARAM Pd_param;
    WORD  res2;
    LP_PARAM Lp_Param;
       
    BYTE  BkLight;      // 背光亮度 1-100
    //char MP3Txt[112];
    char       VerSion[6];
    BYTE Cps_Enable;                // CPS通道开关
    BYTE Cps_Check;                 // CPS通道开关显示
    //BYTE ProbeSwitch;        //0:GM  1:NaI   2:PD
    WORD  Sum;
}SYS_PARAM;

typedef struct
{
    BYTE Charging;
    WORD Vol;       // 电压/10:保留一位小数 
}BAT_INFO;


extern RUN_STATUS xdata RunStatus;

extern alt_u8 g_Output[OUT_IO_COUNT];
extern BYTE ProbeSwitch;


void WriteParam();
void Delay(WORD ms);
void delay_ms(unsigned int ms);
void Delayms(WORD ms);

void Error();
void OutCtl(alt_u8 id, alt_u8 st);

void GeneralParam();
void DevParamDef();

void Init();
void SaveParam();
void OpenPump();
void ClosePump();
void CheckAlarm();
void UpdataUI();


void DevRun();
void StopAlmLight();
void SendReadFlowCmd(BYTE ch);
void AbnorAlarm();
void SyncModBusDev();
void StartMP3(BYTE id);
void StopMP3(BYTE id);
void CheckDoseRate();
void GetInput();
void PDTask();
void PowerOff();
void ProbeSelect();


void DebugMsg(char *msg);
void DebugInt(int msg);
void DumpCmd(BYTE *dat, BYTE len);

#endif

