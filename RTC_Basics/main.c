
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ds1307.h"  // Change to DS3231 RTC driver
cyhal_i2c_t i2c_master_obj;

int main(void)
{
    cy_rslt_t result;

    // Initialize the board (e.g., LED, button, etc.)
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    // Initialize UART for retargeting (for debugging purposes)
    result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                     CYBSP_DEBUG_UART_CTS, CYBSP_DEBUG_UART_RTS, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // I2C Master configuration
    cyhal_i2c_cfg_t i2c_master_config = {
        CYHAL_I2C_MODE_MASTER,  // Set I2C mode as master
        0,                      // Address is not used for master mode
        100000u                 // Set frequency for I2C communication (100kHz)
    };

    // Initialize I2C master
    result = cyhal_i2c_init(&i2c_master_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    result = cyhal_i2c_configure(&i2c_master_obj, &i2c_master_config);

    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // Days of the week array
    const char *DAYS_OF_WEEK[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

    // Initialize the DS3231 RTC (ensure your DS3231 driver is set up correctly)
    DS1307_Init();

    // Set time and date (example values)
    DS1307_SetTimeZone(+8, 00);   // Set timezone to GMT+8
    DS1307_SetDate(26);           // Set day of the month
    DS1307_SetMonth(3);          // Set month (February)
    DS1307_SetYear(2025);        // Set year (2025)
    DS1307_SetDayOfWeek(4);      // Set day of the week (Friday)
    DS1307_SetHour(7);          // Set hour (12 PM)
    DS1307_SetMinute(10);        // Set minute (01)
    DS1307_SetSecond(00);        // Set second (55)

    for (;;)
    {
        // Retrieve current time and date from DS3231
        uint8_t date = DS1307_GetDate();
        uint8_t month =DS1307_GetMonth();
        uint16_t year = DS1307_GetYear();
        uint8_t dow = DS1307_GetDayOfWeek();
        uint8_t hour = DS1307_GetHour();
        uint8_t minute = DS1307_GetMinute();
        uint8_t second = DS1307_GetSecond();
        int8_t zone_hr = DS1307_GetTimeZoneHour();
        uint8_t zone_min = DS1307_GetTimeZoneMin();

        // Print time in ISO8601 format
        printf("ISO8601 FORMAT: %04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d  %s\r\n",
                year, month, date, hour, minute, second, zone_hr, zone_min, DAYS_OF_WEEK[dow]);

        // Delay for 1 second before updating the time again
        cyhal_system_delay_ms(1000);
    }}
