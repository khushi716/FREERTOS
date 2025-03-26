//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//
///* PWM Duty-cycle (0 to 100 percent) */
//float PWM_DUTY_CYCLE =0; // Default starting value
//cy_rslt_t rslt;
//cyhal_pwm_t pwm_obj;
//
///* Main Function */
//int main(void)
//{
//    cy_rslt_t result;
//    char input_str[10];
//    uint32_t duty_cycle_input;
//
//    /* Initialize the device and board peripherals */
//    result = cybsp_init();
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Halt if initialization fails
//    }
//
//    /* Enable global interrupts */
//    __enable_irq();
//
//    /* Initialize retarget-io to use the debug UART port */
//    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Halt if UART initialization fails
//    }
//
//    /* Print initialization message */
//    printf("\x1b[2J\x1b[;H");
//    printf("-----------------------------------------------------------\r\n");
//    printf("HAL: PWM using HAL - Adjust LED Brightness\r\n");
//    printf("-----------------------------------------------------------\r\n\n");
//
//    // Initialize PWM on the supplied pin and assign a new clock
//    rslt = cyhal_pwm_init(&pwm_obj, CYBSP_USER_LED, NULL);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Halt if PWM initialization fails
//    }
//
//    // Start the PWM output
//    rslt = cyhal_pwm_start(&pwm_obj);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Halt if PWM start fails
//    }
//
//
//    // Main loop where user can input the duty cycle value
//    for (;;)
//    {
//
//        // Read input from UART (assuming input is a single number)
//        fgets(input_str, sizeof(input_str), stdin);
//        sscanf(input_str, "%lu", &duty_cycle_input);
//        // Validate the input and ensure it is within 0-100 range
//        if (duty_cycle_input > 100)
//        {
//            printf("Invalid input! Please enter a number between 0 and 100.\r\n");
//            continue;
//        }
//
//        PWM_DUTY_CYCLE = (float)duty_cycle_input; // Set PWM duty cycle
//
//        // Print the current PWM duty cycle value
//        printf("Setting PWM Duty Cycle to: %.2f%%\r\n", PWM_DUTY_CYCLE);
//
//        // Update the PWM duty cycle
//        rslt = cyhal_pwm_set_duty_cycle(&pwm_obj, PWM_DUTY_CYCLE, 100);
//        if (rslt != CY_RSLT_SUCCESS)
//        {
//            CY_ASSERT(0);  // Halt if setting the duty cycle fails
//        }
//
//        // Add a delay to give user time to adjust PWM value
//        cyhal_system_delay_ms(500);
//    }
//}


#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "cy_sysint.h"

#define BAUD_RATE       115200
#define UART_DELAY      10u
#define RX_BUF_SIZE     64  // Size of the RX buffer (enough to hold the command string)
#define TX_BUF_SIZE     64  // Size of the TX buffer for sending responses


cy_rslt_t    rslt;
cyhal_uart_t uart_obj;
uint32_t     actualbaud;
uint8_t      rx_buf[RX_BUF_SIZE];  // Buffer to store received data (user input)
uint8_t      tx_buf[TX_BUF_SIZE];  // Buffer to send responses
size_t       rx_index = 0;         // Track the current index in rx_buf for received data
float PWM_DUTY_CYCLE = 0.0;
cyhal_pwm_t pwm_obj;

// Initialize the UART configuration structure
const cyhal_uart_cfg_t uart_config =
{
    .data_bits      = 8,
    .stop_bits      = 1,
    .parity         = CYHAL_UART_PARITY_NONE,
    .rx_buffer      = NULL,
    .rx_buffer_size = 0
};


void uart_write(const char *data)
{
    while (*data != '\0') {
        cyhal_uart_putc(&uart_obj, *data);
        data++;
    }
}


int uart_read(uint8_t *buffer, size_t max_len)
{
    size_t index = 0;
    uint8_t c;

    while (index < max_len) {
        cy_rslt_t result = cyhal_uart_getc(&uart_obj, &c, 100);
        if (result == CY_RSLT_SUCCESS) {
            if (c == '\r' || c == '\n') {
                buffer[index] = '\0';  // Null-terminate the string
                return index;
            }
            buffer[index++] = c;
        }
    }
    return index;
}

int main(void)
{
    cy_rslt_t result;

    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS) {
        CY_ASSERT(0);
    }

    __enable_irq();  // Enable global interrupts

    // Initialize the UART Block
    rslt = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
    if (rslt != CY_RSLT_SUCCESS) {
        uart_write("UART initialization failed\n");
        return -1;
    }

    // Set the baud rate
    rslt = cyhal_uart_set_baud(&uart_obj, BAUD_RATE, &actualbaud);
    if (rslt != CY_RSLT_SUCCESS) {
        uart_write("Failed to set UART baud rate\n");
        return -1;
    }

    // Initialize PWM on the supplied pin and assign a new clock
    rslt = cyhal_pwm_init(&pwm_obj, CYBSP_USER_LED, NULL);
    if (rslt != CY_RSLT_SUCCESS) {
        CY_ASSERT(0);  // Halt if PWM initialization fails
    }

    // Send the initial prompt using uart_write
    uart_write("Enter a value:\r\n");

    while (1) {
        uint8_t c;
        int duty_cycle_input = 0;
        uart_read(rx_buf, RX_BUF_SIZE);  // Read input from UART


            // Convert the received string to an integer
            if (sscanf((char*)rx_buf, "%d", &duty_cycle_input) == 1) {
                if (duty_cycle_input >= 0 && duty_cycle_input <= 100) {
                    PWM_DUTY_CYCLE = (float)duty_cycle_input;
                    uart_write("PWM Duty Cycle: ");
                    sprintf((char*)tx_buf, "%f%%\r\n", PWM_DUTY_CYCLE);
                    uart_write((char*)tx_buf);

                    // Set the PWM duty cycle
                    rslt = cyhal_pwm_set_duty_cycle(&pwm_obj, PWM_DUTY_CYCLE, 100);
                    if (rslt != CY_RSLT_SUCCESS) {
                        CY_ASSERT(0);
                    }

                    // Start the PWM output
                    rslt = cyhal_pwm_start(&pwm_obj);
                    if (rslt != CY_RSLT_SUCCESS) {
                        CY_ASSERT(0);
                    }
                } else {
                    uart_write("Invalid input! Please enter a value between 0 and 100.\r\n");
                }

        }

        cyhal_system_delay_ms(UART_DELAY);
    }
}

