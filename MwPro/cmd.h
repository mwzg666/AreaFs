#ifndef _CMD_H_
#define _CMD_H_

#include "main.h"

#define CMD_READ_NH    0xE1
#define CMD_WRITE_NH   0xE2
#define CMD_READ_FIX    0xE3
#define CMD_WRITE_FIX   0xE4
#define CMD_READ_INFO   0xEE
#define CMD_WRITE_INFO   0xEF

extern LP_SYSTEM_STTAE SysRunState;
#pragma pack(1)
typedef struct
{
    u8 head;    //
    u8 length;
    u8 type;
    u8 Id;
    u8 cmd;
    //u8 *data;
    //u16 SumCheck;
    //u8 tail;
}STU_CMD;
#pragma pack()

void DataPro(u8 *cdata, u8 length);

void ACK_CMD_C(void);//握手命令
void ACK_CMD_H(u8 Address);//设置探测器地址


void ACK_CMD_R(void);//读参数命令
void ACK_CMD_W(u8 *cdata);//写参数命令
void ACK_CMD_N(unsigned char *cdata);//设置校准因子
void ACK_CMD_J(void);//读校准因子

void ACK_CMD_V(void);//读计数命令
void ACK_CMD_B(u8 *cdata);//写报警参数命令
void ACK_CMD_F(void);//读报警参数命令
void ACK_CMD_P(void);
void ACK_CMD_T(void);
void ACK_CMD_Q(u8 range);
void ACK_CMD_M(void);

void ReadCS();
void SetCS(u8 *cs);
void ReadNh();
void WriteNh(u8 *dat);
void ReadFix();
void WriteFix(u8 *dat);
void ACK_CMD_RINFO();
void ACK_CMD_WINFO(unsigned char *cdata);

DWORD DwordToSmall(DWORD dat);



void CMD(u32 id,unsigned char dlc,unsigned char *cdata);
u8 GetCautionStatus_Alphy(float CNT);
u8 GetCautionStatus_Beta(float CNT);
void Get_Cur_Effic(unsigned char netro_num);
unsigned char CheckSum(char *buf,int len);
void Updata_BenDi_Value();

#endif /* CMD_CMD_H_ */
