/*******************************************************************************
* File Name:   main.c
*
* Description: This is a simple FreeRTOS example for the CY8CKIT-062 BLE board.
*              It creates two tasks which print messages to the UART periodically.
*
* Copyright 2021-2024, Cypress Semiconductor Corporation (an Infineon company) or
* its affiliates. All rights reserved.
*******************************************************************************/

#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define mainDELAY_LOOP_COUNT   (1000000UL) // Delay loop count for crude delays

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vPrintString(const char *pcString);

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/**
 * Function Name: vPrintString
 * Description: This function sends a string to UART using retargeted IO
 */
void vPrintString(const char *pcString)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
}

/**
 * Function Name: vTask1
 * Description: This is the first FreeRTOS task, which prints a message to UART.
 */
void vTask1(void *pvParameters)
{
    const char *pcTaskName = "Task 1 is running\r\n";

    // Task loop
    for (;;)
    {
        vPrintString(pcTaskName);  // Print task name
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1000 ms (1 second)
    }
}

/**
 * Function Name: vTask2
 * Description: This is the second FreeRTOS task, which prints a message to UART.
 */
void vTask2(void *pvParameters)
{
    const char *pcTaskName = "Task 2 is running\r\n";

    // Task loop
    for (;;)
    {
        vPrintString(pcTaskName);  // Print task name
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1000 ms (1 second)
    }
}

/**
 * Function Name: main
 * Description: This is the main entry point for the application. It initializes the
 *              hardware, creates FreeRTOS tasks, and starts the scheduler.
 */
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Initialization failed, stop here
    }

    /* Initialize UART for retargeted IO (so we can print to UART) */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // UART initialization failed, stop here
    }

    /* Enable global interrupts */
    __enable_irq();

    // Create Task 1
    xTaskCreate(vTask1,    // Task function
                "Task 1",  // Task name (for debugging)
                1000,      // Stack size
                NULL,      // Task parameter (none used)
                1,         // Task priority
                NULL);     // Task handle (not used)

    // Create Task 2
    xTaskCreate(vTask2,    // Task function
                "Task 2",  // Task name (for debugging)
                1000,      // Stack size
                NULL,      // Task parameter (none used)
                1,         // Task priority
                NULL);     // Task handle (not used)

    /* Start the scheduler so the tasks begin executing */
    vTaskStartScheduler();

    /* If the scheduler starts, the following line should never be reached */
    for (;;)
    {
        // The application should never reach here because the scheduler is running
    }
}

/* [] END OF FILE */
