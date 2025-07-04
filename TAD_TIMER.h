#ifndef _T_TIMER_H_
#define _T_TIMER_H_

#include <xc.h>

// CONSTANTS
#define TI_FALS 0
#define TI_CERT 1


//La RSI
void RSI_Timer0(void);
    // IMPORTANT! Funció que ha der ser cridadda des de la RSI, en en cas que TMR0IF==1.
    // La RSI ha de contenir: if (TMR0IF==1) RSI_Timer0();

void TI_Init (void);
	// Post: Constructor. És precondició global haver cridat aquesta funció quan es cridi qualsevol altra funció del TAD

unsigned char TI_NewTimer(unsigned char *TimerHandle) ;
	// Post: Retorna TI_CERT en cas que s'hagi creat adequadament un nou timer, i TI_FALS en cas contrati.
    // Posa a *TimerHandle l'identificador de timer assignat, necessari per usar les funcions TI_GetTics i TI_ResetTics.

void TI_ResetTics (unsigned char TimerHandle);
	// Pre: Handle ha estat retornat per Ti_NewTimer.
	// Post: Engega la temporització associada a 'TimerHandle', guardant la referencia temporal en el moment de la crida.

unsigned long TI_GetTics (unsigned char TimerHandle);
	// Pre: Handle ha estat retornat per TI_NewGetTimer.
	// Post: Retorna els tics transcorreguts des de la crida a TI_ResetTics per al mateix TimerHandle.

#endif
