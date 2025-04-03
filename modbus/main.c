//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include "modbus.h"
//#include "APP_modbus.h"
//#include "APP_modbus_port.h"
//#include "cy_retarget_io.h"
//#include <inttypes.h>
//modbusSlave_t modbusSlave;
//tAPP_All_Mesaurement_Results tMesurementsResult; // Structure to hold measurement results
//// Example transmit callback for Modbus
//cb_transmitPacket transmitCallback;
//
//// Define the actual callback function
//uint32_t cb_transmitPacketImpl(uint8_t *pdata, uint16_t datalength, enumModbusPktType packetTyp)
//{
//    // Transmit each byte of Modbus packet over UART
//return vPrintStringAndNumberfloat("result",pdata);
//}
///* Macros for ADC Configuration */
//#define VPLUS_CHANNEL_0  (P10_0)   // Define the ADC input pin connected to potentiometer wiper
//
///* PWM Frequency = 2Hz */
//#define PWM_FREQUENCY (10000u)
//
///* PWM Duty-cycle = 50% */
//#define PWM_DUTY_CYCLE (30.0f)
//
///* ADC Object */
//cyhal_adc_t adc_obj;
//
///* ADC Channel 0 Object */
//cyhal_adc_channel_t adc_chan_0_obj;
//extern cyhal_pwm_t pwm_led_control;
//
///* ADC Configuration */
//const cyhal_adc_config_t adc_config = {
//    .continuous_scanning = false,    // Disable continuous scanning
//    .average_count = 1,              // Disable averaging
//    .vref = CYHAL_ADC_REF_VDDA,      // Use VDDA (3.3V from the microcontroller) as the reference voltage
//    .vneg = CYHAL_ADC_VNEG_VSSA,     // VNEG for Single-ended channel set to VSSA (Ground)
//    .resolution = 12u,               // 12-bit resolution
//    .ext_vref = NC,                  // No external VREF
//    .bypass_pin = NC                 // No bypass pin
//};
//
///* Function Prototypes */
//void adc_single_channel_init(void);
//void adc_single_channel_process(void);
//void check_status(char *message, cy_rslt_t status)
//{
//    if (CY_RSLT_SUCCESS != status)
//    {
//        printf("\r\n=====================================================\r\n");
//        printf("\nFAIL: %s\r\n", message);
//        printf("Error Code: 0x%08" PRIX32 "\n", status);
//        printf("\r\n=====================================================\r\n");
//
//        while(true);
//    }
//}
//
///* Main Function */
//int main(void)
//{
//    cy_rslt_t result;
//
//#if defined(CY_DEVICE_SECURE)
//    cyhal_wdt_t wdt_obj;
//    /* Clear watchdog timer so that it doesn't trigger a reset */
//    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
//    CY_ASSERT(CY_RSLT_SUCCESS == result);
//    cyhal_wdt_free(&wdt_obj);
//#endif
//
//    /* Initialize the device and board peripherals */
//    result = cybsp_init();
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Halt if initialization fails
//    }
//
//    /* Enable global interrupts */
//    __enable_irq();
//
//    /* Initialize retarget-io to use the debug UART port */
//    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Halt if UART initialization fails
//    }
//
//    /* Print initialization message */
//    printf("\x1b[2J\x1b[;H");
//    printf("-----------------------------------------------------------\r\n");
//    printf("HAL: ADC using HAL\r\n");
//    printf("-----------------------------------------------------------\r\n\n");
//
//    /* Initialize the single-ended ADC channel */
//    adc_single_channel_init();
//
//    /* Configure ADC */
//    result = cyhal_adc_configure(&adc_obj, &adc_config);
//    if(result != CY_RSLT_SUCCESS)
//    {
//        printf("ADC configuration update failed. Error: %ld\n", (long unsigned int)result);
//        CY_ASSERT(0);  // Halt if ADC configuration fails
//    }
//
//    /* Initialize the PWM outside the loop */
//    result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
//    check_status("API cyhal_pwm_init failed with error code", result);
//
//    /* Set the PWM output frequency and duty cycle */
//    result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE, PWM_FREQUENCY);
//    check_status("API cyhal_pwm_set_duty_cycle failed with error code", result);
//
//    /* Start the PWM */
//    result = cyhal_pwm_start(&pwm_led_control);
//    check_status("API cyhal_pwm_start failed with error code", result);
//    // Assign the callback function to the function pointer
//               transmitCallback = cb_transmitPacketImpl;
//
//               // Initialize Modbus Slave and pass the callback function
//               vAPP_ModbusSlaveInit(&modbusSlave, transmitCallback);;
//
//           	// Process Modbus requests
//           	        modbusProcess(&modbusSlave);  // Handle incoming Modbus requests
//
//
//    for (;;)
//    {
//        /* Read and process ADC channel */
//        adc_single_channel_process();
//        cyhal_system_delay_ms(200);  // Delay for 200ms between readings
//        // Send Modbus response after processing
//               if (modbusSlave.packetAvailbleForProcessing)
//               {
//                   // Call the function to transmit the response packet
//                   ulAPP_ModbusTrasmitPacket(modbusSlave.pTxBuffer, modbusSlave.lengthTxBuffer, modbusSlave.mbusPacketTyp);
//               }
//
//    }
//}
//// Function to print strings
//void vPrintString(const char *pcString)
//{
//    printf("%s", pcString);
//}
//void vPrintStringAndNumber(const char *pcString, uint32_t ul)
//{
//    printf("%s", pcString);  // Print the string to UART (Retarget IO)
//    printf(" %ld\n", ul);  // Print the value to UART (Retarget IO)
//}
//
//void vPrintStringAndNumberfloat(const char *pcString, float ul)
//{
//    printf("%s", pcString);  // Print the string to UART (Retarget IO)
//    printf(" %f\n", ul);  // Print the value to UART (Retarget IO)
//}
//
///* Initialize ADC for single-ended mode */
//void adc_single_channel_init(void)
//{
//    cy_rslt_t result;
//
//    /* Initialize the ADC object for the channel 0 input pin */
//    result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_0, NULL);
//    if(result != CY_RSLT_SUCCESS)
//    {
//        printf("ADC initialization failed. Error: %ld\n", (long unsigned int)result);
//        CY_ASSERT(0);  // Halt if ADC initialization fails
//    }
//
//    /* ADC channel configuration */
//    const cyhal_adc_channel_config_t channel_config = {
//        .enable_averaging = false,  // Disable averaging
//        .min_acquisition_ns = 1000, // Minimum acquisition time set to 1us
//        .enabled = true             // Enable the channel for ADC sampling
//    };
//
//    /* Initialize channel 0 to scan the channel 0 input pin in single-ended mode */
//    result = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, VPLUS_CHANNEL_0, CYHAL_ADC_VNEG, &channel_config);
//    if(result != CY_RSLT_SUCCESS)
//    {
//        printf("ADC single-ended channel initialization failed. Error: %ld\n", (long unsigned int)result);
//        CY_ASSERT(0);  // Halt if ADC channel initialization fails
//    }
//
//    printf("ADC is configured in single-channel configuration\r\n\n");
//    printf("Provide input voltage at the channel 0 input pin. \r\n\n");
//}
//
///* Process ADC readings from channel 0 */
//void adc_single_channel_process(void)
//{
//    int32_t adc_result_0 = 0;
//
//    /* Read ADC value and convert it to millivolts */
//    adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;
//
//    float duty_cycle = (float)adc_result_0 / 3300.0f * 100.0f;
//    cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
//    check_status("API cyhal_pwm_set_duty_cycle failed with error code", result);
//
//    /* Print the ADC result */
//    printf("Channel 0 input: %4ldmV\r\n", (long int)adc_result_0);
//}

