#include "uart.h"
#include "Lcd.h"
#include "PD_Probe.h"
#include "ModBusDev.h"
#include "cmd.h"
u16 Rx1_Timer  = 0;
u16 Rx2_Timer  = 0;
u16 Rx3_Timer  = 0;
u16 Rx4_Timer  = 0;


u8  TX1_Cnt;    //发送计数
u8  RX1_Cnt;    //接收计数
bit B_TX1_Busy; //发送忙标志

u8  TX2_Cnt;    //发送计数
u8  RX2_Cnt;    //接收计数
bit B_TX2_Busy; //发送忙标志

u8  TX3_Cnt;
u8  RX3_Cnt;
bit B_TX3_Busy;

u8  TX4_Cnt;
u8  RX4_Cnt;
bit B_TX4_Busy;


u8  RX1_Buffer[MAX_LENGTH]; //接收缓冲
u8  RX2_Buffer[MAX_LENGTH]; //接收缓冲
u8  RX3_Buffer[MAX_LENGTH]; //接收缓冲
u8  RX4_Buffer[MAX_LENGTH]; //接收缓冲


void UART1_config()
{
    /*********** 波特率使用定时器1 *****************/
    {
        SCON |= 0x50;        //8位数据,可变波特率
        AUXR |= 0x01;       //串口1选择定时器2为波特率发生器
        AUXR |= 0x04;       //定时器时钟1T模式
        T2H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3) / 256);
        T2L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3) % 256);
        //T2L = 0xE8;         //设置定时初始值
        //T2H = 0xFF;         //设置定时初始值
        AUXR |= 0x10;       //定时器2开始计时
        ES = 1;             //使能串口1中断
        ET2 = 0;    //禁止中断
    }

    /*************************************************/
    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 
    //           0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率 

    //SCON = (SCON & 0x3f) | 0x40; 

    PS  = 0;    //中断高优先级
    PSH = 0;
    //PS  = 1;    //高优先级中断
//    ES  = 1;    //允许中断
//    REN = 1;    //允许接收
    
    //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 
    //                 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    P_SW1 &= 0x3f;
    P_SW1 |= 0x40;  
    
    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    RX1_Cnt = 0;
}


void UART2_config()
{

    S2CON = 0x50;       //8位数据,可变波特率
    AUXR |= 0x04;       //定时器时钟1T模式  
    S2CFG |= 0x01;
    T2H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)/ 256);
    T2L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)% 256);
    AUXR |= 0x10;       //定时器2开始计时
    IE2 |= 0x01;        //使能串口2中断

    //S2CON = (S2CON & 0x3f) | 0x40;
    
    PS2  = 0;    //中断高优先级
    PS2H = 0;
    
//    ES2   = 1;
//    S2REN = 1;
    P_SW2 &= ~0x01; 

    B_TX2_Busy = 0;
    TX2_Cnt = 0;
    RX2_Cnt = 0;
        
}


void UART3_config()
{
    S3CON = 0x10;       //8位数据,可变波特率
    S3CON &= 0xBF;      //串口3选择定时器2为波特率发生器
    AUXR |= 0x04;       //定时器时钟1T模式
    T2H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)/ 256);
    T2L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)% 256);
    //T2L = 0xE0;         //设置定时初始值
    //T2H = 0xFE;         //设置定时初始值
    
//    T2L = 0xE8;         //设置定时初始值 115200
//    T2H = 0xFF;         //设置定时初始值
    AUXR |= 0x10;       //定时器2开始计时
    IE2 |= 0x08;        //使能串口3中断

    // 中断优先级
    PS3H = 0;
    PS3 = 0;

//    ES3 = 1;
//    S3REN = 1;
    P_SW2 &= ~0x02; 

    B_TX3_Busy = 0;
    TX3_Cnt = 0;
    RX3_Cnt = 0;

        
}

void UART4_config()
{
    S4CON = 0x10;       //8位数据,可变波特率
    S4CON &= 0xBF;      //串口4选择定时器2为波特率发生器
    AUXR |= 0x04;       //定时器时钟1T模式
    T2H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)/ 256);
    T2L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)% 256);
//    T2L = 0xE0;         //设置定时初始值
//    T2H = 0xFE;         //设置定时初始值
    AUXR |= 0x10;       //定时器2开始计时
    IE2 |= 0x10;        //使能串口4中断

    // 中断优先级
    PS4H = 0;
    PS4 = 0;    
    
//    ES4 = 1;    // 串口4中断允许
//    S4REN = 1;  // 允许串口接收数据
    
    P_SW2 &= ~0x08;   // S4_S == 0; 引脚：P0.2, P0.3 

    B_TX4_Busy = 0;
    TX4_Cnt = 0;
    RX4_Cnt = 0;
        
}

