#include <xc.h>
#include "TAD_I2C.h"

//-------------- Private functions: --------------

void I2C_Ready()
{
   while(PIR2bits.BCLIF);    /* Wait if bit collision interrupt flag is set*/
   while(SSPSTATbits.BF || (SSPSTATbits.R_nW));
   PIR1bits.SSPIF=0;          /* Clear SSPIF interrupt flag*/
}

void I2C_Ack()
{
    SSPCON2bits.ACKDT=0;        /* Acknowledge data 1:NACK,0:ACK */
    SSPCON2bits.ACKEN=1;        /* Enable ACK/NACK to send */
    while(SSPCON2bits.ACKEN);
 }

void I2C_Nack()
{
    SSPCON2bits.ACKDT=1;        /* Acknowledge data 1:NACK,0:ACK */
    SSPCON2bits.ACKEN=1;        /* Enable ACK/NACK to send */
    while(SSPCON2bits.ACKEN);
}

//-------------- Public functions: --------------

void InitI2C()
{
  TRISCbits.TRISC3 = 1;
  TRISCbits.TRISC4 = 1;
  SSPSTAT =0x80;
  SSPCON1 = 0x28;
  SSPCON2 = 0x0;
  SSPADD = BAUDRATE;
  PIE1bits.SSPIE=0;
  PIR1bits.SSPIF=0;
}


char StartI2C( char slave_rw_addr )
{
  SSPCON2bits.SEN = 1;
  while(SSPCON2bits.SEN);
  PIR1bits.SSPIF=0;
  if(!(SSPSTATbits.S)){return 0;}
  return (I2C_Write(slave_rw_addr));
}

char ReStartI2C_( char slave_rw_addr )
{
  SSPCON2bits.RSEN = 1;
  while(SSPCON2bits.RSEN);
  PIR1bits.SSPIF=0;
  if(!(SSPSTATbits.S)){return 0;}
  return (I2C_Write(slave_rw_addr));
}
 
 char I2C_Write(unsigned char data)
{
      SSPBUF=data;	/* Write data to SSPBUF*/
      I2C_Ready();
      if (SSPCON2bits.ACKSTAT)	/* Check for acknowledge bit from slave*/
        return 1;
      else
        return 2;
}
 
 char I2C_Stop_()
{
    I2C_Ready();
    SSPCON2bits.PEN=1;		/* Stop communication*/
    while(SSPCON2bits.PEN);		/* Wait for end of stop pulse*/
    PIR1bits.SSPIF = 0;
    if (!SSPSTATbits.P)
       return 0;
    return 1;
}

char I2C_Read(char flag)
{
    char buffer=0;
    SSPCON2bits.RCEN=1;			/* Enable receive */

	/* Wait for buffer full flag which when complete byte received */
    while(!SSPSTATbits.BF);
    buffer=SSPBUF;		/* Copy SSPBUF to buffer */

	/* Send acknowledgment or negative acknowledgment after read to 
	continue or stop reading */
    if(flag==0)
        I2C_Ack();
    else
        I2C_Nack();
        I2C_Ready();
    return(buffer);
}