//
//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include "modbus.h"
//#include "APP_modbus.h"
//#include <inttypes.h>
//
//modbusSlave_t modbusSlave;
//tAPP_All_Mesaurement_Results tMesurementsResult; // Structure to hold measurement results
//
///* Macros for ADC Configuration */
//#define VPLUS_CHANNEL_0  (P10_0)   // Define the ADC input pin connected to potentiometer wiper
//
///* PWM Frequency = 10kHz */
//#define PWM_FREQUENCY (10000u)
//
///* PWM Duty-cycle */
//#define PWM_DUTY_CYCLE (30.0f)  // Initial duty cycle value
//
///* ADC Object */
//cyhal_adc_t adc_obj;
//
///* ADC Channel 0 Object */
//cyhal_adc_channel_t adc_chan_0_obj;
//extern cyhal_pwm_t pwm_led_control;
///** APP modbus port transmit packet */
//static tAPP_Modbus_RX_TX_Data tModbusTxPkt;
///* ADC Configuration */
//const cyhal_adc_config_t adc_config = {
//    .continuous_scanning = false,    // Disable continuous scanning
//    .average_count = 1,              // Disable averaging
//    .vref = CYHAL_ADC_REF_VDDA,      // Use VDDA (3.3V from the microcontroller) as the reference voltage
//    .vneg = CYHAL_ADC_VNEG_VSSA,     // VNEG for Single-ended channel set to VSSA (Ground)
//    .resolution = 12u,               // 12-bit resolution
//    .ext_vref = NC,                  // No external VREF
//    .bypass_pin = NC                 // No bypass pin
//};
//
///* Function Prototypes */
//void check_status(char *message, cy_rslt_t status);
//void adc_single_channel_init(void);
//void adc_single_channel_process(void);
//void modbusInit(void);
//void modbusProcessRequests(void);
//void vPrintNumber( uint32_t ul );
//
//
///* Main Function */
//int main(void)
//{
//    cy_rslt_t result;
//
//#if defined(CY_DEVICE_SECURE)
//    cyhal_wdt_t wdt_obj;
//    /* Clear watchdog timer so that it doesn't trigger a reset */
//    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
//    CY_ASSERT(CY_RSLT_SUCCESS == result);
//    cyhal_wdt_free(&wdt_obj);
//#endif
//
//    /* Initialize the device and board peripherals */
//    result = cybsp_init();
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Halt if initialization fails
//    }
//
//    /* Enable global interrupts */
//    __enable_irq();
//
//
//
//    /* Initialize retarget-io to use the debug UART port */
//    result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX,
//                                        CYBSP_DEBUG_UART_RX,
//                                        CYBSP_DEBUG_UART_CTS,
//                                        CYBSP_DEBUG_UART_RTS,
//                                        CY_RETARGET_IO_BAUDRATE);
//
//    /* Print initialization message */
//    printf("\x1b[2J\x1b[;H");
//    printf("-----------------------------------------------------------\r\n");
//    printf("HAL: ADC using HAL\r\n");
//    printf("-----------------------------------------------------------\r\n\n");
//
//    // Initialize ADC and Modbus
//    adc_single_channel_init();  // Initialize ADC
//    modbusInit();  // Initialize Modbus slave
//
//    // Initialize PWM outside the loop
//    result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
//    check_status("API cyhal_pwm_init failed with error code", result);
//
//
//
//    // Main loop
//    for (;;)
//    {
//
//
//
//
//        // Process ADC values and update Modbus registers
//         adc_single_channel_process();  // Process ADC and update Modbus registers
//
//         // Process Modbus requests (handle incoming Modbus packets)
//                modbusProcessRequests();  // Process Modbus requests
//        cyhal_system_delay_ms(1200);
//
//    }
//}
//
//// Function to check the status of peripheral initialization
//void check_status(char *message, cy_rslt_t status)
//{
//    if (CY_RSLT_SUCCESS != status)
//    {
//        printf("\r\n=====================================================\r\n");
//        printf("\nFAIL: %s\r\n", message);
//        printf("Error Code: 0x%08" PRIX32 "\n", status);
//        printf("\r\n=====================================================\r\n");
//
//        while(true);
//    }
//}
//
///*******************************************************************************
// * Function Name: vAPP_ModbusPaketReceived
// *******************************************************************************
// * Summary:
// * This function is used when the any packet received on the modbus.
// *
// * Parameters:
// * modbusSlave_t *pMbusSlave	: Pointer of the modbus slave.
// * void *pDataBuff				: Received data buffer of the modbus slave.
// * uint16_t wDataLen			: Data length.
// *
// * Return:
// *  None
// *
// *******************************************************************************/
//
//void vAPP_ModbusPaketReceived( modbusSlave_t *pMbusSlave, void *pDataBuff, uint8_t wDataLen )
//{
//	modbusSlaveOnRxPacket(pMbusSlave, PACKET_RTU, (uint8_t*)pDataBuff, wDataLen);
//}
//
///*******************************************************************************
// * Function Name: ulAPP_ModbusTrasmitPacket
// *******************************************************************************
// * Summary:
// * This function is used when the any packet transmit on the modbus.
// *
// * Parameters:
// * uint8_t *pTransmitData			: Buffer data that need to transfer on modbus
// * uint16_t wDataLength             : Data length
// * enumModbusPktType ePacketType	: Packet type
// *
// * Return:
// *  uint32_t status
// *
// *******************************************************************************/
//uint32_t ulAPP_ModbusTrasmitPacket( uint8_t *pTransmitData, uint8_t wDataLength, enumModbusPktType ePacketType )
//{
//	uint8_t bLenToWrite;
//	// clear the transfer buffer
//	memset(&tModbusTxPkt, 0x00, sizeof(tAPP_Modbus_RX_TX_Data));
//	// Copy data into the transfer packet
//	memcpy(tModbusTxPkt.buffer, pTransmitData, wDataLength);
//	tModbusTxPkt.length = wDataLength;
//	memcpy(abSendResponseData, tModbusTxPkt.buffer, tModbusTxPkt.length);
//
//        for( bLenToWrite = APP_CMD_RESET_VALUE; bLenToWrite < tModbusTxPkt.length; bLenToWrite ++ )
//        {
//        	printf("%d\r\n", abSendResponseData[bLenToWrite]);
//        }
//
//        memset( abSendResponseData, APP_CMD_RESET_VALUE, APP_BUFFER_DATA_LEN);
//
//
//	return 0;
//}
//
///* Initialize ADC for single-ended mode */
//void adc_single_channel_init(void)
//{
//    cy_rslt_t result;
//
//    /* Initialize the ADC object for the channel 0 input pin */
//    result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_0, NULL);
//    if(result != CY_RSLT_SUCCESS)
//    {
//        printf("ADC initialization failed. Error: %ld\n", (long unsigned int)result);
//        CY_ASSERT(0);  // Halt if ADC initialization fails
//    }
//
//    /* ADC channel configuration */
//    const cyhal_adc_channel_config_t channel_config = {
//        .enable_averaging = false,  // Disable averaging
//        .min_acquisition_ns = 1000, // Minimum acquisition time set to 1us
//        .enabled = true             // Enable the channel for ADC sampling
//    };
//
//    /* Initialize channel 0 to scan the channel 0 input pin in single-ended mode */
//    result = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, VPLUS_CHANNEL_0, CYHAL_ADC_VNEG, &channel_config);
//    if(result != CY_RSLT_SUCCESS)
//    {
//        printf("ADC single-ended channel initialization failed. Error: %ld\n", (long unsigned int)result);
//        CY_ASSERT(0);  // Halt if ADC channel initialization fails
//    }
//
//    printf("ADC is configured in single-channel configuration\r\n\n");
//    printf("Provide input voltage at the channel 0 input pin. \r\n\n");
//}
//
//
//
//
///* Process ADC readings from channel 0 and update Modbus register */
//void adc_single_channel_process(void)
//{
//	int32_t adc_result_0 = 0;
//
//	/* Read ADC value and convert it to millivolts */
//	adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;
//	   printf("%ld\r\n", (long int)adc_result_0);
//
//	uint8_t *pRcxBuffer;  /* Receive buffer pointer */
//
//	/* Adjust PWM duty cycle based on ADC value */
//	float duty_cycle = (float)adc_result_0 / 3300.0f * 100.0f;
//
//	/* Convert duty cycle to 8-bit value */
//	uint8_t duty_cycle_8bit = (uint8_t)(duty_cycle * 2.55f);
//
//	/* Set PWM duty cycle */
//	cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
//	check_status("cyhal_pwm_set_duty_cycle failed with error code", result);
//
//	/* Add duty cycle to Modbus receive buffer */
//	pRcxBuffer = &duty_cycle_8bit;  /* Assign pointer to duty cycle byte */
//	uint8_t rece_length = 1;  /* Length of the received packet (1 byte for duty cycle) */
//
//	/* Send data to Modbus */
//	modbusSlaveOnRxPacket(&modbusSlave, 0, &pRcxBuffer, rece_length);
//
//	/* Start the PWM */
//	result = cyhal_pwm_start(&pwm_led_control);
//	check_status("API cyhal_pwm_start failed with error code", result);
//
//    check_status("cyhal_pwm_set_duty_cycle failed with error code", result);
//    // Start the PWM
//        result = cyhal_pwm_start(&pwm_led_control);
//        check_status("API cyhal_pwm_start failed with error code", result);
//}
///* Initialize Modbus */
//void modbusInit(void)
//{
//
//    // Initialize Modbus Slave and pass the callback function
//    vAPP_ModbusSlaveInit(&modbusSlave, ulAPP_ModbusTrasmitPacket);
//}
///* Process Modbus requests */
//void modbusProcessRequests(void)
//{
//    // Process Modbus requests (called periodically in the main loop)
//    modbusProcess(&modbusSlave);  // Handle incoming Modbus requests
//}
//
//void vPrintNumber( uint32_t ul)
//{
//    printf(" %ld\n", ul);  // Print the value to UART (Retarget IO)
//}
//

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "modbus.h"
#include "APP_modbus.h"
#include <inttypes.h>
#define DATA_BITS_8     8
#define STOP_BITS_1     1
#define BAUD_RATE       9600
#define UART_DELAY      10u

