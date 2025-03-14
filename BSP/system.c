#include "main.h"
#include "uart.h"
#include "system.h"

u16  Timer0Cnt = 0;
u32  Pluse1Cnt = 0;
u32  Pluse3Cnt = 0;
u32  Pluse4Cnt = 0;
u16  IDLCnt = 0;

u8   LowOverCnt1 = 0;
u8   LowOverCnt3 = 0;
u8   HightOverCnt4 = 0;

extern LP_SYSTEM_STTAE SysRunState;
extern u16 Eth_delaytime;
extern bool Eth_delayflag;

void SysInit()
{
    HIRCCR = 0x80;           // 启动内部高速IRC
    while(!(HIRCCR & 1));
    CLKSEL = 0x00; 
}


//========================================================================
// 函数名称: void Timer0_Init(void)	
// 函数功能: 在时钟源为11.0592MHz下TIMER0初始化为10ms定时
// 入口参数: @无
// 函数返回: 无
// 当前版本: VER1.0
// 修改日期: 2024.6.13
// 当前作者:
// 其他备注: 
//========================================================================
void Timer0_Init(void)		//100毫秒@6MHz
{
	AUXR = 0x00;    //Timer0 set as 12T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
    
    // 中断优先级3
    PT0  = 0;
    PT0H = 0;

}

//========================================================================
// 函数名称: void Timer3_Init(void)
// 函数功能: TIMER3初始化为下降沿计数模式
// 入口参数: @无
// 函数返回: 无
// 当前版本: VER1.0
// 修改日期: 2024.6.13
// 当前作者:
// 其他备注: 
//========================================================================
void Timer3_Init(void)
{
    T3L = 0x00;
    T3H = 0x00;
    T4T3M |= 0x0C;
    IE2 |= 0x20;             //Timer3 interrupt enable
}



//========================================================================
// 函数名称: void Timer4_Init(void)
// 函数功能: TIMER4初始化为下降沿计数模式
// 入口参数: @无
// 函数返回: 无
// 当前版本: VER1.0
// 修改日期: 2024.6.13
// 当前作者:
// 其他备注: 
//========================================================================
void Timer4_Init(void)
{
    T4L = 0x00;
    T4H = 0x00;
    T4T3M |= 0xC0;
    IE2 |= 0x40;             //Timer4 interrupt enable

}

//========================================================================
// 函数名称: void Timer1_Init(void)	
// 函数功能: TIMER1初始化为下降沿计数模式
// 入口参数: @无
// 函数返回: 无
// 当前版本: VER1.0
// 修改日期: 2024.6.13
// 当前作者:
// 其他备注: 
//========================================================================
void Timer1_Init(void)		//10毫秒@11.0592MHz
{
	TL1 = 0x00;				//设置定时初始值
	TH1 = 0x00;				//设置定时初始值
	TMOD = 0x50;				//清除TF1标志
	TR1 = 1;				//定时器1开始计时
	ET1 = 1;				//使能定时器1中断
}

u32 GetNaICounter(void)
{
  	u32 Return = 0;
    Pluse1Cnt =((u32)LowOverCnt1*65536) | (u32)((u32)TH1*256+TL1) ;
    Return =Pluse1Cnt;
    //TR1 = 0;
    TH1 = 0;
    TL1 = 0;
    //TR1 = 1;
    //printf("Pluse1Cnt = %d\r\n",Pluse1Cnt);
    LowOverCnt1 = 0;
	Pluse1Cnt = 0;
	return Return;
}

//========================================================================
// 函数名称: u32 GetCounter(void)
// 函数功能: 获取TIMER3的脉冲计数
// 入口参数: @无
// 函数返回: 脉冲计数的值
// 当前版本: VER1.0
// 修改日期: 2024.6.13
// 当前作者:
// 其他备注: 
//========================================================================
u32 GetCounter(void)
{
  	u32 Return = 0;
    Pluse3Cnt =((u32)LowOverCnt3*65536) | (u32)((u32)T3H*256+T3L) ;
    Return =Pluse3Cnt;
    T3R = 0;
    T3H = 0;
    T3L = 0;
    T3R = 1;
    LowOverCnt3 = 0;
	Pluse3Cnt = 0;
	return Return;
}

u32 GetHightCounter(void)
{
  	u32 Return = 0; 
    Pluse4Cnt = ((u32)HightOverCnt4*65536) | (u32)((u32)T4H*256+T4L) ;
    Return = Pluse4Cnt;

    T4R = 0;
    T4H = 0;
    T4L = 0;
    T4R = 1;
    HightOverCnt4 = 0;
	Pluse4Cnt = 0;
	return Return;
}

void Timer0_Isr (void) interrupt 1   //Timer0中断
{
    Timer0Cnt ++;
    if(SysRunState.keydownFlag == 1)
    {
         SysRunState.keydownTime++;//10ms*n
    }
//  if(Eth_delayflag)
//    {
//        Eth_delaytime -= 10;
//        if(!Eth_delaytime)
//        {
//            Eth_delayflag = 0;
//        }
//    }
}

void Timer1_Isr() interrupt 3      //Timer1中断
{
    LowOverCnt1++;
   
}

void Timer3_Isr() interrupt 19      //Timer3中断
{
    LowOverCnt3++;
   
}

void Timer4_Isr() interrupt 20      //Timer4中断
{
    HightOverCnt4++;
   
}



