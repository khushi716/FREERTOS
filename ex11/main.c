
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
#include "FreeRtos.h"
#include "task.h"
#include "queue.h"

/******************************************************************************
* Macros
*******************************************************************************/


/*******************************************************************************
* Global Variables
*******************************************************************************/
 /* Declare two variables of type QueueHandle_t. Both queues are added to the same
queue set. */
static QueueHandle_t xQueue1 = NULL, xQueue2 = NULL;
/* Declare a variable of type QueueSetHandle_t. This is the queue set to which the
two queues are added. */
static QueueSetHandle_t xQueueSet = NULL;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/

 void vPrintString(const char *pcString);
 void vSenderTask1( void *pvParameters );
 void vSenderTask2( void *pvParameters );
 void vReceiverTask( void *pvParameters );

/*******************************************************************************
* Function Definitions
*******************************************************************************/
// * Function Name: vPrintString
// * Description: This function sends a string to UART using retargeted IO
// */
 void vPrintString(const char *pcString)
 {
     printf("%s", pcString);  // Print the string to UART (Retarget IO)
 }

// function to print and number

// function to send data to the queue
void vSenderTask1( void *pvParameters )
{
const TickType_t xBlockTime = pdMS_TO_TICKS( 100 );
const char * const pcMessage = "Message from vSenderTask1\r\n";
 /* As per most tasks, this task is implemented within an infinite loop. */
 for( ;; )
 {
 /* Block for 100ms. */
 vTaskDelay( xBlockTime );

 xQueueSend( xQueue1, &pcMessage, 0 );
 }
}


void vSenderTask2( void *pvParameters )
{
const TickType_t xBlockTime = pdMS_TO_TICKS( 200 );
const char * const pcMessage = "Message from vSenderTask2\r\n";
 /* As per most tasks, this task is implemented within an infinite loop. */
 for( ;; )
 {
 /* Block for 200ms. */
 vTaskDelay( xBlockTime );
 xQueueSend( xQueue2, &pcMessage, 0 );
 }
}

//funtion to receive data from queue
void vReceiverTask( void *pvParameters )
{
QueueHandle_t xQueueThatContainsData;
char *pcReceivedString;
 /* As per most tasks, this task is implemented within an infinite loop. */
 for( ;; )
 {

 xQueueThatContainsData = ( QueueHandle_t ) xQueueSelectFromSet( xQueueSet,
 portMAX_DELAY );

 xQueueReceive( xQueueThatContainsData, &pcReceivedString, 0 );

 vPrintString( pcReceivedString );
 }
}

/*******************************************************************************
* Function Name: main
*********************************************************************************
* Summary:
* This is the main function for CPU. It...
*    1.
*    2.
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;

#if defined (CY_DEVICE_SECURE) && defined (CY_USING_HAL)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    /* Initialize UART for retargeted IO (so we can print to UART) */
        result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
        if (result != CY_RSLT_SUCCESS)
        {
            CY_ASSERT(0);  // UART initialization failed, stop here
        }
    __enable_irq();


     xQueue1 = xQueueCreate( 1, sizeof( char * ) );
     xQueue2 = xQueueCreate( 1, sizeof( char * ) );

     xQueueSet = xQueueCreateSet( 1 * 2 );

     xQueueAddToSet( xQueue1, xQueueSet );
     xQueueAddToSet( xQueue2, xQueueSet );

     xTaskCreate( vSenderTask1, "Sender1", 1000, NULL, 1, NULL );
     xTaskCreate( vSenderTask2, "Sender2", 1000, NULL, 1, NULL );

     xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 2, NULL );

     vTaskStartScheduler();

     for( ;; );
     return 0;

}
//means task 3 run after task 1 send data to queue queue size is 2 so task 2 also send the data to quesue now task 3 unblock then it print both data of queue it is correct
/* [] END OF FILE */