//重定向Printf
char putchar(char c )
{
    SBUF = c;
    while(!TI);
    TI = 0;
    return c;
}


void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;
        Rx1_Timer = 0;
        RX1_Buffer[RX1_Cnt] = SBUF;
        
        if(++RX1_Cnt >= MAX_LENGTH)   
        {
            RX1_Cnt = 0;
        }
    }

    if(TI)
    {
        TI = 0;
        B_TX1_Busy = 0;
    }
}



void UART2_int (void) interrupt 8
{
    if(S2RI)
    {
        S2RI = 0;
        RX2_Buffer[RX2_Cnt] = S2BUF;
        Rx2_Timer = 0;
        
        if(++RX2_Cnt >= MAX_LENGTH)   
        {
            RX2_Cnt = 0;
        }
    }

    if(S2TI)
    {
        S2TI = 0;
        B_TX2_Busy = 0;
    }
}


void UART3_int (void) interrupt 17
{
    if(S3RI)
    {
        S3RI = 0;
        Rx3_Timer = 0;
        RX3_Buffer[RX3_Cnt] = S3BUF;
        
        if(++RX3_Cnt >= MAX_LENGTH)   
        {
            RX3_Cnt = 0;
        }
    }

    if(S3TI)
    {
        S3TI = 0;
        B_TX3_Busy = 0;
    }
}

void UART4_int (void) interrupt 18
{
    if(S4RI)
    {
        S4RI = 0;
        Rx4_Timer = 0;
        RX4_Buffer[RX4_Cnt] = S4BUF;
        
        if(++RX4_Cnt >= MAX_LENGTH)   
        {
            RX4_Cnt = 0;
        }
    }

    if(S4TI)
    {
        S4TI = 0;
        B_TX4_Busy = 0;
    }
}


void Uart1Send(u8 *buf, u8 len)
{
    u8 i;
    for (i=0;i<len;i++)     
    {
        SBUF = buf[i];
        B_TX1_Busy = 1;
        while(B_TX1_Busy);
    }
}

void Uart2Send(u8 *buf, u8 len)
{
    u8 i;
    for (i=0;i<len;i++)     
    {
        S2BUF = buf[i];
        B_TX2_Busy = 1;
        while(B_TX2_Busy);
    }  
}

void Uart3Send(u8 *buf, u8 len)
{
    u8 i;

    for (i=0;i<len;i++)     
    {
        S3BUF = buf[i];
        B_TX3_Busy = 1;
        while(B_TX3_Busy);
    }

}

void Uart4Send(u8 *buf, u8 len)
{
    u8 i;
    RS485_EN(1);
    for (i=0;i<len;i++)     
    {
        S4BUF = buf[i];
        B_TX4_Busy = 1;
        while(B_TX4_Busy);
    }
    RS485_EN(0);
}


void ClearUart1Buf()
{
    memset(RX1_Buffer,0,MAX_LENGTH);
    RX1_Cnt = 0;
}


void ClearUart2Buf()
{
    memset(RX2_Buffer,0,MAX_LENGTH);
    RX2_Cnt = 0;
}

void ClearUart3Buf()
{
    memset(RX3_Buffer,0,MAX_LENGTH);
    RX3_Cnt = 0;
}

void ClearUart4Buf()
{
    memset(RX4_Buffer,0,MAX_LENGTH);
    RX4_Cnt = 0;
}

void Uart1Hnd()
{
    if (Rx1_Timer > 20)
    {
        Rx1_Timer = 0;
//        printf("进入UART1\r\n");
//        printf("RX1 = %s\r\n",RX1_Buffer);
        //DumpCmd(RX1_Buffer, RX1_Cnt);
        memcpy(RecvBuf1,RX1_Buffer, RX1_Cnt);
        RecLength1 = RX1_Cnt;
        ClearUart1Buf();
        
        HndLcdData();
    }
}


void Uart2Hnd()
{
    if (Rx2_Timer > 20)
    {
        Rx2_Timer = 0;
        //DumpCmd(RX2_Buffer, RX2_Cnt);
        DataPro(RX2_Buffer,RX2_Cnt);
        ClearUart2Buf();
    }
}



void Uart3Hnd()
{
    if (Rx3_Timer > 20)
    {
        Rx3_Timer = 0;
        //Error();
        //printf("RX3_Buffer = %s\r\n",RX3_Buffer);
        //PDProbe_Ack((BYTE *)RX3_Buffer,(BYTE)RX3_Cnt);
        
        ClearUart3Buf();
    }
}


void Uart4Hnd()
{
    if (Rx4_Timer > 20)
    {
        Rx4_Timer = 0;
        //HndModBusRecv(RS485, RX4_Buffer,RX4_Cnt);
        PDProbe_Ack((BYTE *)RX4_Buffer,(BYTE)RX4_Cnt);
        ClearUart4Buf();
    }
}


