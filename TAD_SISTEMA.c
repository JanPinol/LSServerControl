#include "TAD_SISTEMA.h"

enum TipusComanda {
    CMD_ID_INVALID = 0,
    CMD_ID_INITIALIZE,
    CMD_ID_SET_TIME,
    CMD_ID_GET_LOGS,
    CMD_ID_GET_GRAPH,
    CMD_ID_RESET
};

static unsigned char systemInitialized = 0;

static unsigned char previousButton = 1;
static unsigned char timerRebots;
static char rxBuffer[MAX_BUFFER];
static unsigned char received;
static char index = 0;
static unsigned char any, mes, dia;
static unsigned char hora, minuts, segons;
static unsigned char tempsMostreig;
static unsigned char llindarLow;
static unsigned char llindarModerate;
static unsigned char llindarHigh;
static unsigned char llindarCritical;
static unsigned char flag_rtc = 0;
static unsigned char timerTemperatura;


char iniciaAmb(const char *entrada, const char *prefix);
char obtenirNumero(char *buf, char *idx);
void processaComanda(char *comanda);
void parsejaInitialize(char *comanda);
void parsejaSetTime(char *comanda);
char identificaComanda(char *comanda);
void SIS_ControlTemperatura(void);
void resetCommand(void);


void SIS_Init(void) {
    TI_NewTimer(&timerRebots);
    TI_NewTimer(&timerTemperatura);
    TI_ResetTics(timerRebots);
    TI_ResetTics(timerTemperatura);

    TRISBbits.TRISB0 = 1;
    INTCON2bits.RBPU = 0;
    previousButton = 1;
    systemInitialized = 0;
    flag_rtc = 0;

    ADCON1 = 0x0B;  // AN0-AN2 analògics
}

void RSI_INT1(void) {
    INTCON3bits.INT1IF = 0;
    
    SIO_SendDecimal(minuts); SIO_SendString("\r\n");
    
    if (!systemInitialized) return;
    
    RTC_DemanaTime(&hora, &minuts, &segons, &dia, &mes, &any);
    
    if(!segons) {
        SIO_SendString("UPDATETIME:");
        SIO_SendDecimal(hora);   
        SIO_SendBlocking(':');
        SIO_SendDecimal(minuts);
        SIO_SendString("\r\n");
    }
}

void SIS_Motor(void) {
    static char state = 0;
    static char i = 0;

    switch (state) {
        case 0:
            if (SIO_Receive(&received)) {
                if (i < MAX_BUFFER - 1) {
                    rxBuffer[i++] = received;

                    if (received == '\n') {
                        rxBuffer[i] = '\0';
                        state = 1;
                    }
                } else {
                    i = 0;
                }
            }
            break;

        case 1:
            processaComanda(rxBuffer);
            i = 0;
            state--;
            break;
    }
}

void SIS_MovementMotor(void) {
    static unsigned char state = 0;

    switch (state) {
        case 0:
            if (TI_GetTics(timerRebots) >= T_REBOTS) {
                TI_ResetTics(timerRebots);
                state++;
            }
            break;

        case 1:
            JOY_IsCenter();
            if (JOY_IsUp())    SIO_SendBuffer("UP\r\n", 4);
            if (JOY_IsDown())  SIO_SendBuffer("DOWN\r\n", 6);
            if (JOY_IsLeft())  SIO_SendBuffer("LEFT\r\n", 6);
            if (JOY_IsRight()) SIO_SendBuffer("RIGHT\r\n", 7);
            if (PORTBbits.RB0 == 0 && 
                previousButton == 1) SIO_SendBuffer("SELECT\r\n", 8);
            previousButton = PORTBbits.RB0;
            state--;
            break;
    }
}

void SIS_ControlTemperatura(void) {
    static char state = 0;
    static char temperatura = 0;
    static unsigned char estat_anterior = 0;


    if (!systemInitialized) return;

    switch (state) {
        case 0:
            TI_ResetTics(timerTemperatura);
            state = 1;
            break;

        case 1:
            if (TI_GetTics(timerTemperatura) >= T_1s * tempsMostreig) {
                ADCON0bits.CHS = 2;     // Sensor temperatura RA2
                ADCON0bits.GO = 1;      // Inicia conversió
                state = 2;
            }
            break;

        case 2:
            if (!ADCON0bits.GO) {
                temperatura = ADRESH - 50;
                //SIO_SendDecimal(temperatura); SIO_SendString("\r\n");
                
                if (temperatura > 50) {
                    ADCON0bits.CHS = 0;  // Torna a RA0
                    state = 0;
                    break;
                }
                
                RAM_DemanaDesar(temperatura);

                if (temperatura < llindarLow) {
                    FAN_SetDutyFan0(50);
                    FAN_SetDutyFan1(0);
                    RGB_SetColor(0, 1, 0); // Verd
                    estat_anterior = 0;
                } else if (temperatura < llindarModerate) {
                    FAN_SetDutyFan0(50);
                    FAN_SetDutyFan1(50);
                    RGB_SetColor(0, 0, 1); // Blau
                    estat_anterior = 0;
                } else if (temperatura < llindarHigh) {
                    FAN_SetDutyFan0(100);
                    FAN_SetDutyFan1(100);
                    RGB_SetColor(1, 0, 0); // Vermell
                    estat_anterior = 0;
                } else {
                    FAN_SetDutyFan0(0);
                    FAN_SetDutyFan1(0);
                    RGB_SetColorAlternatCritic();
                    if (estat_anterior == 0) {
                        EEPROM_DemanaDesarLog(hora, minuts, segons, dia, mes, any);
                        estat_anterior = 1;
                    }
                }

                ADCON0bits.CHS = 0;  // Torna a RA0
                state = 0;
            }
            break;
    }
}


