


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
#define mainTHIRD_TASK_BIT (1UL << 2UL)

/*******************************************************************************
* Global Variables
*******************************************************************************/
EventGroupHandle_t xEventGroup;

static void vEventTaskSynchronous(void* pvParameters);

/*******************************************************************************
* Function Definitions
*******************************************************************************/
// Print string function
void vPrintStrings(const char *pcString1, const char *pcString2)
{
    printf("%s", pcString1);
    printf("%s", pcString2);
}

static void vEventTaskSynchronous(void* pvParameters)
{
    // The event bit for this task
    EventBits_t uxSyncTask = (EventBits_t)pvParameters;

    // Wait for all bits to be set (including this task's specific bit)
    EventBits_t uxAllSet = (mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainTHIRD_TASK_BIT);

    for (;;)
    {
    	vTaskDelay(pdMS_TO_TICKS(300));
        vPrintStrings( pcTaskGetName(NULL),"reach synchonize point\r\n");

        // Set the event bit for this task and wait for all three bits to be set
        xEventGroupSync(xEventGroup, uxSyncTask, uxAllSet, portMAX_DELAY);

        vPrintStrings( pcTaskGetName(NULL),"exit synchonize point\r\n");
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

/*********************************************************************************
* Function Name: main
**********************************************************************************
* Summary:
* The main function initializes the board, creates the tasks, and starts the FreeRTOS scheduler.
*
* Parameters:
*  void
*
* Return:
*  int
**********************************************************************************/
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

    // Enable global interrupts
    __enable_irq();

    xEventGroup = xEventGroupCreate();


    xTaskCreate(vEventTaskSynchronous, "Task 1\t", 1000, (void*)mainFIRST_TASK_BIT, 1, NULL);
    xTaskCreate(vEventTaskSynchronous, "Task 2\t", 1000, (void*)mainSECOND_TASK_BIT, 1, NULL);
    xTaskCreate(vEventTaskSynchronous, "Task 3\t", 1000, (void*)mainTHIRD_TASK_BIT, 1, NULL);


    vTaskStartScheduler();

    // Infinite loop in case scheduler fails to start
    for (;;)
    {
    }
}





