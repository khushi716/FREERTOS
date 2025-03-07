


#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS(500)
#define mainONE_SHOT_TIMER_PERIOD1 pdMS_TO_TICKS(700)
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS(200)
#define GPIO_INTERRUPT_PRIORITY (7u)

volatile bool gpio_intr_flag = false;
cyhal_gpio_callback_data_t gpio_btn_callback_data;
BaseType_t xTimer1Started;
TimerHandle_t xOneShotTimer;
void vPrintString(const char *pcString)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
}

void vPrintStringAndNumber(const char *pcString, uint32_t ul)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
    printf(" %ld\n", ul);  // Print the string to UART (Retarget IO)
}


static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    gpio_intr_flag = true;  // Set the flag when the button is pressed

}

static void gpio_timer_callback(TimerHandle_t xTimer)
{
    if (gpio_intr_flag)
    {
        gpio_intr_flag = false;  // Clear the flag
        TickType_t xTimeNow = xTaskGetTickCount();
        vPrintStringAndNumber("GPIO interrupt handled by timer at time", xTimeNow);  // Print current time
        xTimer1Started = xTimerStart(xOneShotTimer, 0);
    }
//    add functionality after 500 ticks one shot timer executed



}

static void prvOneShotTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow = xTaskGetTickCount();
    vPrintStringAndNumber("One-shot timer callback executing at time", xTimeNow);
}

static void prvAutoReloadTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow = xTaskGetTickCount();
    vPrintStringAndNumber("Auto-reload timer callback executing at time", xTimeNow);
}

int main(void)
{
    cy_rslt_t result;

    // Initialize the device and board peripherals
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Initialization failed, stop here
    }

    // Initialize retarget-io to use the debug UART port
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // UART initialization failed, stop here
        vPrintString("UART Initialization failed!\n");
    }
    else
    {
        vPrintString("UART Initialized successfully.\n");
    }

    // Initialize the user button
    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYBSP_USER_BTN_DRIVE, CYBSP_BTN_OFF);

    if (result != CY_RSLT_SUCCESS)
       {
           CY_ASSERT(0);  // UART initialization failed, stop here
           vPrintString("button Initialization failed!\n");
       }
       else
       {
           vPrintString("button Initialized successfully.\n");
       }
    // Configure GPIO interrupt
    gpio_btn_callback_data.callback = gpio_interrupt_handler;
    cyhal_gpio_register_callback(CYBSP_USER_BTN, &gpio_btn_callback_data);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, GPIO_INTERRUPT_PRIORITY, true);

    // Enable global interrupts
    __enable_irq();

    TimerHandle_t xAutoReloadTimer, xGpioTimer;
    BaseType_t  xTimer2Started, xTimer3Started;

    TickType_t xTickCountBeforeTimers = xTaskGetTickCount();
    vPrintStringAndNumber("Tick count before starting timers:", xTickCountBeforeTimers);

    xOneShotTimer = xTimerCreate("OneShot", mainONE_SHOT_TIMER_PERIOD, pdFALSE, 0, prvOneShotTimerCallback);
    xAutoReloadTimer = xTimerCreate("AutoReload", mainAUTO_RELOAD_TIMER_PERIOD, pdTRUE, 0, prvAutoReloadTimerCallback);
    xGpioTimer = xTimerCreate("GpioTimer", mainONE_SHOT_TIMER_PERIOD1, pdTRUE, 0, gpio_timer_callback);

    if (xOneShotTimer != NULL)
    {
        vPrintString("One-shot timer created successfully.\n");
    }
    else
    {
        vPrintString("Failed to create One-shot timer.\n");
    }

    if (xAutoReloadTimer != NULL)
    {
        vPrintString("Auto-reload timer created successfully.\n");
    }
    else
    {
        vPrintString("Failed to create Auto-reload timer.\n");
    }

    if (xGpioTimer != NULL)
    {
        vPrintString("GPIO timer created successfully.\n");
    }
    else
    {
        vPrintString("Failed to create GPIO timer.\n");
    }

    xTimer1Started = xTimerStart(xOneShotTimer, 0);
    xTimer2Started = xTimerStart(xAutoReloadTimer, 0);
    xTimer3Started = xTimerStart(xGpioTimer, 0);

    vPrintStringAndNumber("Result of starting One-shot timer:", xTimer1Started);
    vPrintStringAndNumber("Result of starting Auto-reload timer:", xTimer2Started);
    vPrintStringAndNumber("Result of starting GPIO checking timer:", xTimer3Started);

    if (xTimer1Started == pdPASS && xTimer2Started == pdPASS && xTimer3Started == pdPASS)
    {
        vPrintString("Timers started successfully, starting scheduler...\n");
        vTaskStartScheduler();
    }
    else
    {
        vPrintString("Failed to start one or more timers.\n");
    }

    // Infinite loop if the scheduler doesn't start
    for (;;)
    {
        vPrintString("Main loop running...\n"); // Keep the program alive if the scheduler doesn't start
    }
}
