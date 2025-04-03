////
/////* Main application entry */
////#include "cy_pdl.h"
////#include "cyhal.h"
////#include "cybsp.h"
////#include "cy_retarget_io.h"
////#include "sht2x_for_stm32_hal.h"
////#include "FreeRTOS.h"
////#include "task.h"
////#include "event_groups.h"
////#include "semphr.h"
////#include "queue.h"
////#include <inttypes.h>
////
////#define GPIO_INTERRUPT_PRIORITY (7u)
////volatile bool gpio_intr_flag = false;  // Flag set by GPIO interrupt
////cyhal_gpio_callback_data_t gpio_btn_callback_data;  // GPIO callback data
////cy_rslt_t result;
////
////
/////* Macros for ADC Configuration */
////#define VPLUS_CHANNEL_0  (P10_0)   // Define the ADC input pin connected to potentiometer wiper
////
/////* PWM Frequency = 10kHz */
////#define PWM_FREQUENCY (10000u)
////
/////* PWM Duty-cycle = 50% */
////#define PWM_DUTY_CYCLE (30.0f)
////
/////* ADC Object */
////cyhal_adc_t adc_obj;
////
/////* ADC Channel 0 Object */
////cyhal_adc_channel_t adc_chan_0_obj;
////cyhal_pwm_t pwm_led_control;
////
/////* ADC Configuration */
////const cyhal_adc_config_t adc_config = {
////    .continuous_scanning = false,    // Disable continuous scanning
////    .average_count = 1,              // Disable averaging
////    .vref = CYHAL_ADC_REF_VDDA,      // Use VDDA (3.3V from the microcontroller) as the reference voltage
////    .vneg = CYHAL_ADC_VNEG_VSSA,     // VNEG for Single-ended channel set to VSSA (Ground)
////    .resolution = 12u,               // 12-bit resolution
////    .ext_vref = NC,                  // No external VREF
////    .bypass_pin = NC                 // No bypass pin
////};
////
/////* Function Prototypes */
////void adc_single_channel_init(void);
////void adc_single_channel_process(void);
////
////// GPIO interrupt handler
////static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
////{
////    gpio_intr_flag = true;  // Set flag when button is pressed
////}
////
////void check_status(char *message, cy_rslt_t status)
////{
////    if (CY_RSLT_SUCCESS != status)
////    {
////        printf("\r\n=====================================================\r\n");
////        printf("\nFAIL: %s\r\n", message);
////        printf("Error Code: 0x%08" PRIX32 "\n", status);
////        printf("\r\n=====================================================\r\n");
////        cyhal_gpio_write(CYBSP_LED_RGB_RED, CYBSP_LED_STATE_ON);
////        cyhal_gpio_write(CYBSP_LED_RGB_BLUE, CYBSP_LED_STATE_OFF);
////        while(true);
////    }
////}
////
////static void manualhandling(void*pvParameters) {
//////        if(gpio_intr_flag){
////        // Reset the interrupt flag after processing
////        gpio_intr_flag = false;
////
////        // Initialize the single-ended ADC channel
////        adc_single_channel_init();
////
////        // Configure ADC
////        result = cyhal_adc_configure(&adc_obj, &adc_config);
////        if(result != CY_RSLT_SUCCESS)
////        {
////            printf("ADC configuration update failed. Error: %ld\n", (long unsigned int)result);
////            CY_ASSERT(0);  // Halt if ADC configuration fails
////        }
////
////        printf("Entering manual mode\n");
////        cyhal_gpio_write(CYBSP_LED_RGB_BLUE, CYBSP_LED_STATE_ON);
////
////        // Initialize the PWM outside the loop
////        result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
////        check_status("API cyhal_pwm_init failed with error code", result);
////
////        // Set the PWM output frequency and duty cycle
////        result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE, PWM_FREQUENCY);
////        check_status("API cyhal_pwm_set_duty_cycle failed with error code", result);
////
////        // Start the PWM
////        result = cyhal_pwm_start(&pwm_led_control);
////        check_status("API cyhal_pwm_start failed with error code", result);
////
////        for (;;) {
////            // Read and process ADC channel
////            adc_single_channel_process();
////            cyhal_system_delay_ms(200);  // Delay for 200ms between readings
////        }}
//////    }
////
////
////int main(void) {
////    /* Initialize the device and I2C */
////    cybsp_init();
////
////    /* Initialize retarget-io to use the debug UART port */
////    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
////    if (result != CY_RSLT_SUCCESS)
////    {
////        CY_ASSERT(0);  // Halt if UART initialization fails
////    }
////
////    // Initialize the user button (assumed to be connected to CYBSP_USER_BTN)
////    result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYBSP_USER_BTN_DRIVE, CYBSP_BTN_OFF);
////    if (result != CY_RSLT_SUCCESS)
////    {
////        CY_ASSERT(0);  // GPIO initialization failed, stop here
////    }
////
////    // Configure GPIO interrupt for falling edge (button press detection)
////    gpio_btn_callback_data.callback = gpio_interrupt_handler;
////    cyhal_gpio_register_callback(CYBSP_USER_BTN, &gpio_btn_callback_data);
////    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, GPIO_INTERRUPT_PRIORITY, true);
////    __enable_irq();
////
////    // Initialize Blue LED
////    result = cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
////    if (result != CY_RSLT_SUCCESS)
////    {
////        CY_ASSERT(0);  // GPIO initialization failed, stop here
////    }
////    // Initialize red LED
////        result = cyhal_gpio_init(CYBSP_LED_RGB_RED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
////        if (result != CY_RSLT_SUCCESS)
////        {
////            CY_ASSERT(0);  // GPIO initialization failed, stop here
////        }
////
////
////
////    // Create a FreeRTOS task for manual handling
////    xTaskCreate(manualhandling, "Manual Handling", 1000, NULL,NULL, NULL);
////
////    // Start the FreeRTOS scheduler
////    vTaskStartScheduler();
////
////    // Main loop - infinite loop to allow FreeRTOS to take over
////    for(;;) {
////        /* Loop forever */
////    }
////}
////
/////* Initialize ADC for single-ended mode */
////void adc_single_channel_init(void)
////{
////    cy_rslt_t result;
////
////    /* Initialize the ADC object for the channel 0 input pin */
////    result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_0, NULL);
////    if(result != CY_RSLT_SUCCESS)
////    {
////        printf("ADC initialization failed. Error: %ld\n", (long unsigned int)result);
////        CY_ASSERT(0);  // Halt if ADC initialization fails
////    }
////
////    /* ADC channel configuration */
////    const cyhal_adc_channel_config_t channel_config = {
////        .enable_averaging = false,  // Disable averaging
////        .min_acquisition_ns = 1000, // Minimum acquisition time set to 1us
////        .enabled = true             // Enable the channel for ADC sampling
////    };
////
////    /* Initialize channel 0 to scan the channel 0 input pin in single-ended mode */
////    result = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, VPLUS_CHANNEL_0, CYHAL_ADC_VNEG, &channel_config);
////    if(result != CY_RSLT_SUCCESS)
////    {
////        printf("ADC single-ended channel initialization failed. Error: %ld\n", (long unsigned int)result);
////        CY_ASSERT(0);  // Halt if ADC channel initialization fails
////    }
////
////    printf("ADC is configured in single-channel configuration\r\n\n");
////    printf("Provide input voltage at the channel 0 input pin. \r\n\n");
////}
////
/////* Process ADC readings from channel 0 */
////void adc_single_channel_process(void)
////{
////    int32_t adc_result_0 = 0;
////
////    /* Read ADC value and convert it to millivolts */
////    adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;
////
////    float duty_cycle = (float)adc_result_0 / 3300.0f * 100.0f;
////    cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
////    check_status("API cyhal_pwm_set_duty_cycle failed with error code", result);
////
////    /* Print the ADC result */
////    printf("Channel 0 input: %4ldmV\r\n", (long int)adc_result_0);
////}
////
////#include "cy_pdl.h"
////#include "cyhal.h"
////#include "cybsp.h"
////#include "cy_retarget_io.h"
////#include "FreeRTOS.h"
////#include "task.h"
////#include "event_groups.h"
////#include "semphr.h"
////#include "queue.h"
////#include "adc_init.h"
////#include "modbus.h"
////#include "uart.h"
////#include "APP_modbus.h"
////#include "APP_modbus_port.h"
////
////
////cy_rslt_t rslt;
////cy_rslt_t result;
////// Modbus Slave Instance
////modbusSlave_t modbusSlave;
////tAPP_All_Mesaurement_Results tMesurementsResult; // Structure to hold measurement results
////// Example transmit callback for Modbus
////cb_transmitPacket transmitCallback;
////
////// Define the actual callback function
////uint32_t cb_transmitPacketImpl(uint8_t *pdata, uint16_t datalength, enumModbusPktType packetTyp)
////{
////    // Transmit each byte of Modbus packet over UART
////    return uart_transmit_packet(pdata, datalength);
////}
////static void manualhandling(void* pvParameters)
////{
////    // Initialize ADC and PWM
////    adc_single_channel_init();
////    pwm_init();
////
////    // Assign the callback function to the function pointer
////        transmitCallback = cb_transmitPacketImpl;
////
////        // Initialize Modbus Slave and pass the callback function
////        vAPP_ModbusSlaveInit(&modbusSlave, transmitCallback);;
////
////    for (;;)
////    {
////    	// Process Modbus requests
////    	        modbusProcess(&modbusSlave);  // Handle incoming Modbus requests
////
////        // Read the ADC value and process it
////        adc_single_channel_process();  // This will update the PWM based on the ADC value
////        // Send Modbus response after processing
////        if (modbusSlave.packetAvailbleForProcessing)
////        {
////            // Call the function to transmit the response packet
////            ulAPP_ModbusTrasmitPacket(modbusSlave.pTxBuffer, modbusSlave.lengthTxBuffer, modbusSlave.mbusPacketTyp);
////        }
////
////        // Delay for a short period before reading again
////        cyhal_system_delay_ms(200);
////    }
////}
////
////
////
////// Main function
////int main(void) {
////    /* Initialize the device and I2C */
////    cybsp_init();
////    __enable_irq();
////
////    // Initialize UART
////    rslt = uart_init();
////    if (rslt != CY_RSLT_SUCCESS) {
////        printf("UART initialization failed\n");
////        return -1;
////    }
////
////    /* Initialize retarget-io to use the debug UART port */
////    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
////    if (result != CY_RSLT_SUCCESS) {
////        CY_ASSERT(0);  // Halt if UART initialization fails
////    }
////
////    // Initialize Blue LED
////    result = cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
////    if (result != CY_RSLT_SUCCESS) {
////        CY_ASSERT(0);  // GPIO initialization failed, stop here
////    }
////
////    // Create a FreeRTOS task for manual handling
////    xTaskCreate(manualhandling, "Manual Handling", 1000, NULL, 0, NULL);
////
////    // Start the FreeRTOS scheduler
////    vTaskStartScheduler();
////
////    // Main loop - infinite loop to allow FreeRTOS to take over
////    for(;;) {
////        /* Loop forever */
////    }
////}
////
//
//
//
//
//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "event_groups.h"
//#include "semphr.h"
//#include "queue.h"
//#include "adc_init.h"
//#include "modbus.h"
//#include "uart.h"
//#include "APP_modbus.h"
//#include "APP_modbus_port.h"
//
//// Modbus Slave Instance
//modbusSlave_t modbusSlave;
//tAPP_All_Mesaurement_Results tMesurementsResult; // Structure to hold measurement results
//
//// Example transmit callback for Modbus
//cb_transmitPacket transmitCallback;
//
//// Define the actual callback function
//uint32_t cb_transmitPacketImpl(uint8_t *pdata, uint16_t datalength, enumModbusPktType packetTyp)
//{
//    // Transmit each byte of Modbus packet over UART
//    return uart_transmit_packet(pdata, datalength);
//}
//
//static void manualhandling(void* pvParameters)
//{
//    // Initialize ADC and PWM
//    adc_single_channel_init();
//    pwm_init();
//
//    // Initialize Modbus Slave and assign the callback function
//    transmitCallback = cb_transmitPacketImpl;
//    vAPP_ModbusSlaveInit(&modbusSlave, transmitCallback);
//
//    for (;;)
//    {
//        // Process Modbus requests
//        modbusProcess(&modbusSlave);  // Handle incoming Modbus requests
//
//        // Read the ADC value and process it
//        adc_single_channel_process();  // This will update the PWM based on the ADC value
//
//
//
//        // Send Modbus response after processing
//        if (modbusSlave.packetAvailbleForProcessing)
//        {
//            // Call the function to transmit the response packet
//            ulAPP_ModbusTrasmitPacket(modbusSlave.pTxBuffer, modbusSlave.lengthTxBuffer, modbusSlave.mbusPacketTyp);
//        }
//
//        // Delay for a short period before reading again
//        cyhal_system_delay_ms(200);
//    }
//}
//
//// Main function
//int main(void)
//{
//    cy_rslt_t rslt, result;
//
//    // Initialize the device and I2C
//    cybsp_init();
//    __enable_irq();
//
//    // Initialize UART
//    rslt = uart_init();
//    if (rslt != CY_RSLT_SUCCESS)
//    {
//        printf("UART initialization failed\n");
//        return -1;
//    }
//
//    // Initialize retarget-io to use the debug UART port
////    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, 115200);
////    if (result != CY_RSLT_SUCCESS)
////    {
////        CY_ASSERT(0);  // Halt if UART initialization fails
////    }
//
//    // Initialize Blue LED
//    result = cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // GPIO initialization failed, stop here
//    }
//
//    // Create a FreeRTOS task for manual handling
//    xTaskCreate(manualhandling, "Manual Handling", 1000, NULL, 0, NULL);
//
//    // Start the FreeRTOS scheduler
//    vTaskStartScheduler();
//
//    // Main loop - infinite loop to allow FreeRTOS to take over
//    for (;;)
//    {
//        /* Loop forever */
//    }
//}


