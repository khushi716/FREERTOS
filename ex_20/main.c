/*******************************************************************************
* File Name:   main.c
*
* Description: This is the source code for the Empty Application Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2021-2024, Cypress Semiconductor Corporation (an Infineon company) or
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

#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/*******************************************************************************
* Macros
*******************************************************************************/

/*******************************************************************************
* Global Variables
*******************************************************************************/
SemaphoreHandle_t new_Sem;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
static void prvNewPrintString(const char *pcString);
static void prvPrintTask(void *pvParameters);

/*******************************************************************************
* Function Definitions
*******************************************************************************/

static void prvNewPrintString(const char *pcString)
{
    // Take the semaphore to ensure that only one task is printing at a time
    if (xSemaphoreTake(new_Sem, portMAX_DELAY) == pdTRUE)
    {
        printf("%s", pcString);  // Print the string
        fflush(stdout);  // Ensure the string is printed immediately
        xSemaphoreGive(new_Sem);  // Release the semaphore
    }
    else
    {
        // Handle semaphore acquisition failure if needed
        CY_ASSERT(0);  // This should not happen under normal conditions
    }
}

static void prvPrintTask(void *pvParameters)
{
    char *task_String = (char *)pvParameters;

    for (;;)
    {
        prvNewPrintString(task_String);  // Print the string passed as a parameter
        vTaskDelay(pdMS_TO_TICKS(200));  // Delay between prints
    }
}

/*******************************************************************************
* Function Name: main
*********************************************************************************
* Summary:
* The main function initializes the board, creates the tasks, and starts the FreeRTOS scheduler.
*
* Parameters:
*  void
*
* Return:
*  int
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

#if defined (CY_DEVICE_SECURE) && defined (CY_USING_HAL)
    cyhal_wdt_t wdt_obj;

    // Clear watchdog timer so that it doesn't trigger a reset
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    // Initialize the device and board peripherals
    result = cybsp_init();

    // Board init failed. Stop program execution
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Initialization failed, halt execution
    }

    // Initialize the UART for printf retargeting
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // UART initialization failed, halt execution
    }

    // Enable global interrupts
    __enable_irq();

    // Create the mutex for protecting the critical section
    new_Sem = xSemaphoreCreateMutex();
    if (new_Sem == NULL)
    {
        CY_ASSERT(0);  // Mutex creation failed, halt execution
    }
    else
    {
        // Create Task 1
        if (xTaskCreate(prvPrintTask, "Print1", 1000, "Task 1 ***************************************\r\n", 1, NULL) != pdPASS)
        {
            CY_ASSERT(0);  // Task creation failed, halt execution
        }

        // Create Task 2 with a higher priority
        if (xTaskCreate(prvPrintTask, "Print2", 1000, "Task 2 ---------------------------------------\r\n", 2, NULL) != pdPASS)
        {
            CY_ASSERT(0);  // Task creation failed, halt execution
        }

        // Start the FreeRTOS scheduler to begin executing tasks
        vTaskStartScheduler();
    }

    // Infinite loop in case scheduler fails to start
    for (;;)
    {
    }
}

/* [] END OF FILE */
