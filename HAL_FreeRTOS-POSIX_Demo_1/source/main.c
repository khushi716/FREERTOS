/*******************************************************************************
* File Name:   main.c
*
* Description: This is a simple FreeRTOS example for the CY8CKIT-062 BLE board.
*              It creates two tasks which print messages to the UART periodically.
*              It also demonstrates the use of an Idle Hook function that increments
*              a counter during idle time.
*
* Copyright 2021-2024, Cypress Semiconductor Corporation (an Infineon company) or
* its affiliates. All rights reserved.
*******************************************************************************/
//configUSE_IDLE_HOOK 1
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
void vTaskFunction(void *pvParameters);
void vPrintStringAndNumber(const char *pcString,volatile uint32_t ul);
void vApplicationIdleHook(void);  // Idle hook function prototype

/*******************************************************************************
* Global Variables
*******************************************************************************/
volatile uint32_t ulIdleCycleCount = 0UL;  // Global variable for idle count
static const char *pcTextForTask1 = "Task 1 is running\r\n";
static const char *pcTextForTask2 = "Task 2 is running\r\n";

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/**
 * Function Name: vPrintStringAndNumber
 * Description: This function sends a string to UART using retargeted IO
 */
void vPrintStringAndNumber(const char *pcString, volatile uint32_t ul)
{
    printf("%s\n", pcString);  // Print the string to UART (Retarget IO)
    printf(" ulIdleCycleCount%ld\n", ul);  // Print the string to UART (Retarget IO)
}

/**
 * Function Name: vTaskFunction
 * Description: This is the first FreeRTOS task, which prints a message to UART.
 */
void vTaskFunction( void *pvParameters )
{
    char *pcTaskName;
    const TickType_t xDelay250ms = pdMS_TO_TICKS( 250 );
    /* The string to print out is passed in via the parameter. Cast this to a
    character pointer. */
    pcTaskName = ( char * ) pvParameters;
    /* As per most tasks, this task is implemented in an infinite loop. */
    for( ;; )
    {
        /* Print out the name of this task AND the number of times ulIdleCycleCount
        has been incremented. */
        vPrintStringAndNumber( pcTaskName, ulIdleCycleCount );
        /* Delay for a period of 250 milliseconds. */
        vTaskDelay( xDelay250ms );
    }
}

/**
 * Function Name: vApplicationIdleHook
 * Description: This function is called during idle time to perform background tasks
 *              or to increment the idle cycle counter.
 */
void vApplicationIdleHook(void)
{
    /* Increment the idle cycle counter */
    ulIdleCycleCount++;
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
    xTaskCreate(vTaskFunction,    // Task function
                "Task 1",         // Task name (for debugging)
                1000,             // Stack size
                (void*)pcTextForTask1, // Task parameter (none used)
                1,                // Task priority
                NULL);            // Task handle (not used)
    xTaskCreate(vTaskFunction,    // Task function
                   "Task 2",         // Task name (for debugging)
                   1000,             // Stack size
                   (void*)pcTextForTask2, // Task parameter (none used)
                   1,                // Task priority
                   NULL);            // Task handle (not used)
    /* Start the scheduler so the tasks begin executing */
    vTaskStartScheduler();

    /* If the scheduler starts, the following line should never be reached */
    for (;;)
    {
        // The application should never reach here because the scheduler is running
    }
}

/* [] END OF FILE */