#define UART_RX CYBSP_D2
#define UART_TX CYBSP_D3
    // Variable Declarations
    cy_rslt_t    rslt;
    cyhal_uart_t uart_obj;
    uint32_t     actualbaud;
    uint8_t modbus_rx_buffer[256];
    uint8_t modbus_tx_buffer[256];
    uint32_t     value;

    // Initialize the UART configuration structure
    const cyhal_uart_cfg_t uart_config =
    {
        .data_bits      = DATA_BITS_8,
        .stop_bits      = STOP_BITS_1,
        .parity         = CYHAL_UART_PARITY_NONE,
		.rx_buffer = modbus_rx_buffer,
	     .rx_buffer_size = sizeof(modbus_rx_buffer)
    };


modbusSlave_t modbusSlave;
tAPP_All_Mesaurement_Results tMesurementsResult; // Structure to hold measurement results

/* Macros for ADC Configuration */
#define VPLUS_CHANNEL_0  (P10_0)   // Define the ADC input pin connected to potentiometer wiper

/* PWM Frequency = 10kHz */
#define PWM_FREQUENCY (10000u)

/* PWM Duty-cycle */
#define PWM_DUTY_CYCLE (30.0f)  // Initial duty cycle value

/* ADC Object */
cyhal_adc_t adc_obj;

