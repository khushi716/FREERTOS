#include "cybsp.h"
#include "cyhal.h"
#include "cy_result.h"
#include "cyhal_i2c.h"
#include "mtb_ssd1306.h"
#include <string.h>

// Define the I2C address of the OLED (SSD1306)
#define OLED_I2C_ADDRESS 0x3C  // Common address for 128x64 OLED display

// Define control bytes for I2C communication
#define OLED_CONTROL_BYTE_CMD  0x00
#define OLED_CONTROL_BYTE_DATA 0x40

// Initialize the I2C interface
cyhal_i2c_t i2c_obj;

void send_oled_command(uint8_t command)
{
    uint8_t data[2] = { OLED_CONTROL_BYTE_CMD, command };
    cy_rslt_t result = cyhal_i2c_master_write(&i2c_obj, OLED_I2C_ADDRESS, data, sizeof(data), 0, true);
    CY_ASSERT(result == CY_RSLT_SUCCESS);
}

void send_oled_data(uint8_t data)
{
    uint8_t buffer[2] = { OLED_CONTROL_BYTE_DATA, data };
    cy_rslt_t result = cyhal_i2c_master_write(&i2c_obj, OLED_I2C_ADDRESS, buffer, sizeof(buffer), 0, true);
    CY_ASSERT(result == CY_RSLT_SUCCESS);
}

void oled_init(void)
{
    // Send the initialization sequence to the OLED
    send_oled_command(0xAE);  // Display OFF (sleep mode)
    send_oled_command(0xD5);  // Set display clock divide ratio
    send_oled_command(0x80);  // Suggested value for 100 Hz frequency
    send_oled_command(0xA8);  // Set multiplex ratio
    send_oled_command(0x3F);  // 1/64 duty cycle
    send_oled_command(0xD3);  // Set display offset
    send_oled_command(0x00);  // No offset
    send_oled_command(0x40);  // Set start line
    send_oled_command(0x8D);  // Enable charge pump
    send_oled_command(0x14);  // Enable charge pump during display ON
    send_oled_command(0xAF);  // Display ON (wake up)
}

void oled_clear(void)
{
    for (uint8_t page = 0; page < 8; page++) {
        send_oled_command(0xB0 + page); // Set page address
        send_oled_command(0x00); // Set column start address low
        send_oled_command(0x10); // Set column start address high

        for (uint8_t col = 0; col < 128; col++) {
            send_oled_data(0x00); // Clear each pixel
        }
    }
}

void oled_draw_text(const char* text)
{
    while (*text) {
        // Assuming you have a font to convert the character into pixels
        // This is a placeholder function that would need a font to convert characters
        send_oled_data(*text);
        text++;
    }
}

int main(void)
{
    cy_rslt_t result;

    // Initialize the device and board peripherals
    result = cybsp_init();
    if (CY_RSLT_SUCCESS != result) {
        CY_ASSERT(0);
    }

    // Initialize the I2C interface on appropriate pins (e.g., SDA, SCL)
    result = cyhal_i2c_init(&i2c_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    // Initialize the OLED
    oled_init();

    // Clear the OLED display
    oled_clear();

    // Display some text (You'd need a font library to render text)
    oled_draw_text("Hello OLED!");

    // Main loop (here you can add other functionality)
    for(;;) {
        // Add more functionality or refresh screen if needed
    }
}
