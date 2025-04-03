#include "lsm6dsl.h"
#include "cyhal_i2c.h"
#include "stdio.h"
// I2C object for PSoC 6
extern cyhal_i2c_t i2c_obj;

void SENSOR_IO_Write(uint8_t devAddr, uint8_t regAddr, uint8_t value)
{
    uint8_t writeData[2] = {regAddr, value};

    // Use cyhal_i2c_master_write() for I2C write operation
    cy_rslt_t result = cyhal_i2c_master_write(&i2c_obj, devAddr, writeData, 2, 0, true);
    if (result != CY_RSLT_SUCCESS)
    {
        // Handle error
        printf("I2C write failed!\n");
    }
}

uint8_t SENSOR_IO_Read(uint8_t devAddr, uint8_t regAddr)
{
    uint8_t readData;

    // Use cyhal_i2c_master_write_read() for I2C read operation
    cy_rslt_t result = cyhal_i2c_master_read(&i2c_obj, devAddr,   &readData,1, 0, true);
    if (result != CY_RSLT_SUCCESS)
    {
        // Handle error
        printf("I2C read failed!\n");
    }

    return readData;
}

void LSM6DSL_AccInit(uint16_t InitStruct)
{
    uint8_t ctrl = 0x00;
    uint8_t tmp;

    // Read current accelerometer control register settings
    tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL);
    ctrl = (uint8_t) InitStruct;
    tmp &= ~(0xFC);  // Mask relevant bits
    tmp |= ctrl;     // Set new control value
    SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL, tmp);

    // Modify the control register 3
    tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C);
    ctrl = ((uint8_t) (InitStruct >> 8)); // Set upper bits for ctrl
    tmp &= ~(0x44);  // Mask out bits
    tmp |= ctrl;     // Apply new configuration
    SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C, tmp);
}

void LSM6DSL_AccReadXYZ(int16_t* pData)
{
    int16_t pnRawData[3];
    uint8_t buffer[6];
    uint8_t i = 0;
    float sensitivity = 0;

    uint8_t ctrlx = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL1_XL);

    // Read 6 bytes of accelerometer data (X, Y, Z)
    SENSOR_IO_ReadMultiple(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_OUTX_L_XL, buffer, 6);

    // Combine the data from the 6 bytes into 3 16-bit values for X, Y, Z
    for (i = 0; i < 3; i++)
    {
        pnRawData[i] = (((uint16_t)buffer[2 * i + 1]) << 8) + (uint16_t)buffer[2 * i];
    }

    // Set the sensitivity based on the accelerometer control register value
    switch (ctrlx & 0x0C)
    {
    case 0x00:
        sensitivity = LSM6DSL_ACC_SENSITIVITY_2G;
        break;
    case 0x04:
        sensitivity = LSM6DSL_ACC_SENSITIVITY_4G;
        break;
    case 0x08:
        sensitivity = LSM6DSL_ACC_SENSITIVITY_8G;
        break;
    case 0x0C:
        sensitivity = LSM6DSL_ACC_SENSITIVITY_16G;
        break;
    }

    // Apply sensitivity and store the final values in pData
    for (i = 0; i < 3; i++)
    {
        pData[i] = (int16_t)(pnRawData[i] * sensitivity);
    }
}

void LSM6DSL_GyroInit(uint16_t InitStruct)
{
    uint8_t ctrl = 0x00;
    uint8_t tmp;

    // Read current gyroscope control register settings
    tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G);
    ctrl = (uint8_t) InitStruct;
    tmp &= ~(0xFC);  // Mask relevant bits
    tmp |= ctrl;     // Set new control value
    SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G, tmp);

    // Modify the control register 3 for gyroscope settings
    tmp = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C);
    ctrl = ((uint8_t) (InitStruct >> 8)); // Set upper bits for ctrl
    tmp &= ~(0x44);  // Mask out bits
    tmp |= ctrl;     // Apply new configuration
    SENSOR_IO_Write(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL3_C, tmp);
}

void LSM6DSL_GyroReadXYZAngRate(float* pfData)
{
    int16_t pnRawData[3];
    uint8_t buffer[6];
    uint8_t i = 0;
    float sensitivity = 0;

    uint8_t ctrlg = SENSOR_IO_Read(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_CTRL2_G);

    // Read 6 bytes of gyroscope data (X, Y, Z)
    SENSOR_IO_ReadMultiple(LSM6DSL_ACC_GYRO_I2C_ADDRESS_LOW, LSM6DSL_ACC_GYRO_OUTX_L_G, buffer, 6);

    // Combine the data from the 6 bytes into 3 16-bit values for X, Y, Z
    for (i = 0; i < 3; i++)
    {
        pnRawData[i] = (((uint16_t)buffer[2 * i + 1]) << 8) + (uint16_t)buffer[2 * i];
    }

    // Set the sensitivity based on the gyroscope control register value
    switch (ctrlg & 0x0C)
    {
    case 0x00:
        sensitivity = LSM6DSL_GYRO_SENSITIVITY_245DPS;
        break;
    case 0x04:
        sensitivity = LSM6DSL_GYRO_SENSITIVITY_500DPS;
        break;
    case 0x08:
        sensitivity = LSM6DSL_GYRO_SENSITIVITY_1000DPS;
        break;
    case 0x0C:
        sensitivity = LSM6DSL_GYRO_SENSITIVITY_2000DPS;
        break;
    }

    // Apply sensitivity and store the final values in pfData
    for (i = 0; i < 3; i++)
    {
        pfData[i] = (float)(pnRawData[i] * sensitivity);
    }
}
void SENSOR_IO_ReadMultiple(uint8_t devAddr, uint8_t regAddr, uint8_t* pData, uint16_t size)
{
    // Start the read transaction by writing the register address
    cy_rslt_t result = cyhal_i2c_master_write(&i2c_obj, devAddr, &regAddr, 1, 0, false);
    if (result != CY_RSLT_SUCCESS)
    {
        // Handle error if write fails
        printf("I2C write failed for register address!\n");
        return;
    }

    // Now, perform the read to get 'size' bytes
    result = cyhal_i2c_master_read(&i2c_obj, devAddr, pData, size, 0, true);
    if (result != CY_RSLT_SUCCESS)
    {
        // Handle error if read fails
        printf("I2C read failed!\n");
        return;
    }
}