/* ADC Channel 0 Object */
cyhal_adc_channel_t adc_chan_0_obj;
extern cyhal_pwm_t pwm_led_control;
/** APP modbus port transmit packet */
static tAPP_Modbus_RX_TX_Data tModbusTxPkt;
/* ADC Configuration */
const cyhal_adc_config_t adc_config = {
    .continuous_scanning = false,    // Disable continuous scanning
    .average_count = 1,              // Disable averaging
    .vref = CYHAL_ADC_REF_VDDA,      // Use VDDA (3.3V from the microcontroller) as the reference voltage
    .vneg = CYHAL_ADC_VNEG_VSSA,     // VNEG for Single-ended channel set to VSSA (Ground)
    .resolution = 12u,               // 12-bit resolution
    .ext_vref = NC,                  // No external VREF
    .bypass_pin = NC                 // No bypass pin
};
// Register Map for Modbus
regMap_t holdingRegMap[2] = {
    {0x0001, 1, &tMesurementsResult.adc_v, NULL, NULL}, // Holding register for ADC value
};

regMap_t inputRegMap[2] = {
    {0x0003, 1, &tMesurementsResult.adc_v, NULL, NULL}, // Input register for ADC value
};


/* Function Prototypes */
void check_status(char *message, cy_rslt_t status);
void adc_single_channel_init(void);
void adc_single_channel_process(void);
void modbusInit(void);
void modbusProcessRequests(void);
void vPrintNumber( uint32_t ul );
void modbus_uart_handler(void);