void processaComanda(char *comanda) {
    switch (identificaComanda(comanda)) {
        case CMD_ID_INITIALIZE:
            parsejaInitialize(comanda);
            systemInitialized = 1;
            break;

        case CMD_ID_SET_TIME:
            parsejaSetTime(comanda);
            break;

        case CMD_ID_GET_LOGS:
            EEPROM_DemanaLlegirLogs();
            break;

        case CMD_ID_GET_GRAPH:
            RAM_DemanaLlegir();
            break;

        case CMD_ID_RESET:
            resetCommand();
            break;
    }
}

void resetCommand(void) {
    systemInitialized = 0;
    
    FAN_SetDutyFan0(0);
    FAN_SetDutyFan1(0);
    RGB_SetColor(0, 0, 0);
    EEPROM_DemanaEsborrarLogs();
    RAM_DemanaEsborrar();
    
    // Reset de valors globals
    any = 0;
    mes = 0;
    dia = 0;
    hora = 0;
    minuts = 0;
    segons = 0;
    tempsMostreig = 0;
    llindarLow = 0;
    llindarModerate = 0;
    llindarHigh = 0;
    llindarCritical = 0;
}


char iniciaAmb(const char *entrada, const char *prefix) {
    int i = 0;
    while (prefix[i] != '\0') {
        if (entrada[i] != prefix[i]) return 0;
        i++;
    }
    return 1;
}

char obtenirNumero(char *buf, char *idx) {
    char val = 0;

    // Salta '$' inicial si hi és
    if (buf[*idx] == '$') (*idx)++;

    // Llegeix número decimal fins al pròxim separador
    while (buf[*idx] >= '0' && buf[*idx] <= '9') {
        val = val * 10 + (buf[*idx] - '0');
        (*idx)++;
    }

    // Salta el següent '$' si hi és
    if (buf[*idx] == '$') (*idx)++;

    return val;
}

void parsejaInitialize(char *comanda) {
    //INITIALIZE:2025-04-12 22:00$polling$Low$Moderate$High$Critical
    char i = 13; // Saltem "INITIALIZE:"

    any = (comanda[i] - '0') * 10 + (comanda[i+1] - '0');       i += 3; // any + '-'
    mes = (comanda[i] - '0') * 10 + (comanda[i+1] - '0');       i += 3; // mes + '-'
    dia = (comanda[i] - '0') * 10 + (comanda[i+1] - '0');       i += 3; // dia + ' '
    hora = (comanda[i] - '0') * 10 + (comanda[i+1] - '0');      i += 3; // hora + ':'
    minuts = (comanda[i] - '0') * 10 + (comanda[i+1] - '0');    i += 2; // minuts
    
    // Ara els llindars
    tempsMostreig = obtenirNumero(comanda, &i);
    llindarLow = obtenirNumero(comanda, &i);
    llindarModerate = obtenirNumero(comanda, &i);
    llindarHigh = obtenirNumero(comanda, &i);
    llindarCritical = obtenirNumero(comanda, &i);
    
    RTC_SetDate(dia, mes, any);
    RTC_SetTime(hora, minuts, segons);
}

void parsejaSetTime(char *comanda) {
    char i = 9; // "SET_TIME:" és de 9 caràcters

    hora = (comanda[i] - '0') * 10 + (comanda[i + 1] - '0');    i += 3;
    minuts = (comanda[i] - '0') * 10 + (comanda[i + 1] - '0');  i += 2;
    
    RTC_SetTime(hora, minuts, segons);
}

char identificaComanda(char *comanda) {
    if (iniciaAmb(comanda, CMD_INITIALIZE)) return CMD_ID_INITIALIZE;
    if (iniciaAmb(comanda, CMD_SET_TIME))   return CMD_ID_SET_TIME;
    if (iniciaAmb(comanda, CMD_GET_LOGS))   return CMD_ID_GET_LOGS;
    if (iniciaAmb(comanda, CMD_GET_GRAPH))  return CMD_ID_GET_GRAPH;
    if (iniciaAmb(comanda, CMD_RESET))      return CMD_ID_RESET;
    return CMD_ID_INVALID;
}
