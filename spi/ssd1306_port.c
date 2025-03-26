#include "ssd1306_port.h"
#include "cyhal.h"
extern cyhal_spi_t mSPI;
void deselectOLEDDisplay(void)
{
	cyhal_gpio_write( CYBSP_SPI_CS, true);//cs pin
}

void selectOLEDDisplay(void)
{
	cyhal_gpio_write( CYBSP_SPI_CS, false);//cs pin
}

void ssd1306_Reset(void)
{
	deselectOLEDDisplay();

	// Reset the OLED
	cyhal_gpio_write(CYBSP_TRACE_CLK, false);//reset pin
	cyhal_system_delay_ms(1);
	cyhal_gpio_write(CYBSP_TRACE_CLK, true);//reset pin
	cyhal_system_delay_ms(1);
}

/**
 * @brief: This use to send command to ssd1306 display
 * @param byte: Command to be send
 */
void ssd1306_WriteCommand(uint8_t byte)
{
	uint8_t receive_data;
	// Select ssd1306
	selectOLEDDisplay();
	cyhal_gpio_write( CYBSP_D9, false);//dc pin command
	// Set ssd1306 in command mode
	cyhal_spi_transfer(&mSPI, &byte ,  1u, &receive_data , 1u,0xFF);
	// De-select ssd1306
	deselectOLEDDisplay();

}

/**
 * @brief: This function use send data to ssd1306 display
 * @param buffer: Pointer to data
 * @param buff_size: Data size
 */
void ssd1306_WriteData(uint8_t* buffer, size_t buff_size)
{
	uint8_t receive_data;
	// Select ssd1306
	selectOLEDDisplay();
	cyhal_gpio_write( CYBSP_D9, true);//dc pin data
	 cyhal_spi_transfer(&mSPI,  buffer,  buff_size, &receive_data , 1u,0xFF);
	// De-select ssd1306
	deselectOLEDDisplay();
}




