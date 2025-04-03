#ifndef SHT2X_FOR_PSOC6_H
#define SHT2X_FOR_PSOC6_H

#include "cyhal.h"

/* I2C address for the SHT2x sensor (7-bit address) */
#define SHT2x_I2C_ADDR            (0x40U)

/* Timeout for I2C operations in milliseconds */
#define SHT2x_TIMEOUT             (1000U)

/* SHT2x Commands */
#define SHT2x_SOFT_RESET          0xFE
#define SHT2x_READ_REG            0xE7
#define SHT2x_WRITE_REG           0xE6
#define SHT2x_READ_TEMP_HOLD      0xE3
#define SHT2x_READ_TEMP_NOHOLD    0xF3
#define SHT2x_READ_RH_HOLD        0xE5
#define SHT2x_READ_RH_NOHOLD      0xF5

/* SHT2x Resolution Options */
#define RES_14_12 0x00
#define RES_12_8  0x01
#define RES_13_10 0x02
#define RES_11_11 0x03

/* I2C handle for communication */
extern cyhal_i2c_t _sht2x_ui2c;

/* Function Prototypes */
void SHT2x_Init(cyhal_i2c_t *i2c);
void SHT2x_SoftReset(void);
uint8_t SHT2x_ReadUserReg(void);
uint16_t SHT2x_GetRaw(uint8_t cmd);
float SHT2x_GetTemperature(uint8_t hold);
float SHT2x_GetRelativeHumidity(uint8_t hold);
void SHT2x_SetResolution(uint8_t res);
float SHT2x_CelsiusToFahrenheit(float celsius);
float SHT2x_CelsiusToKelvin(float celsius);
int32_t SHT2x_GetInteger(float num);
uint32_t SHT2x_GetDecimal(float num, int digits);
uint32_t SHT2x_Ipow(uint32_t base, uint32_t power);

#endif /* SHT2X_FOR_PSOC6_H */
