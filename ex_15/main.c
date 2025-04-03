//#include "cybsp.h"
//#include "cyhal.h"
//#include "cy_retarget_io.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"
//
//#define GPIO_INTERRUPT_PRIORITY (7u)
//#define BACKLIGHT_TIMEOUT pdMS_TO_TICKS(5000)  // Time for backlight to turn off (5 seconds)
//
//volatile bool gpio_intr_flag = false;  // Flag set by GPIO interrupt
//BaseType_t xSimulatedBacklightOn =pdFALSE; // Simulated backlight state
//TimerHandle_t xBacklightTimer;  // Timer handle
//
//cyhal_gpio_callback_data_t gpio_btn_callback_data;  // GPIO callback data
//
//// Print string function
//void vPrintString(const char *pcString)
//{
//    printf("%s", pcString);  // Print string to UART
//}
//
//// Print string and number function
//void vPrintStringAndNumber(const char *pcString, uint32_t ul)
//{
//    printf("%s", pcString);  // Print string to UART
//    printf(" %ld\n", ul);  // Print the number to UART
//}
//
//// GPIO interrupt handler
//static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
//{
//    gpio_intr_flag = true;  // Set flag when button is pressed
//    vPrintString("GPIO Interrupt triggered\n");
//}
//
//// Backlight timer callback function
//static void prvBacklightTimerCallback(TimerHandle_t xTimer)
//{
//    TickType_t xTimeNow = xTaskGetTickCount();
//    // The backlight timer expired, turn the backlight off
//    xSimulatedBacklightOn = pdFALSE;
//    // Print the time at which the backlight was turned off
//    vPrintStringAndNumber("Timer expired, turning backlight OFF at time\t\t", xTimeNow);
//}
//
//// Key hit task (this task now checks the GPIO interrupt flag)
//static void vKeyHitTask(void *pvParameters)
//{
//    const TickType_t xShortDelay = pdMS_TO_TICKS(50);  // Short delay to prevent excessive CPU usage
//
//    for (;;)
//    {
//        // If the GPIO interrupt flag is set, handle the interrupt
//        if (gpio_intr_flag)
//        {
//            TickType_t xTimeNow = xTaskGetTickCount();
//
//            // Backlight handling logic
//            if (xSimulatedBacklightOn == pdFALSE)
//            {
//                // Turn on the backlight and print the time
//                xSimulatedBacklightOn = pdTRUE;
//                vPrintStringAndNumber("Button pressed, turning backlight ON at time\t\t", xTimeNow);
//            }
//            else
//            {
//                // Backlight already on, reset the timer and print time
//                vPrintStringAndNumber("Button pressed, resetting timer at time\t\t", xTimeNow);
//            }
//
//            // Reset the backlight timer
//            xTimerReset(xBacklightTimer, xShortDelay);
//
//            // Clear the interrupt flag after processing
//            gpio_intr_flag = false;
//        }
//
//        // Delay to simulate some work being done in the task
//        vTaskDelay(xShortDelay);
//    }
//}
//
//int main(void)
//{
//    cy_rslt_t result;
//
//    // Initialize the device and board peripherals
//    result = cybsp_init();
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Initialization failed, stop here
//    }
//
//    // Initialize retarget-io for UART
//    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // UART initialization failed, stop here
//    }
//
//    // Initialize the user button (assumed to be connected to CYBSP_USER_BTN)
//    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYBSP_USER_BTN_DRIVE, CYBSP_BTN_OFF);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // GPIO initialization failed, stop here
//    }
//
//    // Configure GPIO interrupt for falling edge (button press detection)
//    gpio_btn_callback_data.callback = gpio_interrupt_handler;
//    cyhal_gpio_register_callback(CYBSP_USER_BTN, &gpio_btn_callback_data);
//    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, GPIO_INTERRUPT_PRIORITY, true);
//
//    // Enable global interrupts
//    __enable_irq();
//
//    // Create the backlight timer
//    xBacklightTimer = xTimerCreate("BacklightTimer", BACKLIGHT_TIMEOUT, pdFALSE, 0, prvBacklightTimerCallback);
//    if (xBacklightTimer != NULL)
//    {
//        // Start the timer
//        if (xTimerStart(xBacklightTimer, 0) != pdPASS)
//        {
//            CY_ASSERT(0);  // Timer failed to start, stop here
//        }
//    }
//
//    // Create the key hit task
//    xTaskCreate(vKeyHitTask, "KeyHitTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
//
//    // Start the scheduler
//    vTaskStartScheduler();
//
//    // Infinite loop in case the scheduler fails to start
//    for (;;);
//}
//
//
//
#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#define DELAY_SHORT_MS          (250)   /* milliseconds */
#define DELAY_LONG_MS           (500)   /* milliseconds */
#define LED_BLINK_COUNT         (4)
#define GPIO_INTERRUPT_PRIORITY (7u)
#define BACKLIGHT_TIMEOUT pdMS_TO_TICKS(5000)  // Time for backlight to turn off (5 seconds)
cyhal_i2c_t i2c_obj;

