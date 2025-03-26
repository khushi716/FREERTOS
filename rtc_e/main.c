//
//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//#include "cyhal_i2c.h"
//#include <stdio.h>
//#include <time.h>
//
//#define I2C_MASTER_FREQUENCY 100000u  // 100kHz I2C frequency
//#define I2C_SLAVE_ADDR        0x68u    // DS3231 RTC I2C address
//#define TM_YEAR_BASE 1900        // Base year for tm structure
//
//// Declare I2C master object
//cy_rslt_t rslt;
//cyhal_i2c_t i2c_master_obj;
//uint8_t tx_buf[7];  // Buffer for setting date and time (7 bytes for sec, min, hour, etc.)
//uint8_t rx_buf[7];  // Buffer for reading date and time
//cy_rslt_t   result;
//cyhal_rtc_t my_rtc;
//// RTC data structure
//struct tm current_date_time = {0};
//
//// Function to convert BCD to binary
//uint8_t bcd_to_bin(uint8_t bcd) {
//    return ((bcd / 16) * 10) + (bcd % 16);
//}
//
//// Function to convert binary to BCD
//uint8_t bin_to_bcd(uint8_t bin) {
//    return ((bin / 10) * 16) + (bin % 10);
//}
//
//// Function to initialize I2C master
//void i2c_master_init(void)
//{
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
//// Function to read the current time from the RTC
//void rtc_read_time(void)
//{
//    // Send the address of the seconds register to start reading (0x00 for DS3231)
//    rslt = cyhal_i2c_master_write(&i2c_master_obj, I2C_SLAVE_ADDR, tx_buf, 1, 1000, NULL);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("Failed to set RTC register address, result: %d\n", rslt);
//        return;
//    }
//
//    // Read 7 bytes of data (seconds, minutes, hours, day, date, month, year)
//    rslt = cyhal_i2c_master_read(&i2c_master_obj, I2C_SLAVE_ADDR, rx_buf, 7, 1000, NULL);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("Failed to read data from RTC, result: %d\n", rslt);
//        return;
//    }
//
//    // Convert the RTC data to tm structure
//    current_date_time.tm_sec = bcd_to_bin(rx_buf[0]);
//    current_date_time.tm_min = bcd_to_bin(rx_buf[1]);
//    current_date_time.tm_hour = bcd_to_bin(rx_buf[2]);
//    current_date_time.tm_wday = bcd_to_bin(rx_buf[3]);
//    current_date_time.tm_mday = bcd_to_bin(rx_buf[4]);
//    current_date_time.tm_mon = bcd_to_bin(rx_buf[5]) - 1;  // DS3231 month is 1-indexed, so subtract 1
//    current_date_time.tm_year = bcd_to_bin(rx_buf[6]) + 2000;  // DS3231 year is 2 digits, add 2000
//
//    // Print the current time and date in DD/MM/YYYY format
//    printf("Current date from RTC: %02d/%02d/%04d %02d:%02d:%02d\n",
//           current_date_time.tm_mday,
//           current_date_time.tm_mon + 1,  // Adding 1 back to the month for correct output
//           current_date_time.tm_year,
//           current_date_time.tm_hour,
//           current_date_time.tm_min,
//           current_date_time.tm_sec);
//}
//
//// Main function
//int main(void)
//{
//    // Initialize the board (e.g., LED, button, etc.)
//    rslt = cybsp_init();
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("Board initialization failed, result: %d\n", rslt);
//        return -1;
//    }
//
//    // Initialize UART for retargeting (for debugging purposes)
//    rslt = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("UART initialization failed, result: %d\n", rslt);
//        return -1;
//    }
//
//    // Initialize I2C master
//    i2c_master_init();
//
//    // Continuously read and print the current date and time from the RTC
//    while (1)
//    {
//        // Read and print the current date and time
//        rtc_read_time();
//
//        // Delay for 1 second before the next update
//        cyhal_system_delay_ms(1000);
//    }
//
//    return 0;
//}
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cyhal_i2c.h"
#include <stdio.h>
#include <time.h>

#define I2C_MASTER_FREQUENCY 100000u  // 100kHz I2C frequency
#define I2C_SLAVE_ADDR        0x68u    // DS3231 RTC I2C address
#define TM_YEAR_BASE 1900        // Base year for tm structure

