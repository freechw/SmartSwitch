#include <stdio.h>
#include <string.h>
#include "hal_flash.h"
#include "onboard.h"
#include "delay.h"
#include "uart.h"
#include "global.h"
#include "utils.h"
#include "ds18b20.h"
#include "adc.h"
#include "dht22.h"

#include "app.h"

void FactoryReset(void)
{
  UART_String("Performing factory reset...");
  for (int i = HAL_NV_PAGE_BEG; i <= (HAL_NV_PAGE_BEG + HAL_NV_PAGE_CNT); i++)
  {
    HalFlashErase(i);
  }
  
  UART_String("Performing system reset...");
  SystemReset();
}

void Relais(uint8 state)
{
  /*
  if (state)	
  {
    sysPwmSet(TWO);
  }
  else
  {
    sysPwmSet(OFF);
  }
  STATE_LIGHT = state;
  */
}

unsigned char dat[4];
void Measure_Sensor(void)
{
  char buffer[100];
  uint8 i;
  
  // make new measurement depending of autodetected sensor type
  if (TEMP_SENSOR == -1) 
    DHT22_Measure();
  else if (TEMP_SENSOR != 0) 
  {
    ds18b20_start_conversion();
    
    i = ds18b20_current;
    
    EXT_Temperatures[i] = ds18b20_get_sensor_temperature(i);
    sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X%02X: %.2f °C",
            ds18b20_FoundROM[i][7],ds18b20_FoundROM[i][6],ds18b20_FoundROM[i][5],ds18b20_FoundROM[i][4],
            ds18b20_FoundROM[i][3],ds18b20_FoundROM[i][2],ds18b20_FoundROM[i][1],ds18b20_FoundROM[i][0],
            EXT_Temperatures[i]);
    if(i==0)
      EXT_Temperature = EXT_Temperatures[0];
    UART_String(buffer);
    
    sprintf(EXT_Temperature_string, "\x01%02X%02X%02X%02X%02X%02X%02X%02X:%.2f",
            ds18b20_FoundROM[i][7],ds18b20_FoundROM[i][6],ds18b20_FoundROM[i][5],ds18b20_FoundROM[i][4],
            ds18b20_FoundROM[i][3],ds18b20_FoundROM[i][2],ds18b20_FoundROM[i][1],ds18b20_FoundROM[i][0],
            EXT_Temperatures[i]);
    EXT_Temperature_string[0] = strlen(EXT_Temperature_string) - 1;
    
    
    ds18b20_current++;
    
    if(ds18b20_current >= ds18b20_numROMs)
      ds18b20_current = 0;
    
  }
}
