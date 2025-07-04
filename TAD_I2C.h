#ifndef _TAD_I2C_H
#define _TAD_I2C_H

/* pin out
SDA: Serial Data  PIN C4
SCL: Serial Clock PIN C3
SQW: Active low interrupt (if used connect to the desired pin)
32KHZ: not connected
GND
VCC: min=2.3V | typ = 3.3V | max = 5.5V
*/

#define BAUDRATE 99 // Modify it to make I2C work at 100KHz

//-------------- Private functions: --------------
void I2C_Ready();
void I2C_Ack();
void I2C_Nack();

//-------------- Public functions: --------------
void InitI2C();
char StartI2C( char  );
char ReStartI2C_( char );
char I2C_Stop_( void );
char I2C_Write(unsigned char data);
char I2C_Read(char flag);

#endif
