#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <stdio.h>

// Timer object used
cyhal_timer_t timer_obj;
uint32_t read_val;
const cyhal_timer_cfg_t timer_cfg =
{
    .compare_value = 0,                  // Timer compare value, not used
    .period        = 2000000,              // Timer period set to a large enough value
    .direction     = CYHAL_TIMER_DIR_UP, // Timer counts up
    .is_compare    = false,              // Don't use compare mode
    .is_continuous = false,              // Do not run timer indefinitely
    .value         = 0                   // Initial value of counter
};

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
    printf("HAL: TIMERS using HAL\r\n");
    printf("-----------------------------------------------------------\r\n\n");

    // Initialize the timer object. Does not use pin output ('pin' is NC) and does not use a
    // pre-configured clock source ('clk' is NULL).
    result = cyhal_timer_init(&timer_obj, NC, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if initialization fails
    }

    // Apply timer configuration such as period, count direction, run mode, etc.
    cyhal_timer_configure(&timer_obj, &timer_cfg);

    // Set the frequency of the timer to 10000 counts in a second or 10000 Hz
    cyhal_timer_set_frequency(&timer_obj, 10000);

    // Start the timer with the configured settings
    cyhal_timer_start(&timer_obj);

    // Infinite loop to repeatedly print the timer value
    for (;;)
    {
        // Add a small delay to let the timer increment
        cyhal_system_delay_ms(500);  // Delay for 500 ms

        // Read the current timer value
        read_val = cyhal_timer_read(&timer_obj);

        // Print the current timer value
        printf("timer: %ld\r\n", read_val);
    }
}
