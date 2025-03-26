#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

/* Macros for ADC Configuration */
#define VPLUS_CHANNEL_0  (P10_0)  // ADC Input Pin 0
#define VPLUS_CHANNEL_1  (P10_1)  // ADC Input Pin 1
#define VPLUS_CHANNEL_2  (P10_2)  // ADC Input Pin 2
#define VPLUS_CHANNEL_3  (P10_3)  // ADC Input Pin 3

/* SPI Pin Configuration */
// Define the SPI and GPIO pins
#define SSD1306_DC_Pin          CYBSP_D9
#define SSD1306_RST_Pin         CYBSP_TRACE_CLK
#define SSD1306_CS_Pin          CYBSP_SPI_CS

// Function to initialize GPIO pins for DC, Reset, and CS
void GPIO_Init(void)
{
    // Initialize DC pin as output
    cyhal_gpio_init(SSD1306_DC_Pin, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 0);

    // Initialize Reset pin as output
    cyhal_gpio_init(SSD1306_RST_Pin, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);

    // Initialize Chip Select pin as output (Active Low)
    cyhal_gpio_init(SSD1306_CS_Pin, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, 1);
}

/* ADC Objects */
cyhal_adc_t adc_obj;
cyhal_adc_channel_t adc_chan_0_obj;
cyhal_adc_channel_t adc_chan_1_obj;
cyhal_adc_channel_t adc_chan_2_obj;
cyhal_adc_channel_t adc_chan_3_obj;

/* SPI Objects */
cyhal_spi_t mSPI;

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
void adc_multiple_channels_init(void);
void adc_multiple_channels_process(void);
void display_voltages(float voltage1, float voltage2, float voltage3, float voltage4);
void spi_init(void);


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

    /* Initialize SPI for OLED communication */
    spi_init();
    GPIO_Init();     // Initialize GPIO
    /* Initialize the multiple ADC channels */
    adc_multiple_channels_init();

    /* Configure ADC */
    result = cyhal_adc_configure(&adc_obj, &adc_config);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC configuration update failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);  // Halt if ADC configuration fails
    }
    ssd1306_Init();
    for (;;)
    {
        /* Read and process ADC channels */
        adc_multiple_channels_process();
        cyhal_system_delay_ms(200);  // Delay for 200ms between readings
    }
}

/* Initialize ADC for multiple channels */
void adc_multiple_channels_init(void)
{
    cy_rslt_t result;

    /* Initialize the ADC object for the input pins */
    result = cyhal_adc_init(&adc_obj, VPLUS_CHANNEL_0, NULL);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);  // Halt if ADC initialization fails
    }

    /* Channel configurations */
    const cyhal_adc_channel_config_t channel_config = {
        .enable_averaging = false,  // Disable averaging
        .min_acquisition_ns = 1000, // Minimum acquisition time set to 1us
        .enabled = true             // Enable the channel for ADC sampling
    };

    /* Initialize channel 0 to scan the channel 0 input pin in single-ended mode */
    result = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, VPLUS_CHANNEL_0, CYHAL_ADC_VNEG, &channel_config);
    if(result != CY_RSLT_SUCCESS)
    {
        printf("ADC channel 0 initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);  // Halt if channel initialization fails
    }

    /* Initialize other channels in a similar manner */
    result = cyhal_adc_channel_init_diff(&adc_chan_1_obj, &adc_obj, VPLUS_CHANNEL_1, CYHAL_ADC_VNEG, &channel_config);
    result |= cyhal_adc_channel_init_diff(&adc_chan_2_obj, &adc_obj, VPLUS_CHANNEL_2, CYHAL_ADC_VNEG, &channel_config);
    result |= cyhal_adc_channel_init_diff(&adc_chan_3_obj, &adc_obj, VPLUS_CHANNEL_3,  CYHAL_ADC_VNEG,&channel_config);

    if (result != CY_RSLT_SUCCESS)
    {
        printf("ADC multiple channels initialization failed. Error: %ld\n", (long unsigned int)result);
        CY_ASSERT(0);  // Halt if any channel initialization fails
    }

    printf("ADC is configured for multiple channels\r\n\n");
}

/* Process ADC readings from multiple channels */
void adc_multiple_channels_process(void)
{
    int32_t adc_result_0 = 0, adc_result_1 = 0, adc_result_2 = 0, adc_result_3 = 0;

    /* Read ADC values and convert them to millivolts */
    adc_result_0 = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;
    adc_result_1 = cyhal_adc_read_uv(&adc_chan_1_obj) / 1000;
    adc_result_2 = cyhal_adc_read_uv(&adc_chan_2_obj) / 1000;
    adc_result_3 = cyhal_adc_read_uv(&adc_chan_3_obj) / 1000;

    /* Print the voltage values to the console */
    printf("Channel 0: %4ldmV\r\n", (long int)adc_result_0);
    printf("Channel 1: %4ldmV\r\n", (long int)adc_result_1);
    printf("Channel 2: %4ldmV\r\n", (long int)adc_result_2);
    printf("Channel 3: %4ldmV\r\n", (long int)adc_result_3);

    /* Display the voltages on the OLED using SPI */
    display_voltages(adc_result_0, adc_result_1 , adc_result_2 , adc_result_3 );
}

/* Display voltages of all channels on the OLED */
void display_voltages(float voltage1, float voltage2, float voltage3, float voltage4)
{   // Initialize the SSD1306 OLED display

    // Clear the screen
    ssd1306_Fill(Black);

    // Set cursor to the first line
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Ch0: ", Font_6x8, White);
    char voltage_str[10];
    sprintf(voltage_str, "%.2fmV", voltage1);
    ssd1306_WriteString(voltage_str, Font_6x8, White);

    // Set cursor to the second line
    ssd1306_SetCursor(0, 16);
    ssd1306_WriteString("Ch1: ", Font_6x8, White);
    sprintf(voltage_str, "%.2fmV", voltage2);
    ssd1306_WriteString(voltage_str, Font_6x8, White);

    // Set cursor to the third line
    ssd1306_SetCursor(0, 32);
    ssd1306_WriteString("Ch2: ", Font_6x8, White);
    sprintf(voltage_str, "%.2fmV", voltage3);
    ssd1306_WriteString(voltage_str, Font_6x8, White);

    // Set cursor to the fourth line
    ssd1306_SetCursor(0, 48);
    ssd1306_WriteString("Ch3: ", Font_6x8, White);
    sprintf(voltage_str, "%.2fmV", voltage4);
    ssd1306_WriteString(voltage_str, Font_6x8, White);

    // Update the screen
    ssd1306_UpdateScreen();

}

/* SPI Initialization */
void spi_init(void)
{
    cy_rslt_t result;

    // Configure SPI (MOSI, MISO, SCK, and optionally CS)
    cyhal_spi_init(&mSPI, CYBSP_SPI_MOSI, CYBSP_SPI_MISO, CYBSP_SPI_CLK, CYBSP_SPI_CS, NULL,
            8, CYHAL_SPI_MODE_00_MSB, false);

    // Set SPI frequency to 1 MHz (adjust as necessary)
    cyhal_spi_set_frequency(&mSPI, 1000000);

}
