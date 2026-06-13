/*******************
 * Title: FWR_KL_ARDUINO.ino
 * Name: Edson Claiton
 * Date: 2025-06-26
 * Description: Firmware for Arduino to interface with the FWR-KL footswitch.
 * version: 1.0
 * Client: Biquad Broadcast
 */

#include "definitions.h"

//***************************************************************************
void setup() {
  // put your setup code here, to run once:

  pinMode(SW_01, INPUT);
  pinMode(SW_02, INPUT);
  pinMode(SW_03, INPUT);
  pinMode(SW_04, INPUT);
  pinMode(SW_05, INPUT);
  pinMode(SW_06, INPUT);
  pinMode(SW_07, INPUT);
  pinMode(SW_08, INPUT);

  pinMode(MUX_LEDS_CLOCK_1, OUTPUT);
  pinMode(MUX_LEDS_DATA_1, OUTPUT);
  pinMode(MUX_LEDS_CLOCK_2, OUTPUT);
  pinMode(MUX_LEDS_DATA_2, OUTPUT);
  pinMode(MUX_LEDS_CLOCK_3, OUTPUT);
  pinMode(MUX_LEDS_DATA_3, OUTPUT);
  pinMode(MUX_LEDS_CLOCK_4, OUTPUT);
  pinMode(MUX_LEDS_DATA_4, OUTPUT);
  pinMode(MUX_LEDS_LATCH_ALL, OUTPUT);

  Serial.begin(9600);
  // Serial1.begin(9600);

  pinMode(ledPin, OUTPUT);
}

//***************************************************************************
void loop() {
  // put your main code here, to run repeatedly:
  init_variables();

  while (1) {
    readSwitches();
    delay(200);
    // leds_operators();

    if (enable_serial == true) {
      enable_serial = false;
      status_buttons = 0u;
    }
  }
}

/*****************************************************************************
 * FUNCAO DE LEITURA DE BOTOES
 * Entrada: Varredura dos botoes
 * SAIDA: Somente um botao funciona por vez
 * GLOBAL: STD_STATE
 */
void readSwitches() {
  /* Studio - STD*/
  /*SW_01 - STD_PGM */
  actualSW_01 = digitalRead(SW_01);
  if (SW_01_Old == false && actualSW_01 == true) {
    SW_01_Detect = true;
  } else {
    SW_01_Detect = false;
  }
  if (SW_01_Detect == true) {
    /* Sinal de envio para atuacao no FWR */
    SW_01_Flag = !SW_01_Flag;

    /*Posicao Vetor */
    cache_buffer[1] = (uint8_t)SW_01_Flag;
    enable_serial = true;
    status_buttons = 1u;
    Serial.print("CH1: ");
    Serial.println(SW_01_Flag);
  }
  SW_01_Old = actualSW_01;

  /*SW_02 - STD_AUX */
  actualSW_02 = digitalRead(SW_02);
  if (SW_02_Old == false && actualSW_02 == true) {
    SW_02_Detect = true;
  } else {
    SW_02_Detect = false;
  }
  if (SW_02_Detect == true) {
    /* Sinal de envio para atuacao no FWR */
    SW_02_Flag = !SW_02_Flag;

    /*Posicao Vetor */
    cache_buffer[2] = (uint8_t)SW_02_Flag;
    enable_serial = true;
    status_buttons = 2u;
    Serial.print("CH2: ");
    Serial.println(SW_02_Flag);
  }
  SW_02_Old = actualSW_02;

  /*SW_03 - STD_STM */
  actualSW_03 = digitalRead(SW_03);
  if (SW_03_Old == false && actualSW_03 == true) {
    SW_03_Detect = true;
  } else {
    SW_03_Detect = false;
  }
  if (SW_03_Detect == true) {
    /* Sinal de envio para atuacao no FWR */
    SW_03_Flag = !SW_03_Flag;

    /*Posicao Vetor */
    cache_buffer[3] = (uint8_t)SW_03_Flag;
    enable_serial = true;
    status_buttons = 3u;
    Serial.print("CH3: ");
    Serial.println(SW_03_Flag);
  }
  SW_03_Old = actualSW_03;

  /*SW_04 - STD_EX1 */
  actualSW_04 = digitalRead(SW_04);
  if (SW_04_Old == false && actualSW_04 == true) {
    SW_04_Detect = true;
  } else {
    SW_04_Detect = false;
  }
  if (SW_04_Detect == true) {
    /* Sinal de envio para atuacao no FWR */
    SW_04_Flag = !SW_04_Flag;

    /*Posicao Vetor */
    cache_buffer[4] = (uint8_t)SW_04_Flag;
    enable_serial = true;
    status_buttons = 4u;
    Serial.print("CH4: ");
    Serial.println(SW_04_Flag);
  }
  SW_04_Old = actualSW_04;

  /*SW_05 - STD_EX2 */
  actualSW_05 = digitalRead(SW_05);
  if (SW_05_Old == false && actualSW_05 == true) {
    SW_05_Detect = true;
  } else {
    SW_05_Detect = false;
  }
  if (SW_05_Detect == true) {
    /* Sinal de envio para atuacao no FWR */
    SW_05_Flag = !SW_05_Flag;

    /*Posicao Vetor */
    cache_buffer[5] = (uint8_t)SW_05_Flag;
    enable_serial = true;
    status_buttons = 5u;
    Serial.print("CH5: ");
    Serial.println(SW_05_Flag);
  }
  SW_05_Old = actualSW_05;

  /*SW_06 - STD_TALK */
  actualSW_06 = digitalRead(SW_06);
  if (SW_06_Old == false && actualSW_06 == true) {
    SW_06_Detect = true;
  } else {
    SW_06_Detect = false;
  }
  if (SW_06_Detect == true) {
    /* Sinal de envio para atuacao no FWR */
    SW_06_Flag = !SW_06_Flag;

    /*Posicao Vetor */
    cache_buffer[6] = (uint8_t)SW_06_Flag;
    enable_serial = true;
    status_buttons = 6u;
    Serial.print("CH6: ");
    Serial.println(SW_06_Flag);
  }
  SW_06_Old = actualSW_06;

  /* Control Room - CR*/
  /*SW_07 - CR_PGM */
  actualSW_07 = digitalRead(SW_07);
  if (SW_07_Old == false && actualSW_07 == true) {
    SW_07_Detect = true;
  } else {
    SW_07_Detect = false;
  }
  if (SW_07_Detect == true) {
    /* Sinal de envio para atuacao no FWR */
    SW_07_Flag = !SW_07_Flag;

    /*Posicao Vetor */
    cache_buffer[7] = (uint8_t)SW_07_Flag;
    enable_serial = true;
    status_buttons = 7u;
    Serial.print("CH7: ");
    Serial.println(SW_07_Flag);
  }
  SW_07_Old = actualSW_07;

  /*SW_08 - CR_AUX */
  actualSW_08 = digitalRead(SW_08);
  if (SW_08_Old == false && actualSW_08 == true) {
    SW_08_Detect = true;
  } else {
    SW_08_Detect = false;
  }
  if (SW_08_Detect == true) {
    /* Sinal de envio para atuacao no FWR */
    SW_08_Flag = !SW_08_Flag;

    /*Posicao Vetor */
    cache_buffer[8] = (uint8_t)SW_08_Flag;
    enable_serial = true;
    status_buttons = 8u;
    Serial.print("CH8: ");
    Serial.println(SW_08_Flag);
  }
  SW_08_Old = actualSW_08;
}

