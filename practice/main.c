#include "cyhal.h"
#include "cybsp.h"

#define BUTTON_PIN    CYBSP_USER_BTN    // Replace with the actual pin for the button
#define LED_PIN       CYBSP_USER_LED    // Replace with the actual pin for the LED

int main(void)
{
    cy_rslt_t result;
    bool button_state;
    bool case_state = false;

    // Initialize the device and board peripherals
    result = cybsp_init();

    if (result != CY_RSLT_SUCCESS) {
        CY_ASSERT(0);  // Initialization failed, stop the program
    }

    // Initialize the user button pin as input with pull-up resistor
    result = cyhal_gpio_init(BUTTON_PIN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

    if (result != CY_RSLT_SUCCESS) {
        CY_ASSERT(0);  // Button initialization failed, stop the program
    }

    // Initialize the user LED pin as output
    result = cyhal_gpio_init(LED_PIN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    if (result != CY_RSLT_SUCCESS) {
        CY_ASSERT(0);  // LED initialization failed, stop the program
    }

    // Enable global interrupts
    __enable_irq();

    while (1)
    {
        // Read the state of the button
        button_state = cyhal_gpio_read(BUTTON_PIN);

        // If the button is pressed (state LOW due to pull-up configuration) and LED is off
        if (button_state == false && case_state == false) {
            // Turn the LED on
            cyhal_gpio_write(LED_PIN, CYBSP_LED_STATE_ON);
            case_state = true;

            // Add a small delay to debounce the button
            cyhal_system_delay_ms(200);  // 200 ms debounce delay
        }
        // If the button is pressed (state LOW) and LED is on
        else if (button_state == false && case_state == true) {
            // Turn the LED off
            cyhal_gpio_write(LED_PIN, CYBSP_LED_STATE_OFF);
            case_state = false;

            // Add a small delay to debounce the button
            cyhal_system_delay_ms(200);  // 200 ms debounce delay
        }
    }
}



