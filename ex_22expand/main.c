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
#include "event_groups.h"
#include "semphr.h"

/*******************************************************************************
* Macros
*******************************************************************************/
#define mainFIRST_TASK_BIT (1UL << 0UL)
#define mainSECOND_TASK_BIT (1UL << 1UL)
#define mainISR_BIT (1UL << 2UL)
#define GPIO_INTERRUPT_PRIORITY (7u)

/*******************************************************************************
* Global Variables
*******************************************************************************/
EventGroupHandle_t xEventGroup;
volatile bool gpio_intr_flag = false;  // Flag set by GPIO interrupt
cyhal_gpio_callback_data_t gpio_btn_callback_data;  // GPIO callback data
SemaphoreHandle_t new_Sem;
/*******************************************************************************
* Function Definitions
*******************************************************************************/
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

// Event Bit Setting Task
static void vEventBitSettingTask(void *pvParameters)
{
    const TickType_t xDelay200ms = pdMS_TO_TICKS(200UL);
    for (;;)
    {
      if(xSemaphoreTake(new_Sem, portMAX_DELAY) == pdTRUE){
    	  vTaskDelay(xDelay200ms);
    	        vPrintString("going to set bit 0...\r\n");
    	        xEventGroupSetBits(xEventGroup, mainFIRST_TASK_BIT);
    	        vTaskDelay(xDelay200ms);
    	        vPrintString("going to set bit 1...\r\n");
    	        xEventGroupSetBits(xEventGroup, mainSECOND_TASK_BIT);
      }
    }
}

// GPIO interrupt handler
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    gpio_intr_flag = true;  // Set flag when button is pressed

}

// Interrupt Event Bit Reading Task
static void vInterruptEventBitReadingTask(void *pvParameters)
{
    const TickType_t xDelay200ms = pdMS_TO_TICKS(200UL);
    for (;;)
    {
        if (gpio_intr_flag)
        {
            gpio_intr_flag = false;  // Reset the flag
            vTaskDelay(xDelay200ms);  // Debounce the GPIO interrupt
            vPrintString("going to set bit 2...\r\n");
            xEventGroupSetBits(xEventGroup, mainISR_BIT);  // Set event bit 2

        }
        vTaskDelay(10);  // Yield periodically to avoid blocking other tasks
    }
}

// Event Bit Reading Task
static void vEventBitReadingTask(void *pvParameters)
{
    EventBits_t xEventGroupValue;
    const EventBits_t xBitsToWaitFor = (mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainISR_BIT);
    for (;;)
    {
        // Block to wait for event bits to become set within the event group
        xEventGroupValue = xEventGroupWaitBits(xEventGroup, xBitsToWaitFor, pdTRUE,  pdTRUE, portMAX_DELAY);

        // Print a message for each bit that was set
        if ((xEventGroupValue & mainFIRST_TASK_BIT) != 0)
        {
            vPrintString("Event bit 0 was set\r\n");
        }
        if ((xEventGroupValue & mainSECOND_TASK_BIT) != 0)
        {
            vPrintString("Event bit 1 was set\r\n");
        }
        if ((xEventGroupValue & mainISR_BIT) != 0)
        {
            vPrintString("Event bit 2 was set\r\n");
        }
        xSemaphoreGive(new_Sem);
    }
}

/*******************************************************************************
* Function Name: main
********************************************************************************
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

#if defined(CY_DEVICE_SECURE) && defined(CY_USING_HAL)
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

    // Create event group
    xEventGroup = xEventGroupCreate();
    new_Sem = xSemaphoreCreateMutex();
      if (new_Sem == NULL)
      {
          CY_ASSERT(0);  // Mutex creation failed, halt execution
      }
    // Create tasks
    xTaskCreate(vEventBitSettingTask, "Bit Setter", 1000, NULL, 1, NULL);
    xTaskCreate(vEventBitReadingTask, "Bit Reader", 1000, NULL, 2, NULL);
    xTaskCreate(vInterruptEventBitReadingTask, "Interrupt Bit Setter", 1000, NULL, 3, NULL);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Infinite loop in case scheduler fails to start
    for (;;)
    {
    }
}




