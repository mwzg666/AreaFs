#ifndef __LCDAPP_H__
#define __LCDAPP_H__

#define BUTTON_OK       0x0D0A
#define LOGIN_PASSWORD  1000


#define SPIN_DEGREE_START    0      //420  //0x01A4      //��תָ����ʼ����
#define SPIN_DEGREE_END     100     //300  //0x012C      //��תָ���������
#define SPIN_DEGREE_SCALE      3    //0x0003      //��תָ��ÿ1�����̶ȶ���

#define SCALE_NUM 11                  //��תָ���������

#define SPIN_SCALE0_VAL   0     //0x0000      //��תָ�����1���� 0
#define SPIN_SCALE1_VAL   1    //0x000A      //��תָ�����2���� 10
#define SPIN_SCALE2_VAL   2    //0x0014      //��תָ�����3���� 20 
#define SPIN_SCALE3_VAL   3    //0x001E      //��תָ�����4���� 30 
#define SPIN_SCALE4_VAL   4    //0x0028      //��תָ�����5���� 40
#define SPIN_SCALE5_VAL   5    //0x0032      //��תָ�����6���� 50 
#define SPIN_SCALE6_VAL   6    //0x003C      //��תָ�����7���� 60
#define SPIN_SCALE7_VAL   7    //0x0046      //��תָ�����8���� 70 
#define SPIN_SCALE8_VAL   8    //0x0050      //��תָ�����9���� 80
#define SPIN_SCALE9_VAL    9   //0x005A      //��תָ�����10���� 90
#define SPIN_SCALE10_VAL 10  //0x0064      //��תָ�����11���� 100

// �豸�Ĵ���
// ��������
#define REG_STRART      0x1900    // ��ʼ��ť
#define REG_PASSWORD    0x1901    // ���ð�ť
//#define REG_MODEHINT  0x1800

// ������
#define REG_DEV_CTL     0x1080   // ��ͣ��ť
#define REG_RETRUN      0x1081   // ���ذ�ť
#define REG_TIME_BTN    0x1082   // ���ʱ�� 

#define REG_DOSERATE   0x1200   //0x1200  //0x1001   // ������
#define REG_NAME       0x1205   //0x1209  //0x1006   // ����
#define REG_UNIT       0x1215   //0x1205  //0x1002   // ��λ
#define REG_STATUS     0x1219   //0x1219  //0x1016   // ����ͼ��
#define REG_CPS1       0x121A   //0x1219  //0x1016   // CPS1
#define REG_CPS2       0x121C   //0x1219  //0x1016   // CPS2


#define REG_SPIN        0x1035  //��תָ��

#define REG_SCALE_START 0x101F      //��ת�̶ȿ�ʼ��ַ
#define REG_SCALE0      0x101F      //��ת�̶�0
#define REG_SCALE1      0x1020      //��ת�̶�1
#define REG_SCALE2      0x1021      //��ת�̶�2
#define REG_SCALE3      0x1022      //��ת�̶�3
#define REG_SCALE4      0x1023      //��ת�̶�4
#define REG_SCALE5      0x1024      //��ת�̶�5
#define REG_SCALE6      0x1025      //��ת�̶�6
#define REG_SCALE7      0x1026      //��ת�̶�7
#define REG_SCALE8      0x1027      //��ת�̶�8
#define REG_SCALE9      0x1028      //��ת�̶�9
#define REG_SCALE10     0x1029      //��ת�̶�10
#define REG_SCALE_UNIT  0x1030      // ��ת��λ


#define REG_Y_BORDER    0x1040   // ��ɫ�߿������ַ
#define REG_R_BORDER    0x1041   // ��ɫ�߿������ַ

#define REG_TOP_BORDER      0x1040   // �ϱ߿������ַ
#define REG_LEFT_BORDER     0x1041   // ��߿������ַ
#define REG_BELOW_BORDER    0x1042   // �±߿������ַ
#define REG_RIGHT_BORDER    0x1043   // �ұ߿������ַ


