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

#define Unit_uSv 0           // ��Sv/h 
#define Unit_mSv  1          // mSv/h 
#define Unit_Sv   2          // Sv/h

//Ԥ��ѡ��˿�
#define D4() (P6 & (1<<4))       
#define D3() (P6 & (1<<5)) 
#define D2() (P6 & (1<<6))    //PD     Switch_6
#define D1() (P6 & (1<<7))    //NaI   Switch_5


#define DEV_NAME "FS-1F-01"

#define POE_SEN     // POE̽ͷ�Ǹ�� 

#define MAIN_Fosc        11059200UL    // 11.0592M
//#define MAIN_Fosc        35000000UL    // 35M

#define SysTick     9216   // 10ms    // ��/��, ϵͳ�δ�Ƶ��, ��4000~16000֮��

#define Timer0_Reload   (65536UL - SysTick)     //Timer 0 �ж�Ƶ��

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

#define PW_MAIN(x)   (x)?(P7 |= (1<<5)):(P7 &= ~(1<<5))    // ����Դ����
#define RUN_LED(x) (x)?(P4 |= (1<<0)):(P4 &= ~(1<<0))    // ����LED

// ָʾ��
#define GRE_LIGHT(x) (x)?(P2 |= (1<<1)):(P2 &= ~(1<<1))      // �̵�
#define RED_LIGHT(x) (x)?(P2 |= (1<<2)):(P2 &= ~(1<<2))      // ���
#define BLU_LIGHT(x) (x)?(P2 |= (1<<3)):(P2 &= ~(1<<3))      // ����
#define ALARM(x)      (x)?(P2 |= (1<<0)):(P2 &= ~(1<<0))      // ������

#define SINU_M(x)     (P3 & (1<<2))      

#define RS485_EN(x)  (x)?(P5 |= (1<<2)):(P5 &= ~(1<<2))     //����4 485ʹ��

#define POWER_LOCK()    (P4 & (1<<5))       // ���ػ���
#define LOCK_BIT()      (1<<5)

#define BAT_CHARGE()    (P5 & (1<<1))        //��س��״̬

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

#define PAUSE_M(x)   (x)?(P7 |= (1<<0)):(P7 &= ~(1<<0))     //������ͣ

//HLK_RM08K  ETH
#define ES0_M(x)    (x)?(P3 |= (1<<3)):(P3 &= ~(1<<3))     
#define RST(x)      (x)?(P3 |= (1<<4)):(P3 &= ~(1<<4))
#define ES1_M(x)    (x)?(P5 |= (1<<0)):(P5 &= ~(1<<0))


#define CMD_OUT_CTL      'O'  // 0x4F
#define CMD_IN_STATUE    'I'  // 0x49
#define CMD_GET_VER      'M'  // 0x56
#define CMD_DATA_TT      'T'  // 0x54 ����͸��
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
    float  DoseRatePreAlarm;        //������һ������(uSv/h)
    float  DoseRateAlarm;              //�����ʶ�������(uSv/h)
    float  DosePreAlarm;              //�ۼƼ���һ������(uSv)
    float  DoseAlarm;                  //�ۼƼ�����������(uSv)
}PD_ALARM;

typedef struct
{
	//float res[2];
    WORD Det_Thr[3];                 //̽������ֵ(0-255)
    WORD AnalogChannel;             //ģ��ͨ��
}PD_THR;