//
///* Main application entry */
//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//#include "sht2x_for_stm32_hal.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "event_groups.h"
//#include "semphr.h"
//#include "queue.h"
//#include <inttypes.h>
//
//#define GPIO_INTERRUPT_PRIORITY (7u)
//volatile bool gpio_intr_flag = false;  // Flag set by GPIO interrupt
//cyhal_gpio_callback_data_t gpio_btn_callback_data;  // GPIO callback data
//cy_rslt_t result;
//
//
///* Macros for ADC Configuration */
//#define VPLUS_CHANNEL_0  (P10_0)   // Define the ADC input pin connected to potentiometer wiper
//
///* PWM Frequency = 10kHz */
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
//cyhal_pwm_t pwm_led_control;
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
//
//// GPIO interrupt handler
//static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
//{
//    gpio_intr_flag = true;  // Set flag when button is pressed
//}
//
//void check_status(char *message, cy_rslt_t status)
//{
//    if (CY_RSLT_SUCCESS != status)
//    {
//        printf("\r\n=====================================================\r\n");
//        printf("\nFAIL: %s\r\n", message);
//        printf("Error Code: 0x%08" PRIX32 "\n", status);
//        printf("\r\n=====================================================\r\n");
//        cyhal_gpio_write(CYBSP_LED_RGB_RED, CYBSP_LED_STATE_ON);
//        cyhal_gpio_write(CYBSP_LED_RGB_BLUE, CYBSP_LED_STATE_OFF);
//        while(true);
//    }
//}
//
//static void manualhandling(void*pvParameters) {
////        if(gpio_intr_flag){
//        // Reset the interrupt flag after processing
//        gpio_intr_flag = false;
//
//        // Initialize the single-ended ADC channel
//        adc_single_channel_init();
//
//        // Configure ADC
//        result = cyhal_adc_configure(&adc_obj, &adc_config);
//        if(result != CY_RSLT_SUCCESS)
//        {
//            printf("ADC configuration update failed. Error: %ld\n", (long unsigned int)result);
//            CY_ASSERT(0);  // Halt if ADC configuration fails
//        }
//
//        printf("Entering manual mode\n");
//        cyhal_gpio_write(CYBSP_LED_RGB_BLUE, CYBSP_LED_STATE_ON);
//
//        // Initialize the PWM outside the loop
//        result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
//        check_status("API cyhal_pwm_init failed with error code", result);
//
//        // Set the PWM output frequency and duty cycle
//        result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE, PWM_FREQUENCY);
//        check_status("API cyhal_pwm_set_duty_cycle failed with error code", result);
//
//        // Start the PWM
//        result = cyhal_pwm_start(&pwm_led_control);
//        check_status("API cyhal_pwm_start failed with error code", result);
//
//        for (;;) {
//            // Read and process ADC channel
//            adc_single_channel_process();
//            cyhal_system_delay_ms(200);  // Delay for 200ms between readings
//        }}
////    }
//
//
//int main(void) {
//    /* Initialize the device and I2C */
//    cybsp_init();
//
//    /* Initialize retarget-io to use the debug UART port */
//    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // Halt if UART initialization fails
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
//    __enable_irq();
//
//    // Initialize Blue LED
//    result = cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        CY_ASSERT(0);  // GPIO initialization failed, stop here
//    }
//    // Initialize red LED
//        result = cyhal_gpio_init(CYBSP_LED_RGB_RED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
//        if (result != CY_RSLT_SUCCESS)
//        {
//            CY_ASSERT(0);  // GPIO initialization failed, stop here
//        }
//
//
//
//    // Create a FreeRTOS task for manual handling
//    xTaskCreate(manualhandling, "Manual Handling", 1000, NULL,NULL, NULL);
//
//    // Start the FreeRTOS scheduler
//    vTaskStartScheduler();
//
//    // Main loop - infinite loop to allow FreeRTOS to take over
//    for(;;) {
//        /* Loop forever */
//    }
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
//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "event_groups.h"
//#include "semphr.h"
//#include "queue.h"
//#include "adc_init.h"
//#include "modbus.h"
//#include "uart.h"
//#include "APP_modbus.h"
//#include "APP_modbus_port.h"
//
//
//cy_rslt_t rslt;
//cy_rslt_t result;
//
//static void manualhandling(void* pvParameters)
//{
//    // Initialize ADC and PWM
//    adc_single_channel_init();
//    pwm_init();
//
//    for (;;)
//    {
//
//
//        // Read the ADC value and process it
//        adc_single_channel_process();  // This will update the PWM based on the ADC value
//
//        // Delay for a short period before reading again
//        cyhal_system_delay_ms(200);
//    }
//}
//
//
//
//// Main function
//int main(void) {
//
//    cybsp_init();
//    __enable_irq();
//
//    // Initialize UART
//    rslt = uart_init();
//    if (rslt != CY_RSLT_SUCCESS) {
//        printf("UART initialization failed\n");
//        return -1;
//    }
//
//    /* Initialize retarget-io to use the debug UART port */
//    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
//    if (result != CY_RSLT_SUCCESS) {
//    }
//
//    // Initialize Blue LED
//    result = cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
//    if (result != CY_RSLT_SUCCESS) {
//        CY_ASSERT(0);  // GPIO initialization failed, stop here
//    }
//
//    // Create a FreeRTOS task for manual handling
//    xTaskCreate(manualhandling, "Manual Handling", 1000, NULL, 1, NULL);
//
//    // Start the FreeRTOS scheduler
//    vTaskStartScheduler();
//
//    // Main loop - infinite loop to allow FreeRTOS to take over
//    for(;;) {
//        /* Loop forever */
//    }
//}

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"
#include "queue.h"
#include "adc_init.h"
#include "modbus.h"
#include "uart.h"
#include "APP_modbus.h"
#include "APP_modbus_port.h"


