#ifndef ADC_INIT_H
#define ADC_INIT_H

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#define VPLUS_CHANNEL_0  (P10_0)   // Define the ADC input pin connected to potentiometer wiper

/* PWM Frequency = 10kHz */
#define PWM_FREQUENCY (10000u)

/* PWM Duty-cycle = 50% */
#define PWM_DUTY_CYCLE (30.0f)

/* ADC Object */
extern cyhal_adc_t adc_obj;

/* ADC Channel 0 Object */
extern cyhal_adc_channel_t adc_chan_0_obj;

/* PWM Object */
extern cyhal_pwm_t pwm_led_control;

/* ADC Configuration */
extern const cyhal_adc_config_t adc_config;

/* Function Prototypes */
void adc_single_channel_init(void);
void adc_single_channel_process(void);
void pwm_init(void);

#endif /* ADC_INIT_H */