/* Main Function */
int main(void)
{
    cy_rslt_t result;

#if defined(CY_DEVICE_SECURE)
    cyhal_wdt_t wdt_obj;
    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if initialization fails
    }

    /* Enable global interrupts */
    __enable_irq();

    // Initialize the UART Block
        rslt = cyhal_uart_init(&uart_obj, UART_TX, UART_RX, NC, NC, NULL,
                               &uart_config);

        // Set the baud rate
        rslt = cyhal_uart_set_baud(&uart_obj, BAUD_RATE, &actualbaud);


    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX,
                                        CYBSP_DEBUG_UART_RX,
                                        CYBSP_DEBUG_UART_CTS,
                                        CYBSP_DEBUG_UART_RTS,
                                        CY_RETARGET_IO_BAUDRATE);

    /* Print initialization message */
    printf("\x1b[2J\x1b[;H");
    printf("-----------------------------------------------------------\r\n");
    printf("HAL: ADC using HAL\r\n");
    printf("-----------------------------------------------------------\r\n\n");

    // Initialize ADC and Modbus
    adc_single_channel_init();  // Initialize ADC
    /* Configure ADC */
        result = cyhal_adc_configure(&adc_obj, &adc_config);
        check_status("ADC configuration failed", result);
    modbusInit();  // Initialize Modbus slave

    // Initialize PWM outside the loop
    result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
    check_status("API cyhal_pwm_init failed with error code", result);



    // Main loop
    for (;;)
    {




         adc_single_channel_process();  // Process ADC and update Modbus registers
         /* Handle Modbus communication */
//         modbus_uart_handler();

         // Process Modbus requests (handle incoming Modbus packets)
         modbusProcessRequests();  // Process Modbus requests
        cyhal_system_delay_ms(200);

    }
}

// Function to check the status of peripheral initialization
void check_status(char *message, cy_rslt_t status)
{
    if (CY_RSLT_SUCCESS != status)
    {
        printf("\r\n=====================================================\r\n");
        printf("\nFAIL: %s\r\n", message);
        printf("Error Code: 0x%08" PRIX32 "\n", status);
        printf("\r\n=====================================================\r\n");

        while(true);
    }
}


void modbus_uart_handler(void)
{
    static uint8_t rx_buffer[256];
    static uint16_t rx_index = 0;
    uint8_t rx_data;

    /* Check for received data */
    while (cyhal_uart_readable(&uart_obj))
    {
        cyhal_uart_getc(&uart_obj, &rx_data, 0);
        rx_buffer[rx_index++] = rx_data;

            /* Process received packet */
        uint32_t status = modbusSlaveOnRxPacket(&modbusSlave, PACKET_RTU, rx_buffer, rx_index);

        if(status != MODBUS_OK) {
            printf("Modbus error: 0x%lX\n", status);
            // Handle specific error codes

        }
            rx_index = 0;

    }
}
/* Modbus transmit callback function */
uint32_t modbus_transmit_packet(uint8_t *pdata, uint16_t datalength, enumModbusPktType packetType)
{
    /* Send each byte via UART */
    for (uint16_t i = 0; i < datalength; i++)
    {
        cyhal_uart_putc(&uart_obj, pdata[i]);
        cyhal_system_delay_ms(1);  // Small delay between bytes
    }

    return MODBUS_OK;
}

/*******************************************************************************
 * Function Name: ulAPP_ModbusTrasmitPacket
 *******************************************************************************
 * Summary:
 * This function is used when the any packet transmit on the modbus.
 *
 * Parameters:
 * uint8_t *pTransmitData			: Buffer data that need to transfer on modbus
 * uint16_t wDataLength             : Data length
 * enumModbusPktType ePacketType	: Packet type
 *
 * Return:
 *  uint32_t status
 *
 *******************************************************************************/
