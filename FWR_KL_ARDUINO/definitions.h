#include <String.h>

/* Constantes */
#define true 1u
#define false 0u

#define AJUSTE_VALUE 300u
#define AJUSTE_VALUE1 400u

#define MUX_LEDS_CLOCK_1 8u
#define MUX_LEDS_DATA_1 9u
#define MUX_LEDS_CLOCK_2 10u
#define MUX_LEDS_DATA_2 11u
#define MUX_LEDS_LATCH_ALL 12u


const int ledPin = 13u;

/* Enuns */
enum entradas
{
  SW_01 = 0, // STD_PGM
  SW_02,      // STD_AUX
  SW_03,      // STD_STM
  SW_04,      // STD_EX1
  SW_05,      // STD_EX2
  SW_06,      // STD_TALK
  SW_07,      // CR_PGM
  SW_08,      // CR_AUX
};

enum stages
{
  STAGE_0 = 0,
  STAGE_1,
  STAGE_2,
  STAGE_3,
  STAGE_4,
  STAGE_5,
  STAGE_6,
  STAGE_7
};


enum channels
{
  STD = 1,
  CR,
  PH,
  F_MON,
  STM
};


/* Variaveis Estado Atual Chaves */
unsigned int actualSW_01;  // STD_PGM
unsigned int actualSW_02;  // STD_AUX
unsigned int actualSW_03;  // STD_STM
unsigned int actualSW_04;  // STD_EX1
unsigned int actualSW_05;  // STD_EX2
unsigned int actualSW_06;  // STD_TALK
unsigned int actualSW_07;  // CR_PGM
unsigned int actualSW_08;  // CR_AUX


/* Variaveis Estado Antigo Chaves */
unsigned int SW_01_Old;  // STD_PGM
unsigned int SW_02_Old;  // STD_AUX
unsigned int SW_03_Old;  // STD_STM
unsigned int SW_04_Old;  // STD_EX1
unsigned int SW_05_Old;  // STD_EX2
unsigned int SW_06_Old;  // STD_TALK
unsigned int SW_07_Old;  // CR_PGM
unsigned int SW_08_Old;  // CR_AUX


/* Flag Booleanas de Deteccao */
bool SW_01_Detect;  // STD_PGM
bool SW_02_Detect;  // STD_AUX
bool SW_03_Detect;  // STD_STM
bool SW_04_Detect;  // STD_EX1
bool SW_05_Detect;  // STD_EX2
bool SW_06_Detect;  // STD_TALK
bool SW_07_Detect;  // CR_PGM
bool SW_08_Detect;  // CR_AUX


/* Flag Booleanas de Atuacao */
bool SW_01_Flag;  // STD_PGM
bool SW_02_Flag;  // STD_AUX
bool SW_03_Flag;  // STD_STM
bool SW_04_Flag;  // STD_EX1
bool SW_05_Flag;  // STD_EX2
bool SW_06_Flag;  // STD_TALK
bool SW_07_Flag;  // CR_PGM
bool SW_08_Flag;  // CR_AUX


/* Variaveis de Status de Maquina */
unsigned int STD_STATE; // Maquina de estado Studio
unsigned int CR_STATE;  // Maquina de estado Control Room
unsigned int PH_STATE;  // Maquina de estado Phone
unsigned int STM_STATE; // Maquina de estado Studio Monitor
unsigned int F_MON_STATE; // Maquina de estado Studio Monitor

bool enable_serial;
uint8_t status_buttons;

uint8_t command_update;
uint8_t parameter_update;

uint8_t cache_buffer[27];

/* Funcoes */
void readSwitches(void);
void init_variables(void);

void shiftOut1(void);
void shiftOut2(void);
void leds_operators(void);