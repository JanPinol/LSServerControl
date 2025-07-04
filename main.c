#include <xc.h>

/*
 *  ---------------- RA ------------------
 * RA0 --> EIX_X
 * RA1 --> EIX_Y
 * RA2 --> TMP
 * RA3 --> RED
 * RA4 --> GREEN
 * RA5 --> BLUE
 * RA6 --> OSC
 * RA7 --> OSC

 * ---------------- RB ------------------
 * RB0 --> Button
 * RB1 --> SQW
 * RB2 --> OE_RAM
 * RB3 --> WE_RAM
 * RB4 --> CLK_RAM
 * RB5 --> RESET_RAM
 * RB6 --> 
 * RB7 --> 

 * ---------------- RC ------------------
 * RC0 --> PWM0
 * RC1 --> PWM1
 * RC2 --> CE
 * RC3 --> SCL
 * RC4 --> SDA
 * RC5 --> 
 * RC6 --> TX
 * RC7 --> RX

 * ---------------- RD ------------------
 * RD0 --> OUT_RAM0
 * RD1 --> OUT_RAM1
 * RD2 --> OUT_RAM2
 * RD3 --> OUT_RAM3
 * RD4 --> OUT_RAM4
 * RD5 --> OUT_RAM5
 * RD6 --> OUT_RAM6
 * RD7 --> OUT_RAM7

 * ---------------- RE ------------------
 * RE0 --> 
 * RE1 --> 
 * RE2 --> 
 * RE3 --> MCLR
 */

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG
#pragma config MCLRE = ON
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

#include "TAD_EEPROM.h"
#include "TAD_FAN.h"
#include "TAD_I2C.h"
#include "TAD_JOYSTICK.h"
#include "TAD_RAM.h"
#include "TAD_RGB.h"
#include "TAD_RTC.h"
#include "TAD_SIO.h"
#include "TAD_SISTEMA.h"
#include "TAD_TIMER.h"
#include "TAD_JOYSTICK.h"


void __interrupt(high_priority) RSI_High(void) {
    if (INTCONbits.TMR0IF) RSI_Timer0();
    if (INTCON3bits.INT1IF) RSI_INT1();
}


void INT_Init(void) {
    RCONbits.IPEN = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    TRISBbits.TRISB1 = 1;        // Pin RB1 com entrada (SQW del RTC)
    INTCON3bits.INT1IF = 0;      // Esborra flag
    INTCON3bits.INT1IE = 1;      // Activa interrupció INT1
    INTCON2bits.INTEDG1 = 0;     // Flanc de baixada
}
    
void main(void) {    

    INT_Init();
    TI_Init();
    SIO_Init();
    SIS_Init();
    JOY_Init();
    EEPROM_Init();
    FAN_Init();
    RGB_Init();
    RAM_Init();
    RTC_Init();
    
    TRISCbits.TRISC2 = 0;
    LATCbits.LATC2 = 0;
    while (1) {
        LATCbits.LATC2 = !LATCbits.LATC2;
        
        SIS_Motor();
        SIS_MovementMotor();
        SIS_ControlTemperatura();
        JOY_Motor();
        FAN_Motor();
        RGB_Motor();
        RAM_Motor();
        EEPROM_Motor();
        RTC_Motor();
    }
}
