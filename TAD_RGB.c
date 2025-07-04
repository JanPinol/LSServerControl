#include "TAD_RGB.h"

static unsigned char red, green, blue;
static char estatCritic = 0;
static unsigned char timerRGB;

void RGB_Init(void) {
    TRISAbits.TRISA3 = 0; // RED
    TRISAbits.TRISA4 = 0; // GREEN
    TRISAbits.TRISA5 = 0; // BLUE

    LATAbits.LATA3 = 0;
    LATAbits.LATA4 = 0;
    LATAbits.LATA5 = 0;

    TI_NewTimer(&timerRGB);
    TI_ResetTics(timerRGB);
}

void RGB_SetColor(unsigned char r, unsigned char g, unsigned char b) {
    red = r;
    green = g;
    blue = b;
    estatCritic = 0; // Normal, no alterna
}

void RGB_SetColorAlternatCritic(void) {
    estatCritic = 1; // Activem l'alternança
    TI_ResetTics(timerRGB);
}

void RGB_Motor(void) {
    static char state = 0;

    if (estatCritic == 0) {
        // Color normal
        LATAbits.LATA3 = red;
        LATAbits.LATA4 = green;
        LATAbits.LATA5 = blue;
    } else {
        // Estat Crític: alternar colors
        LATAbits.LATA3 = 1;
        LATAbits.LATA4 = 0;
        switch (state) {
            case 0:
                LATAbits.LATA5 = 0;     // BLUE
                if (TI_GetTics(timerRGB) > T_500ms) { // 500 ms
                    TI_ResetTics(timerRGB);
                    state++;
                }
                break;

            case 1:
                LATAbits.LATA5 = 1;     // BLUE
                if (TI_GetTics(timerRGB) > T_500ms) { // 500 ms
                    TI_ResetTics(timerRGB);
                    state--;
                }
                break;
        }
    }
}
