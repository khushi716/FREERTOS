#include "uart.h"
#include "stdio.h"
#include "cybsp.h"

// Variable Declarations
extern cyhal_uart_t uart_obj;
uint32_t     actualbaud;
uint8_t      tx_buf[TX_BUF_SIZE] ;
uint8_t      rx_buf[RX_BUF_SIZE];
size_t       tx_length = TX_BUF_SIZE;
size_t       rx_length = RX_BUF_SIZE;
uint32_t     value ;

// Initialize the UART communication
cy_rslt_t uart_init(void)
{
    cy_rslt_t rslt;

    // Initialize the UART configuration structure
    const cyhal_uart_cfg_t uart_config =
    {
        .data_bits      = DATA_BITS_8,
        .stop_bits      = STOP_BITS_1,
        .parity         = CYHAL_UART_PARITY_NONE,
        .rx_buffer      = rx_buf,
        .rx_buffer_size = RX_BUF_SIZE
    };

    // Initialize the UART Block
    rslt = cyhal_uart_init(&uart_obj,CYBSP_DEBUG_UART_TX, CYBSP_D0, NC, NC, NULL, &uart_config);
    if (rslt != CY_RSLT_SUCCESS) {
        printf("UART initialization failed with error code: %ld\n", rslt);
        return rslt;
    }

    // Set the baud rate
    rslt = cyhal_uart_set_baud(&uart_obj, BAUD_RATE, &actualbaud);
    if (rslt != CY_RSLT_SUCCESS) {
        printf("Failed to set UART baud rate with error code: %ld\n", rslt);
        return rslt;
    }

    return CY_RSLT_SUCCESS;
}

// Function to send data over UART
uint32_t uart_transmit_packet(uint8_t *tx_buf, uint16_t datalength)
{
    cy_rslt_t rslt;
    size_t tx_length = datalength;

    // Begin Tx Transfer
    rslt = cyhal_uart_write(&uart_obj, (void*)tx_buf, &tx_length);
    if (rslt != CY_RSLT_SUCCESS) {
        printf("UART transmit error\n");
        return rslt;
    }

    cyhal_system_delay_ms(UART_DELAY);  // Optional delay between transfers
    return CY_RSLT_SUCCESS;
}

// Function to send a single character via UART
void uart_putc(uint8_t value)
{
    // Send a single character over UART
    cyhal_uart_putc(&uart_obj, value);
    cyhal_system_delay_ms(UART_DELAY);  // Optional delay
}

// Function to receive data over UART
uint32_t uart_receive_packet(uint8_t *rx_buf, uint16_t datalength)
{
    cy_rslt_t rslt;
    size_t rx_length = datalength;

    // Begin Rx Transfer
    rslt = cyhal_uart_read(&uart_obj, (void*)rx_buf, &rx_length);
    if (rslt != CY_RSLT_SUCCESS) {
        printf("UART receive error\n");
        return rslt;
    }

    cyhal_system_delay_ms(UART_DELAY);  // Optional delay between transfers
    return CY_RSLT_SUCCESS;
}
