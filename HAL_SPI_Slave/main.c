/*******************************************************************************
* File Name:   main.c
*
* Description: This example project demonstrates the basic operation of SPI
* resource as Slave using HAL. The SPI slave receives command packets from the
* SPI master to control an user LED.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2022-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

/*******************************************************************************
* Header Files
*******************************************************************************/
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"


/*******************************************************************************
* Macros
*******************************************************************************/
/* SPI baud rate in Hz */
#define SPI_FREQ_HZ                (1000000UL)
/* SPI transfer bits per frame */
#define BITS_PER_FRAME             (8)


/*******************************************************************************
* Global Variables
*******************************************************************************/


/*******************************************************************************
* Function Prototypes
*******************************************************************************/


/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function Name: handle_error
********************************************************************************
* Summary:
* User defined error handling function
*
* Parameters:
*  uint32_t status - status indicates success or failure
*
* Return:
*  void
*
*******************************************************************************/
void handle_error(uint32_t status)
{
    if (status != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
}


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* The main function.
*   1. Initializes the board, retarget-io and led
*   2. Configures the SPI slave to receive packet from the master and execute
*   the instructions
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    cyhal_spi_t sSPI;
    uint32_t cmd_recv = CYBSP_LED_STATE_OFF;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    /* Board init failed. Stop program execution */
    handle_error(result);

    /* Initialize retarget-io for uart logs */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                  CY_RETARGET_IO_BAUDRATE);
    /* Retarget-io init failed. Stop program execution */
    handle_error(result);

    /* Initialize the user LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                              CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    /* User LED init failed. Stop program execution */
    handle_error(result);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("************** "
           "HAL: SPI Slave "
           "************** \r\n\n");

    printf("Configuring SPI slave...\r\n");
    /* Init SPI slave */
    result = cyhal_spi_init(&sSPI,CYBSP_SPI_MOSI,CYBSP_SPI_MISO,CYBSP_SPI_CLK,
                            CYBSP_SPI_CS,NULL,BITS_PER_FRAME,
                            CYHAL_SPI_MODE_00_MSB,true);
    handle_error(result);

    /* Set the SPI baud rate */
    result = cyhal_spi_set_frequency(&sSPI, SPI_FREQ_HZ);
    handle_error(result);

    /* Enable interrupts */
    __enable_irq();

    for (;;)
    {
        /* Read the command packet sent from master */
        if (CY_RSLT_SUCCESS == cyhal_spi_recv(&sSPI, &cmd_recv))
        {
            /* Execute command */
            cyhal_gpio_write(CYBSP_USER_LED, cmd_recv);
        }
        else
        {
            handle_error(1);
        }
    }
}


/* [] END OF FILE */
