#ifndef _TAD_RGB_H_
#define _TAD_RGB_H_

#include <xc.h>
#include "TAD_TIMER.h"

#define T_500ms 500

void RGB_Init(void);
void RGB_Motor(void);

void RGB_SetColor(unsigned char r, unsigned char g, unsigned char b);
void RGB_SetColorAlternatCritic(void);

#endif
