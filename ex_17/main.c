#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// Task handles
TaskHandle_t xHandlerTask1;
TaskHandle_t xInterruptTask;

// Semaphore handle
SemaphoreHandle_t xCountingSemaphore;
UBaseType_t uxPriority;

void vPortGenerateSimulatedInterrupt();
static void ulExampleInterruptHandler(void);
void vPrintString(const char *pcString)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
}

static void vPeriodicTask(void *pvParameters)
{
    const TickType_t xDelay500ms = pdMS_TO_TICKS(500UL);

    for (;;)
    {
        vTaskDelay(xDelay500ms);
        vPrintString("Periodic task - About to generate an interrupt.\r\n");

        // Generate the interrupt (simulated)
        vPortGenerateSimulatedInterrupt();

        vPrintString("Periodic task - Interrupt generated.\r\n\r\n\r\n");
    }
}

// Custom simulated interrupt function
void vPortGenerateSimulatedInterrupt()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Simulate the interrupt by directly calling the interrupt handler
    ulExampleInterruptHandler();
}

static void ulExampleInterruptHandler(void)
{

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // Give the semaphore from ISR
    xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
    xSemaphoreGiveFromISR(xCountingSemaphore, &xHigherPriorityTaskWoken);
    // Request context switch if a higher priority task is woken
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static void vHandlerTask1(void *pvParameters)
{
    for (;;)
    {
        // Wait for the semaphore to be given
        if (xSemaphoreTake(xCountingSemaphore, portMAX_DELAY) == pdTRUE)
        {
            vPrintString("Handler task 1 - Processing event1.\r\n");
        }


    }


}



int main(void)
{
    cy_rslt_t result;

    // Initialize the board and peripherals
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Initialization failed, stop here
    }

    // Initialize UART for retargeting IO (printf)
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // UART initialization failed, stop here
    }

    // Enable global interrupts
    __enable_irq();

    // Create counting semaphore with a maximum count of 10 and initial count of 0
    xCountingSemaphore = xSemaphoreCreateCounting(10, 0);
    if (xCountingSemaphore == NULL)
    {
        CY_ASSERT(0);  // Semaphore creation failed, stop here
    }

    // Create handler tasks
    if (xTaskCreate(vHandlerTask1, "Handler Task 1", 1000, NULL, 3, &xHandlerTask1) != pdPASS)
    {
        CY_ASSERT(0);  // Task creation failed, stop here
    }
    // Create the periodic task
    if (xTaskCreate(vPeriodicTask, "Periodic Task", 1000, NULL, 1, &xInterruptTask) != pdPASS)
    {
        CY_ASSERT(0);  // Task creation failed, stop here
    }

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Infinite loop in case the scheduler doesn't start
    for (;;);
}
