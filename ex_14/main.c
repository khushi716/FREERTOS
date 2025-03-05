#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS(700)
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS(700)

TimerHandle_t xOneShotTimer, xAutoReloadTimer;
BaseType_t xTimer1Started, xTimer2Started;

void vPrintStringAndNumber(const char *pcString, uint32_t ul)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
    printf(" %ld\n", ul);  // Print the value to UART (Retarget IO)
}

// Timer callback function
static void prvTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow;
    uint32_t ulExecutionCount;

    // Retrieve the execution count from the Timer ID and increment it
    ulExecutionCount = (uint32_t)pvTimerGetTimerID(xTimer);
    ulExecutionCount++;

    // Update the Timer ID with the new execution count
    vTimerSetTimerID(xTimer, (void *)ulExecutionCount);

    // Check if it's the one-shot timer that expired
    if (xTimer == xOneShotTimer)
    {

        vPrintStringAndNumber("One-shot timer executed", ulExecutionCount);
    }
    // Check if it's the auto-reload timer that expired
    else
    {

        vPrintStringAndNumber("Auto-reload timer executed", ulExecutionCount);

        // Stop the auto-reload timer after 5 executions
        if (ulExecutionCount == 5)
        {

            xTimerStop(xTimer, 0);
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

    // Create timers with their respective periods
    xOneShotTimer = xTimerCreate("OneShot", mainONE_SHOT_TIMER_PERIOD, pdFALSE, 0, prvTimerCallback);
    xAutoReloadTimer = xTimerCreate("AutoReload", mainAUTO_RELOAD_TIMER_PERIOD, pdTRUE, 0, prvTimerCallback);

    // Start the timers if they were successfully created
    if (xOneShotTimer != NULL && xAutoReloadTimer != NULL)
    {
        xTimer1Started = xTimerStart(xOneShotTimer, 0);
        xTimer2Started = xTimerStart(xAutoReloadTimer, 0);

        vPrintStringAndNumber("Result of starting One-shot timer:", xTimer1Started);
        vPrintStringAndNumber("Result of starting Auto-reload timer:", xTimer2Started);

        // If both timers are started successfully, start the scheduler
        if (xTimer1Started == pdPASS && xTimer2Started == pdPASS)
        {
            vTaskStartScheduler();
        }
    }

    // Infinite loop in case the scheduler doesn't start
    for (;;);
}
