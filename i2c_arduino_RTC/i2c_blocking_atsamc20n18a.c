#include "sam.h"                          // ou "definitions.h" se estiver usando Harmony
#include "plib_sercom5_i2c_master.h"      // Biblioteca que você enviou
#include <stdint.h>
#include <stdio.h>                        // Para printf (ou use sua UART)

// ==================== DEFINIÇÕES ====================
#define PCA8565_ADDR    0x51u

// Protótipos
uint8_t bin2bcd(uint8_t val);
uint8_t bcd2bin(uint8_t val);
void setTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day,
             uint8_t weekday, uint8_t month, uint8_t year);
void readTime(void);

// ===================== FUNÇÕES AUXILIARES =====================
uint8_t bin2bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t bcd2bin(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

// ===================== INICIALIZAÇÃO =====================
void i2c_init(void) {
    SERCOM5_I2C_Initialize();           // Inicializa o SERCOM5 como I2C Master (100kHz)
}

// ===================== FUNÇÕES DO RTC PCA8565 =====================
void setTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day,
             uint8_t weekday, uint8_t month, uint8_t year) {

    uint8_t buf[7];

    // Stop the clock (Control/Status 1 register)
    uint8_t stop = 0x20;
    SERCOM5_I2C_Write(PCA8565_ADDR, &stop, 1);   // Escreve no registrador 0x00
    while (SERCOM5_I2C_IsBusy());

    // Prepara os dados dos registradores de tempo (a partir de 0x02)
    buf[0] = bin2bcd(sec) & 0x7F;      // Seconds + VL=0
    buf[1] = bin2bcd(min);
    buf[2] = bin2bcd(hour);
    buf[3] = bin2bcd(day);
    buf[4] = bin2bcd(weekday);
    buf[5] = bin2bcd(month);           // Century bit = 0 → 2000-2099
    buf[6] = bin2bcd(year);

    // Escreve a partir do registrador 0x02
    SERCOM5_I2C_WriteRead(PCA8565_ADDR, (uint8_t[]){0x02}, 1, buf, 7);
    while (SERCOM5_I2C_IsBusy());

    // Restart the clock
    stop = 0x00;
    SERCOM5_I2C_Write(PCA8565_ADDR, &stop, 1);
    while (SERCOM5_I2C_IsBusy());
}

void readTime(void) {
    uint8_t buf[7];

    // Lê 7 bytes a partir do registrador 0x02
    SERCOM5_I2C_WriteRead(PCA8565_ADDR, (uint8_t[]){0x02}, 1, buf, 7);
    while (SERCOM5_I2C_IsBusy());

    uint8_t sec     = bcd2bin(buf[0] & 0x7F);
    uint8_t min     = bcd2bin(buf[1] & 0x7F);
    uint8_t hour    = bcd2bin(buf[2] & 0x3F);
    uint8_t day     = bcd2bin(buf[3] & 0x3F);
    uint8_t weekday = bcd2bin(buf[4] & 0x07);
    uint8_t month   = bcd2bin(buf[5] & 0x1F);
    uint8_t year    = bcd2bin(buf[6]);

    int full_year = (buf[5] & 0x80) ? 1900 + year : 2000 + year;

    // Imprime (substitua por sua função de UART se não estiver usando printf)
    printf("%04d-%02d-%02d %02d:%02d:%02d  Weekday:%d\n",
           full_year, month, day, hour, min, sec, weekday);
}

// ===================== MAIN =====================
int main(void) {
    SystemInit();               // Configura clock do sistema (normalmente 48 MHz)
    SERCOM5_I2C_Initialize();   // ou chame i2c_init() se preferir

    // Exemplo: Configurar 23/04/2026  22:14:00  (quinta-feira = 4)
    // sec, min, hour, day, weekday(0=domingo), month, year
    setTime(0, 14, 22, 23, 4, 4, 26);

    while (1) {
        readTime();
        
        // Delay aproximado de 1 segundo (ajuste conforme seu clock ou use Timer)
        for (volatile uint32_t i = 0; i < 12000000; i++);   // ~1s em 48MHz
    }
}