#ifndef __MODBUS_DEV_H__
#define __MODBUS_DEV_H__

#define MODBUS_PARAM_ADD  0xA800
#define MODBUS_STATUS_ADD 0xA900
#define MODBUS_INFO_ADD  0xFFFD
#define MODBUS_REM_CTL  0xFFF0


#define MODBUS_DOSERATE_ADD     0x8000
#define MODBUS_ALARM_ADD        0x8010
#define MODBUS_THR_ADD          0x8020
#define MODBUS_FACTOR_ADD       0x9000
#define MODBUS_DEV_INFO         0xFF00

#define MODBUS_DEV_ADD          0xFFF0
#define MODBUS_DEV_VER          0xFFFF



#pragma pack(1)
typedef struct
{
    PD_DOSERATE s_DoseRate;
    PD_ALARM s_Alarm;
    PD_THR s_Thr;
    float fix[17];
    PD_INFO s_PdInfo;
    WORD Address;
    WORD Ver;
}MODBUS_PD_PARAM;


#pragma pack()


extern MODBUS_PD_PARAM xdata ModBusPD_Param;

BYTE ReadAck(BYTE Mode);
void HndModBusRecv(BYTE Mode, BYTE *buf, BYTE len);

#endif /*PROTOCOL_H_*/

