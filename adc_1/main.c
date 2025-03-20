#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* Macros for ADC Configuration */
#define VPLUS_CHANNEL_0  (P10_0)  // Define the ADC input pin

/* ADC Object */
cyhal_adc_t adc_obj;

/* ADC Channel 0 Object */
cyhal_adc_channel_t adc_chan_0_obj;

/* ADC Configuration */
const cyhal_adc_config_t adc_config = {
    .continuous_scanning = false,  // Disable continuous scanning
    .average_count = 1,           // Disable averaging
    .vref = CYHAL_ADC_REF_VDDA,   // VREF for Single-ended channel set to VDDA
    .vneg = CYHAL_ADC_VNEG_VSSA,  // VNEG for Single-ended channel set to VSSA
    .resolution = 12u,            // 12-bit resolution
    .ext_vref = NC,               // No external VREF
    .bypass_pin = NC              // No bypass pin
};

/* Function Prototypes */
void adc_single_channel_init(void);
void adc_single_channel_process(void);

/* Main Function */
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if initialization fails
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if UART initialization fails
    }

    /* Print initialization message */
    printf("\x1b[2J\x1b[;H");
    printf("-----------------------------------------------------------\r\n");
    printf("HAL: ADC using HAL\r\n");
    printf("-----------------------------------------------------------\r\n\n");

    /* Initialize the single-ended ADC channel */
    adc_single_channel_init();

    /* Configure ADC */
    result = cyhal_adc_configure(&adc_obj, &adc_config);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC configuration update failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);  // Halt if ADC configuration fails
    }

    for (;;)
    {
        /* Read and process ADC channel */
        adc_single_channel_process();
        cyhal_system_delay_ms(200);  // Delay for 200ms between readings
    }
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

/* Process ADC readings from channel 0 */
void adc_single_channel_process(void)
{
    int32_t adc_result_0 = 0;

    /* Read ADC value and convert it to millivolts */
    adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;
    printf("Channel 0 input: %4ldmV\r\n", (long int)adc_result_0);
}


