#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS( 3333 )
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS( 500 )

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
 void vPrintStringAndNumber(const char *pcString, uint32_t ul);
 static  void prvOneShotTimerCallback( TimerHandle_t xTimer );
 static void prvAutoReloadTimerCallback( TimerHandle_t xTimer );
/*******************************************************************************
* Global Variables
*******************************************************************************/


/*******************************************************************************
* Function Definitions
*******************************************************************************/
// function to print and number
void vPrintStringAndNumber(const char *pcString, uint32_t ul)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
    printf(" %ld\n", ul);  // Print the string to UART (Retarget IO)
}

static void prvOneShotTimerCallback( TimerHandle_t xTimer )
{
    TickType_t xTimeNow;
    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();
    vPrintStringAndNumber( "One-shot timer callback executing at time", xTimeNow );
}

static void prvAutoReloadTimerCallback( TimerHandle_t xTimer )
{
    TickType_t xTimeNow;
    /* Obtain the current tick count. */
    xTimeNow = xTaskGetTickCount();
    vPrintStringAndNumber( "Auto-reload timer callback executing at time", xTimeNow );
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

    // Create timer handles
    TimerHandle_t xAutoReloadTimer, xOneShotTimer;
    BaseType_t xTimer1Started, xTimer2Started;

    // Print the tick count before timers start
    TickType_t xTickCountBeforeTimers = xTaskGetTickCount();
    vPrintStringAndNumber("Tick count before starting timers:", xTickCountBeforeTimers);

    // Create One-shot Timer
    xOneShotTimer = xTimerCreate(
        "OneShot",
        mainONE_SHOT_TIMER_PERIOD,
        pdFALSE,  // One-shot timer (no auto-reload)
        0,
        prvOneShotTimerCallback
    );

    // Create Auto-Reload Timer
    xAutoReloadTimer = xTimerCreate(
        "AutoReload",
        mainAUTO_RELOAD_TIMER_PERIOD,
        pdTRUE,  // Auto-reload timer
        0,
        prvAutoReloadTimerCallback
    );

    /* Check the software timers were created. */
    if (xOneShotTimer != NULL && xAutoReloadTimer != NULL)
    {
        // Start both timers
        xTimer1Started = xTimerStart(xOneShotTimer, 0);
        xTimer2Started = xTimerStart(xAutoReloadTimer, 0);

        // Print the result of starting timers
        vPrintStringAndNumber("Result of starting One-shot timer:", xTimer1Started);
        vPrintStringAndNumber("Result of starting Auto-reload timer:", xTimer2Started);

        if ((xTimer1Started == pdPASS) && (xTimer2Started == pdPASS))
        {
            // Start the scheduler
            vTaskStartScheduler();
        }
    }
    /* As always, this line should not be reached. */
    for (;;);

    return 0;
}
