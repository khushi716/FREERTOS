//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//#include "cyhal_i2c.h"
//#include <stdio.h>
//
//#define I2C_MASTER_FREQUENCY 100000u  // I2C frequency set to 100kHz
//#define I2C_SLAVE_ADDR        0x08u    // I2C slave address (8-bit address)
//#define TX_BUF_SIZE           4        // Size of the transmission buffer
//#define RX_BUF_SIZE           4        // Size of the receive buffer
//
//// Declare I2C Master variables
//cy_rslt_t rslt;
//cyhal_i2c_t i2c_master_obj;
//uint8_t tx_buf[TX_BUF_SIZE] = { 0x01, 0x02, 0x03, 0x04 }; // Data to send
//size_t tx_length = TX_BUF_SIZE;  // Length of data to send
//
//cyhal_i2c_t i2c_slave_obj;
//uint8_t rx_buf[RX_BUF_SIZE];  // Buffer to receive data from the master
//size_t rx_length = RX_BUF_SIZE;  // Length of data to receive
//
//// Function to initialize the I2C master
//void i2c_master_init(void)
//{
//    // Initialize I2C master with SDA and SCL pins from the board
//    rslt = cyhal_i2c_init(&i2c_master_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("I2C Master initialization failed, rslt: %d\n", rslt);
//        return;
//    }
//
//    // Configure I2C master settings
//    cyhal_i2c_cfg_t master_config = {
//        CYHAL_I2C_MODE_MASTER,  // Set I2C mode as master
//        0,                      // Address is not used for master mode
//        I2C_MASTER_FREQUENCY    // Set frequency for I2C communication
//    };
//
//    // Apply the configuration settings to the I2C master
//    rslt = cyhal_i2c_configure(&i2c_master_obj, &master_config);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("I2C Master configuration failed, rslt: %d\n", rslt);
//        return;
//    }
//}
//
//// Function to send data from master to slave
//void i2c_master_send_data(void)
//{
//    // Send data to the slave device
//    rslt = cyhal_i2c_master_write(&i2c_master_obj, I2C_SLAVE_ADDR, tx_buf, tx_length, 10, false);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("I2C Master write failed, rslt: %d\n", rslt);
//    }
//    else
//    {
//        printf("I2C Master sent data: ");
//        for (size_t i = 0; i < tx_length; i++)
//        {
//            printf("0x%02X ", tx_buf[i]);
//        }
//        printf("\n");
//    }
//}
//
//// Function to initialize I2C slave
//void i2c_slave_init(void)
//{
//    // Initialize I2C slave with SDA and SCL pins from the board
//    rslt = cyhal_i2c_init(&i2c_slave_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("I2C Slave initialization failed, rslt: %d\n", rslt);
//        return;
//    }
//
//    // Configure I2C slave settings
//    cyhal_i2c_cfg_t slave_config = {
//        CYHAL_I2C_MODE_SLAVE,  // Set I2C mode as slave
//        I2C_SLAVE_ADDR,        // Slave address
//        I2C_MASTER_FREQUENCY   // Set frequency for I2C communication
//    };
//
//    // Apply the configuration settings to the I2C slave
//    rslt = cyhal_i2c_configure(&i2c_slave_obj, &slave_config);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("I2C Slave configuration failed, rslt: %d\n", rslt);
//        return;
//    }
//}
//
//// Function to receive data from the master
//void i2c_slave_receive_data(void)
//{
//    // Wait for data from the master
//    rslt = cyhal_i2c_slave_read(&i2c_slave_obj, rx_buf, rx_length, 10);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("I2C Slave read failed, rslt: %d\n", rslt);
//    }
//    else
//    {
//        printf("I2C Slave received data: ");
//        for (size_t i = 0; i < rx_length; i++)
//        {
//            printf("0x%02X ", rx_buf[i]);
//        }
//        printf("\n");
//    }
//}
//
//// Main function
//int main(void)
//{
//    // Initialize the board (e.g., LED, button, etc.)
//    rslt = cybsp_init();
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("Board initialization failed, rslt: %d\n", rslt);
//        return -1;
//    }
//
//    // Initialize the UART for retargeting (for debugging purposes)
//    rslt = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("UART initialization failed, rslt: %d\n", rslt);
//        return -1;
//    }
//
//
//    printf("\x1b[2J\x1b[;H");
//    printf("****************** I2C Master-Slave Example ******************\r\n\n");
//
//    // Initialize I2C master and slave
//    i2c_master_init();
//    i2c_slave_init();
//
//    // Main loop
//    while (1)
//    {
//
//        i2c_master_send_data();
//
//
//        cyhal_system_delay_ms(5000);
//
//
//        i2c_slave_receive_data();
//
//
//        cyhal_system_delay_ms(5000);
//    }
//
//    return 0;
//
//}
//
//
//
//
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cyhal_i2c.h"
#include <stdio.h>

