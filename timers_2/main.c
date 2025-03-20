#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include <stdio.h>

// Timer object used
cyhal_timer_t timer_obj;
uint32_t val;
bool timer_interrupt_flag = false;
cy_rslt_t snippet_cyhal_timer_event_interrupt();

// Interrupt Service Routine (ISR) for the timer
static void isr_timer(void* callback_arg, cyhal_timer_event_t event)
{
    (void)callback_arg; // Not used
    (void)event;        // Not used

    // Set the interrupt flag to indicate the interrupt has occurred
    timer_interrupt_flag = true;
    printf("Timer reached terminal count and interrupt occurred!\n"); // Debugging message
}

cy_rslt_t snippet_cyhal_timer_event_interrupt()
{
    cy_rslt_t rslt;

    // Timer configuration
    const cyhal_timer_cfg_t timer_cfg =
    {
        .compare_value = 0,                  // Not using compare mode
        .period        = 9999,               // Timer period (9999 ticks)
        .direction     = CYHAL_TIMER_DIR_UP, // Timer counts up
        .is_compare    = false,              // No compare mode
        .is_continuous = true,               // Timer runs indefinitely
        .value         = 0                   // Initial value of counter
    };

    // Initialize the timer object (no pin and no clock source)
    rslt = cyhal_timer_init(&timer_obj, NC, NULL);

    // Apply the timer configuration
    if (CY_RSLT_SUCCESS == rslt)
    {
        rslt = cyhal_timer_configure(&timer_obj, &timer_cfg);
    }

    // Set the frequency of the timer to 10 kHz (100 µs per tick)
    if (CY_RSLT_SUCCESS == rslt)
    {
        rslt = cyhal_timer_set_frequency(&timer_obj, 10000);
    }


    cyhal_timer_register_callback(&timer_obj, isr_timer, NULL);

   cyhal_timer_enable_event(&timer_obj, CYHAL_TIMER_IRQ_TERMINAL_COUNT, 3, true);

    // Start the timer
    if (CY_RSLT_SUCCESS == rslt)
    {
        rslt = cyhal_timer_start(&timer_obj);
    }

    return rslt;
}

/* Main Function */
int main(void)
{
    cy_rslt_t result;

    // Initialize the device and board peripherals
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if initialization fails
    }

    // Enable global interrupts
    __enable_irq();

    // Initialize retarget-io to use the debug UART port
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if UART initialization fails
    }

    // Initialize the User LED GPIO
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if GPIO initialization fails
    }

    // Print initialization message
    printf("\x1b[2J\x1b[;H");
    printf("-----------------------------------------------------------\r\n");
    printf("HAL: TIMERS using HAL\r\n");
    printf("-----------------------------------------------------------\r\n\n");

    // Initialize the timer and configure the interrupt
    result = snippet_cyhal_timer_event_interrupt();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if timer initialization fails
    }

    // Main loop
    for (;;)
    {
        // If the interrupt flag is set, process it
        if (timer_interrupt_flag == true)
        {
            // Turn on the User LED to indicate interrupt occurred
            cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
            printf("Interrupt flag is set, LED ON\n");

            // Reset the interrupt flag after processing
            timer_interrupt_flag = false;
            cyhal_system_delay_ms(15);
        }
        else
        {
            // Turn off the User LED if the interrupt has not occurred
            cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
        }
    }
}
