#include "TAD_SIO.h"

void SIO_Init(void) {
    TRISCbits.TRISC6 = 1;  // TX com entrada (configura com digital si cal)
    TRISCbits.TRISC7 = 1;  // RX com entrada

    SPBRG = 81;            // Valor per 115200 bps amb Fosc = 40 MHz
    SPBRGH = 0;

    BAUDCONbits.BRG16 = 1; // 16-bit Baud Rate Generator
    TXSTAbits.BRGH = 1;    // Alta velocitat

    TXSTAbits.SYNC = 0;    // Mode asíncron
    RCSTAbits.SPEN = 1;    // Activa mòdul sèrie

    TXSTAbits.TXEN = 1;    // Activa transmissió
    RCSTAbits.CREN = 1;    // Activa recepció contínua
}


unsigned char SIO_Send (unsigned char character) {
    if (TXSTAbits.TRMT) {
        TXREG = character;
        return CERT;
    }
    return FALS;
}

unsigned char SIO_Receive(unsigned char* character) {
    if (PIR1bits.RCIF) {
        *character = RCREG;
        return CERT;
    }
    return FALS;
}


void SIO_SendString(const char *str) {
    while(*str) {
        if (SIO_Send(*str)) {
            str++;
        }
    }
}

void SIO_SendBuffer(const char *buffer, char length) {
    for (char i = 0; i < length; i++) {
        while (!TXSTAbits.TRMT);
        TXREG = buffer[i];
    }
}

void SIO_SendBlocking(unsigned char character) {
    while (!TXSTAbits.TRMT); // Espera que el registre estigui buit
    TXREG = character;       // Envia el caràcter
}

void SIO_SendDecimal(unsigned char num) {
    if (num >= 100) {
        SIO_SendBlocking((num / 100) + '0');
        num %= 100;
    }
    SIO_SendBlocking((num / 10) + '0');
    SIO_SendBlocking((num % 10) + '0');
}




