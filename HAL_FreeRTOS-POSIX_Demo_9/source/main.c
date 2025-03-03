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
void vTaskFunction1(void *pvParameters);
void vTaskFunction2(void *pvParameters);
void vPrintString(const char *pcString);
void vApplicationIdleHook(void);

/*******************************************************************************
* Global Variables
*******************************************************************************/
volatile uint32_t ulIdleCycleCount = 0UL;  // Global variable for idle count
static const char *pcTextForTask1 = "Task 1 is running\r\n";
static const char *pcTextForTask2 = "Task 2 is deleting itself\r\n";
TaskHandle_t xTask2Handle = NULL;

/*******************************************************************************
* Function Definitions
*******************************************************************************/

/**
 * Function Name: vPrintString
 * Description: This function sends a string to UART using retargeted IO
 */
void vPrintString(const char *pcString)
{
    printf("%s\n", pcString);  // Print the string to UART (Retarget IO)
}

/**
 * Function Name: vTaskFunction1
 * Description: This is the first FreeRTOS task, which prints a message to UART.
 */
void vTaskFunction1( void *pvParameters )
{
    char *pcTaskName;
    const TickType_t xDelay250ms = pdMS_TO_TICKS( 550 );
    pcTaskName = ( char * ) pvParameters;
    for( ;; )
    {
        vPrintString( pcTaskName );


        // Raise the priority of Task 2
        // Create Task 2
            xTaskCreate(vTaskFunction2,    // Task function
                        "Task 2",         // Task name (for debugging)
                        1000,             // Stack size
                        (void*)pcTextForTask2, // Task parameter (none used)
                        2,                // Task priority
                        &xTask2Handle);   // Task handle (for manipulating priority)


        vTaskDelay( xDelay250ms );
    }
}

/**
 * Function Name: vTaskFunction2
 * Description: This is the second FreeRTOS task, which prints a message to UART.
 */
void vTaskFunction2( void *pvParameters )
{
    char *pcTaskName;
    const TickType_t xDelay250ms = pdMS_TO_TICKS( 250 );
    pcTaskName = ( char * ) pvParameters;
    for( ;; )
    {
        vPrintString( pcTaskName );
        vTaskDelete( xTask2Handle);
        vTaskDelay( xDelay250ms );
    }
}

/**
 * Function Name: vApplicationIdleHook
 * Description: This function runs when the CPU is idle. It increments a counter.
 */
void vApplicationIdleHook(void)
{
    // Increment the idle cycle count during idle time
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
    xTaskCreate(vTaskFunction1,    // Task function
                "Task 1",         // Task name (for debugging)
                1000,             // Stack size
                (void*)pcTextForTask1, // Task parameter (none used)
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
