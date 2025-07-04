#include "TAD_EEPROM.h"

static unsigned char totalLogs = 0;
static char log[EEPROM_LOG_SIZE + 1];
static unsigned char iLog = 0, jLog = 0;
unsigned char pendingReadRequest = 0;

static char logPend[EEPROM_LOG_SIZE];  // buffer temporal
static unsigned char idxLog = 0, idxDesplac = 0, idxChar = 0;
static unsigned char desantLog = 0;
static unsigned char estatDesar = 0;


void desarLogASCII(const char *log);
void writeByte(unsigned char addr, unsigned char data);
unsigned char readByte(unsigned char addr);

void EEPROM_Init(void) {
    totalLogs = readByte(EEPROM_ADDR_TOTAL_LOGS);
    if (totalLogs > EEPROM_MAX_LOGS) totalLogs = 0;
}

void EEPROM_DemanaDesarLog(char hh, char mm, char ss, char dd, char mo, char yyyy) {
    if (desantLog) return;  // Ja n'hi ha un en procés

    logPend[0]  = (hh / 10) + '0'; logPend[1]  = (hh % 10) + '0';
    logPend[2]  = (mm / 10) + '0'; logPend[3]  = (mm % 10) + '0';
    logPend[4]  = (ss / 10) + '0'; logPend[5]  = (ss % 10) + '0';
    logPend[6]  = (dd / 10) + '0'; logPend[7]  = (dd % 10) + '0';
    logPend[8]  = (mo / 10) + '0'; logPend[9]  = (mo % 10) + '0';
    logPend[10] = '2';
    logPend[11] = '0';
    logPend[12] = (yyyy / 10) % 10 + '0';
    logPend[13] = yyyy % 10 + '0';

    desantLog = 1;
    estatDesar = 0;
}

void EEPROM_DemanaEsborrarLogs(void) {
    for (unsigned char addr = 0; addr < EEPROM_TOTAL_BYTES; addr++) {
        writeByte(addr, 0xFF);
    }
    totalLogs = 0;
    writeByte(EEPROM_ADDR_TOTAL_LOGS, 0);
}

// Escriu 1 byte a la EEPROM interna
void writeByte(unsigned char addr, unsigned char data) {
    EEADR = addr;
    EEDATA = data;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1;
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    INTCONbits.GIE = 1;
    while (EECON1bits.WR);
    EECON1bits.WREN = 0;
}

// Llegeix 1 byte de la EEPROM interna
unsigned char readByte(unsigned char addr) {
    EEADR = addr;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;
    return EEDATA;
}

void EEPROM_DemanaLlegirLogs(void) {
    if (pendingReadRequest == 0) {
        pendingReadRequest = 1;  // sol·licitud de lectura
    }
}

