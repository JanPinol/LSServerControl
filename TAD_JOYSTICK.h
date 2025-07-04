#ifndef _TAD_JOYSTICK_H_
#define _TAD_JOYSTICK_H_

#include <xc.h>

#define CERT 1
#define FALS 0

#define THRESHOLD_HIGH 200
#define THRESHOLD_LOW 50

void JOY_Init(void);
// Pre: --
// Post: Configura els registres de configuracio
// ADCON0,ADCON1 per a que hi hagin 2
// canals el RA0 i el RA1 analogics per a fer la
// conversio de les posicions XY del Joystick.

char JOY_IsUp(void);
// Pre: --
// Post: Retorna CERT si el joystick esta en una
// posicio de l'eix Y (UP) i l'eix X (CENTER),
// retona FALS altrament.

char JOY_IsDown(void);
// Pre: --
// Post: Retorna CERT si el joystick esta en una
// posicio de l'eix Y (DOWN) i l'eix X (CENTER),
// retona FALS altrament.

char JOY_IsLeft(void);
// Pre: --
// Post: Retorna CERT si el joystick esta en una
// posicio de l'eix Y (DOWN) i l'eix X (CENTER),
// retona FALS altrament.

char JOY_IsRight(void);
// Pre: --
// Post: Retorna CERT si el joystick esta en una
// posicio de l'eix Y (DOWN) i l'eix X (CENTER),
// retona FALS altrament.

void JOY_IsCenter(void);
// Pre: --
// Post: Retorna CERT si el joystick esta en una
// posicio de l'eix Y (CENTER) i l'eix X (CENTER),
// retona FALS altrament.

void JOY_Motor(void);
// Pre: --
// Post: --

#endif