typedef struct
{
	float res[2];
    DWORD Det_Thr[3];                 //̽������ֵ(0-255)
    DWORD AnalogChannel;             //ģ��ͨ��
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
#define LP_VER "V2.0.0"    //�6���ֽ�
#else
#define LP_VER "V1.9.6"    //�6���ֽ�
#endif

#define DEV_VER "V1.0.5"    //�6���ֽ�
#define BD_BK   0.1

#ifndef POE_SEN
#define P2P_NH         // ��Ե���� --   ��ʱֻ֧�ֵ�GM��̽ͷ
#endif

#define NH_COUNT  16   // ��ϵ���
#define FIX_COUNT 11  // ��������

#define MIB_CST_DOSERATE_THRESHOLD_ALARM    2    
#define MIB_CST_DOSERATE_THRESHOLD_WARNING    2

#define DOSERATE_PRE_ALARM_BIT (1<<7)        //������Ԥ��λ
#define DOSERATE_ALARM_BIT (1<<6)            //�����ʱ���λ
#define DOSE_PRE_ALARM_BIT (1<<5)            //��������Ԥ��λ
#define DOSE_ALARM_BIT (1<<4)                //������������λ
#define OVER_RANGE_BIT (1<<3)                //���ر���λ
#define LOW_BACK_BIT (1<<2)                  //�ͱ��ױ���λ

enum SNESOR_TYPE{LOW,HIGH,WHOLE}; 
//���л��ٽ��л�ֵ
#define LOW_SEG 0 //�����̶�״̬
#define HIG_SEG 1 //�����̶�״̬


typedef struct
{
    float Canshu1;    // ������ͨ��У׼����C        
    float Canshu2;    // ������ͨ��У׼����C     
    char yuzhi1[4];    // 
    char yuzhi2[4]; 
	char yuzhi3[4]; 
    char PingHuaShiJian[4]; // ƽ��ʱ��,���͵�ʱ����Ҫת��Ϊ4�ֽ�ASC�ַ�����
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

//ϵͳ���ò���
typedef struct
{
    // < 80u
    float DI_A;    //������ͨ��У׼����A        
    float DI_B;    // ������ͨ��У׼����B     
    float DI_C;    //������ͨ��У׼����C    
    #ifdef POE_SEN
    float DI_D;    //������ͨ��У׼����D

    // 80 - 1300
    float MID_A;    //������ͨ��У׼����A        
    float MID_B;    //������ͨ��У׼����B     
    float MID_C;    //������ͨ��У׼����C    
    float MID_D;    //������ͨ��У׼����D  
    #endif
    float GAO_A;    // ������ͨ��У׼����A        
    float GAO_B;    // ������ͨ��У׼����B 
    float GAO_C;    // ������ͨ��У׼����C     
    #ifdef POE_SEN
    float GAO_D;    //������ͨ��У׼����D
    #endif
}SYS_JIAOZHUNYINZI;

//������Ϣ
typedef struct
{
    float  DoseRatePreAlarm;      //������һ������(uSv/h)
    float  DoseRateAlarm;          //�����ʶ�������(uSv/h)
    float  DosePreAlarm;          //�ۼƼ���Ԥ����ֵ
    float  DoseAlarm;              //�ۼƼ���������ֵ
}SYS_ALARM;

//�����ṹ��    
typedef struct
{
    float  DoseRate;
    float  Dose;         //�ۻ�����
    
    float WAITINGFORUSE;//����
    DWORD PULSE1;//����1
    DWORD PULSE2;//����2

    /*
    bit7   ������һ��������
    bit6λ �����ʶ���������
    bit5λ �ۼƼ�����һ��������
    bit4λ �ۼƼ����ʶ���������
    bit3λ ���ر�����       
    bit2λ ʧЧһ��������
    bit1λ ʧЧ����������
    bit0λ �ͱ��׹���
    */
    u8 State;    //����״̬
    //INF_SOUND_CERTAIN CERTAIN_KEY;//��ť��
    DWORD PULSE3;//����3 -- �ٽ�̽ͷר��
}STU_DOSERATE;

//��������ṹ��    
typedef struct
{
    float  CPS1;  //���͵�ʱ��ҪתΪ6�ֽ�asc���뷢��
    float  CPS2;    //���͵�ʱ��ҪתΪ6�ֽ�asc���뷢��
    u8 State;    //״̬�֡�����Ϊ��Ч��������Ϊ��Ч������2λ�ֱ��ʾ������һ�������������ʶ����������ۼƼ�����һ���������ۼƼ����ʶ������������ر������ͱ��׹��ϣ�������λδ����
}STU_CPS;
#pragma pack()


typedef struct
{
    u16   Sign;                             // 0x4142             
    u8    address;                          // ̽ͷ����
    //char       VerSion[6];                // �汾��     10
    LP_SYS_PRAM   s_SysParam;               //        20
    SYS_JIAOZHUNYINZI s_Jiaozhun;           //У׼����   24
    SYS_ALARM  s_Alarm;                     //        16
    float      CoCsRate;                    //        4
    
    #ifdef P2P_NH
    NH_PARAM   NhParam[NH_COUNT];         //   -- 128
    #endif

    float   Fix[FIX_COUNT];                  //  -- 40
    //char    Name[32];
    
    u16   ParaCheckSum;//����У���
}LP_PARAM;



typedef struct
{
    LP_PARAM    stParam;
    u8 SensorType;
    u8 keydownFlag;//�������±�־
    u8 keyValue;//������ֵ
    u32 keydownTime;//�������¼���ʱ��*10ms
    u8 isCanReadSensor;
    STU_DOSERATE s_DoseMSG;
    STU_CPS s_CPS;
    u8 range;    //���õ�����
    u8 RealRange;//ʵ��ʹ�õ�����
    u8 isSwitching;    // >0�����л���λ��
    u16 DoseRatePreAlarmCnt;//������Ԥ������������2�α����ܱ���
    u16 DoseRateAlarmCnt;//�����ʱ�������������2�α����ܱ���
    u32 LChannelNoCountTime;            //��ͨ���޼���ʱ��
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
       
    BYTE  BkLight;      // �������� 1-100
    //char MP3Txt[112];
    char       VerSion[6];
    BYTE Cps_Enable;                // CPSͨ������
    BYTE Cps_Check;                 // CPSͨ��������ʾ
    //BYTE ProbeSwitch;        //0:GM  1:NaI   2:PD
    WORD  Sum;
}SYS_PARAM;

typedef struct
{
    BYTE Charging;
    WORD Vol;       // ��ѹ/10:����һλС�� 
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

