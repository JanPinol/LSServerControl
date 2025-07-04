#ifndef _TAD_EEPROM_H_
#define _TAD_EEPROM_H_

#include "TAD_SIO.h"

// Constants de configuració
#define EEPROM_LOG_SIZE         14
#define EEPROM_MAX_LOGS         15
#define EEPROM_START_ADDR       0x00
#define EEPROM_TOTAL_BYTES      (EEPROM_LOG_SIZE * EEPROM_MAX_LOGS)
#define EEPROM_ADDR_TOTAL_LOGS  0xFF

void EEPROM_Init(void);
void EEPROM_DemanaDesarLog(char hh, char mm, char ss, char dd, char mo, char yyyy);
void EEPROM_DemanaLlegirLogs(void);
void EEPROM_DemanaEsborrarLogs(void);
void EEPROM_Motor(void);

#endif