cy_rslt_t rslt;
cy_rslt_t result;
// Modbus Slave Instance
modbusSlave_t modbusSlave;
tAPP_All_Mesaurement_Results tMesurementsResult; // Structure to hold measurement results
// Example transmit callback for Modbus
cb_transmitPacket transmitCallback;

// Define the actual callback function
uint32_t cb_transmitPacketImpl(uint8_t *pdata, uint16_t datalength, enumModbusPktType packetTyp)
{
    // Transmit each byte of Modbus packet over UART
    return uart_transmit_packet(pdata, datalength);
}
static void manualhandling(void* pvParameters)
{
    // Initialize ADC and PWM
    adc_single_channel_init();
    pwm_init();

    // Assign the callback function to the function pointer
        transmitCallback = cb_transmitPacketImpl;

        // Initialize Modbus Slave and pass the callback function
        vAPP_ModbusSlaveInit(&modbusSlave, transmitCallback);;

    for (;;)
    {
    	// Process Modbus requests
    	        modbusProcess(&modbusSlave);  // Handle incoming Modbus requests

        // Read the ADC value and process it
        adc_single_channel_process();  // This will update the PWM based on the ADC value
        // Send Modbus response after processing
        if (modbusSlave.packetAvailbleForProcessing)
        {
            // Call the function to transmit the response packet
            ulAPP_ModbusTrasmitPacket(modbusSlave.pTxBuffer, modbusSlave.lengthTxBuffer, modbusSlave.mbusPacketTyp);
        }

        // Delay for a short period before reading again
        cyhal_system_delay_ms(200);
    }
}



