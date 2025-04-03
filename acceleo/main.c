#include "cyhal.h"
#include "cybsp.h"
#include "cyhal_i2c.h"
#include "lsm6dsl.h"
#include <stdio.h>
#include "cy_retarget_io.h"

#define I2C_MASTER_FREQUENCY 100000u  // 100kHz I2C frequency
#define I2C_SLAVE_ADDR        0x6Au    // DS3231 RTC I2C address

cy_rslt_t rslt;

// I2C object for PSoC 6
cyhal_i2c_t i2c_obj;

// Function to initialize I2C master
void i2c_master_init(void)
{
    rslt = cyhal_i2c_init(&i2c_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("I2C Master initialization failed, rslt: %d\n", rslt);
        return;
    }

    // Configure I2C master settings
    cyhal_i2c_cfg_t master_config = {
        CYHAL_I2C_MODE_MASTER,  // Set I2C mode as master
        0,                      // Address is not used for master mode
        I2C_MASTER_FREQUENCY    // Set frequency for I2C communication
    };

    // Apply the configuration settings to the I2C master
    rslt = cyhal_i2c_configure(&i2c_obj, &master_config);
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("I2C Master configuration failed, rslt: %d\n", rslt);
        return;
    }
}


// Main function
int main(void)
{

    // Initialize the board (e.g., LED, button, etc.)
    rslt = cybsp_init();
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("Board initialization failed, result: %d\n", rslt);
        return -1;
    }
    // Initialize UART for retargeting (for debugging purposes)
      rslt = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
      if (rslt != CY_RSLT_SUCCESS)
      {
          printf("UART initialization failed, result: %d\n", rslt);
          return -1;
      }


    i2c_master_init();

    // Example of initializing accelerometer and gyroscope
    LSM6DSL_AccInit(0x6A);  // Example config value for accelerometer initialization

    // Read accelerometer data
    int16_t accData[3];
    LSM6DSL_AccReadXYZ(accData);
    printf("Accelerometer data: X: %d, Y: %d, Z: %d\n", accData[0], accData[1], accData[2]);



    // Main loop
    while (1)
    {

    }
}
