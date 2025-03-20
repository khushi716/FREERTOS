/*******************************************************************************
* File Name:   main.c

* Description: This is the source code for Low-Power Timer Example for
*              ModusToolbox.
*
* Related Document: See README.md
*
*
********************************************************************************
* Copyright 2019-2023, Cypress Semiconductor Corporation (an Infineon company) or
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
#define GPIO_INTERRUPT_PRIORITY (7u)


/*******************************************************************************
* Global Variables
*******************************************************************************/
cyhal_lptimer_t lptimer_obj;
cyhal_lptimer_info_t lptimer_obj_info;
volatile bool gpio_intr_flag = false;
cyhal_gpio_callback_data_t gpio_btn_callback_data;


/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void handle_error(uint32_t status);
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);
void enter_deepsleep(void);


/*******************************************************************************
* Function Definitions
*******************************************************************************/

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
* This is the main function, lptimer HAL APIs are used to work with the MCWDT
* block. The main loop waits till the button switch is pressed. Once pressed, it
* reads the timer value and gets the difference in time between the last two
* switch press events. It then prints the time over UART.
*
* Parameters:
*  none
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

    /* Switch press event count value */
    uint32_t event1_cnt, event2_cnt;

    /* The time between two presses of switch */
    uint32_t timegap;

    #if defined (CY_DEVICE_SECURE)
    cyhal_wdt_t wdt_obj;
    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
    #endif

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    /* BSP initialization failed. Stop program execution */
    handle_error(result);

    /* Initialize the User LED */
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                             CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    /* GPIO initialization failed. Stop program execution */
    handle_error(result);

    /* Initialize the User button */
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT,
                CYBSP_USER_BTN_DRIVE, CYBSP_BTN_OFF);
    /* GPIO initialization failed. Stop program execution */
    handle_error(result);

    /* Configure GPIO interrupt */
    gpio_btn_callback_data.callback = gpio_interrupt_handler;

    /* Configure GPIO interrupt */
    cyhal_gpio_register_callback(CYBSP_USER_BTN, &gpio_btn_callback_data);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL,
                            GPIO_INTERRUPT_PRIORITY, true);

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                 CY_RETARGET_IO_BAUDRATE);
    /* retarget-io initialization failed. Stop program execution */
    handle_error(result);

    /* Print a message on UART */
    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("****************** "
           "HAL: Low-Power Timer Example "
           "****************** \r\n\n");

    /* Initialize the LPTIMER */
    result = cyhal_lptimer_init(&lptimer_obj);
    /* LPTIMER initialization failed. Stop program execution */
    handle_error(result);

    /* Initialize event count value */
    event1_cnt = 0;
    event2_cnt = 0;

    /* Get the information about the LPTimer */
    cyhal_lptimer_get_info(&lptimer_obj, &lptimer_obj_info);

    printf("lptimer info:\r\n");
    printf("frequency_hz = %lu\r\n",
            (unsigned long)lptimer_obj_info.frequency_hz);
    printf("min_set_delay = %d\r\n",
            lptimer_obj_info.min_set_delay);
    printf("max_counter_value = %lu\r\n",
            (unsigned long)lptimer_obj_info.max_counter_value);

    printf("\r\nPress the user button to display the time between two presses "
            "of the user button.\r\n");

    for (;;)
    {
        /*Check the interrupt status which indicates if the switch is pressed*/
        if (true == gpio_intr_flag)
        {
            gpio_intr_flag = false;

            /* Consider previous key press as 1st key press event */
            event1_cnt = event2_cnt;

            /* Consider the current switch press as 2nd switch press event and 
             * Get counter value from LPTIMER. */
            event2_cnt = cyhal_lptimer_read(&lptimer_obj);

            /* Calculate the time between two presses of switch and print on the
             * terminal. LPTIMER is clocked by LFClk. */
            if (event2_cnt > event1_cnt)
            {
                timegap = (event2_cnt-event1_cnt)/(lptimer_obj_info.frequency_hz);

                /* Print the timegap value */
                printf("\r\nThe time between two presses of user button = %d\r"
                        "\n", (unsigned int)timegap);
            }
            else /* counter overflow */
            {
                /* Print a message on overflow of counter */
                printf("\r\n\r\nCounter overflow detected\r\n");
            }
        }
        enter_deepsleep();
    }
}


/*******************************************************************************
* Function Name: gpio_interrupt_handler
********************************************************************************
* Summary:
*   GPIO interrupt handler.
*
* Parameters:
*  void *handler_arg
*  cyhal_gpio_irq_event_t
*
* Return:
*  static void
*
*******************************************************************************/
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    gpio_intr_flag = true;
}


/*******************************************************************************
* Function Name: enter_deepsleep
********************************************************************************
* Summary:
*   The function ensures that the UART has completed printing on the terminal
*   and then puts the system to deep sleep mode.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void enter_deepsleep(void)
{
    cy_rslt_t result;

    /* Ensure that the UART has completed its operation */
    if (cyhal_uart_is_tx_active(&cy_retarget_io_uart_obj) == false)
    {
        /* Turn OFF LED to indicate UART is not in use */
        cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);

        /* Enter deep sleep mode */
        result = cyhal_syspm_deepsleep();

        /* System failed to enter deep sleep mode. Stop program execution */
        handle_error(result);
    }
    else
    {
        /* Turn ON LED to indicate UART is in use */
        cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
    }
}


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

/* [] END OF FILE */
