#include "cyhal.h"
#include "cybsp.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_port.h"

cyhal_spi_t mSPI;

// Define the SPI and GPIO pins
#define SSD1306_DC_Pin          CYBSP_D9
#define SSD1306_RST_Pin         CYBSP_TRACE_CLK
#define SSD1306_CS_Pin          CYBSP_SPI_CS

// Function to initialize SPI
void SPI_Init(void)
{
    // Configure SPI (MOSI, MISO, SCK, and optionally CS)
    cyhal_spi_init(&mSPI, CYBSP_SPI_MOSI, CYBSP_SPI_MISO, CYBSP_SPI_CLK, CYBSP_SPI_CS, NULL,
            8, CYHAL_SPI_MODE_00_MSB, false);

    // Set SPI frequency to 1 MHz (adjust as necessary)
    cyhal_spi_set_frequency(&mSPI, 1000000);
}

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


// Main program
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

    SPI_Init();      // Initialize SPI
    GPIO_Init();     // Initialize GPIO

    // Initialize the SSD1306 OLED display
    ssd1306_Init();

    // Clear the display buffer
    ssd1306_Fill(Black);

    // Set the cursor position and write some text
    ssd1306_SetCursor(40, 40);  // Set cursor to top-left corner
    ssd1306_WriteString("Hello, OLED!", Font_6x8, White); // Use a smaller font like Font_6x8
    ssd1306_SetCursor(0, 0);
    ssd1306_FillRectangle(0,30,30,5,White);

    // Update the screen with the buffer content
    ssd1306_UpdateScreen();

    // Main loop
    while (1)
    {
        // Add any other logic you want to perform in the main loop
    }
}


