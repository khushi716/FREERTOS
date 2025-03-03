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
void vcontinousprocessingTask1(void *pvParameters);
void vcontinousprocessingTask2(void *pvParameters);
void vperiodicTask(void *pvParameters);

void vPrintString(const char *pcString);

/*******************************************************************************
* Function Definitions
*******************************************************************************/
void vPrintString(const char *pcString)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
}

/**
 * Function Name: vcontinousprocessingTask1
 * Description: This task continuously prints a message to UART.
 */
void vcontinousprocessingTask1(void *pvParameters)
{
    const char *pcTaskName = "Task 1 continue is running\r\n";

    // Task loop
    for (;;)
    {
        vPrintString(pcTaskName);  // Print task name
    }
}

/**
 * Function Name: vcontinousprocessingTask2
 * Description: This task continuously prints a message to UART.
 */
void vcontinousprocessingTask2(void *pvParameters)
{
    const char *pcTaskName = "Task 2 continue is running\r\n";

    // Task loop
    for (;;)
    {
        vPrintString(pcTaskName);  // Print task name
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * Function Name: vperiodicTask
 * Description: This task prints a message to UART every second.
 */
void vperiodicTask(void *pvParameters)
{
    const char *pcTaskName = "Task 2 is running\r\n";
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    // Task loop
    for (;;)
    {
        vPrintString(pcTaskName);  // Print task name
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2000));  // Delay for 1000 ms (1 second)
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

    // Create Task 1: Continuously printing task
    xTaskCreate(vcontinousprocessingTask1,    // Task function
                "Task 1",  // Task name (for debugging)
                1000,      // Stack size
                NULL,      // Task parameter (none used)
                1,         // Task priority
                NULL);     // Task handle (not used)

    // Create Task 2: Continuously printing task
    xTaskCreate(vcontinousprocessingTask2,    // Task function
                 "Task 2",  // Task name (for debugging)
                 1000,      // Stack size
                 NULL,      // Task parameter (none used)
                 1,         // Task priority
                 NULL);     // Task handle (not used)


    // Create Task 3: Periodically printing task
    xTaskCreate(vperiodicTask,    // Task function
                "Task 3",  // Task name (for debugging)
                1000,      // Stack size
                NULL,      // Task parameter (none used)
                2,         // Task priority
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
