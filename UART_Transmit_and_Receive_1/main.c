#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include <stdio.h>

// Macro Definitions
#define DATA_BITS_8     8
#define STOP_BITS_1     1
#define BAUD_RATE       9600
#define UART_DELAY      10u
#define RX_BUF_SIZE     4
#define TX_BUF_SIZE     4

// Variable Declarations
cy_rslt_t    rslt;
cyhal_uart_t uart_obj;
uint32_t     actualbaud;
uint8_t      tx_buf[TX_BUF_SIZE] = { '1', '2', '3', '4' };
uint8_t      rx_buf[RX_BUF_SIZE];
size_t       tx_length = TX_BUF_SIZE;
size_t       rx_length = RX_BUF_SIZE;
uint32_t     value = 'A';

// UART Configuration structure
const cyhal_uart_cfg_t uart_config =
{
    .data_bits      = DATA_BITS_8,
    .stop_bits      = STOP_BITS_1,
    .parity         = CYHAL_UART_PARITY_NONE,
    .rx_buffer      = rx_buf,
    .rx_buffer_size = RX_BUF_SIZE
};

// Function Prototypes
void check_status(char *message, cy_rslt_t status);

int main(void)
{
    cy_rslt_t result;

    // Initialize the device and board peripherals
    result = cybsp_init();
    check_status("Board initialization failed", result);

    // Enable global interrupts
    __enable_irq();

    // Initialize the UART Block
    rslt = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
    check_status("UART initialization failed", rslt);

    // Set the baud rate
    rslt = cyhal_uart_set_baud(&uart_obj, BAUD_RATE, &actualbaud);
    check_status("Setting baud rate failed", rslt);

    // Print initial status to the console
    printf("\x1b[2J\x1b[;H");  // Clear the screen (ANSI escape code)
    printf("UART Test Program\n");
    printf("Baud Rate: %ld\n", actualbaud);

    // Begin Tx Transfer
    rslt = cyhal_uart_write(&uart_obj, (void*)tx_buf, tx_length);
    check_status("UART write failed", rslt);
    cyhal_system_delay_ms(UART_DELAY); // Wait for a moment to ensure data is transmitted

    // Send a Character
    rslt = cyhal_uart_putc(&uart_obj, value);
    check_status("UART putc failed", rslt);
    cyhal_system_delay_ms(UART_DELAY); // Wait for a moment to ensure data is transmitted

    // Begin Rx Transfer
    rslt = cyhal_uart_read(&uart_obj, (void*)rx_buf, rx_length);
    check_status("UART read failed", rslt);
    cyhal_system_delay_ms(UART_DELAY); // Wait for a moment to ensure data is received

    // Print received data to console
    printf("Received Data: ");
    for (size_t i = 0; i < rx_length; i++) {
        printf("%c", rx_buf[i]);
    }
    printf("\n");

    // Main loop
    for (;;) {
        // The system will continue to wait and handle UART operations.
        // Additional code or logic could be implemented here to manage UART communication continuously.
    }
}

// Function to check the status of peripheral initialization
void check_status(char *message, cy_rslt_t status)
{
    if (CY_RSLT_SUCCESS != status)
    {
        printf("\r\n=====================================================\r\n");
        printf("FAIL: %s\r\n", message);
        printf("\r\n=====================================================\r\n");
        while (true); // Infinite loop to halt execution
    }
}

