#include "main.h"
#include "mcp4725.h"
#include "i2c.h"


BYTE ThresAddr[SENSOR_COUNT] =
{
    MCP4725_S1_ADDR,
    MCP4725_S2_ADDR,
    MCP4725_S3_ADDR,
    MCP4725_S4_ADDR
};

unsigned int MCP4725_OutVol(BYTE addr,char *voltage)
{
	DWORD uiVolTemp;
    unsigned int vol_tmp;
    BYTE out_data = 0;

    char yuzhibuf[5]={0};
    memcpy(yuzhibuf,voltage,4);
    yuzhibuf[4] = 0;
    vol_tmp = atoi(yuzhibuf);
    
    if(vol_tmp >= MCP4725_REF_VOL)
    {
        //printf("input voltage > ref voltage\r\n");
        return 0;
    }

    uiVolTemp = vol_tmp;
	uiVolTemp = uiVolTemp*4096l/MCP4725_REF_VOL;
	I2C_Start(1);
	out_data = addr<<1;
	I2C_Write_Byte(1);
	I2C_ReadAck(1);
	out_data = ((uiVolTemp>>8)&0xf)|MCP4725_PD_MODE;   // 高8位
    //out_data = uiVolTemp & 0xff;       // 低8位
	I2C_Write_Byte(1);
	I2C_ReadAck(1);
    //out_data = ((uiVolTemp>>8)&0xf)|MCP4725_PD_MODE;   // 高8位
	out_data = uiVolTemp & 0xff;       // 低8位
	I2C_Write_Byte(1);
	I2C_ReadAck(1);
	I2C_Stop(1);

    return 1;
}

#if 0
unsigned int MCP4725_Read(unsigned char addr)
{
	unsigned int uiTemp1,uiTemp2,dat;

	I2C_Start();
	I2C_Write_Byte((addr<<1) | 1);
	I2C_Read_Byte(I2C_ACK);
	uiTemp1 = I2C_Read_Byte(I2C_ACK);
	uiTemp2 = I2C_Read_Byte(I2C_NACK);
	I2C_Stop();
	dat = ((uiTemp1&0xf)<<4 |(uiTemp2>>4) ) | (uiTemp1<<4);	
	return dat;
}
#endif

