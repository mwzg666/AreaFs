#ifndef __LCDAPP_H__
#define __LCDAPP_H__

#define BUTTON_OK       0x0D0A
#define LOGIN_PASSWORD  1000


#define SPIN_DEGREE_START    0      //420  //0x01A4      //旋转指针起始度数
#define SPIN_DEGREE_END     100     //300  //0x012C      //旋转指针结束度数
#define SPIN_DEGREE_SCALE      3    //0x0003      //旋转指针每1剂量刻度度数

#define SCALE_NUM 11                  //旋转指针变量个数

#define SPIN_SCALE0_VAL   0     //0x0000      //旋转指针变量1数据 0
#define SPIN_SCALE1_VAL   1    //0x000A      //旋转指针变量2数据 10
#define SPIN_SCALE2_VAL   2    //0x0014      //旋转指针变量3数据 20 
#define SPIN_SCALE3_VAL   3    //0x001E      //旋转指针变量4数据 30 
#define SPIN_SCALE4_VAL   4    //0x0028      //旋转指针变量5数据 40
#define SPIN_SCALE5_VAL   5    //0x0032      //旋转指针变量6数据 50 
#define SPIN_SCALE6_VAL   6    //0x003C      //旋转指针变量7数据 60
#define SPIN_SCALE7_VAL   7    //0x0046      //旋转指针变量8数据 70 
#define SPIN_SCALE8_VAL   8    //0x0050      //旋转指针变量9数据 80
#define SPIN_SCALE9_VAL    9   //0x005A      //旋转指针变量10数据 90
#define SPIN_SCALE10_VAL 10  //0x0064      //旋转指针变量11数据 100

// 设备寄存器
// 启动界面
#define REG_STRART      0x1900    // 开始按钮
#define REG_PASSWORD    0x1901    // 设置按钮
//#define REG_MODEHINT  0x1800

// 主界面
#define REG_DEV_CTL     0x1080   // 启停按钮
#define REG_RETRUN      0x1081   // 返回按钮
#define REG_TIME_BTN    0x1082   // 点击时间 

#define REG_DOSERATE   0x1200   //0x1200  //0x1001   // 剂量率
#define REG_NAME       0x1205   //0x1209  //0x1006   // 名称
#define REG_UNIT       0x1215   //0x1205  //0x1002   // 单位
#define REG_STATUS     0x1219   //0x1219  //0x1016   // 辐射图标
#define REG_CPS1       0x121A   //0x1219  //0x1016   // CPS1
#define REG_CPS2       0x121C   //0x1219  //0x1016   // CPS2


#define REG_SPIN        0x1035  //旋转指针

#define REG_SCALE_START 0x101F      //旋转刻度开始地址
#define REG_SCALE0      0x101F      //旋转刻度0
#define REG_SCALE1      0x1020      //旋转刻度1
#define REG_SCALE2      0x1021      //旋转刻度2
#define REG_SCALE3      0x1022      //旋转刻度3
#define REG_SCALE4      0x1023      //旋转刻度4
#define REG_SCALE5      0x1024      //旋转刻度5
#define REG_SCALE6      0x1025      //旋转刻度6
#define REG_SCALE7      0x1026      //旋转刻度7
#define REG_SCALE8      0x1027      //旋转刻度8
#define REG_SCALE9      0x1028      //旋转刻度9
#define REG_SCALE10     0x1029      //旋转刻度10
#define REG_SCALE_UNIT  0x1030      // 旋转单位


#define REG_Y_BORDER    0x1040   // 黄色边框变量地址
#define REG_R_BORDER    0x1041   // 红色边框变量地址

#define REG_TOP_BORDER      0x1040   // 上边框变量地址
#define REG_LEFT_BORDER     0x1041   // 左边框变量地址
#define REG_BELOW_BORDER    0x1042   // 下边框变量地址
#define REG_RIGHT_BORDER    0x1043   // 右边框变量地址


#define REG_HINT_ENTER  0x1083  // 提示框上的确认按钮 0x0D0A
#define REG_HINT_END    0x1084  // 提示框变量地址

#define MP_DOSERATE_END     0xA000  // 剂量率描述指针
#define MP_SCALE_UNIT_END   0xA200  // 表盘单位描述指针
#define MP_Y_BORDER         0xA400  // 黄色边框描述指针  
#define MP_R_BORDER         0xA500  // 红色边框描述指针  
#define MP_SCALE_END        0xA800  // 刻度指针描述指针  
#define MP_UNIT_END         0xA900  // 单位描述指针
#define MP_CPS_ONOFF_END    0xAA00  // CPS选择描述指针
#define MP_CPS1_END         0xAB00  // CPS1描述指针
#define MP_CPS2_END         0xAC00  // CPS2描述指针

// 参数设置界面
#define REG_SYS_PARAM               0x4000   // 屏幕参数起始地址
#define REG_SP_NAME                 0x4000
#define REG_SP_ADDR                 0x4010
#define REG_SP_LIGHT                0x4011
#define REG_SP_DOSERATE_PREALM      0x4012
#define REG_SP_DOSERATE_ALM         0x4013
#define REG_SP_MP3                  0x4014
//#define REG_SP_YUZHI                0x4014


#define REG_SP_RET                  0x4050

// 修改时间
#define REG_ADJ_TIME     0x2010  // 返回按钮
#define REG_DATE_TIME    0x2000  // 7个地址
#define REG_CPS_CHECK    0x2020  // 选择CPS显示按钮
#define REG_CPS_ONOFF    0x2021  // CPS图标位按钮


// IO测试
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



// 关于界面
#define REG_DEV_TYPE  0x7000    // 设备型号

#define PAGE_START   0
#define PAGE_MAIN   1
#define PAGE_SET    3


#pragma pack(1)

// 显示屏运行界面信息
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
    //char ModeTxt[16];  // 取样模式
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