//
//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include <string.h>
//
//// UART Configuration
//#define DATA_BITS_8     8
//#define STOP_BITS_1     1
//#define BAUD_RATE       115200
//#define UART_DELAY      10u
//#define RX_BUF_SIZE     64
//#define TX_BUF_SIZE     64
//
//// Status message buffers
//const uint8_t init_msg[] = "UART Initialized\r\n";
//const uint8_t tx_msg[] = "Sent: ";
//const uint8_t rx_msg[] = "Received: ";
//const uint8_t err_msg[] = "Error!\r\n";
//const uint8_t echo_msg[] = "Echo: ";
//const uint8_t newline[] = "\r\n";
//
//void uart_send_string(cyhal_uart_t *uart, const uint8_t *str)
//{
//    size_t len = strlen((const char*)str);
//    cyhal_uart_write(uart, (void*)str, &len);
//}
//
//void uart_send_hex(cyhal_uart_t *uart, uint8_t value)
//{
//    uint8_t hex_buf[3];
//    const uint8_t hex_chars[] = "0123456789ABCDEF";
//
//    hex_buf[0] = hex_chars[(value >> 4) & 0x0F];
//    hex_buf[1] = hex_chars[value & 0x0F];
//    hex_buf[2] = ' ';
//
//    size_t len = 3;
//    cyhal_uart_write(uart, hex_buf, &len);
//}
//
//int main(void)
//{
//    // Variable Declarations
//    cy_rslt_t    rslt;
//    cyhal_uart_t uart_obj;
//    uint32_t     actualbaud;
//    uint8_t      tx_buf[TX_BUF_SIZE] = {'1', '2', '3', '4'};
//    uint8_t      rx_buf[RX_BUF_SIZE] = {0};
//    size_t       tx_length = 4; // Send first 4 bytes initially
//    size_t       rx_length = RX_BUF_SIZE;
//
//    // Initialize board peripherals
//    rslt = cybsp_init();
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);
//    }
//
//    // Enable global interrupts
//    __enable_irq();
//
//    // Initialize the UART configuration structure
//    const cyhal_uart_cfg_t uart_config =
//    {
//        .data_bits      = DATA_BITS_8,
//        .stop_bits      = STOP_BITS_1,
//        .parity         = CYHAL_UART_PARITY_NONE,
//        .rx_buffer      = rx_buf,
//        .rx_buffer_size = RX_BUF_SIZE
//    };
//
//    // Initialize the UART Block
//    rslt = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        while(1); // Hang on error
//    }
//
//    // Set the baud rate
//    rslt = cyhal_uart_set_baud(&uart_obj, BAUD_RATE, &actualbaud);
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        while(1); // Hang on error
//    }
//
//    // Send initialization message
//    uart_send_string(&uart_obj, init_msg);
//
//    // Main communication loop
//    for (;;)
//    {
//        // Transmit data
//        rslt = cyhal_uart_write(&uart_obj, (void*)tx_buf, &tx_length);
//        if (rslt == CY_RSLT_SUCCESS)
//        {
//            uart_send_string(&uart_obj, tx_msg);
//            for (int i = 0; i < tx_length; i++)
//            {
//                uart_send_hex(&uart_obj, tx_buf[i]);
//            }
//            uart_send_string(&uart_obj, newline);
//        }
//        else
//        {
//            uart_send_string(&uart_obj, err_msg);
//        }
//        cyhal_system_delay_ms(UART_DELAY);
//
//        // Receive data (wait until data is available)
//        while (cyhal_uart_readable(&uart_obj) == 0)
//        {
//            cyhal_system_delay_ms(1);
//        }
//
//        rx_length = RX_BUF_SIZE;
//        rslt = cyhal_uart_read(&uart_obj, (void*)rx_buf, &rx_length);
//        if (rslt == CY_RSLT_SUCCESS && rx_length > 0)
//        {
//            uart_send_string(&uart_obj, rx_msg);
//            for (int i = 0; i < rx_length; i++)
//            {
//                uart_send_hex(&uart_obj, rx_buf[i]);
//            }
//            uart_send_string(&uart_obj, newline);
//
//            // Echo back received data
//            uart_send_string(&uart_obj, echo_msg);
//            size_t echo_len = rx_length;
//            cyhal_uart_write(&uart_obj, (void*)rx_buf, &echo_len);
//            uart_send_string(&uart_obj, newline);
//        }
//        else
//        {
//            uart_send_string(&uart_obj, err_msg);
//        }
//
//        cyhal_system_delay_ms(1000); // Delay between cycles
//    }
//}
