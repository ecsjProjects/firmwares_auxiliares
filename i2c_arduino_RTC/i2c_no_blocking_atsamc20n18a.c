#include "sam.h"
#include "plib_sercom5_i2c_master.h"
#include <stdint.h>
#include <stdio.h>

// ==================== DEFINIÇÕES ====================
#define PCA8565_ADDR    0x51u

// Estados da máquina de estados para o RTC
typedef enum {
    RTC_STATE_IDLE,
    RTC_STATE_SET_STOP,
    RTC_STATE_WRITE_TIME,
    RTC_STATE_SET_START,
    RTC_STATE_READ_TIME
} RTC_State_t;

volatile RTC_State_t rtc_state = RTC_STATE_IDLE;
volatile bool rtc_transfer_done = false;
volatile SERCOM_I2C_ERROR rtc_error = SERCOM_I2C_ERROR_NONE;

// Buffer para leitura/escrita
static uint8_t i2c_buffer[7];

// Protótipos
uint8_t bin2bcd(uint8_t val);
uint8_t bcd2bin(uint8_t val);
void SERCOM5_I2C_Callback(uintptr_t context);
void setTime_nonblocking(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t weekday, uint8_t month, uint8_t year);
void readTime_nonblocking(void);
void process_rtc(void);

// ===================== FUNÇÕES AUXILIARES =====================
uint8_t bin2bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t bcd2bin(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

// ===================== CALLBACK DO I2C =====================
void SERCOM5_I2C_Callback(uintptr_t context)
{
    rtc_error = SERCOM5_I2C_ErrorGet();
    
    if (rtc_error != SERCOM_I2C_ERROR_NONE) {
        rtc_state = RTC_STATE_IDLE;
        rtc_transfer_done = true;
        return;
    }

    rtc_transfer_done = true;
}

// ===================== FUNÇÕES DO RTC (NÃO BLOQUEANTES) =====================
void setTime_nonblocking(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day,
                        uint8_t weekday, uint8_t month, uint8_t year)
{
    if (rtc_state != RTC_STATE_IDLE) return;

    // Prepara os dados
    i2c_buffer[0] = bin2bcd(sec) & 0x7F;   // Seconds + VL=0
    i2c_buffer[1] = bin2bcd(min);
    i2c_buffer[2] = bin2bcd(hour);
    i2c_buffer[3] = bin2bcd(day);
    i2c_buffer[4] = bin2bcd(weekday);
    i2c_buffer[5] = bin2bcd(month);        // Century = 0 (2000-2099)
    i2c_buffer[6] = bin2bcd(year);

    rtc_state = RTC_STATE_SET_STOP;
    rtc_transfer_done = false;

    // Passo 1: Parar o relógio (registrador 0x00 = 0x20)
    uint8_t stop_cmd = 0x20;
    SERCOM5_I2C_Write(PCA8565_ADDR, &stop_cmd, 1);
}

void readTime_nonblocking(void)
{
    if (rtc_state != RTC_STATE_IDLE) return;

    rtc_state = RTC_STATE_READ_TIME;
    rtc_transfer_done = false;

    // Lê 7 bytes a partir do registrador 0x02
    uint8_t reg = 0x02;
    SERCOM5_I2C_WriteRead(PCA8565_ADDR, &reg, 1, i2c_buffer, 7);
}

// Processa a máquina de estados do RTC
void process_rtc(void)
{
    if (!rtc_transfer_done) return;

    rtc_transfer_done = false;

    switch (rtc_state)
    {
        case RTC_STATE_SET_STOP:
            // Após parar o clock, escreve os dados de tempo
            rtc_state = RTC_STATE_WRITE_TIME;
            rtc_transfer_done = false;
            SERCOM5_I2C_WriteRead(PCA8565_ADDR, (uint8_t[]){0x02}, 1, i2c_buffer, 7);
            break;

        case RTC_STATE_WRITE_TIME:
            // Após escrever o tempo, reinicia o clock
            rtc_state = RTC_STATE_SET_START;
            rtc_transfer_done = false;
            uint8_t start_cmd = 0x00;
            SERCOM5_I2C_Write(PCA8565_ADDR, &start_cmd, 1);
            break;

        case RTC_STATE_SET_START:
            rtc_state = RTC_STATE_IDLE;
            printf("Hora configurada com sucesso!\n");
            break;

        case RTC_STATE_READ_TIME:
        {
            rtc_state = RTC_STATE_IDLE;

            uint8_t sec     = bcd2bin(i2c_buffer[0] & 0x7F);
            uint8_t min     = bcd2bin(i2c_buffer[1] & 0x7F);
            uint8_t hour    = bcd2bin(i2c_buffer[2] & 0x3F);
            uint8_t day     = bcd2bin(i2c_buffer[3] & 0x3F);
            uint8_t weekday = bcd2bin(i2c_buffer[4] & 0x07);
            uint8_t month   = bcd2bin(i2c_buffer[5] & 0x1F);
            uint8_t year    = bcd2bin(i2c_buffer[6]);

            int full_year = (i2c_buffer[5] & 0x80) ? 1900 + year : 2000 + year;

            printf("%04d-%02d-%02d %02d:%02d:%02d  Weekday:%d\n",
                   full_year, month, day, hour, min, sec, weekday);
            break;
        }

        default:
            rtc_state = RTC_STATE_IDLE;
            break;
    }
}

// ===================== MAIN =====================
int main(void)
{
    SystemInit();

    // Inicializa I2C SERCOM5
    SERCOM5_I2C_Initialize();

    // Registra o callback (não bloqueante)
    SERCOM5_I2C_CallbackRegister(SERCOM5_I2C_Callback, 0);

    // Configura o horário inicial (23/04/2026 - 22:14:00 - Quinta-feira = 4)
    setTime_nonblocking(0, 14, 22, 23, 4, 4, 26);

    uint32_t delay_counter = 0;

    while (1)
    {
        process_rtc();           // Processa máquina de estados do RTC

        // Lê o horário a cada ~1 segundo
        if (rtc_state == RTC_STATE_IDLE)
        {
            delay_counter++;
            if (delay_counter >= 800000)   // Ajuste conforme seu clock (aprox. 1s em 48MHz)
            {
                delay_counter = 0;
                readTime_nonblocking();
            }
        }
    }
}