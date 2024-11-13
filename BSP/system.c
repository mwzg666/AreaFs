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
    HIRCCR = 0x80;           // �����ڲ�����IRC
    while(!(HIRCCR & 1));
    CLKSEL = 0x00; 
}


//========================================================================
// ��������: void Timer0_Init(void)	
// ��������: ��ʱ��ԴΪ11.0592MHz��TIMER0��ʼ��Ϊ10ms��ʱ
// ��ڲ���: @��
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2024.6.13
// ��ǰ����:
// ������ע: 
//========================================================================
void Timer0_Init(void)		//100����@6MHz
{
	AUXR = 0x00;    //Timer0 set as 12T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
    
    // �ж����ȼ�3
    PT0  = 0;
    PT0H = 0;

}

//========================================================================
// ��������: void Timer3_Init(void)
// ��������: TIMER3��ʼ��Ϊ�½��ؼ���ģʽ
// ��ڲ���: @��
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2024.6.13
// ��ǰ����:
// ������ע: 
//========================================================================
void Timer3_Init(void)
{
    T3L = 0x00;
    T3H = 0x00;
    T4T3M |= 0x0C;
    IE2 |= 0x20;             //Timer3 interrupt enable
}



//========================================================================
// ��������: void Timer4_Init(void)
// ��������: TIMER4��ʼ��Ϊ�½��ؼ���ģʽ
// ��ڲ���: @��
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2024.6.13
// ��ǰ����:
// ������ע: 
//========================================================================
void Timer4_Init(void)
{
    T4L = 0x00;
    T4H = 0x00;
    T4T3M |= 0xC0;
    IE2 |= 0x40;             //Timer4 interrupt enable

}

//========================================================================
// ��������: void Timer1_Init(void)	
// ��������: TIMER1��ʼ��Ϊ�½��ؼ���ģʽ
// ��ڲ���: @��
// ��������: ��
// ��ǰ�汾: VER1.0
// �޸�����: 2024.6.13
// ��ǰ����:
// ������ע: 
//========================================================================
void Timer1_Init(void)		//10����@11.0592MHz
{
	TL1 = 0x00;				//���ö�ʱ��ʼֵ
	TH1 = 0x00;				//���ö�ʱ��ʼֵ
	TMOD = 0x50;				//���TF1��־
	TR1 = 1;				//��ʱ��1��ʼ��ʱ
	ET1 = 1;				//ʹ�ܶ�ʱ��1�ж�
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
// ��������: u32 GetCounter(void)
// ��������: ��ȡTIMER3���������
// ��ڲ���: @��
// ��������: ���������ֵ
// ��ǰ�汾: VER1.0
// �޸�����: 2024.6.13
// ��ǰ����:
// ������ע: 
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

void Timer0_Isr (void) interrupt 1   //Timer0�ж�
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

void Timer1_Isr() interrupt 3      //Timer1�ж�
{
    LowOverCnt1++;
   
}

void Timer3_Isr() interrupt 19      //Timer3�ж�
{
    LowOverCnt3++;
   
}

void Timer4_Isr() interrupt 20      //Timer4�ж�
{
    HightOverCnt4++;
   
}



