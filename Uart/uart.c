#include "uart.h"
#include "Lcd.h"
#include "PD_Probe.h"
#include "ModBusDev.h"
#include "cmd.h"
u16 Rx1_Timer  = 0;
u16 Rx2_Timer  = 0;
u16 Rx3_Timer  = 0;
u16 Rx4_Timer  = 0;


u8  TX1_Cnt;    //���ͼ���
u8  RX1_Cnt;    //���ռ���
bit B_TX1_Busy; //����æ��־

u8  TX2_Cnt;    //���ͼ���
u8  RX2_Cnt;    //���ռ���
bit B_TX2_Busy; //����æ��־

u8  TX3_Cnt;
u8  RX3_Cnt;
bit B_TX3_Busy;

u8  TX4_Cnt;
u8  RX4_Cnt;
bit B_TX4_Busy;


u8  RX1_Buffer[MAX_LENGTH]; //���ջ���
u8  RX2_Buffer[MAX_LENGTH]; //���ջ���
u8  RX3_Buffer[MAX_LENGTH]; //���ջ���
u8  RX4_Buffer[MAX_LENGTH]; //���ջ���


void UART1_config()
{
    /*********** ������ʹ�ö�ʱ��1 *****************/
    {
        SCON |= 0x50;        //8λ����,�ɱ䲨����
        AUXR |= 0x01;       //����1ѡ��ʱ��2Ϊ�����ʷ�����
        AUXR |= 0x04;       //��ʱ��ʱ��1Tģʽ
        T2H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3) / 256);
        T2L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3) % 256);
        //T2L = 0xE8;         //���ö�ʱ��ʼֵ
        //T2H = 0xFF;         //���ö�ʱ��ʼֵ
        AUXR |= 0x10;       //��ʱ��2��ʼ��ʱ
        ES = 1;             //ʹ�ܴ���1�ж�
        ET2 = 0;    //��ֹ�ж�
    }

    /*************************************************/
    //UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 
    //           0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨���� 

    //SCON = (SCON & 0x3f) | 0x40; 

    PS  = 0;    //�жϸ����ȼ�
    PSH = 0;
    //PS  = 1;    //�����ȼ��ж�
//    ES  = 1;    //�����ж�
//    REN = 1;    //�������
    
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

    S2CON = 0x50;       //8λ����,�ɱ䲨����
    AUXR |= 0x04;       //��ʱ��ʱ��1Tģʽ  
    S2CFG |= 0x01;
    T2H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)/ 256);
    T2L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)% 256);
    AUXR |= 0x10;       //��ʱ��2��ʼ��ʱ
    IE2 |= 0x01;        //ʹ�ܴ���2�ж�

    //S2CON = (S2CON & 0x3f) | 0x40;
    
    PS2  = 0;    //�жϸ����ȼ�
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
    S3CON = 0x10;       //8λ����,�ɱ䲨����
    S3CON &= 0xBF;      //����3ѡ��ʱ��2Ϊ�����ʷ�����
    AUXR |= 0x04;       //��ʱ��ʱ��1Tģʽ
    T2H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)/ 256);
    T2L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)% 256);
    //T2L = 0xE0;         //���ö�ʱ��ʼֵ
    //T2H = 0xFE;         //���ö�ʱ��ʼֵ
    
//    T2L = 0xE8;         //���ö�ʱ��ʼֵ 115200
//    T2H = 0xFF;         //���ö�ʱ��ʼֵ
    AUXR |= 0x10;       //��ʱ��2��ʼ��ʱ
    IE2 |= 0x08;        //ʹ�ܴ���3�ж�

    // �ж����ȼ�
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
    S4CON = 0x10;       //8λ����,�ɱ䲨����
    S4CON &= 0xBF;      //����4ѡ��ʱ��2Ϊ�����ʷ�����
    AUXR |= 0x04;       //��ʱ��ʱ��1Tģʽ
    T2H = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)/ 256);
    T2L = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate3)% 256);
//    T2L = 0xE0;         //���ö�ʱ��ʼֵ
//    T2H = 0xFE;         //���ö�ʱ��ʼֵ
    AUXR |= 0x10;       //��ʱ��2��ʼ��ʱ
    IE2 |= 0x10;        //ʹ�ܴ���4�ж�

    // �ж����ȼ�
    PS4H = 0;
    PS4 = 0;    
    
//    ES4 = 1;    // ����4�ж�����
//    S4REN = 1;  // �����ڽ�������
    
    P_SW2 &= ~0x08;   // S4_S == 0; ���ţ�P0.2, P0.3 

    B_TX4_Busy = 0;
    TX4_Cnt = 0;
    RX4_Cnt = 0;
        
}

//�ض���Printf
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
//        printf("����UART1\r\n");
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


