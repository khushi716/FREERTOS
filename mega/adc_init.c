#include "adc_init.h"
#include "APP_modbus.h"
/* ADC Object */
cyhal_adc_t adc_obj;

/* ADC Channel 0 Object */
cyhal_adc_channel_t adc_chan_0_obj;

/* PWM Object */
cyhal_pwm_t pwm_led_control;

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

/* Process ADC readings from channel 0 */
void adc_single_channel_process(void)
{
    int32_t adc_result_0 = 0;

    /* Read ADC value and convert it to millivolts */
    adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;

    // Update Modbus input register with ADC value
//    tMesurementsResult.adc_v = adc_result_0;

    /* Set PWM duty cycle based on ADC value */
    float duty_cycle = (float)adc_result_0 / 3300.0f * 100.0f;
    cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("API cyhal_pwm_set_duty_cycle failed with error code: %ld\n", (long unsigned int)result);
    }

    /* Print the ADC result */
    printf("Channel 0 input: %4ldmV\r\n", (long int)adc_result_0);
}

/* Initialize PWM */
void pwm_init(void)
{
    cy_rslt_t result;

    // Initialize the PWM
    result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("API cyhal_pwm_init failed with error code: %ld\n", (long unsigned int)result);
    }

    // Set the PWM output frequency and duty cycle
    result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE, PWM_FREQUENCY);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("API cyhal_pwm_set_duty_cycle failed with error code: %ld\n", (long unsigned int)result);
    }

    // Start the PWM
    result = cyhal_pwm_start(&pwm_led_control);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("API cyhal_pwm_start failed with error code: %ld\n", (long unsigned int)result);
    }
}
