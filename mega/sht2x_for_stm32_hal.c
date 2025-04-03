#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "sht2x_for_stm32_hal.h"

/* Define the I2C address for the sensor */
#define SHT2x_I2C_ADDR            (0x40U) // 7-bit address for SHT2x sensor
#define SHT2x_TIMEOUT             (1000U) // Timeout for I2C operations (in ms)

/* I2C handle */
extern cyhal_i2c_t _sht2x_ui2c;

/* Function to initialize the SHT2x sensor */
void SHT2x_Init(cyhal_i2c_t *i2c) {
    _sht2x_ui2c = *i2c;
}

/* Function to perform a soft reset */
void SHT2x_SoftReset(void) {
    uint8_t cmd = 0xFE; // Soft reset command
    cyhal_i2c_master_write(&_sht2x_ui2c, SHT2x_I2C_ADDR, &cmd, 1,1000, false);
}

/* Function to read the user register */
uint8_t SHT2x_ReadUserReg(void) {
    uint8_t cmd = 0xE7; // Command to read user register
    uint8_t val;
    cyhal_i2c_master_write(&_sht2x_ui2c, SHT2x_I2C_ADDR, &cmd, 1,1000, true);
    cyhal_i2c_master_read(&_sht2x_ui2c, SHT2x_I2C_ADDR, &val,1, 1000,false);
    return val;
}

/* Function to get a raw measurement (temperature or humidity) */
uint16_t SHT2x_GetRaw(uint8_t cmd) {
    uint8_t val[3] = {0};
    cyhal_i2c_master_write(&_sht2x_ui2c, SHT2x_I2C_ADDR, &cmd, 1,1000, true);
    cyhal_i2c_master_read(&_sht2x_ui2c, SHT2x_I2C_ADDR, val, 3,1000,false);
    return (val[0] << 8) | val[1];
}

/* Function to measure and get the current temperature */
float SHT2x_GetTemperature(uint8_t hold) {
    uint8_t cmd = (hold ? 0xE3 : 0xF3); // Command for temperature measurement (hold/no-hold)
    return -46.85 + 175.72 * (SHT2x_GetRaw(cmd) / 65536.0);
}

/* Function to measure and get the current relative humidity */
float SHT2x_GetRelativeHumidity(uint8_t hold) {
    uint8_t cmd = (hold ? 0xE5 : 0xF5); // Command for humidity measurement (hold/no-hold)
    return -6 + 125.00 * (SHT2x_GetRaw(cmd) / 65536.0);
}

/* Function to set the measurement resolution */
void SHT2x_SetResolution(uint8_t res) {
    uint8_t val = SHT2x_ReadUserReg();
    val = (val & 0x7E) | res;
    uint8_t temp[2] = {0xE6, val}; // Command to write user register
    cyhal_i2c_master_write(&_sht2x_ui2c, SHT2x_I2C_ADDR, temp, 2,1000, false);
}

/* Function to convert Celsius to Fahrenheit */
float SHT2x_CelsiusToFahrenheit(float celsius) {
    return (9.0 / 5.0) * celsius + 32;
}

/* Function to convert Celsius to Kelvin */
float SHT2x_CelsiusToKelvin(float celsius) {
    return celsius + 273;
}

/* Function to get the integer part of a floating point number */
int32_t SHT2x_GetInteger(float num) {
    return (int32_t)num;
}

/* Function to get the decimal part of a floating point number */
uint32_t SHT2x_GetDecimal(float num, int digits) {
    float postDec = num - SHT2x_GetInteger(num);
    uint32_t multiplier = 1;
    for (int i = 0; i < digits; i++) {
        multiplier *= 10;
    }
    return postDec * multiplier;
}

/* Integer equivalent of pow() in math.h */
uint32_t SHT2x_Ipow(uint32_t base, uint32_t power) {
    uint32_t result = 1;
    for (uint32_t i = 0; i < power; i++) {
        result *= base;
    }
    return result;
}