// Define I2C Address for DS3231 RTC (I2C 7-bit address)
#define DS3231_I2C_ADDR     0x68    // 7-bit I2C address (8-bit address would be 0xD0)
#define I2C_MASTER_FREQUENCY 100000u  // 100kHz I2C frequency
#define I2C_SDA_PIN P0_0
#define I2C_SCL_PIN P0_1

// I2C Master object
cyhal_i2c_t i2c_master;
cy_rslt_t rslt;
// Function to initialize the I2C communication
void i2c_init(void) {
	 rslt = cyhal_i2c_init(&i2c_master, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
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
	      rslt = cyhal_i2c_configure(&i2c_master, &master_config);
	      if (rslt != CY_RSLT_SUCCESS)
	      {
	          printf("I2C Master configuration failed, rslt: %d\n", rslt);
	          return;
	      }
}

// Convert BCD (Binary Coded Decimal) to decimal format
uint8_t bcd_to_decimal(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// Function to read time from the RTC
void get_rtc_time(void) {
    uint8_t time_data[7];  // Array to hold the date and time from the RTC

    // Read 7 bytes of data from the DS3231 RTC
    // The first byte is the seconds register, and the last byte is the year.
    cy_rslt_t result = cyhal_i2c_master_read(&i2c_master, DS3231_I2C_ADDR, time_data, sizeof(time_data),1000, false);

    if (result == CY_RSLT_SUCCESS) {
        // Convert the BCD values to decimal for easier reading
        uint8_t seconds = bcd_to_decimal(time_data[0] & 0x7F);  // Mask the 7th bit for seconds
        uint8_t minutes = bcd_to_decimal(time_data[1]);
        uint8_t hours = bcd_to_decimal(time_data[2] & 0x3F);    // Mask the 6th bit for hours (24-hour format)
        uint8_t day = bcd_to_decimal(time_data[4]);
        uint8_t month = bcd_to_decimal(time_data[5]);
        uint8_t year = bcd_to_decimal(time_data[6]);

        // Display the time (format: HH:MM:SS DD/MM/YYYY)
        printf("RTC Time: %02d:%02d:%02d %02d/%02d/%02d\n", hours, minutes, seconds, day, month, year);
    } else {
        printf("Failed to read time from RTC\n");
    }
}

// Function to initialize UART for retargeting
void uart_init(void) {
    cy_rslt_t result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS) {
        printf("UART Initialization failed\n");
    }
}

// Main function
int main(void) {
    // Initialize the board peripherals (e.g., UART, LED, etc.)
    cy_rslt_t rslt = cybsp_init();
    if (rslt != CY_RSLT_SUCCESS) {
        printf("Board initialization failed\n");
        return -1;
    }

    // Initialize UART for retargeting printf
    uart_init();

    printf("\x1b[2J\x1b[;H");
    printf("****************** I2C RTC Read Example ******************\r\n\n");

    // Initialize I2C communication with the RTC
    i2c_init();

    // Get and print the current time from the RTC
    get_rtc_time();

    return 0;
}
