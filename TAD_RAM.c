#include "TAD_RAM.h"

static unsigned int posAddrRAM = 0;
static char ramReadState = 0;
static unsigned int ramReadIndex = 0;
static unsigned int ramAddrBackup = 0;
static unsigned char pendingReadRequest = 0;


void RAM_ResetAddr(void);
void RAM_IncAddr(void);
void RAM_Write(unsigned char v);
unsigned char RAM_Read(void);

void delay_us(unsigned char us) {
    while(--us) {
        __asm("NOP"); __asm("NOP"); __asm("NOP");
        __asm("NOP"); __asm("NOP"); __asm("NOP");
    }
}

void RAM_Init(void) {
    TRISD = 0x00; // Bus de dades com a sortida
    LATD = 0x00;

    TRISBbits.TRISB2 = 0; // OE
    TRISBbits.TRISB3 = 0; // WE
    TRISBbits.TRISB4 = 0; // CLK
    TRISBbits.TRISB5 = 0; // RESET
    TRISCbits.TRISC2 = 0; // CE

    RAM_CLK = 0;    // CLK a 0
    RAM_OE = 1;     // OE desactivat
    RAM_WE = 1;     // WE desactivat
    RAM_CE = 1;     // CE desactivat
    RAM_RESET = 1;  // RESET desactivat
    
    
    posAddrRAM = 0;
    RAM_ResetAddr();

    RAM_DemanaEsborrar();
}

void RAM_ResetAddr(void) {
    RAM_CLK = 0;
    RAM_RESET = 0;  // LOAD actiu
    delay_us(1);

    RAM_CLK = 1;    // flanc de pujada
    delay_us(1);

    RAM_CLK = 0;
    delay_us(1);

    RAM_RESET = 1;  // LOAD desactivat després del rellotge
    delay_us(1);    
    
    posAddrRAM = 0;
}


void RAM_IncAddr(void) {
    RAM_CLK = 0;    // CLK desactivat
    
    delay_us(3);
    RAM_CLK = 1;    // CLK activat
    delay_us(3);
    
    RAM_CLK = 0;    // CLK desactivat
    
    posAddrRAM++;  
    if(posAddrRAM >= RAM_MAX_ADDR) RAM_ResetAddr();
} 

void RAM_Write(unsigned char v) {
    TRISD = 0x00;            // PORTD com a sortida
    delay_us(1);
    LATD = v;                // posa dades al bus

    RAM_OE = 1;     // OE desactivat
    RAM_CE = 0;     // CE activat
    RAM_WE = 0;     // WE activat
    delay_us(1);
    RAM_WE = 1;     // Final de l'escriptura
    RAM_CE = 1;     // Desactiva el xip
    
    TRISD = 0xFF;    // Posar com a entrada
    LATD = 0x00;
}

unsigned char RAM_Read(void) {
    TRISD = 0xFF;    // bus dades com a entrada
    LATD = 0x00;     // neteja el latch intern

    RAM_WE = 1;      // WE desactivat
    RAM_CE = 0;      // CE activat
    RAM_OE = 0;      // OE activat

    delay_us(5);
    unsigned char r = PORTD;

    RAM_OE = 1;      // OE desactivat
    RAM_CE = 1;      // CE desactivat

    return r;
}


void RAM_DemanaDesar(unsigned char temperatura) {
    RAM_Write(temperatura);
    RAM_IncAddr();
}

void RAM_DemanaEsborrar(void) {
    RAM_ResetAddr();
    
    unsigned int i;
    for(i = 0; i < RAM_MAX_ADDR; i++) {
        RAM_Write(0);
        RAM_IncAddr();
    }
    
    RAM_ResetAddr();
}

void RAM_DemanaLlegir(void) {
    if (pendingReadRequest == 0) {
        pendingReadRequest = 1;  // sol·licitud de lectura
    }
}

void RAM_Motor(void) {
    if (!pendingReadRequest) return;

    switch (ramReadState) {
        case 0:
            ramAddrBackup = posAddrRAM;
            RAM_ResetAddr();
            ramReadIndex = 0;
            ramReadState = 1;
            break;

        case 1:
            if (ramReadIndex < RAM_MAX_ADDR) {
                unsigned char v = RAM_Read();
                SIO_SendString("DATAGRAPH:");
                SIO_SendDecimal(v);
                SIO_SendString("\r\n");
                RAM_IncAddr();
                ramReadIndex++;
            } else {
                ramReadState = 2;
            }
            break;

        case 2:
            for (unsigned int i = 0; i < ramAddrBackup; i++) RAM_IncAddr();
            SIO_SendString("FINISH\r\n");
            ramReadState = 0;
            pendingReadRequest = 0;
            break;
    }
}

