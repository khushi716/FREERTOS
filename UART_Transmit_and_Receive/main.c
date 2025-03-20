#include "cy_sysclk.h"         // Header for system clock API
#include "cy_scb_uart.h"       // Header for UART API
#include "cy_gpio.h"           // Header for GPIO configuration
#include "cyhal.h"             // Hardware Abstraction Layer (HAL) header for platform control
#include "cybsp.h"             // Board support package header for system initialization

// Define Constants
#define UART_CLK_DIV_TYPE     (CY_SYSCLK_DIV_8_BIT)   // Set the divider type to 8-bit
#define UART_CLK_DIV_NUMBER   (0U)                     // Select divider 0 for clock assignment
#define UART_PORT             P5_0_PORT                // Port for UART pins
#define UART_RX_NUM           P5_0_NUM                 // Pin number for UART RX
#define UART_TX_NUM           P5_1_NUM                 // Pin number for UART TX
#define BUFFER_SIZE           3                          // Size of the data buffer
#define CMD_START_TRANSFER    0x01                       // Command byte for initiating transfer

// Create context structure for UART driver
cy_stc_scb_uart_context_t uartContext;

// Configuration structure for UART initialization
const cy_stc_scb_uart_config_t uartConfig =
{
    .uartMode                   = CY_SCB_UART_STANDARD,                // Set standard UART mode
    .enableMutliProcessorMode   = false,                               // Disable multi-processor mode
    .smartCardRetryOnNack       = false,                               // Disable retry on NACK for smart card mode
    .irdaInvertRx               = false,                               // IRDA RX inversion disabled
    .irdaEnableLowPowerReceiver = false,                               // IRDA low power receiver disabled
    .oversample                 = 12UL,                                // Set oversampling rate (12x)
    .enableMsbFirst             = false,                               // Data transfer is LSB first
    .dataWidth                  = 8UL,                                 // Data width is 8 bits
    .parity                     = CY_SCB_UART_PARITY_NONE,             // No parity checking
    .stopBits                   = CY_SCB_UART_STOP_BITS_1,             // 1 stop bit
    .enableInputFilter          = false,                               // Disable input filter
    .breakWidth                 = 11UL,                                // Break width for UART break conditions
    .dropOnFrameError           = false,                               // Do not drop data on frame error
    .dropOnParityError          = false,                               // Do not drop data on parity error
    .receiverAddress            = 0UL,                                 // No address filtering
    .receiverAddressMask        = 0UL,                                 // No address mask
    .acceptAddrInFifo           = false,                               // No address filtering in FIFO
    .enableCts                  = false,                               // Disable CTS flow control
    .ctsPolarity                = CY_SCB_UART_ACTIVE_LOW,              // Active low CTS signal
    .rtsRxFifoLevel             = 0UL,                                 // No RTS FIFO level
    .rtsPolarity                = CY_SCB_UART_ACTIVE_LOW,              // Active low RTS signal
    .rxFifoTriggerLevel         = 0UL,                                 // Set RX FIFO trigger level to 0
    .rxFifoIntEnableMask        = 0UL,                                 // Disable interrupts for RX FIFO
    .txFifoTriggerLevel         = 0UL,                                 // Set TX FIFO trigger level to 0
    .txFifoIntEnableMask        = 0UL,                                 // Disable interrupts for TX FIFO
};

// Clock Configuration
#define CLK_FREQUENCY   50000000UL  // 50 MHz system clock frequency for UART operation

int main(void)
{
    cy_rslt_t result;   // Result variable to check return status of API calls

    // Initialize the device (Configure GPIO, system clock, etc.)
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        // Initialization failed, halt the program
        printf("Device Initialization Failed\n");
        return -1;
    }

    // Set the clock divider to achieve the desired UART baud rate (Assuming 115200 Baud rate)
    Cy_SysClk_PeriphAssignDivider(PCLK_SCB5_CLOCK, UART_CLK_DIV_TYPE, UART_CLK_DIV_NUMBER);  // Assign divider
    Cy_SysClk_PeriphSetDivider(UART_CLK_DIV_TYPE, UART_CLK_DIV_NUMBER, 35UL);  // Set divider value to get the correct clock speed
    Cy_SysClk_PeriphEnableDivider(UART_CLK_DIV_TYPE, UART_CLK_DIV_NUMBER);  // Enable the clock divider

    // Set UART pins (P5_0 for RX, P5_1 for TX)
    Cy_GPIO_SetHSIOM(UART_PORT, UART_RX_NUM, P5_0_SCB5_UART_RX); // Set RX pin to SCB5 UART RX function
    Cy_GPIO_SetHSIOM(UART_PORT, UART_TX_NUM, P5_1_SCB5_UART_TX); // Set TX pin to SCB5 UART TX function

    // Configure the GPIO pins for UART
    Cy_GPIO_SetDrivemode(UART_PORT, UART_RX_NUM, CY_GPIO_DM_HIGHZ);          // Set RX to High-Z mode (input)
    Cy_GPIO_SetDrivemode(UART_PORT, UART_TX_NUM, CY_GPIO_DM_STRONG_IN_OFF);  // Set TX to Strong Drive mode (output)

    // Initialize the UART block with the configuration
    (void) Cy_SCB_UART_Init(SCB5, &uartConfig, &uartContext); // Initialize UART with configuration

    // Enable UART operation
    Cy_SCB_UART_Enable(SCB5); // Enable UART block for operation

    // Data to send
    uint8_t txBuffer[BUFFER_SIZE] = {CMD_START_TRANSFER, 0x00, 0x01}; // Initialize buffer with command to initiate transfer

    // Transmit the data using polling mode (blocking)
    Cy_SCB_UART_PutArrayBlocking(SCB5, txBuffer, sizeof(txBuffer));  // Send data using blocking mode
    printf("Data Sent: 0x%X 0x%X 0x%X\n", txBuffer[0], txBuffer[1], txBuffer[2]);  // Print sent data

    // Data reception (blocking until complete)
    uint8_t rxBuffer[BUFFER_SIZE];
    (void) Cy_SCB_UART_Receive(SCB5, rxBuffer, sizeof(rxBuffer), &uartContext);  // Receive data

    // Wait until the reception is complete
    while (0UL != (CY_SCB_UART_RECEIVE_ACTIVE & Cy_SCB_UART_GetReceiveStatus(SCB5, &uartContext)))
    {
    }

    // Print received data
    printf("Data Received: 0x%X 0x%X 0x%X\n", rxBuffer[0], rxBuffer[1], rxBuffer[2]);

    return 0; // Return success
}
