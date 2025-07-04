#ifndef _TAD_SISTEMA_H_
#define _TAD_SISTEMA_H_

#include <xc.h>

#include "TAD_JOYSTICK.h"
#include "TAD_SIO.h"
#include "TAD_TIMER.h"
#include "TAD_EEPROM.h"
#include "TAD_RGB.h"
#include "TAD_FAN.h"
#include "TAD_RAM.h"
#include "TAD_RTC.h"

#define NUM_OPTIONS_MENU 4
#define T_REBOTS 20
#define CMD_INITIALIZE "INITIALIZE:"
#define CMD_SET_TIME   "SET_TIME:"
#define CMD_GET_LOGS   "GET_LOGS"
#define CMD_GET_GRAPH  "GET_GRAPH"
#define CMD_RESET      "RESET"
#define MAX_BUFFER 64
#define T_1s 1000

void RSI_INT1(void);
void SIS_Init(void);
void SIS_Motor(void);
void SIS_MovementMotor(void);
void SIS_ControlTemperatura(void);

#endif