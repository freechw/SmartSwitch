#include "hal_types.h"

#define LED_OFF 0
#define LED_ON 1

void sysInit(void);
void sysPinInit(void);
void sysAdcInit(void);
uint16 sysAdcGet(void);
void sysGetVoltage(void);
void sysTimInit(void);
void sysUsartInit(void);
void sysSendByte(unsigned char c);
void sysSendString(unsigned char *str);
void sysPwmSet(uint8 ucFlag);
void Delay(unsigned int xms);
void ledSet(uint8 status);