

#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
// Task handles
TaskHandle_t xHandlerTask1;
TaskHandle_t xInterruptTask;
TaskHandle_t xHigherPriorityTaskHandle = NULL; // Task handle for higher priority task
QueueHandle_t  xIntegerQueue;
QueueHandle_t xStringQueue;
// Semaphore handle

void vPrintStringAndNumber(const char *pcString, uint32_t ul);
void vPortGenerateSimulatedInterrupt();
static void ulExampleInterruptHandler(void);


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
	TickType_t xLastExecutionTime;
	uint32_t ulValueToSend = 0;
	int i;
	 xLastExecutionTime = xTaskGetTickCount();


    for (;;)
    {
    	vTaskDelayUntil( &xLastExecutionTime, pdMS_TO_TICKS( 200 ) );

        for( i =0;i<5;i++){
        	xQueueSendToBack( xIntegerQueue ,&ulValueToSend,0);
        	ulValueToSend++;
        	vPrintStringAndNumber("integer",ulValueToSend);
        }
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

static void ulExampleInterruptHandler( void )
{
BaseType_t xHigherPriorityTaskWoken;
uint32_t ulReceivedNumber;

static const char *pcStrings[] =
{
 "String 0\r\n",
 "String 1\r\n",
 "String 2\r\n",
 "String 3\r\n"
};
 xHigherPriorityTaskWoken = pdFALSE;
 /* Read from the queue until the queue is empty. */
 while( xQueueReceiveFromISR( xIntegerQueue,&ulReceivedNumber,&xHigherPriorityTaskWoken ) != errQUEUE_EMPTY )
 {
 ulReceivedNumber &= 0x03;
 xQueueSendToBackFromISR( xStringQueue,
 &pcStrings[ ulReceivedNumber ],
&xHigherPriorityTaskWoken );
 }
 portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
 }

static void vStringPrinter(void*pvParameters){
	char *pcString;
	 for( ;; )
	 {
	 /* Block on the queue to wait for data to arrive. */
	 xQueueReceive( xStringQueue, &pcString, portMAX_DELAY );
	 /* Print out the string received. */
	 vPrintString( pcString );
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
    xIntegerQueue = xQueueCreate( 10, sizeof( uint32_t ) );
     xStringQueue = xQueueCreate( 10, sizeof( char * ) );

    // Create the higher-priority task with priority 2
    if ( xTaskCreate( vStringPrinter, "String", 1000, NULL, 2, NULL )!= pdPASS)
    {
        CY_ASSERT(0);  // Task creation failed, stop here
    }

    // Create the periodic task with lower priority (priority 1)

    if (xTaskCreate(vPeriodicTask, "Periodic Task", 1000, NULL, 1, &xInterruptTask) != pdPASS)
    {
        CY_ASSERT(0);  // Task creation failed, stop here
    }

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Infinite loop in case the scheduler doesn't start
    for (;;);
}