// Main function
int main(void) {

    cybsp_init();
    __enable_irq();

    // Initialize UART
//    rslt = uart_init();
    if (rslt != CY_RSLT_SUCCESS) {
        printf("UART initialization failed\n");
        return -1;
    }

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS) {
    }

    // Initialize Blue LED
    result = cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS) {
        CY_ASSERT(0);  // GPIO initialization failed, stop here
    }

    // Create a FreeRTOS task for manual handling
    xTaskCreate(manualhandling, "Manual Handling", 1000, NULL, 0, NULL);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Main loop - infinite loop to allow FreeRTOS to take over
    for(;;) {
        /* Loop forever */
    }
}
//uart
//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include "modbus.h"
//#include "APP_modbus.h"
//#include <inttypes.h>
//// Macro Definitions
//#define DATA_BITS_8     8
//#define STOP_BITS_1     1
//#define BAUD_RATE       9600
//#define UART_DELAY      10u
//#define RX_BUF_SIZE     64  // Size of the RX buffer (enough to hold the command string)
//#define TX_BUF_SIZE     64  // Size of the TX buffer for sending responses
//
//// Variable Declarations
//cy_rslt_t    rslt;
//cyhal_uart_t uart_obj;
//uint32_t     actualbaud;
//uint8_t      rx_buf[RX_BUF_SIZE];  // Buffer to store received data (user input)
//uint8_t      tx_buf[TX_BUF_SIZE];  // Buffer to send responses
//size_t       rx_index = 0;         // Track the current index in rx_buf for received data
//
//// Initialize the UART configuration structure
//const cyhal_uart_cfg_t uart_config =
//{
//    .data_bits      = DATA_BITS_8,
//    .stop_bits      = STOP_BITS_1,
//    .parity         = CYHAL_UART_PARITY_NONE,
//    .rx_buffer      = NULL,  // Not using a buffer, we're using cyhal_uart_getc()
//    .rx_buffer_size = 0      // Not needed when using cyhal_uart_getc()
//};
//
//
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
//    // Initialize the UART Block
//    rslt = cyhal_uart_init(&uart_obj, CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, NC, NC, NULL, &uart_config);
//    if (rslt != CY_RSLT_SUCCESS) {
//        printf("UART initialization failed\n");
//        return -1;
//    }
//
//    // Set the baud rate
//    rslt = cyhal_uart_set_baud(&uart_obj, BAUD_RATE, &actualbaud);
//    if (rslt != CY_RSLT_SUCCESS) {
//        printf("Failed to set UART baud rate\n");
//        return -1;
//    }
//
//    strcpy((char*)tx_buf, "hello\r\n");
//       for (size_t i = 0; i < strlen((char*)tx_buf); i++)
//       {
//           cyhal_uart_putc(&uart_obj, tx_buf[i]);
//       }
//
////    /* Initialize retarget-io to use the debug UART port */
////    result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX,
////                                        CYBSP_DEBUG_UART_RX,
////                                        CYBSP_DEBUG_UART_CTS,
////                                        CYBSP_DEBUG_UART_RTS,
////                                        CY_RETARGET_IO_BAUDRATE);
//
//        strcpy((char*)tx_buf, "hello\r\n");
//          for (size_t i = 0; i < strlen((char*)tx_buf); i++)
//          {
//              cyhal_uart_putc(&uart_obj, tx_buf[i]);
//          }
//
//    /* Print initialization message */
//    printf("\x1b[2J\x1b[;H");
//    printf("-----------------------------------------------------------\r\n");
//    printf("HAL: ADC using HAL\r\n");
//    printf("-----------------------------------------------------------\r\n\n");
//
//    // Initialize ADC and Modbus
//    adc_single_channel_init();  // Initialize ADC
//
//    // Initialize PWM outside the loop
//    result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
//    check_status("API cyhal_pwm_init failed with error code", result);
//
//    // Set the PWM output frequency and duty cycle
//    result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE, PWM_FREQUENCY);
//    check_status("API cyhal_pwm_set_duty_cycle failed with error code", result);
//
//    // Start the PWM
//    result = cyhal_pwm_start(&pwm_led_control);
//    check_status("API cyhal_pwm_start failed with error code", result);
//
//    // Main loop
//    for (;;)
//    {
//        // Process ADC values and update Modbus registers
//        adc_single_channel_process();  // Process ADC and update Modbus registers
//
//
//        // Delay to control processing rate
//        cyhal_system_delay_ms(200);
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
///* Initialize ADC for single-ended mode */
//void adc_single_channel_init(void)
//{
//    cy_rslt_t result;
//
//    /* Initialize the ADC object for the channel 0 input pin */
//    result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_0, NULL);
//    if(result != CY_RSLT_SUCCESS)
//    {
//    	strcpy((char*)tx_buf, "error hello\r\n");
//    	          for (size_t i = 0; i < strlen((char*)tx_buf); i++)
//    	          {
//    	              cyhal_uart_putc(&uart_obj, tx_buf[i]);
//    	          }
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
///* Process ADC readings from channel 0 and update Modbus register */
//void adc_single_channel_process(void)
//{
//    int32_t adc_result_0 = 0;
//
//    /* Read ADC value and convert it to millivolts */
//    adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;
////
////    /* Update Modbus register with ADC result (16-bit) */
////    uint16_t adc_register_value = (uint16_t)(adc_result_0); // Assuming a 16-bit register for the ADC value
////    modbusSlave.pTxBuffer[0] = adc_register_value ;  // MSB of the register
//
//    /* Print the ADC result */
//    printf("Channel 0 input: %4ldmV\r\n", (long int)adc_result_0);
//
//    /* Adjust PWM duty cycle based on ADC value */
//    float duty_cycle = (float)adc_result_0 / 3300.0f * 100.0f;
//    cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
//    check_status("cyhal_pwm_set_duty_cycle failed with error code", result);
//}
//
//void vPrintNumber( uint32_t ul)
//{
//    printf(" %ld\n", ul);  // Print the value to UART (Retarget IO)
//}
//
