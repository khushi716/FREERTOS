#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <inttypes.h>
#include "lsm6dsl.h"

// I2C Configuration
#define I2C_MASTER_FREQUENCY 100000u  // 100kHz I2C frequency
#define I2C_SLAVE_ADDR        0x6Au    // DS3231 RTC I2C address
#define GPIO_INTERRUPT_PRIORITY (7u)

// PWM Parameters
#define PWM_FREQUENCY (10000u)
#define PWM_DUTY_CYCLE (30.0f)

// ADC Configuration
#define VPLUS_CHANNEL_0  (P10_0)   // Define the ADC input pin connected to potentiometer wiper

// Queue Configuration
#define ADC_QUEUE_SIZE 5

// GPIO Callback Data
cyhal_gpio_callback_data_t gpio_btn_callback_data;  // GPIO callback data
volatile bool gpio_intr_flag = false;  // Flag set by GPIO interrupt
QueueHandle_t adc_queue;  // Queue to pass ADC values to PWM task
QueueHandle_t accel_queue;  // Queue to pass accelerometer data to light intensity task
//i2c object
cyhal_i2c_t i2c_obj;

// ADC and PWM Handles
cyhal_adc_t adc_obj;
cyhal_adc_channel_t adc_chan_0_obj;
extern cyhal_pwm_t pwm_led_control;

// Task handles
TaskHandle_t adc_task_handle = NULL;

// Function Prototypes
void adc_task(void *pvParameters);
void pwm_task(void *pvParameters);
void Light_manage_task(void *pvParameters);
void accelo_task(void *pvParameters);  // Accelerometer task
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event);
void vPrintStringAndNumberfloat(const char *pcString, float ul);
void vPrintString(const char *pcString);
void vPrintStringAndNumber(const char *pcString, uint32_t ul);

// I2C Initialization Function
void i2c_master_init(void)
{
    cy_rslt_t rslt = cyhal_i2c_init(&i2c_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("I2C Master initialization failed, rslt: %d\n", rslt);
        return;
    }

    cyhal_i2c_cfg_t master_config = {
        CYHAL_I2C_MODE_MASTER,
        0,
        I2C_MASTER_FREQUENCY
    };
    rslt = cyhal_i2c_configure(&i2c_obj, &master_config);
    if (rslt != CY_RSLT_SUCCESS)
    {
        printf("I2C Master configuration failed, rslt: %d\n", rslt);
        return;
    }
}

// GPIO Interrupt Handler (Button press handler)
static void gpio_interrupt_handler(void *handler_arg, cyhal_gpio_event_t event)
{
    vPrintString("interrupt");  // Print that interrupt was triggered
    gpio_intr_flag = true;  // Set flag when button is pressed

//    // Notify adc_task to start reading ADC values
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//    xTaskNotifyFromISR(adc_task_handle, &xHigherPriorityTaskWoken);  // Notify adc_task to start
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);  // Yield to higher priority task if needed
}

// ADC Task: Read ADC value and send it to PWM task via Queue
void adc_task(void *pvParameters)
{
    cy_rslt_t result;
    for (;;)
    {
        // Wait for GPIO interrupt to start ADC read
        if (gpio_intr_flag)
        {
            vPrintString("adc task");
       	 cyhal_gpio_write(CYBSP_LED_RGB_GREEN, CYBSP_LED_STATE_ON);
    	 cyhal_gpio_write(CYBSP_LED_RGB_BLUE, CYBSP_LED_STATE_OFF);

            // ADC Initialization and configuration
            result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_0, NULL);
            if(result != CY_RSLT_SUCCESS)
            {
                printf("ADC initialization failed. Error: %ld\n", (long unsigned int)result);
                CY_ASSERT(0);
            }

            const cyhal_adc_channel_config_t channel_config = {
                .enable_averaging = false,
                .min_acquisition_ns = 1000,
                .enabled = true
            };

            result = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, VPLUS_CHANNEL_0, CYHAL_ADC_VNEG, &channel_config);
            if(result != CY_RSLT_SUCCESS)
            {
                printf("ADC channel initialization failed. Error: %ld\n", (long unsigned int)result);
                CY_ASSERT(0);
            }

            printf("ADC is initialized.\n");

            // Read ADC value and send to Light manage task via Queue
            int32_t adc_value;
            while (gpio_intr_flag)
            {
                adc_value = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000; // Convert to mV
                xQueueSend(adc_queue, &adc_value, portMAX_DELAY);
                vTaskDelay(pdMS_TO_TICKS(200)); // Delay before next reading
            }
        }
    }
}

// PWM Task: Receive ADC value and adjust PWM duty cycle
void pwm_task(void *pvParameters)
{
    vPrintString("pm task");

    cy_rslt_t result;

    // PWM Initialization
    result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("PWM initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);
    }

    result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE, PWM_FREQUENCY);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Failed to set PWM duty cycle. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);
    }

    // Start PWM
    result = cyhal_pwm_start(&pwm_led_control);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Failed to start PWM. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);
    }

    printf("PWM is initialized and started.\n");
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(100));  // Add delay to prevent task starvation
    }
}

