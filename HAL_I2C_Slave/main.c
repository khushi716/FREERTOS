/*******************************************************************************
* File Name:   main.c
*
* Description: This example project demonstrates the basic operation of the
* I2C resource as Slave using HAL APIs. The I2C slave receives the
* command packets from the I2C master to control an user LED.
*
* Related Document: See README.md
*
*
********************************************************************************
* Copyright 2019-2024, Cypress Semiconductor Corporation (an Infineon company) or
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
/* Packet positions */
#define PACKET_SOP_POS          (0UL)
#define PACKET_CMD_POS          (1UL)
#define PACKET_EOP_POS          (2UL)
#define PACKET_STATUS_POS       (1UL)

/* Start and end of packet markers */
#define PACKET_SOP              (0x01UL)
#define PACKET_EOP              (0x17UL)

/* I2C slave address to communicate with */
#define I2C_SLAVE_ADDR          (0x24UL)

/* I2C bus frequency */
#define I2C_FREQ                (400000UL)

/* I2C slave interrupt priority */
#define I2C_SLAVE_IRQ_PRIORITY  (7u)

/* Command valid status */
#define STATUS_CMD_DONE         (0x00UL)
#define STATUS_CMD_FAIL         (0xFFUL)

/* Packet size */
#define PACKET_SIZE             (3UL)

#ifdef XMC7200D_E272K8384
#define KIT_XMC72
#endif


/*******************************************************************************
* Global Variables
*******************************************************************************/
cyhal_i2c_t sI2C;
uint8_t i2c_write_buffer[PACKET_SIZE] = {0};
uint8_t i2c_read_buffer[PACKET_SIZE] = {PACKET_SOP,STATUS_CMD_FAIL,PACKET_EOP};


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
* Function Name: i2c_slave_handle_event
********************************************************************************
* Summary:
* This is a callback function for I2C slave events. If a write event occurs,
* the command packet is verified and executed.
*
* Parameters:
*  callback_arg : extra argument that can be passed to callback
*  event        : I2C event
*
* Return:
*  void
*
*******************************************************************************/
void i2c_slave_handle_event(void *callback_arg, cyhal_i2c_event_t event)
{
    if (0UL == (CYHAL_I2C_SLAVE_ERR_EVENT & event))
    {
        if (0UL != (CYHAL_I2C_SLAVE_WR_CMPLT_EVENT & event))
        {
            /* Check start and end of packet markers */
            if ((i2c_write_buffer[PACKET_SOP_POS] == PACKET_SOP) &&
                (i2c_write_buffer[PACKET_EOP_POS] == PACKET_EOP))
            {
                printf("User LED should start blinking \r\n");
                /* Execute command */
                cyhal_gpio_write(CYBSP_USER_LED,
                        i2c_write_buffer[PACKET_CMD_POS]);

                /* Update status of received command */
                i2c_read_buffer[PACKET_STATUS_POS] = STATUS_CMD_DONE;
            }

            /* Configure read buffer for the next write */
            i2c_write_buffer[PACKET_SOP_POS] = 0;
            i2c_write_buffer[PACKET_EOP_POS] = 0;
            cyhal_i2c_slave_config_write_buffer(&sI2C, i2c_write_buffer,
                    PACKET_SIZE);
        }

        if (0UL != (CYHAL_I2C_SLAVE_RD_CMPLT_EVENT & event))
        {
            /* Configure write buffer for the next read */
            i2c_read_buffer[PACKET_STATUS_POS] = STATUS_CMD_FAIL;
            cyhal_i2c_slave_config_read_buffer(&sI2C, i2c_read_buffer,
                    PACKET_SIZE);
        }
    }
}


/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function.
*   1. Initializes the board, retarget-io and led
*   2. Configures the I2C slave to receive packet from the master
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
    cyhal_i2c_cfg_t sI2C_cfg;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    /* Board init failed. Stop program execution */
    handle_error(result);

#if defined(KIT_XMC72)
    /*Configure clock settings for KIT_XMC72_EVK */
    cyhal_clock_t clock_fll, clock_hf, clock_peri;
    result = cyhal_clock_reserve(&clock_hf, &CYHAL_CLOCK_HF[0]);
    result = cyhal_clock_reserve(&clock_fll, &CYHAL_CLOCK_FLL);
    if(result == CY_RSLT_SUCCESS){
    result = cyhal_clock_set_source(&clock_hf, &clock_fll);
    }
    /* Set divider to 1 for Peripheral Clock */
    result = cyhal_clock_reserve(&clock_peri, CYHAL_CLOCK_PERI);
    if(result == CY_RSLT_SUCCESS){
    result = cyhal_clock_set_divider(&clock_peri,1);
    }
#endif

    /* Initialize the retarget-io */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                  CY_RETARGET_IO_BAUDRATE);
    /* Retarget-io init failed. Stop program execution */
    handle_error(result);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("************** "
           "HAL: I2C Slave "
           "************** \r\n\n");

    /* Initialize user LED */
    printf(">> Configuring user LED..... ");
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                              CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    /* LED init failed. Stop program execution */
    handle_error(result);
    printf("Done\r\n");

    /* I2C Slave configuration settings */
    printf(">> Configuring I2C Slave..... ");
    sI2C_cfg.is_slave = true;
    sI2C_cfg.address = I2C_SLAVE_ADDR;
    sI2C_cfg.frequencyhal_hz = I2C_FREQ;

    /* Init I2C slave */
    result = cyhal_i2c_init(&sI2C, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    /* I2C slave init failed. Stop program execution */
    handle_error(result);

    /* Configure I2C slave */
    result = cyhal_i2c_configure(&sI2C, &sI2C_cfg);
    /* I2C slave configuration failed. Stop program execution */
    handle_error(result);

    printf("Done\r\n");

    /* Configure the read buffer on an I2C Slave */
    cyhal_i2c_slave_config_read_buffer(&sI2C, i2c_read_buffer, PACKET_SIZE);
    /* Configure the write buffer on an I2C Slave */
    cyhal_i2c_slave_config_write_buffer(&sI2C, i2c_write_buffer, PACKET_SIZE);
    /* Register an I2C event callback handler */
    cyhal_i2c_register_callback(&sI2C, i2c_slave_handle_event, NULL);
    /* Configure and Enable I2C Interrupt */
    cyhal_i2c_enable_event(&sI2C,
        (cyhal_i2c_event_t)(CYHAL_I2C_SLAVE_WR_CMPLT_EVENT
                           | CYHAL_I2C_SLAVE_RD_CMPLT_EVENT
                           | CYHAL_I2C_SLAVE_ERR_EVENT),
                           I2C_SLAVE_IRQ_PRIORITY, true);

    /* Enable interrupts */
    __enable_irq();

    for (;;)
    {

    }
}
