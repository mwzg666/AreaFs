#ifndef __PD_PROBE_H__
#define __PD_PROBE_H__


void SendReadPD(void);
void SendReadPD_DoseRate(void);
void SendReadPD_Alarm(void);
void SendReadPD_Thr(void);
void SendReadPD_fix(void);
void SendReadPD_Info(void);
void SendReadPD_Address(void);
void SendReadPD_Ver(void);

void WritePD_Alarm();
void WritePD_Thr();
void WritePD_fix();
void WritePD_Info();
void WritePD_Address();

void PDProbe_Ack(BYTE *Buf, BYTE Len);
void Set_PD_DoseRateUnit();

#endif

