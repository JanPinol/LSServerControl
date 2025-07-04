#include "TAD_FAN.h"
#include "TAD_TIMER.h"

static unsigned char dutyFan0 = 0;
static unsigned char dutyFan1 = 0;
static unsigned char timerFan;

static unsigned char counterFan0 = 0;
static unsigned char counterFan1 = 0;

void FAN_Init(void) {
    TRISCbits.TRISC0 = 0; // Fan0
    TRISCbits.TRISC1 = 0; // Fan1
    LATCbits.LATC0 = 0;
    LATCbits.LATC1 = 0;
    
    TI_NewTimer(&timerFan);
    TI_ResetTics(timerFan);
}

void FAN_Motor(void) {
    static char state = 0;

    switch(state) {
        case 0:
            if (TI_GetTics(timerFan) >= 1) { // cada 1ms
                TI_ResetTics(timerFan);

                // FAN0
                if (counterFan0 < dutyFan0) {
                    LATCbits.LATC0 = 1; // ON
                } else {
                    LATCbits.LATC0 = 0; // OFF
                }

                // FAN1
                if (counterFan1 < dutyFan1) {
                    LATCbits.LATC1 = 1; // ON
                } else {
                    LATCbits.LATC1 = 0; // OFF
                }

                // Avancem comptadors
                counterFan0++;
                counterFan1++;

                if (counterFan0 >= 100) counterFan0 = 0;
                if (counterFan1 >= 100) counterFan1 = 0;
            }
            break;
    }
}

void FAN_SetDutyFan0(unsigned char duty) {
    if (duty > 100) duty = 100;
    dutyFan0 = duty;
}

void FAN_SetDutyFan1(unsigned char duty) {
    if (duty > 100) duty = 100;
    dutyFan1 = duty;
}
