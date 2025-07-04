#ifndef _TAD_SIO_H_
#define _TAD_SIO_H_

#include <xc.h>

#define CERT 1
#define FALS 0

// SIO configurada per enviar a 9615 de BAUDRATE

void SIO_Init(void);
// Pre: PORT6 i PORT7 disponibles
// Post: Sio inicialitzada

unsigned char SIO_Send(unsigned char character);
// Pre: character ha de ser un caracter valid en ASCII.
// Post: retorna 1 si s'ha enviat el byte, 0 altrament

unsigned char SIO_Receive(unsigned char* characters);
// Pre: --
// Post: retorna el byte rebut, 0 altrament

void SIO_SendString(const char *str);
// Pre: --
// Post: envia el string per la SIO

void SIO_SendBuffer(const char *buffer, char length);

void SIO_SendBlocking(unsigned char character);

void SIO_SendDecimal(unsigned char num);

#endif