volatile bool gpio_intr_flag = false;  // Flag set by GPIO interrupt
BaseType_t xSimulatedBacklightOn =pdFALSE; // Simulated backlight state
TimerHandle_t xBacklightTimer;  // Timer handle

cyhal_gpio_callback_data_t gpio_btn_callback_data;  // GPIO callback data

// Print string function
void vPrintString(const char *pcString)
{
    printf("%s", pcString);  // Print string to UART
}

// Print string and number function
void vPrintStringAndNumber(const char *pcString, uint32_t ul)
{
    printf("%s", pcString);  // Print string to UART
    printf(" %ld\n", ul);  // Print the number to UART
}

// GPIO interrupt handler
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    gpio_intr_flag = true;  // Set flag when button is pressed
    vPrintString("GPIO Interrupt triggered\n");
}

// Backlight timer callback function
static void prvBacklightTimerCallback(TimerHandle_t xTimer)
{
    TickType_t xTimeNow = xTaskGetTickCount();
    // The backlight timer expired, turn the backlight off
    xSimulatedBacklightOn = pdFALSE;
    // Print the time at which the backlight was turned off
    vPrintStringAndNumber("Timer expired, turning backlight OFF at time\t\t", xTimeNow);
    cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
}

// Key hit task (this task now checks the GPIO interrupt flag)
static void vKeyHitTask(void *pvParameters)
{
    const TickType_t xShortDelay = pdMS_TO_TICKS(50);  // Short delay to prevent excessive CPU usage

    for (;;)
    {
        // If the GPIO interrupt flag is set, handle the interrupt
        if (gpio_intr_flag)
        {
        	 cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
            TickType_t xTimeNow = xTaskGetTickCount();

            // Backlight handling logic
            if (xSimulatedBacklightOn == pdFALSE)
            {
                // Turn on the backlight and print the time
                xSimulatedBacklightOn = pdTRUE;
                vPrintStringAndNumber("Button pressed, turning backlight ON at time\t\t", xTimeNow);
            }
            else
            {
                // Backlight already on, reset the timer and print time
                vPrintStringAndNumber("Button pressed, resetting timer at time\t\t", xTimeNow);
            }

            // Reset the backlight timer
            xTimerReset(xBacklightTimer, xShortDelay);

            // Clear the interrupt flag after processing
            gpio_intr_flag = false;
        }

        // Delay to simulate some work being done in the task
        vTaskDelay(xShortDelay);
    }
}

int main(void)
{
    cy_rslt_t result;

     uint32_t delay_led_blink = DELAY_LONG_MS;
    // Initialize the device and board peripherals
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Initialization failed, stop here
    }

    // Initialize retarget-io for UART
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // UART initialization failed, stop here
    }
    result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                     CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
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

    // Create the backlight timer
    xBacklightTimer = xTimerCreate("BacklightTimer", BACKLIGHT_TIMEOUT, pdFALSE, 0, prvBacklightTimerCallback);
    if (xBacklightTimer != NULL)
    {
        // Start the timer
        if (xTimerStart(xBacklightTimer, 0) != pdPASS)
        {
            CY_ASSERT(0);  // Timer failed to start, stop here
        }
    }

    // Create the key hit task
    xTaskCreate(vKeyHitTask, "KeyHitTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    // Infinite loop in case the scheduler fails to start
    for (;;);
}