void EEPROM_Motor(void) {
    static unsigned char estatGlobal = 0;     // 0 = IDLE, 1 = LLEGINT, 2 = DESANT
    static unsigned char eepromSubState = 0;
    static unsigned char baseAddr = 0;
    static unsigned char sendIndex = 0;

    // Prioritat: lectura de logs
    if (estatGlobal == 0) {
        if (pendingReadRequest) {
            estatGlobal = 1;
            eepromSubState = 0;
        } else if (desantLog) {
            estatGlobal = 2;
            eepromSubState = 0;
        } else {
            return;
        }
    }

    // ---- Mòdul de lectura ----
    if (estatGlobal == 1) {
        switch (eepromSubState) {
            case 0:
                if (totalLogs > 0) {
                    iLog = 0;
                    jLog = 0;
                    baseAddr = EEPROM_START_ADDR;
                    log[EEPROM_LOG_SIZE] = '\0';
                    eepromSubState = 1;
                } else {
                    pendingReadRequest = 0;
                    estatGlobal = 0;
                }
                break;

            case 1:
                if (jLog < EEPROM_LOG_SIZE) {
                    log[jLog] = readByte(baseAddr + (iLog * EEPROM_LOG_SIZE) + jLog);
                    jLog++;
                } else {
                    jLog = 0;
                    sendIndex = 0;
                    eepromSubState = 2;
                }
                break;

            case 2:
                SIO_SendString("DATALOGS:");
                eepromSubState = 3;
                break;

            case 3:
                switch (sendIndex++) {
                    case 0: SIO_SendBlocking(log[6]); break;
                    case 1: SIO_SendBlocking(log[7]); break;
                    case 2: SIO_SendBlocking('-'); break;
                    case 3: SIO_SendBlocking(log[8]); break;
                    case 4: SIO_SendBlocking(log[9]); break;
                    case 5: SIO_SendBlocking('-'); break;
                    case 6: SIO_SendBlocking(log[10]); break;
                    case 7: SIO_SendBlocking(log[11]); break;
                    case 8: SIO_SendBlocking(log[12]); break;
                    case 9: SIO_SendBlocking(log[13]); sendIndex = 0; eepromSubState = 4; break;
                }
                break;

            case 4:
                SIO_SendString(" a les ");
                eepromSubState = 5;
                break;

            case 5:
                switch (sendIndex++) {
                    case 0: SIO_SendBlocking(log[0]); break;
                    case 1: SIO_SendBlocking(log[1]); break;
                    case 2: SIO_SendBlocking(':'); break;
                    case 3: SIO_SendBlocking(log[2]); break;
                    case 4: SIO_SendBlocking(log[3]); break;
                    case 5: SIO_SendBlocking(':'); break;
                    case 6: SIO_SendBlocking(log[4]); break;
                    case 7: SIO_SendBlocking(log[5]); sendIndex = 0; eepromSubState = 6; break;
                }
                break;

            case 6:
                SIO_SendString(" - Estat critic detectat.\r\n");
                eepromSubState = 7;
                break;

            case 7:
                iLog++;
                if (iLog >= totalLogs) {
                    eepromSubState = 8;
                } else {
                    eepromSubState = 1;
                }
                break;

            case 8:
                SIO_SendString("FINISH\r\n");
                pendingReadRequest = 0;
                estatGlobal = 0;
                break;
        }
    }

    // ---- Mòdul d'escriptura ----
    else if (estatGlobal == 2) {
        switch (eepromSubState) {
            case 0:
                if (totalLogs >= EEPROM_MAX_LOGS) {
                    idxDesplac = 0;
                    idxChar = 0;
                    eepromSubState = 1;
                } else {
                    eepromSubState = 3;
                }
                break;

            case 1:
                if (idxChar < EEPROM_LOG_SIZE) {
                    unsigned char from = EEPROM_START_ADDR + ((idxDesplac + 1) * EEPROM_LOG_SIZE) + idxChar;
                    unsigned char to   = EEPROM_START_ADDR + (idxDesplac * EEPROM_LOG_SIZE) + idxChar;
                    writeByte(to, readByte(from));
                    idxChar++;
                } else {
                    idxChar = 0;
                    idxDesplac++;
                    if (idxDesplac >= EEPROM_MAX_LOGS - 1) {
                        totalLogs = EEPROM_MAX_LOGS - 1;
                        eepromSubState = 3;
                    }
                }
                break;

            case 3:
                if (idxLog < EEPROM_LOG_SIZE) {
                    unsigned char addr = EEPROM_START_ADDR + (totalLogs * EEPROM_LOG_SIZE) + idxLog;
                    writeByte(addr, logPend[idxLog]);
                    idxLog++;
                } else {
                    eepromSubState = 4;
                }
                break;

            case 4:
                totalLogs++;
                if (totalLogs > EEPROM_MAX_LOGS) totalLogs = EEPROM_MAX_LOGS;
                writeByte(EEPROM_ADDR_TOTAL_LOGS, totalLogs);
                idxLog = 0;
                desantLog = 0;
                estatGlobal = 0;
                break;
        }
    }
}



