#ifndef _TAD_FAN_H_
#define _TAD_FAN_H_

#include <xc.h>

void FAN_Init(void);
void FAN_Motor(void);

void FAN_SetDutyFan0(unsigned char duty); // 0-100 (%)
// Pre: duty cal que sigui numero entre 1 i 100
// Post: inicia el pwm del venilador amb el duty indicat

void FAN_SetDutyFan1(unsigned char duty); // 0-100 (%)
// Pre: duty cal que sigui numero entre 1 i 100
// Post: inicia el pwm del venilador amb el duty indicat

#endif