// Light Manage Task: Adjust light intensity based on ADC values or accelerometer data
void Light_manage_task(void *pvParameters)
{
    vPrintString("light task");

    int32_t adc_value = 0;
    float acc_z_value = 0.0f;  // Variable to hold the accelerometer Z-axis data

    for (;;)
    {
        if (gpio_intr_flag)
        {
            // Wait for ADC value from the queue (button pressed)
            if (xQueueReceive(adc_queue, &adc_value, portMAX_DELAY))
            {
                // Calculate the duty cycle based on ADC value
                float duty_cycle = (float)adc_value / 3300.0f * 100.0f; // Convert ADC value to duty cycle percentage
                cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
                vPrintStringAndNumberfloat("accel0", duty_cycle);

                if (result != CY_RSLT_SUCCESS)
                {
                    printf("Failed to set PWM duty cycle. Error: %ld\n", (long unsigned int)result);
                }

                printf("ADC Value: %ld mV, PWM Duty Cycle: %.2f%%\n", (long int)adc_value, duty_cycle);
            }
        }
        else
        {
            // If no GPIO interrupt, receive accelerometer data from the queue
            if (xQueueReceive(accel_queue, &acc_z_value, portMAX_DELAY))  // Receiving Z-axis data
            {
                // Adjust light intensity based on accelerometer Z-axis value (or any other logic)
                float duty_cycle = (acc_z_value + 1.0f) * 50.0f;  // Map Z-axis (from -1g to +1g) to PWM duty cycle
                cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
                vPrintStringAndNumberfloat("accel0", duty_cycle);
                if (result != CY_RSLT_SUCCESS)
                {
                    printf("Failed to set PWM duty cycle. Error: %ld\n", (long unsigned int)result);
                }

                printf("Light intensity adjusted based on accelerometer data: %.2f%%\n", duty_cycle);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));  // Small delay before re-checking
    }
}
// Accelerometer Task: Read accelerometer data and display it
void accelo_task(void *pvParameters)
{    cyhal_gpio_write(CYBSP_LED_RGB_GREEN, CYBSP_LED_STATE_OFF);
cyhal_gpio_write(CYBSP_LED_RGB_BLUE, CYBSP_LED_STATE_ON);


    vPrintString("accelo task");
    // Initialize accelerometer (example: set to 2g range)
    LSM6DSL_AccInit(0x6A); // Pass the initialization structure, set accelerometer config (e.g., 2g range)

    int16_t accData[3];
    float accInG[3];

    for (;;)
    {
        // Read accelerometer data
        LSM6DSL_AccReadXYZ(accData);

        // Convert raw accelerometer data to 'g' units
        for (int i = 0; i < 3; i++)
        {
            accInG[i] = (float)accData[i] * 0.061f / 1000.0f;  // Apply the sensitivity (2g = 0.061 mg/LSB)
        }

        // Print the accelerometer data
        printf("Accelerometer data (in 'g'):\n");
        printf("X: %.4f g, Y: %.4f g, Z: %.4f g\n", accInG[0], accInG[1], accInG[2]);

        // Send accelerometer Z-axis value to the Light_manage_task queue (or any other axis you want)
        xQueueSend(accel_queue, &accInG[2], portMAX_DELAY);  // Sending the Z-axis data

        // Delay before the next read
        vTaskDelay(pdMS_TO_TICKS(500));  // Read data every 500 ms
    }
}

// Function to print strings
void vPrintString(const char *pcString)
{
    printf("%s", pcString);
}

void vPrintStringAndNumber(const char *pcString, uint32_t ul)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
    printf(" %ld\n", ul);  // Print the value to UART (Retarget IO)
}

void vPrintStringAndNumberfloat(const char *pcString, float ul)
{
    printf("%s", pcString);  // Print the string to UART (Retarget IO)
    printf(" %f\n", ul);  // Print the value to UART (Retarget IO)
}

int main(void)
{
    cy_rslt_t result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if initialization fails
    }

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
    result = cyhal_gpio_init(CYBSP_LED_RGB_GREEN, CYHAL_GPIO_DIR_OUTPUT,
                       CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
      if (result != CY_RSLT_SUCCESS)
         {
             CY_ASSERT(0);  // UART initialization failed, stop here
         }
      result = cyhal_gpio_init(CYBSP_LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT,
                         CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
        if (result != CY_RSLT_SUCCESS)
           {
               CY_ASSERT(0);  // UART initialization failed, stop here
           }

    // Configure GPIO interrupt for falling edge (button press detection)
    gpio_btn_callback_data.callback = gpio_interrupt_handler;
    cyhal_gpio_register_callback(CYBSP_USER_BTN, &gpio_btn_callback_data);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_FALL, GPIO_INTERRUPT_PRIORITY, true);

    // Enable global interrupts
    __enable_irq();
    i2c_master_init();
    // Create FreeRTOS Queue for ADC values
    adc_queue = xQueueCreate(ADC_QUEUE_SIZE, sizeof(int32_t));

    // Create FreeRTOS Queue for Accelerometer data (Z-axis)
    accel_queue = xQueueCreate(ADC_QUEUE_SIZE, sizeof(float));  // Sending float for the Z-axis value

    // Create FreeRTOS tasks
    xTaskCreate(adc_task, "ADC Task", 1024, NULL, 1, &adc_task_handle);  // Pass handle to ADC task
    xTaskCreate(pwm_task, "PWM Task", 1024, NULL, 1, NULL);
    xTaskCreate(Light_manage_task, "Light intensity", 1024, NULL, 1, NULL);
    xTaskCreate(accelo_task, "Accelerometer task", 1024, NULL, 1, NULL);  // Accelerometer task

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Main loop - should not reach here if scheduler is working
    for (;;)
    {
        // Main application code here (if any)
    }
}
