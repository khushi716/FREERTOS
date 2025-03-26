#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ds1307.h"  // DS1307 RTC driver

/* SPI Pin Configuration */
#define SSD1306_DC_Pin          CYBSP_D9
#define SSD1306_RST_Pin         CYBSP_TRACE_CLK
#define SSD1306_CS_Pin          CYBSP_SPI_CS

// Function to initialize GPIO pins for DC, Reset, and CS
void GPIO_Init(void)
{
    // Initialize DC pin as output
    cyhal_gpio_init(SSD1306_DC_Pin, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 0);

    // Initialize Reset pin as output
    cyhal_gpio_init(SSD1306_RST_Pin, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);

    // Initialize Chip Select pin as output (Active Low)
    cyhal_gpio_init(SSD1306_CS_Pin, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);
}

/* SPI Objects */
cyhal_spi_t mSPI;

void spi_init(void);
void i2c_init(void);
void rtc_oled(void);

/* I2C Objects for DS1307 RTC */
cyhal_i2c_t i2c_master_obj; // I2C master object for DS1307

/* Main Function */
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if initialization fails
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if UART initialization fails
    }

    /* Print initialization message */
    printf("\x1b[2J\x1b[;H");
    printf("-----------------------------------------------------------\r\n");
    printf("Using I2C for RTC and SPI for OLED\r\n");
    printf("-----------------------------------------------------------\r\n\n");

    /* Initialize SPI for OLED communication */
    spi_init();

    /* Initialize I2C for RTC communication */
    i2c_init();

    GPIO_Init();     // Initialize GPIO for OLED

    ssd1306_Init();  // Initialize OLED display

    for (;;)
    {
        rtc_oled(); // Call the function to update the date and time on the OLED
        cyhal_system_delay_ms(1000);  // Delay for 1 second before updating the time again
    }
}

/* Function to display the current date and time on the OLED display */
void rtc_oled(void)
{
//    // Days of the week array
    const char *DAYS_OF_WEEK[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

    // Initialize the DS1307 RTC (ensure your DS1307 driver is set up correctly)
    DS1307_Init();


    // Retrieve current time and date from DS1307
    uint8_t date = DS1307_GetDate();
    uint8_t month = DS1307_GetMonth();
    uint16_t year = DS1307_GetYear();
    uint8_t dow = DS1307_GetDayOfWeek();
    uint8_t hour = DS1307_GetHour();
    uint8_t minute = DS1307_GetMinute();
    uint8_t second = DS1307_GetSecond();

    // Print time in ISO8601 format


    // Clear the screen
    ssd1306_Fill(Black);

    // Set cursor to the first line (for day of the week)
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString(DAYS_OF_WEEK[dow - 1], Font_6x8, White);

    // Set cursor to the second line (for date)
    ssd1306_SetCursor(0, 16);
    char date_str[30];
    sprintf(date_str, "%02d/%02d/%04d", month, date, year);  // Format as MM/DD/YYYY
    ssd1306_WriteString(date_str, Font_6x8, White);

    // Set cursor to the third line (for time)
    ssd1306_SetCursor(0, 32);
    char time_str[30];
    sprintf(time_str, "%02d:%02d:%02d", hour, minute, second);  // Format as HH:MM:SS
    ssd1306_WriteString(time_str, Font_6x8, White);

    // Update the screen
    ssd1306_UpdateScreen();
}

/* SPI Initialization */
void spi_init(void)
{
    cy_rslt_t result;

    // Configure SPI (MOSI, MISO, SCK, and optionally CS)
    cyhal_spi_init(&mSPI, CYBSP_SPI_MOSI, CYBSP_SPI_MISO, CYBSP_SPI_CLK, CYBSP_SPI_CS, NULL,
            8, CYHAL_SPI_MODE_00_MSB, false);

    // Set SPI frequency to 1 MHz (adjust as necessary)
    cyhal_spi_set_frequency(&mSPI, 1000000);
}

/* I2C Initialization */
void i2c_init(void){
// I2C Master configuration


     cyhal_i2c_cfg_t i2c_master_config = {
       CYHAL_I2C_MODE_MASTER,  // Set I2C mode as master
       0,                      // Address is not used for master mode
       100000u                 // Set frequency for I2C communication (100kHz)
   };
         cy_rslt_t result;

	 // Initialize I2C master
	    result = cyhal_i2c_init(&i2c_master_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
	    result = cyhal_i2c_configure(&i2c_master_obj, &i2c_master_config);

	    if (result != CY_RSLT_SUCCESS)
	    {
	        CY_ASSERT(0);
	    }
}

