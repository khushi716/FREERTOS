/*****************************************************************************
* File Name        : APP_modbus.c
*
* Description      : Application layer for Modbus slave with ADC and PWM.
*****************************************************************************/

/*****************************************************************************
 * Includes
 *****************************************************************************/
#include "APP_modbus.h"
#include "cyhal_pwm.h"  // Include for PWM functionality
#include "cybsp.h"      // Include for board support (initialization)

/*****************************************************************************
 * Defines
 *****************************************************************************/
/* ADC Configuration */

/* ADC Channel Configuration */
#define ADC_CHANNEL (P10_0)  // Example pin for ADC (adjust as needed)
#define VPLUS_CHANNEL_0  (P10_0)  // ADC input pin connected to potentiometer
#define MBUS_ADC_VAL_ADDR 1
/* PWM Frequency and Duty Cycle */
#define PWM_FREQUENCY (10000u)  // 10kHz
#define PWM_DUTY_CYCLE (30.0f)  // 30% Duty Cycle

/*****************************************************************************
 * Global Variables
 *****************************************************************************/
modbusSlave_t tModbusSlave;
tAPP_All_Mesaurement_Results tMesurementsResult;
uint8_t abAPPModbusRXBuffer[APP_MODBUS_RX_BUFFER_LENGTH];
uint8_t abAPPModbusTXBuffer[APP_MODBUS_TX_BUFFER_LENGTH];
/* ADC Object */
cyhal_adc_t adc_obj;
/* ADC Channel 0 Object */
cyhal_adc_channel_t adc_chan_0_obj;
cyhal_pwm_t pwm_led_control;

static regMap_t tInputRegMap[] = {
    { MBUS_ADC_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.adc_v, NULL, NULL},  // Example entry for ADC
};

static regMap_t tHoldingRegMap[] = {
    { MBUS_ADC_VAL_ADDR, APP_ONE_BYTE_VAL_LENGTH, &tMesurementsResult.adc_v, cbCmdReadadc, tMesurementsResult.adc_v },  // Callback to read ADC
};

/*****************************************************************************
 * Function Name: vAPP_ModbusSlaveInit
 *****************************************************************************/
void vAPP_ModbusSlaveInit(modbusSlave_t *tModbusSlaveConfig, cb_transmitPacket fpTransmitCallback)
{
    uint16_t wHoldingRegMapSize = sizeof(tHoldingRegMap) / sizeof(regMap_t);
    uint16_t wInputRegMapSize = sizeof(tInputRegMap) / sizeof(regMap_t);

    /* Initialize modbus buffer */
    modbusBufferInit(tModbusSlaveConfig, abAPPModbusTXBuffer, APP_MODBUS_TX_BUFFER_LENGTH, abAPPModbusRXBuffer, APP_MODBUS_RX_BUFFER_LENGTH);

    /* Register Modbus holding and input registers */
    modbusSlaveSetMapHoldingReg(tModbusSlaveConfig, tHoldingRegMap, wHoldingRegMapSize);
    modbusSlaveSetMapInputReg(tModbusSlaveConfig, tInputRegMap, wInputRegMapSize);

    /* Register Modbus transmit callback */
    modbusRegisterTransmitCallback(tModbusSlaveConfig, fpTransmitCallback);
}

/*****************************************************************************
 * Function Name: cbCmdReadadc
 *****************************************************************************/
bool cbCmdReadadc(uint8_t *pData, uint8_t bDataLength, void *pvUserdata)
{
    bool boStatus = false;
    uint16_t adc_value = tMesurementsResult.adc_v;

    /* Check if we have the correct data length */
    if (bDataLength == sizeof(uint16_t))
    {
        modbusWriteBE(pData, adc_value);  // Write ADC value in big-endian format
        boStatus = true;  // Command successful
    }

    return boStatus;
}

/*****************************************************************************
 * Function Name: adc_single_channel_init
 *****************************************************************************/
void adc_single_channel_init(void)
{
    cy_rslt_t result;

    /* Initialize the ADC object */
    result = cyhal_adc_init(&adc_obj, ADC_CHANNEL, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("ADC initialization failed\n");
        CY_ASSERT(0);
    }

    /* Configure the ADC channel */
    const cyhal_adc_channel_config_t channel_config = {
        .enable_averaging = false,
        .min_acquisition_ns = 1000,
        .enabled = true
    };

    result = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, ADC_CHANNEL, CYHAL_ADC_VNEG, &channel_config);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("ADC channel initialization failed\n");
        CY_ASSERT(0);
    }

    printf("ADC Initialized and ready to read\r\n");
}

/*****************************************************************************
 * Function Name: adc_single_channel_process
 *****************************************************************************/
void adc_single_channel_process(void)
{
    int32_t adc_result = 0;

    /* Read ADC value and convert to millivolts */
    adc_result = cyhal_adc_read_uv(&adc_chan_0_obj) / 1000;  // Convert to millivolts

    /* Update PWM Duty Cycle */
    float duty_cycle = (float)adc_result / 3300.0f * 100.0f;  // Map ADC result to PWM duty cycle
    cy_rslt_t result = cyhal_pwm_set_duty_cycle(&pwm_led_control, duty_cycle, PWM_FREQUENCY);
    check_status("API cyhal_pwm_set_duty_cycle failed", result);

    /* Store ADC result */
    tMesurementsResult.adc_v = (uint16_t)adc_result;

    /* Print ADC value for debugging */
    printf("ADC value: %4ldmV\r\n", (long int)adc_result);
}

/*****************************************************************************
 * Function Name: pwm_init
 *****************************************************************************/
void pwm_init(void)
{
    cy_rslt_t result;

    /* Initialize PWM for LED control */
    result = cyhal_pwm_init(&pwm_led_control, CYBSP_USER_LED, NULL);
    check_status("API cyhal_pwm_init failed", result);

    /* Set PWM duty cycle and frequency */
    result = cyhal_pwm_set_duty_cycle(&pwm_led_control, PWM_DUTY_CYCLE, PWM_FREQUENCY);
    check_status("API cyhal_pwm_set_duty_cycle failed", result);

    /* Start PWM */
    result = cyhal_pwm_start(&pwm_led_control);
    check_status("API cyhal_pwm_start failed", result);
}