#define REG_HINT_ENTER  0x1083  // ��ʾ���ϵ�ȷ�ϰ�ť 0x0D0A
#define REG_HINT_END    0x1084  // ��ʾ�������ַ

#define MP_DOSERATE_END     0xA000  // ����������ָ��
#define MP_SCALE_UNIT_END   0xA200  // ���̵�λ����ָ��
#define MP_Y_BORDER         0xA400  // ��ɫ�߿�����ָ��  
#define MP_R_BORDER         0xA500  // ��ɫ�߿�����ָ��  
#define MP_SCALE_END        0xA800  // �̶�ָ������ָ��  
#define MP_UNIT_END         0xA900  // ��λ����ָ��
#define MP_CPS_ONOFF_END    0xAA00  // CPSѡ������ָ��
#define MP_CPS1_END         0xAB00  // CPS1����ָ��
#define MP_CPS2_END         0xAC00  // CPS2����ָ��

// �������ý���
#define REG_SYS_PARAM               0x4000   // ��Ļ������ʼ��ַ
#define REG_SP_NAME                 0x4000
#define REG_SP_ADDR                 0x4010
#define REG_SP_LIGHT                0x4011
#define REG_SP_DOSERATE_PREALM      0x4012
#define REG_SP_DOSERATE_ALM         0x4013
#define REG_SP_MP3                  0x4014
//#define REG_SP_YUZHI                0x4014


#define REG_SP_RET                  0x4050

// �޸�ʱ��
#define REG_ADJ_TIME     0x2010  // ���ذ�ť
#define REG_DATE_TIME    0x2000  // 7����ַ
#define REG_CPS_CHECK    0x2020  // ѡ��CPS��ʾ��ť
#define REG_CPS_ONOFF    0x2021  // CPSͼ��λ��ť


// IO����
#define REG_IO_GREEN       0x5000
#define REG_IO_BLUE        0x5001
#define REG_IO_RED         0x5002
#define REG_IO_SOUND       0x5003

#define REG_MP3_1        0x5004
#define REG_MP3_2        0x5005
#define REG_MP3_3        0x5006
#define REG_MP3_4        0x5007
#define REG_MP3_5        0x5008
#define REG_MP3_6        0x5009
#define REG_MP3_7        0x500A
#define REG_MP3_8        0x500B
#define REG_MP3_9        0x500C
#define REG_MP3_STOP     0x500D



// ���ڽ���
#define REG_DEV_TYPE  0x7000    // �豸�ͺ�

#define PAGE_START   0
#define PAGE_MAIN   1
#define PAGE_SET    3


#pragma pack(1)

// ��ʾ�����н�����Ϣ
typedef struct
{ 
    WORD DoseRate[5];   
    char Name[32];
    char Unit[8];
    WORD FsIcon;
    DWORD Cps1;
    DWORD Cps2;
}RUN_INFO;



typedef struct
{
    char Name[32];
    WORD Address;
    //WORD BkLight;
    DWORD PreAlarmThres;
    DWORD AlarmThres;
    //WORD  Thr;
    char MP3Txt[112];
    //char ModeTxt[16];  // ȡ��ģʽ
}SHOW_PARAM;

typedef struct
{    
    WORD  Cps_Check;
    WORD  Cps_Enable;
}SHOW_CPS;



typedef struct
{
    WORD YearH;
    WORD YearL;
    WORD Month;
    WORD Day;
    WORD Hour;
    WORD Minute;
    WORD Secend;
}LCD_DATE_TIME;

typedef struct
{
    char DevType[16];
    WORD res[2];
    char SoftVer[16];
}DEV_INFO;
#pragma pack()


extern RUN_INFO xdata RunInfo;
extern BYTE DoseRateAlarm;
extern BYTE HisAlarm;
extern bool NeedGetRet;

void EnterPage(BYTE Page);
void ShowDoseRate();
void ShowStatus();
void SendParam();
void ShowDevInfo();
void StatusColor(bool force);

void ShowBorderY();
void ShowBorderR();
void HideBorder();
void ShowCps();
void SendCps();
void Cps_Check();

void ReadReg();
void WriteReg();
void GetRetCode();

#endif

