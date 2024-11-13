#ifndef SYSTEM_H
#define SYSTEM_H

#include  "main.h"
#ifdef __cplusplus
extern  "C" {
#endif
   
//===============================变量定义=============================




//===============================结构体定义===========================




//===============================变量声明=============================
extern u16 Timer0Cnt;
extern u32 Pluse3Cnt;
extern u32 Pluse4Cnt;   
extern u16  IDLCnt;

    
//===============================函数声明=============================   
void SysInit();
void Timer0_Init(void);
void Timer3_Init(void);
void Timer4_Init(void);

void SetVcoreUp (unsigned int level);
void Timer1_Init(void);
u32 GetNaICounter(void);
u32 GetCounter(void);
u32 GetHightCounter(void);


    
#ifdef __cplusplus
}
#endif

#endif