uint32_t ulAPP_ModbusTrasmitPacket( uint8_t *pTransmitData, uint8_t wDataLength, enumModbusPktType ePacketType )
{
	uint8_t bLenToWrite;
	// clear the transfer buffer
	memset(&tModbusTxPkt, 0x00, sizeof(tAPP_Modbus_RX_TX_Data));
	// Copy data into the transfer packet
	memcpy(tModbusTxPkt.buffer, pTransmitData, wDataLength);
	tModbusTxPkt.length = wDataLength;
	memcpy(abSendResponseData, tModbusTxPkt.buffer, tModbusTxPkt.length);

        for( bLenToWrite = APP_CMD_RESET_VALUE; bLenToWrite < tModbusTxPkt.length; bLenToWrite ++ )
        {
     	    cyhal_uart_write(&uart_obj,abSendResponseData[bLenToWrite], tModbusTxPkt.length);


//        	printf("%d\r\n", abSendResponseData[bLenToWrite]);
        }

        memset( abSendResponseData, APP_CMD_RESET_VALUE, APP_BUFFER_DATA_LEN);


	return 0;
}


/* Initialize ADC for single-ended mode */
void adc_single_channel_init(void)
{
    cy_rslt_t result;

    /* Initialize the ADC object for the channel 0 input pin */
    result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_0, NULL);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);  // Halt if ADC initialization fails
    }

    /* ADC channel configuration */
    const cyhal_adc_channel_config_t channel_config = {
        .enable_averaging = false,  // Disable averaging
        .min_acquisition_ns = 1000, // Minimum acquisition time set to 1us
        .enabled = true             // Enable the channel for ADC sampling
    };

    /* Initialize channel 0 to scan the channel 0 input pin in single-ended mode */
    result = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, VPLUS_CHANNEL_0, CYHAL_ADC_VNEG, &channel_config);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC single-ended channel initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);  // Halt if ADC channel initialization fails
    }

    printf("ADC is configured in single-channel configuration\r\n\n");
    printf("Provide input voltage at the channel 0 input pin. \r\n\n");
}




/* Process ADC readings from channel 0 and update Modbus register */
void adc_single_channel_process(void)
{
	int32_t adc_result_0 = 0;

	/* Read ADC value and convert it to millivolts */
	adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;


	/* Adjust PWM duty cycle based on ADC value */
	float duty_cycle = (float)adc_result_0 / 3300.0f * 100.0f;

	/* Convert duty cycle to 8-bit value */
	uint8_t duty_cycle_8bit = (uint8_t)(duty_cycle );
	  printf("%ld\r\n", (long int)duty_cycle_8bit);

	/* Set PWM duty cycle */
	cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
	check_status("cyhal_pwm_set_duty_cycle failed with error code", result);
	tMesurementsResult.adc_v = duty_cycle_8bit;  // Assign the ADC value to the register

	/* Start the PWM */
	result = cyhal_pwm_start(&pwm_led_control);
	check_status("API cyhal_pwm_start failed with error code", result);

    }
/* Initialize Modbus */
void modbusInit(void)
{
	 // Initialize Modbus Slave and pass the callback function for transmitting Modbus packets
	    vAPP_ModbusSlaveInit(&modbusSlave, modbus_transmit_packet);
	    // Set up holding registers map (read/write)
	    modbusSlaveSetMapHoldingReg(&modbusSlave, holdingRegMap, 2);

	    // Set up input registers map (read-only)
	    modbusSlaveSetMapInputReg(&modbusSlave, inputRegMap, 2);

}
/* Process Modbus requests */
void modbusProcessRequests(void)
{
    // Process Modbus requests (called periodically in the main loop)
    modbusProcess(&modbusSlave);  // Handle incoming Modbus requests
      // Transmit the response
    ulAPP_ModbusTrasmitPacket(modbus_rx_buffer,  sizeof(modbus_rx_buffer), PACKET_RTU);
}

