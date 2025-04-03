#ifndef UART_H
#define UART_H

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

// UART configuration constants
#define DATA_BITS_8     8
#define STOP_BITS_1     1
#define BAUD_RATE       115200
#define UART_DELAY      10u
#define RX_BUF_SIZE     4
#define TX_BUF_SIZE     4

// UART initialization function prototype
cy_rslt_t uart_init(void);

// UART transmission function prototype
uint32_t uart_transmit_packet(uint8_t *tx_buf, uint16_t datalength);

// UART send a single character function prototype
void uart_putc(uint8_t value);

// UART reception function prototype
uint32_t uart_receive_packet(uint8_t *rx_buf, uint16_t datalength);

#endif // UART_H
