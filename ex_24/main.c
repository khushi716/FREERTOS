





#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#define GPIO_INTERRUPT_PRIORITY (7u)

// Task handles
TaskHandle_t xHandlerTask;
TaskHandle_t xInterruptTask;
// Semaphore handle
SemaphoreHandle_t xBinarySemaphore;

// GPIO
volatile bool gpio_intr_flag = false;  // Flag set by GPIO interrupt
cyhal_gpio_callback_data_t gpio_btn_callback_data;  // GPIO callback data

// Function to print strings
void vPrintString(const char *pcString)
{
    printf("%s", pcString);  // Print string to UART (Retarget IO)
}

// GPIO interrupt handler
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    gpio_intr_flag = true;  // Set flag when button is pressed
}

// Interrupt handler for deferred processing
static void ulExampleInterruptHandler(void *pvParameter1, uint32_t ulParameter)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Notify the handler task that the interrupt occurred
    vTaskNotifyGiveFromISR(xHandlerTask, &xHigherPriorityTaskWoken);

    // Perform context switch if needed
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Periodic task that generates interrupts and checks the flag
static void vPeriodicTask(void *pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(50);

    for (;;)
    {
        vTaskDelay(xDelay);


        // Check the flag set by the GPIO interrupt
        if (gpio_intr_flag)
        {
            gpio_intr_flag = false;  // Clear the flag
            vPrintString("Periodic task - About to notify interrupt task.\r\n");

            static uint32_t ulParameter = 0;
               BaseType_t xHigherPriorityTaskWoken = pdFALSE;

               // Queue the deferred function call (this will execute outside the ISR context)
               xTimerPendFunctionCallFromISR( ulExampleInterruptHandler, NULL, ulParameter, &xHigherPriorityTaskWoken);
               vTaskDelay(xDelay);
               vPrintString("Periodic task - Interrupt task notified.\r\n\r\n\r\n");
        }


    }
}

// Handler task that processes the notification
static void vHandlerTask(void *pvParameters)
{
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(100);
    uint32_t ulEventsToProcess;

    for (;;)
    {
        // Wait for a notification from the ISR
        ulEventsToProcess = ulTaskNotifyTake(pdTRUE, xMaxExpectedBlockTime);
        if (ulEventsToProcess != 0)
        {
            while (ulEventsToProcess > 0)
            {
                vPrintString("Handler task - Processing event.\r\n");
                ulEventsToProcess--;
            }
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

    // Initialize the user button (assumed to be connected to CYBSP_USER_BTN)
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYBSP_USER_BTN_DRIVE, CYBSP_BTN_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // GPIO initialization failed, stop here
    }

    // Configure GPIO interrupt for falling edge (button press detection)
    gpio_btn_callback_data.callback = gpio_interrupt_handler;
    cyhal_gpio_register_callback(CYBSP_USER_BTN, &gpio_btn_callback_data);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, GPIO_INTERRUPT_PRIORITY, true);

    // Enable global interrupts
    __enable_irq();

    // Create binary semaphore (not used here, but could be useful for synchronization)
    xBinarySemaphore = xSemaphoreCreateBinary();
    if (xBinarySemaphore == NULL)
    {
        CY_ASSERT(0);  // Semaphore creation failed, stop here
    }

    // Create the handler task
    if (xTaskCreate(vHandlerTask, "Handler Task", 1000, NULL, 3, &xHandlerTask) != pdPASS)
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
    for (;;)
    {
    }
}
