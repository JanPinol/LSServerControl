#include "TAD_JOYSTICK.h"

// Variables
static unsigned char joystick[2];
static unsigned char temperatura;
static char indexJoystick;
static char enable;
static char canalActual;

void JOY_Init(void) {
    TRISAbits.TRISA0 = 1; // Eix X
    TRISAbits.TRISA1 = 1; // Eix Y
    TRISAbits.TRISA2 = 1; // TMP36

    ADCON0bits.CHS = 0;
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 0;
    ADCON2bits.ADCS2 = 0;
    ADCON2bits.ADCS1 = 1;
    ADCON2bits.ADCS0 = 1;

    indexJoystick = enable = 1;
    canalActual = 0; // Comencem per canal 0
}

void JOY_Motor(void) {
    static char state = 0;
    static char canal = 0; // 0 = X, 1 = Y

    switch (state) {
        case 0:
            // Configurem canal X o Y
            if (canal == 0) {
                ADCON0bits.CHS = 0; // RA0
            } else {
                ADCON0bits.CHS = 1; // RA1
            }
            ADCON0bits.GO = 1;
            state = 1;
            break;

        case 1:
            if (ADCON0bits.GO == 0) {
                joystick[canal] = ADRESH;
                canal ^= 1; // Alterna 0/1
                state = 0;
            }
            break;
    }
}



void JOY_IsCenter(void) {
    if ((THRESHOLD_LOW < joystick[0]) && (joystick[0] < THRESHOLD_HIGH)) {
        if ((THRESHOLD_LOW < joystick[1]) && (joystick[1] < THRESHOLD_HIGH)) {
            enable = 1;
        }
    }
}

char JOY_IsUp(void) {
    if ((THRESHOLD_LOW < joystick[0]) && (joystick[0] < THRESHOLD_HIGH)) {
        if ((joystick[1] < THRESHOLD_LOW) && enable) {
            enable = 0;
            return CERT;
        }
    }
    return FALS;
}

char JOY_IsDown(void) {
    if ((THRESHOLD_LOW < joystick[0]) && (joystick[0] < THRESHOLD_HIGH)) {
        if ((joystick[1] > THRESHOLD_HIGH) && enable) {
            enable = 0;
            return CERT;
        }
    }
    return FALS;
}

char JOY_IsLeft(void) {
    if ((joystick[1] > THRESHOLD_LOW) && (joystick[1] < THRESHOLD_HIGH)) {
        if ((joystick[0] < THRESHOLD_LOW) && enable) {
            enable = 0;
            return CERT;
        }
    }
    return FALS;
}

char JOY_IsRight(void) {
    if ((joystick[1] > THRESHOLD_LOW) && (joystick[1] < THRESHOLD_HIGH)) {
        if ((joystick[0] > THRESHOLD_HIGH) && enable) {
            enable = 0;
            return CERT;
        }
    }
    return FALS;
}