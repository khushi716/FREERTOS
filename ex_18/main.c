//
//
//#include "cybsp.h"
//#include "cyhal.h"
//#include "cy_retarget_io.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"
//
//// Task handles
//TaskHandle_t xHandlerTask1;
//TaskHandle_t xInterruptTask;
//TaskHandle_t xHigherPriorityTaskHandle = NULL; // Task handle for higher priority task
//
//// Semaphore handle
//
//void vPrintStringAndNumber(const char *pcString, uint32_t ul);
//void vPortGenerateSimulatedInterrupt();
//static void ulExampleInterruptHandler(void);
//static void vDeferredHandlingFunction(void *pvParameter1, uint32_t ulParameter2);
//static void vHigherPriorityTask(void *pvParameters);  // New higher-priority task
//
//void vPrintStringAndNumber(const char *pcString, uint32_t ul)
//{
//    printf("%s", pcString);  // Print the string to UART (Retarget IO)
//    printf(" %ld\n", ul);  // Print the string to UART (Retarget IO)
//}
//
//void vPrintString(const char *pcString)
//{
//    printf("%s", pcString);  // Print the string to UART (Retarget IO)
//}
//
//static void vPeriodicTask(void *pvParameters)
//{
//    const TickType_t xDelay500ms = pdMS_TO_TICKS(500UL);
//
//    for (;;)
//    {
//        vTaskDelay(xDelay500ms);
//        vPrintString("Periodic task - About to generate an interrupt.\r\n");
//
//        // Generate the interrupt (simulated)
//        vPortGenerateSimulatedInterrupt();
//
//        vPrintString("Periodic task - Interrupt generated.\r\n\r\n\r\n");
//    }
//}
//
//// Custom simulated interrupt function
//void vPortGenerateSimulatedInterrupt()
//{
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//
//    // Simulate the interrupt by directly calling the interrupt handler
//    ulExampleInterruptHandler();
//}
//
//static void ulExampleInterruptHandler(void)
//{
//    static uint32_t ulParameterValue = 0;
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//
//    // Queue the deferred function call (this will execute outside the ISR context)
//    xTimerPendFunctionCallFromISR(vDeferredHandlingFunction, NULL, ulParameterValue, &xHigherPriorityTaskWoken);
//
//
//    // Increment the parameter value for the next interrupt
//    ulParameterValue++;
//
//    // Check if a higher-priority task was woken, and trigger a context switch if necessary
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);  // This line triggers a context switch if needed
//}
//
//static void vDeferredHandlingFunction(void *pvParameter1, uint32_t ulParameter2)
//{
//    // Print the event that occurred
//    vPrintStringAndNumber("Handler function - Processing event ", ulParameter2);
//    BaseType_t priority = uxTaskPriorityGet(NULL);
//    vPrintStringAndNumber("Handler function - Processing event1 ",  priority);
//    if (xHigherPriorityTaskHandle != NULL)
//    {
//        // Unblock the higher-priority task (for example, using a semaphore or flag)
//        vPrintString("Handler function - Waking up higher priority task.\r\n");
//        xTaskNotifyGive(xHigherPriorityTaskHandle); // Simulate waking up the higher-priority task
//    }
//}
//
//// New higher-priority task with priority 2
//static void vHigherPriorityTask(void *pvParameters)
//{
//    const TickType_t xDelay = pdMS_TO_TICKS(500);  // Delay for 500ms
//
//    for (;;)
//    {
//        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for a notification (simulated event)
//        UBaseType_t p = uxTaskPriorityGet(NULL);
//
//        vPrintString("Handler  higher priority task.\r\n");
//        vTaskDelay(xDelay);  // Simulate doing work
//    }
//}
//
//int main(void)
//{
//    cy_rslt_t result;
//
//    // Initialize the board and peripherals
//    result = cybsp_init();
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Initialization failed, stop here
//    }
//
//    // Initialize UART for retargeting IO (printf)
//    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // UART initialization failed, stop here
//    }
//
//    // Enable global interrupts
//    __enable_irq();
//    const UBaseType_t ulPeriodicTaskPriority = configTIMER_TASK_PRIORITY - 1;
//    // Create the higher-priority task with priority 2
//    if (xTaskCreate(vHigherPriorityTask, "Higher Priority Task", 1000, NULL,ulPeriodicTaskPriority+1, &xHigherPriorityTaskHandle) != pdPASS)
//    {
//        CY_ASSERT(0);  // Task creation failed, stop here
//    }
//
//    // Create the periodic task with lower priority (priority 1)
//
//    if (xTaskCreate(vPeriodicTask, "Periodic Task", 1000, NULL, ulPeriodicTaskPriority, &xInterruptTask) != pdPASS)
//    {
//        CY_ASSERT(0);  // Task creation failed, stop here
//    }
//
//    // Start the FreeRTOS scheduler
//    vTaskStartScheduler();
//
//    // Infinite loop in case the scheduler doesn't start
//    for (;;);
//}

