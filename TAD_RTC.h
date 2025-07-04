#ifndef _TAD_RTC_H_
#define _TAD_RTC_H_

#include "TAD_I2C.h"
#include "TAD_SIO.h"

#define RTC_ADDRESS_WRITE 0xD0 // 1101 0000
#define RTC_ADDRESS_READ  0xD1 // 1101 0001

// Inicialitza el bus I2C i prepara el RTC (DS3231)
void RTC_Init(void);

void RTC_Enable1HzInterrupt(void);

// Escriu l?hora al RTC: hora, minuts, segons
void RTC_SetTime(unsigned char hh, unsigned char mm, unsigned char ss);

// Escriu la data al RTC: dia, mes, any complet
void RTC_SetDate(unsigned char dd, unsigned char mo, unsigned char yy);

void RTC_DemanaTime(unsigned char *hh, unsigned char *mm, unsigned char *ss,
                    unsigned char *dd, unsigned char *mo, unsigned char *yy);

void RTC_Motor(void);


#endif
