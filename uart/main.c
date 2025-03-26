//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_sysint.h"
//
//// Macro Definitions
//#define DATA_BITS_8     8
//#define STOP_BITS_1     1
//#define BAUD_RATE       115200
//#define UART_DELAY      10u
//#define RX_BUF_SIZE     4
//#define TX_BUF_SIZE     4
//
//// Variable Declarations
//cy_rslt_t    rslt;
//cyhal_uart_t uart_obj;
//uint32_t     actualbaud;
//uint8_t      tx_buf[TX_BUF_SIZE] = { '1', '2', '3', '4' };
//uint8_t      rx_buf[RX_BUF_SIZE];
//size_t       tx_length = TX_BUF_SIZE;
//size_t       rx_length = RX_BUF_SIZE;
//uint32_t     value     = 'A';
//
//// Initialize the UART configuration structure
//const cyhal_uart_cfg_t uart_config =
//{
//    .data_bits      = DATA_BITS_8,
//    .stop_bits      = STOP_BITS_1,
//    .parity         = CYHAL_UART_PARITY_NONE,
//    .rx_buffer      = rx_buf,
//    .rx_buffer_size = RX_BUF_SIZE
//};
//
//// Main Function
//int main(void)
//{
//    // Initialize the device and board
//    cybsp_init();
//    __enable_irq();  // Enable global interrupts
//
//    // Initialize the UART Block
//    rslt = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
//    if (rslt != CY_RSLT_SUCCESS) {
//        printf("UART initialization failed\n");
//        return -1;
//    }
//
//    // Set the baud rate
//    rslt = cyhal_uart_set_baud(&uart_obj, BAUD_RATE, &actualbaud);
//    if (rslt != CY_RSLT_SUCCESS) {
//        printf("Failed to set UART baud rate\n");
//        return -1;
//    }
//
//    while(1)
//    {
//        // Begin Tx Transfer - Send data via UART (this will send the tx_buf array)
//        cyhal_uart_write(&uart_obj, (void*)tx_buf, &tx_length);
//        cyhal_system_delay_ms(UART_DELAY);  // Ensure a delay for the transmission to complete
//
//        // Send a single character via UART
//        cyhal_uart_putc(&uart_obj, value);  // This sends the 'A' character
//        cyhal_system_delay_ms(UART_DELAY);  // Ensure a delay for the transmission to complete
//
//        // Begin Rx Transfer - Read data via UART into rx_buf
//        cyhal_uart_read(&uart_obj, (void*)rx_buf, &rx_length);
//        cyhal_system_delay_ms(UART_DELAY);  // Ensure a delay for the data to be received
//
//        // Print received data (this assumes your UART is connected to a terminal like Tera Term or similar)
//        printf("Received data: ");
//        for (size_t i = 0; i < rx_length; i++) {
//            printf("%c ", rx_buf[i]);  // This will print the received characters
//        }
//        printf("\n");
//
//        // Optionally, add a break condition here to stop the loop after a certain number of transmissions
//    }
//
//    return 0;
//}

#include "cyhal.h"
#include "cybsp.h"
#include "cy_sysint.h"
#include <string.h>

// Macro Definitions
#define DATA_BITS_8     8
#define STOP_BITS_1     1
#define BAUD_RATE       115200
#define UART_DELAY      10u
#define RX_BUF_SIZE     64  // Size of the RX buffer (enough to hold the command string)
#define TX_BUF_SIZE     64  // Size of the TX buffer for sending responses

// Variable Declarations
cy_rslt_t    rslt;
cyhal_uart_t uart_obj;
uint32_t     actualbaud;
uint8_t      rx_buf[RX_BUF_SIZE];  // Buffer to store received data (user input)
uint8_t      tx_buf[TX_BUF_SIZE];  // Buffer to send responses
size_t       rx_index = 0;         // Track the current index in rx_buf for received data

// Initialize the UART configuration structure
const cyhal_uart_cfg_t uart_config =
{
    .data_bits      = DATA_BITS_8,
    .stop_bits      = STOP_BITS_1,
    .parity         = CYHAL_UART_PARITY_NONE,
    .rx_buffer      = NULL,  // Not using a buffer, we're using cyhal_uart_getc()
    .rx_buffer_size = 0      // Not needed when using cyhal_uart_getc()
};

// Function to process received command and send appropriate response
void process_command(const char* command)
{
    if (strcmp(command, "cmd1") == 0)
    {
        strcpy((char*)tx_buf, "Response: Command 1 Executed\r\n");

    }
    else if (strcmp(command, "cmd2") == 0)
    {
        strcpy((char*)tx_buf, "Response: Command 2 Executed\r\n");
    }
    else if (strcmp(command, "cmd3") == 0)
    {
        strcpy((char*)tx_buf, "Response: Command 3 Executed\r\n");
    }
    else if (strcmp(command, "cmd4") == 0)
    {
        strcpy((char*)tx_buf, "Response: Command 4 Executed\r\n");
    }
    else if (strcmp(command, "cmd5") == 0)
    {
        strcpy((char*)tx_buf, "Response: Command 5 Executed\r\n");
    }
    else{
    	 strcpy((char*)tx_buf, "Response:incorrect command\r\n");
    }

    // Send the response back to Tera Term using cyhal_uart_putc
    for (size_t i = 0; i < strlen((char*)tx_buf); i++)
    {
        cyhal_uart_putc(&uart_obj, tx_buf[i]);
    }
}

int main(void)
{
    // Initialize the device and board
    cybsp_init();
    __enable_irq();  // Enable global interrupts

    // Initialize the UART Block
    rslt = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
    if (rslt != CY_RSLT_SUCCESS) {
        printf("UART initialization failed\n");
        return -1;
    }

    // Set the baud rate
    rslt = cyhal_uart_set_baud(&uart_obj, BAUD_RATE, &actualbaud);
    if (rslt != CY_RSLT_SUCCESS) {
        printf("Failed to set UART baud rate\n");
        return -1;
    }

    // Send the initial prompt using cyhal_uart_putc
    strcpy((char*)tx_buf, "Enter a command (cmd1, cmd2, cmd3, cmd4, cmd5):\r\n");
    for (size_t i = 0; i < strlen((char*)tx_buf); i++)
    {
        cyhal_uart_putc(&uart_obj, tx_buf[i]);
    }

    // Main loop to read and process user command
    while (1)
    {
        uint8_t c;

        // Use cyhal_uart_getc to receive the character with a timeout of 100ms (optional)
        cy_rslt_t result = cyhal_uart_getc(&uart_obj, &c, 100);

        if (result == CY_RSLT_SUCCESS)
        {
            // Add received character to rx_buf
            if (rx_index < RX_BUF_SIZE - 1)
            {
                rx_buf[rx_index++] = c;  // Store character
            }

            // Check for end of command (newline or carriage return)
            if (c == '\n' || c == '\r')
            {
                // Null-terminate the received command
                if (rx_index > 0 && (rx_buf[rx_index - 1] == '\n' || rx_buf[rx_index - 1] == '\r'))
                {
                    rx_buf[rx_index - 1] = '\0';  // Remove the newline or carriage return
                }

                // Process the command
                process_command((const char*)rx_buf);

                // Reset buffer and index for the next command
                rx_index = 0;
            }
        }

        // Small delay to allow UART transmission to complete
        cyhal_system_delay_ms(UART_DELAY);
    }

    return 0;
}

