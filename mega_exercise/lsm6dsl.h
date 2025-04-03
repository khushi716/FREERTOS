#ifndef LSM6DSL_H
#define LSM6DSL_H

#include "cyhal_i2c.h"
#include <stdint.h>
#include <stdbool.h>

// LSM6DSL device I2C address
#define LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW    (0x6A) // I2C address with the ADDR pin to ground

// LSM6DSL register addresses
#define LSM6DSL_ACC_GYRO_CTRL1_XL           0x10
#define LSM6DSL_ACC_GYRO_CTRL2_G            0x11
#define LSM6DSL_ACC_GYRO_CTRL3_C            0x12
#define LSM6DSL_ACC_GYRO_OUTX_L_XL         0x28
#define LSM6DSL_ACC_GYRO_OUTX_L_G          0x22

// Accelerometer sensitivity
#define LSM6DSL_ACC_SENSITIVITY_2G          0.061f
#define LSM6DSL_ACC_SENSITIVITY_4G          0.122f
#define LSM6DSL_ACC_SENSITIVITY_8G          0.244f
#define LSM6DSL_ACC_SENSITIVITY_16G         0.488f

// Gyroscope sensitivity
#define LSM6DSL_GYRO_SENSITIVITY_245DPS     8.75f
#define LSM6DSL_GYRO_SENSITIVITY_500DPS     17.5f
#define LSM6DSL_GYRO_SENSITIVITY_1000DPS    35.0f
#define LSM6DSL_GYRO_SENSITIVITY_2000DPS    70.0f

// Function declarations
void SENSOR_IO_Write(uint8_t devAddr, uint8_t regAddr, uint8_t value);
uint8_t SENSOR_IO_Read(uint8_t devAddr, uint8_t regAddr);
void LSM6DSL_AccInit(uint16_t InitStruct);
void LSM6DSL_AccReadXYZ(int16_t* pData);
void LSM6DSL_GyroInit(uint16_t InitStruct);
void LSM6DSL_GyroReadXYZAngRate(float* pfData);
void SENSOR_IO_ReadMultiple(uint8_t devAddr, uint8_t regAddr, uint8_t* pData, uint16_t size);

#endif /* LSM6DSL_H */
