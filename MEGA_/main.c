/* Main application entry */
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "semphr.h"
#include "queue.h"
#include <inttypes.h>
#include "APP_modbus.h"

#define GPIO_INTERRUPT_PRIORITY (7u)
volatile bool gpio_intr_flag = false;  // Flag set by GPIO interrupt
cyhal_gpio_callback_data_t gpio_btn_callback_data;  // GPIO callback data
cy_rslt_t result;

/* Macros for ADC Configuration */
#define VPLUS_CHANNEL_0  (P10_0)   // Define the ADC input pin connected to potentiometer wiper

/* PWM Frequency = 10kHz */
#define PWM_FREQUENCY (10000u)

/* PWM Duty-cycle = 30% */
#define PWM_DUTY_CYCLE (30.0f)

/* ADC Object */
cyhal_adc_t adc_obj;
modbusSlave_t tModbusSlave;
// GPIO interrupt handler
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
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    gpio_intr_flag = true;  // Set flag when button is pressed
}



/* Function to check the status of functions */
void check_status(char *message, cy_rslt_t status)
{
    if (CY_RSLT_SUCCESS != status)
    {
        printf("\r\n=====================================================\r\n");
        printf("\nFAIL: %s\r\n", message);
        printf("Error Code: 0x%08" PRIX32 "\n", status);
        printf("\r\n=====================================================\r\n");
        cyhal_gpio_write(CYBSP_LED_RGB_RED, CYBSP_LED_STATE_ON);
        cyhal_gpio_write(CYBSP_LED_RGB_BLUE, CYBSP_LED_STATE_OFF);
        while(true);
    }
}


/* FreeRTOS task for manual handling of ADC */
cb_transmitPacket fpTransmitCallback;
static void manualhandling(void*pvParameters) {
	 /* Initialize Modbus Slave */
	    vAPP_ModbusSlaveInit(&tModbusSlave, fpTransmitCallback );

    // Reset the interrupt flag after processing
    gpio_intr_flag = false;

    // Initialize the single-ended ADC channel
    adc_single_channel_init();

    // Configure ADC
    result = cyhal_adc_configure(&adc_obj, &adc_config);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("ADC configuration update failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);  // Halt if ADC configuration fails
    }

    printf("Entering manual mode\n");
    cyhal_gpio_write(CYBSP_LED_RGB_BLUE, CYBSP_LED_STATE_ON);
    pwm_init();

    for (;;) {
        // Read and process ADC channel
        adc_single_channel_process();
        cyhal_system_delay_ms(200);  // Delay for 200ms between readings
    }
}

/* Main function */
int main(void) {
    /* Initialize the device and I2C */
    cybsp_init();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if UART initialization fails
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
    __enable_irq();

    // Initialize Blue LED
    result = cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // GPIO initialization failed, stop here
    }
    // Initialize red LED
    result = cyhal_gpio_init(CYBSP_LED_RGB_RED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
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
