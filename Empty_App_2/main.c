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
 QueueHandle_t xQueue ;

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
 void vPrintStringAndNumber(const char *pcString,volatile uint32_t ul);
 void vPrintString(const char *pcString);
 void TaskSendDataToQueue(void *pvParameters);
 void TaskReceiveDataToQueue(void *pvParameters);
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
void vPrintStringAndNumber(const char *pcString, volatile uint32_t ul)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
    printf(" %ld\n", ul);  // Print the string to UART (Retarget IO)
}
// function to send data to the queue
void TaskSendDataToQueue(void *pvParameters){
	int32_t data1;
	data1 = (int32_t)pvParameters;
	BaseType_t xStatus;
	while(1){
		 xStatus=xQueueSendToBack(xQueue,&data1,0);
				 if(xStatus != pdPASS){
					 vPrintString( "Could not send to the queue.\r\n" );

			}
	}

}
//funtion to receive data from queue
void TaskReceiveDataToQueue(void *pvParameters){
    int32_t data1;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 500 );
	while(1){
		 char *pcTaskName="Received";

		if( uxQueueMessagesWaiting( xQueue ) != 0 )
		 {
		 vPrintString( "Queue should have been empty!\r\n" );
		 }
		xStatus=xQueueReceive(xQueue,&data1,xTicksToWait);
		 if(xStatus == pdPASS){

			 vPrintStringAndNumber(pcTaskName,data1);
			}
		 else{
			 vPrintString( "Could not send to the queue.\r\n" );
		 }

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

    xQueue = xQueueCreate(2,sizeof(int32_t));
    if(xQueue != NULL){
        		xTaskCreate(TaskSendDataToQueue,"Task1",1000,(void*)100,1,NULL);
        		xTaskCreate(TaskSendDataToQueue,"Task2",1000,(void*)200,1,NULL);
        		xTaskCreate(TaskReceiveDataToQueue,"Task3",1000,NULL,2,NULL);
        	}

    /* Start the scheduler so the tasks begin executing */
      vTaskStartScheduler();
    for (;;)
    {

    }
}
//means task 3 run after task 1 send data to queue queue size is 2 so task 2 also send the data to quesue now task 3 unblock then it print both data of queue it is correct
/* [] END OF FILE */