// Declare I2C master object
cy_rslt_t rslt;
cyhal_i2c_t i2c_master_obj;
uint8_t tx_buf[7];  // Buffer for setting date and time (7 bytes for sec, min, hour, etc.)
uint8_t rx_buf[7];  // Buffer for reading date and time

// Function to convert BCD to binary
uint8_t bcd_to_bin(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// Function to convert binary to BCD
uint8_t bin_to_bcd(uint8_t bin) {
    return ((bin / 10) * 16) + (bin % 10);
}

// Function to initialize I2C master
void i2c_master_init(void)
{
    rslt = cyhal_i2c_init(&i2c_master_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
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
    rslt = cyhal_i2c_configure(&i2c_master_obj, &master_config);
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("I2C Master configuration failed, rslt: %d\n", rslt);
        return;
    }
}

  struct tm new_date_time = {
      .tm_sec = 10,
      .tm_min = 10,
      .tm_hour = 8,
      .tm_mday = 23,
      .tm_mon = 4,
      .tm_year = 25,
      .tm_wday = 4,       // Wednesday (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
      .tm_isdst = 0
  };

// Function to set the current time to the RTC
void rtc_set_time(void)
{

    // Convert tm structure to DS3231 format
    tx_buf[0] = bin_to_bcd(new_date_time.tm_sec);
    tx_buf[1] = bin_to_bcd(new_date_time.tm_min);
    tx_buf[2] = bin_to_bcd(new_date_time.tm_hour);
    tx_buf[3] = bin_to_bcd(new_date_time.tm_wday);  // Day of the week
    tx_buf[4] = bin_to_bcd(new_date_time.tm_mday);
    tx_buf[5] = bin_to_bcd(new_date_time.tm_mon);  // DS3231 uses 1-indexed months
    tx_buf[6] = bin_to_bcd(new_date_time.tm_year );  // Only 2 digits for the year


    printf("Setting RTC time: ");
    for (int i = 0; i < 7; i++) {
        printf("%02X ", tx_buf[i]);
    }
    printf("\n");

    // Write the time to the RTC (7 bytes)
    rslt = cyhal_i2c_master_write(&i2c_master_obj, I2C_SLAVE_ADDR, tx_buf, 7, 1000, NULL);
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("Failed to write data to RTC, result: %d\n", rslt);
    }
    else
    {
        printf("Successfully set time to RTC\n\n");
    }
}

// Function to read the current time from the RTC
void rtc_read_time(void)
{

    uint8_t reg_addr = 0x00;  // Register address for seconds
    rslt = cyhal_i2c_master_write(&i2c_master_obj, I2C_SLAVE_ADDR, &reg_addr, 1, 1000, NULL);
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("Failed to set RTC register address, result: %d\n", rslt);
        return;
    }

    // Read 7 bytes of data (seconds, minutes, hours, day, date, month, year)
    rslt = cyhal_i2c_master_read(&i2c_master_obj, I2C_SLAVE_ADDR, rx_buf, 7, 1000, NULL);
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("Failed to read data from RTC, result: %d\n", rslt);
        return;
    }

    // Convert the RTC data to tm structure
    struct tm current_date_time = {0};
    current_date_time.tm_sec = bcd_to_bin(rx_buf[0]);
    current_date_time.tm_min = bcd_to_bin(rx_buf[1]);
    current_date_time.tm_hour = bcd_to_bin(rx_buf[2]);
    current_date_time.tm_wday = bcd_to_bin(rx_buf[3]);
    current_date_time.tm_mday = bcd_to_bin(rx_buf[4]);
    current_date_time.tm_mon = bcd_to_bin(rx_buf[5]) - 1;  // DS3231 month is 1-indexed
    current_date_time.tm_year = bcd_to_bin(rx_buf[6]) + 2000;  // Correct year calculation

    // Debug output: Check the raw data received from RTC
    printf(" RTC data received: ");
    for (int i = 0; i < 7; i++) {
        printf("%02X ", rx_buf[i]);
    }
    printf("\n");


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

    // Initialize I2C master
    i2c_master_init();

    // Set the time to the RTC (write to RTC)
    rtc_set_time();
    // Read and print the RTC time
    rtc_read_time();

    while (1)
    {
    }

    return 0;
}
