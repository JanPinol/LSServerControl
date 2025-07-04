#include "TAD_RTC.h"

static unsigned char pendingRTC = 0;

static unsigned char *rtc_hh;
static unsigned char *rtc_mm;
static unsigned char *rtc_ss;
static unsigned char *rtc_dd;
static unsigned char *rtc_mo;
static unsigned char *rtc_yy;

// Conversió BCD <-> Decimal
static unsigned char decToBcd(unsigned char val) {
    return (unsigned char)(((val / 10) << 4) | (val % 10));
}

static unsigned char bcdToDec(unsigned char val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

void RTC_Init(void) {
    InitI2C();
    RTC_Enable1HzInterrupt();
    pendingRTC = 0;
}

void RTC_Enable1HzInterrupt(void) {
    StartI2C(RTC_ADDRESS_WRITE);
    I2C_Write(0x0E); // Control register
    I2C_Write(0x00); // INTCN=0 (output square wave), RS1:RS0=00 (1Hz)
   
    I2C_Stop_();
    /*
    I2C_Write(0xD0);
    I2C_Write(0x0E); // Control register
    I2C_Write(0b01000100); // INTCN=1
    
    I2C_Write(0xD0);
    I2C_Write(0x08);
    I2C_Write(0xE0); // ALARMA 1MIN
    
    SIO_SendDecimal(2); SIO_SendString("\r\n");
    I2C_Stop_();
     */
}

void RTC_SetTime(unsigned char hh, unsigned char mm, unsigned char ss) {
    StartI2C(RTC_ADDRESS_WRITE);
    I2C_Write(0x00); // Segons
    I2C_Write(decToBcd(ss));
    I2C_Write(decToBcd(mm));
    I2C_Write(decToBcd(hh));
    I2C_Stop_();
}

void RTC_SetDate(unsigned char dd, unsigned char mo, unsigned char yy) {
    StartI2C(RTC_ADDRESS_WRITE);
    I2C_Write(0x04); // Dia, mes, any
    I2C_Write(decToBcd(dd));
    I2C_Write(decToBcd(mo));
    I2C_Write(decToBcd(yy));
    I2C_Stop_();
}

void RTC_DemanaTime(unsigned char *hh, unsigned char *mm, unsigned char *ss,
                    unsigned char *dd, unsigned char *mo, unsigned char *yy) {
    rtc_hh = hh;
    rtc_mm = mm;
    rtc_ss = ss;
    rtc_dd = dd;
    rtc_mo = mo;
    rtc_yy = yy;
    pendingRTC = 1;
}

void RTC_Motor(void) {
    static char state = 0;

    if (!pendingRTC) return;
    
    switch (state) {
        case 0:
            StartI2C(RTC_ADDRESS_WRITE);
            I2C_Write(0x00);
            ReStartI2C_(RTC_ADDRESS_READ);
            state++;
            break;

        case 1:
            *rtc_ss = bcdToDec(I2C_Read(0));
            *rtc_mm = bcdToDec(I2C_Read(0));
            *rtc_hh = bcdToDec(I2C_Read(0));
            I2C_Read(0);
            *rtc_dd = bcdToDec(I2C_Read(0));
            *rtc_mo = bcdToDec(I2C_Read(0));
            *rtc_yy = bcdToDec(I2C_Read(1));
            I2C_Stop_();
            pendingRTC = 0;
            state = 0;
            break;
    }
}


