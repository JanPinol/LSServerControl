#ifndef _TAD_RAM_H_
#define _TAD_RAM_H_

#include <xc.h>
#include "TAD_SIO.h"

#define RAM_MAX_ADDR     25

#define RAM_OE LATBbits.LATB2       // OE
#define RAM_WE LATBbits.LATB3       // WE
#define RAM_CLK LATBbits.LATB4      // CLK
#define RAM_RESET LATBbits.LATB5    // RESET
#define RAM_CE LATCbits.LATC2       // CE


// Inicialització del sistema de RAM
void RAM_Init(void);

// Demana desar una nova temperatura a la RAM
void RAM_DemanaDesar(unsigned char temperatura);

// Demana llegir totes les dades de la RAM i enviar-les per SIO
void RAM_DemanaLlegir(void);

// Demana esborrar totes les dades de la RAM
void RAM_DemanaEsborrar(void);

void RAM_Motor(void);

#endif // TAD_RAM_H