//vTaskNotifyGiveFromISR()




#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

// Task handles
TaskHandle_t xHandlerTask1;
TaskHandle_t xInterruptTask;
TaskHandle_t xHigherPriorityTaskHandle = NULL; // Task handle for higher priority task

// Semaphore handle

void vPrintStringAndNumber(const char *pcString, uint32_t ul);
void vPortGenerateSimulatedInterrupt();
static void ulExampleInterruptHandler(void);
static void vDeferredHandlingFunction(void *pvParameter1, uint32_t ulParameter2);
static void vHigherPriorityTask(void *pvParameters);  // New higher-priority task

void vPrintStringAndNumber(const char *pcString, uint32_t ul)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
    printf(" %ld\n", ul);  // Print the string to UART (Retarget IO)
}

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
    static uint32_t ulParameterValue = 0;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Queue the deferred function call (this will execute outside the ISR context)
    xTimerPendFunctionCallFromISR(vDeferredHandlingFunction, NULL, ulParameterValue, &xHigherPriorityTaskWoken);


    // Increment the parameter value for the next interrupt
    ulParameterValue++;

    // Check if a higher-priority task was woken, and trigger a context switch if necessary
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);  // This line triggers a context switch if needed
}

static void vDeferredHandlingFunction(void *pvParameter1, uint32_t ulParameter2)
{
    // Print the event that occurred
    vPrintStringAndNumber("Handler function - Processing event ", ulParameter2);
    BaseType_t priority = uxTaskPriorityGet(NULL);
    vPrintStringAndNumber("Handler function - Processing event1 ",  priority);
    if (xHigherPriorityTaskHandle != NULL)
    {
        // Unblock the higher-priority task (for example, using a semaphore or flag)
        vPrintString("Handler function - Waking up higher priority task.\r\n");
        xTaskNotifyGive(xHigherPriorityTaskHandle); // Simulate waking up the higher-priority task
    }
}

// New higher-priority task with priority 2
static void vHigherPriorityTask(void *pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(500);  // Delay for 500ms

    for (;;)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Wait for a notification (simulated event)
        UBaseType_t p = uxTaskPriorityGet(NULL);

        vPrintString("Handler  higher priority task.\r\n");
        vTaskDelay(xDelay);  // Simulate doing work
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
    const UBaseType_t ulPeriodicTaskPriority = configTIMER_TASK_PRIORITY - 1;
    // Create the higher-priority task with priority 2
    if (xTaskCreate(vHigherPriorityTask, "Higher Priority Task", 1000, NULL,ulPeriodicTaskPriority+1, &xHigherPriorityTaskHandle) != pdPASS)
    {
        CY_ASSERT(0);  // Task creation failed, stop here
    }

    // Create the periodic task with lower priority (priority 1)

    if (xTaskCreate(vPeriodicTask, "Periodic Task", 1000, NULL, ulPeriodicTaskPriority, &xInterruptTask) != pdPASS)
    {
        CY_ASSERT(0);  // Task creation failed, stop here
    }

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Infinite loop in case the scheduler doesn't start
    for (;;);
}