/*****************************************************************************
 * FUNCAO INICIALIZACAO DE VARIAVEIS
 */
void init_variables() {
  /* Variaveis */
  actualSW_01 = false;  // STD_PGM
  actualSW_02 = false;  // STD_AUX
  actualSW_03 = false;  // STD_STM
  actualSW_04 = false;  // STD_EX1
  actualSW_05 = false;  // STD_EX2
  actualSW_06 = false;  // STD_TALK
  actualSW_07 = false;  // CR_PGM
  actualSW_08 = false;  // CR_AUX

  SW_01_Old = true;  // STD_PGM
  SW_02_Old = true;  // STD_AUX
  SW_03_Old = true;  // STD_STM
  SW_04_Old = true;  // STD_EX1
  SW_05_Old = true;  // STD_EX2
  SW_06_Old = true;  // STD_TALK
  SW_07_Old = true;  // CR_PGM
  SW_08_Old = true;  // CR_AUX

  /* Flag Booleanas */
  SW_01_Detect = false;  // STD_PGM
  SW_02_Detect = false;  // STD_AUX
  SW_03_Detect = false;  // STD_STM
  SW_04_Detect = false;  // STD_EX1
  SW_05_Detect = false;  // STD_EX2
  SW_06_Detect = false;  // STD_TALK
  SW_07_Detect = false;  // CR_PGM
  SW_08_Detect = false;  // CR_AUX

  SW_01_Flag = false;  // STD_PGM
  SW_02_Flag = false;  // STD_AUX
  SW_03_Flag = false;  // STD_STM
  SW_04_Flag = false;  // STD_EX1
  SW_05_Flag = false;  // STD_EX2
  SW_06_Flag = false;  // STD_TALK
  SW_07_Flag = false;  // CR_PGM
  SW_08_Flag = false;  // CR_AUX

  enable_serial = true;
}

/*****************************************************************************
 * FUNCAO DE ACIONAMENTO DOS LEDS STD
 */
void shiftOut1(void)  // Acionamento LED STD
{
  bool valor;
  for (unsigned int i = 7; i >= 0; i--) {
    valor = 0;
    // delay(1);
    if (valor) {
      digitalWrite(MUX_LEDS_DATA_1, true);
      // delay(1);
    } else {
      digitalWrite(MUX_LEDS_DATA_1, false);
      // delay(1);
    }

    digitalWrite(MUX_LEDS_CLOCK_1, true);
    // delay(1);
    digitalWrite(MUX_LEDS_CLOCK_1, false);
    // delay(1);
  }
}

/*****************************************************************************
 * FUNCAO DE ACIONAMENTO DOS LEDS CR
 */
void shiftOut2(void)  // Acionamento LED CR
{
  bool valor;
  for (unsigned int i = 14; i > 7; i--) {
    valor = 0;
    // delay(1);
    if (valor) {
      digitalWrite(MUX_LEDS_DATA_2, true);
      // delay(AJUSTE_VALUE1);
    } else {
      digitalWrite(MUX_LEDS_DATA_2, false);
      // delay(AJUSTE_VALUE1);
    }

    digitalWrite(MUX_LEDS_CLOCK_2, true);
    // delay(AJUSTE_VALUE);
    digitalWrite(MUX_LEDS_CLOCK_2, false);
    // delay(AJUSTE_VALUE);
  }
}

/*****************************************************************************
 * FUNCAO DE ACIONAMENTO DOS LEDS GERAL
 */
void leds_operators(void) {
  // delay(1);
  shiftOut1();
  // delay(1);
  shiftOut2();
  // delay(1);
  digitalWrite(MUX_LEDS_LATCH_ALL, true);
  // delay(AJUSTE_VALUE);
  digitalWrite(MUX_LEDS_LATCH_ALL, false);
  // delay(AJUSTE_VALUE);
}