void vPrintNumber( uint32_t ul)
{
    printf(" %ld\n", ul);  // Print the value to UART (Retarget IO)
}
//
//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include "modbus.h"
//#include "APP_modbus.h"
//#include <inttypes.h>
//
//
//
//
//modbusSlave_t modbusSlave;
//tAPP_All_Mesaurement_Results tMesurementsResult; // Structure to hold measurement results
//
///* Macros for ADC Configuration */
//#define VPLUS_CHANNEL_0  (P10_0)   // Define the ADC input pin connected to potentiometer wiper
//
///* PWM Frequency = 10kHz */
//#define PWM_FREQUENCY (10000u)
//
///* PWM Duty-cycle */
//#define PWM_DUTY_CYCLE (30.0f)  // Initial duty cycle value
//
///* ADC Object */
//cyhal_adc_t adc_obj;
//
///* ADC Channel 0 Object */
//cyhal_adc_channel_t adc_chan_0_obj;
//extern cyhal_pwm_t pwm_led_control;
///** APP modbus port transmit packet */
//static tAPP_Modbus_RX_TX_Data tModbusTxPkt;
///* ADC Configuration */
//const cyhal_adc_config_t adc_config = {
//    .continuous_scanning = false,    // Disable continuous scanning
//    .average_count = 1,              // Disable averaging
//    .vref = CYHAL_ADC_REF_VDDA,      // Use VDDA (3.3V from the microcontroller) as the reference voltage
//    .vneg = CYHAL_ADC_VNEG_VSSA,     // VNEG for Single-ended channel set to VSSA (Ground)
//    .resolution = 12u,               // 12-bit resolution
//    .ext_vref = NC,                  // No external VREF
//    .bypass_pin = NC                 // No bypass pin
//};
//// Register Map for Modbus
//regMap_t holdingRegMap[2] = {
//    {0x0001, 1, &tMesurementsResult.adc_v, NULL, NULL}, // Holding register for ADC value
//};
//
//regMap_t inputRegMap[2] = {
//    {0x0003, 1, &tMesurementsResult.adc_v, NULL, NULL}, // Input register for ADC value
//};
//
//
///* Function Prototypes */
//void check_status(char *message, cy_rslt_t status);
//void adc_single_channel_init(void);
//void adc_single_channel_process(void);
//void modbusInit(void);
//void modbusProcessRequests(void);
//void vPrintNumber( uint32_t ul );
//void modbus_uart_handler(void);
//
///* Main Function */
//int main(void)
//{
//    cy_rslt_t result;
//
//#if defined(CY_DEVICE_SECURE)
//    cyhal_wdt_t wdt_obj;
//    /* Clear watchdog timer so that it doesn't trigger a reset */
//    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
//    CY_ASSERT(CY_RSLT_SUCCESS == result);
//    cyhal_wdt_free(&wdt_obj);
//#endif
//
//    /* Initialize the device and board peripherals */
//    result = cybsp_init();
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Halt if initialization fails
//    }
//
//    /* Enable global interrupts */
//    __enable_irq();
//
//
//    /* Initialize retarget-io to use the debug UART port */
//    result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX,
//                                        CYBSP_DEBUG_UART_RX,
//                                        CYBSP_DEBUG_UART_CTS,
//                                        CYBSP_DEBUG_UART_RTS,
//                                        CY_RETARGET_IO_BAUDRATE);
//
//    /* Print initialization message */
//    printf("\x1b[2J\x1b[;H");
//    printf("-----------------------------------------------------------\r\n");
//    printf("HAL: ADC using HAL\r\n");
//    printf("-----------------------------------------------------------\r\n\n");
//
//    // Initialize ADC and Modbus
//    adc_single_channel_init();  // Initialize ADC
//    /* Configure ADC */
//        result = cyhal_adc_configure(&adc_obj, &adc_config);
//        check_status("ADC configuration failed", result);
//    modbusInit();  // Initialize Modbus slave
//
//    // Initialize PWM outside the loop
//    result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
//    check_status("API cyhal_pwm_init failed with error code", result);
//
//
//
//    // Main loop
//    for (;;)
//    {
//
//
//
//
//         adc_single_channel_process();  // Process ADC and update Modbus registers
//         /* Handle Modbus communication */
//         modbus_uart_handler();
//
//         // Process Modbus requests (handle incoming Modbus packets)
//         modbusProcessRequests();  // Process Modbus requests
//        cyhal_system_delay_ms(200);
//
//    }
//}
//
//// Function to check the status of peripheral initialization
//void check_status(char *message, cy_rslt_t status)
//{
//    if (CY_RSLT_SUCCESS != status)
//    {
//        printf("\r\n=====================================================\r\n");
//        printf("\nFAIL: %s\r\n", message);
//        printf("Error Code: 0x%08" PRIX32 "\n", status);
//        printf("\r\n=====================================================\r\n");
//
//        while(true);
//    }
//}
//
//
//void modbus_uart_handler(void)
//{
//    static uint8_t rx_buffer[256];
//    static uint16_t rx_index = 0;
//    uint8_t rx_data;
//
//    /* Check for received data */
//    while (cyhal_uart_readable(&cy_retarget_io_uart_obj))
//    {
//        cyhal_uart_getc(&cy_retarget_io_uart_obj, &rx_data, 0);
//        rx_buffer[rx_index++] = rx_data;
//
//            /* Process received packet */
//            modbusSlaveOnRxPacket(&modbusSlave, PACKET_RTU, rx_buffer, rx_index);
//            rx_index = 0;
//
//    }
//}
///* Modbus transmit callback function */
//uint32_t modbus_transmit_packet(uint8_t *pdata, uint16_t datalength, enumModbusPktType packetType)
//{
//    /* Send each byte via UART */
//    for (uint16_t i = 0; i < datalength; i++)
//    {
//        cyhal_uart_putc(&cy_retarget_io_uart_obj, pdata[i]);
//        cyhal_system_delay_ms(1);  // Small delay between bytes
//    }
//
//    return MODBUS_OK;
//}
///*******************************************************************************
// * Function Name: ulAPP_ModbusTrasmitPacket
// *******************************************************************************
// * Summary:
// * This function is used when the any packet transmit on the modbus.
// *
// * Parameters:
// * uint8_t *pTransmitData			: Buffer data that need to transfer on modbus
// * uint16_t wDataLength             : Data length
// * enumModbusPktType ePacketType	: Packet type
// *
// * Return:
// *  uint32_t status
// *
// *******************************************************************************/
//uint32_t ulAPP_ModbusTrasmitPacket( uint8_t *pTransmitData, uint8_t wDataLength, enumModbusPktType ePacketType )
//{
//	uint8_t bLenToWrite;
//	// clear the transfer buffer
//	memset(&tModbusTxPkt, 0x00, sizeof(tAPP_Modbus_RX_TX_Data));
//	// Copy data into the transfer packet
//	memcpy(tModbusTxPkt.buffer, pTransmitData, wDataLength);
//	tModbusTxPkt.length = wDataLength;
//	memcpy(abSendResponseData, tModbusTxPkt.buffer, tModbusTxPkt.length);
//
//        for( bLenToWrite = APP_CMD_RESET_VALUE; bLenToWrite < tModbusTxPkt.length; bLenToWrite ++ )
//        {
//     	    cyhal_uart_write(&cy_retarget_io_uart_obj,abSendResponseData[bLenToWrite], tModbusTxPkt.length);
//
//
//        }
//
//        memset( abSendResponseData, APP_CMD_RESET_VALUE, APP_BUFFER_DATA_LEN);
//
//
//	return 0;
//}
//
//
///* Initialize ADC for single-ended mode */
//void adc_single_channel_init(void)
//{
//    cy_rslt_t result;
//
//    /* Initialize the ADC object for the channel 0 input pin */
//    result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_0, NULL);
//    if(result != CY_RSLT_SUCCESS)
//    {
//        printf("ADC initialization failed. Error: %ld\n", (long unsigned int)result);
//        CY_ASSERT(0);  // Halt if ADC initialization fails
//    }
//
//    /* ADC channel configuration */
//    const cyhal_adc_channel_config_t channel_config = {
//        .enable_averaging = false,  // Disable averaging
//        .min_acquisition_ns = 1000, // Minimum acquisition time set to 1us
//        .enabled = true             // Enable the channel for ADC sampling
//    };
//
//    /* Initialize channel 0 to scan the channel 0 input pin in single-ended mode */
//    result = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, VPLUS_CHANNEL_0, CYHAL_ADC_VNEG, &channel_config);
//    if(result != CY_RSLT_SUCCESS)
//    {
//        printf("ADC single-ended channel initialization failed. Error: %ld\n", (long unsigned int)result);
//        CY_ASSERT(0);  // Halt if ADC channel initialization fails
//    }
//
//    printf("ADC is configured in single-channel configuration\r\n\n");
//    printf("Provide input voltage at the channel 0 input pin. \r\n\n");
//}
//
//
//
//
///* Process ADC readings from channel 0 and update Modbus register */
//void adc_single_channel_process(void)
//{
//	int32_t adc_result_0 = 0;
//
//	/* Read ADC value and convert it to millivolts */
//	adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;
//
//
//	/* Adjust PWM duty cycle based on ADC value */
//	float duty_cycle = (float)adc_result_0 / 3300.0f * 100.0f;
//
//	/* Convert duty cycle to 8-bit value */
//	uint8_t duty_cycle_8bit = (uint8_t)(duty_cycle );
//	  printf("%ld\r\n", (long int)duty_cycle_8bit);
//
//	/* Set PWM duty cycle */
//	cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
//	check_status("cyhal_pwm_set_duty_cycle failed with error code", result);
//	tMesurementsResult.adc_v = duty_cycle_8bit;  // Assign the ADC value to the register
//
//	/* Start the PWM */
//	result = cyhal_pwm_start(&pwm_led_control);
//	check_status("API cyhal_pwm_start failed with error code", result);
//
//    }
///* Initialize Modbus */
//void modbusInit(void)
//{
//	 // Initialize Modbus Slave and pass the callback function for transmitting Modbus packets
//	    vAPP_ModbusSlaveInit(&modbusSlave, modbus_transmit_packet);
//
//	    // Set up holding registers map (read/write)
//	    modbusSlaveSetMapHoldingReg(&modbusSlave, holdingRegMap, 2);
//
//	    // Set up input registers map (read-only)
//	    modbusSlaveSetMapInputReg(&modbusSlave, inputRegMap, 2);
//
//}
///* Process Modbus requests */
//void modbusProcessRequests(void)
//{
//    // Process Modbus requests (called periodically in the main loop)
//    modbusProcess(&modbusSlave);  // Handle incoming Modbus requests
//    uint8_t responsePacket[256];
//    uint16_t responseLength = 8; // Length of the response packet
//
//      // Transmit the response
//    ulAPP_ModbusTrasmitPacket(responsePacket,  responseLength, PACKET_RTU);
//}
//
//void vPrintNumber( uint32_t ul)
//{
//    printf(" %ld\n", ul);  // Print the value to UART (Retarget IO)
//}
