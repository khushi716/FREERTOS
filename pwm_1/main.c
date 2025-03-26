#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* PWM Duty-cycle (0 to 100 percent) */
float PWM_DUTY_CYCLE = 0.0;
cy_rslt_t rslt;
cyhal_pwm_t pwm_obj;

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
    printf("HAL: PWM using HAL\r\n");
    printf("-----------------------------------------------------------\r\n\n");

    // Initialize PWM on the supplied pin and assign a new clock
    rslt = cyhal_pwm_init(&pwm_obj, CYBSP_USER_LED, NULL);
    if (rslt != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);  // Halt if PWM initialization fails
    }



    //change the PWM duty cycle from 0% to 100%
    for (int i = 0; i <= 100; i++)
    {
        PWM_DUTY_CYCLE = (float)i;

        printf("PWM Duty Cycle: %f%%\r\n", PWM_DUTY_CYCLE);


        rslt = cyhal_pwm_set_duty_cycle(&pwm_obj, PWM_DUTY_CYCLE, 100);
        if (rslt != CY_RSLT_SUCCESS)
        {
            CY_ASSERT(0);  // Halt if setting the duty cycle fails
        }
        // Start the PWM output
            rslt = cyhal_pwm_start(&pwm_obj);
            if (rslt != CY_RSLT_SUCCESS)
            {
                CY_ASSERT(0);  // Halt if PWM start fails
            }

        cyhal_system_delay_ms(100);
    }


}


//
//#include "cy_pdl.h"
//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//
// cy_rslt_t   rslt;
//    cyhal_pwm_t pwm_obj;
///* Main Function */
//int main(void)
//{
//    cy_rslt_t result;
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
//    printf("HAL: PWM using HAL\r\n");
//    printf("-----------------------------------------------------------\r\n\n");
//    // Initialize PWM on the supplied pin and assign a new clock
//    rslt = cyhal_pwm_init(&pwm_obj,CYBSP_USER_LED, NULL);
//
//    // Set a duty cycle of 50% and frequency of 1Hz
//    rslt = cyhal_pwm_set_duty_cycle(&pwm_obj, 50, 1);
//
//
//    for (;;)
//    {
//    	 // Stop the PWM output
//    	        rslt = cyhal_pwm_stop(&pwm_obj);
//
//    	        // Delay for observing the output
//    	        cyhal_system_delay_ms(15);
//
//    	        // (Re-)start the PWM output
//    	        rslt = cyhal_pwm_start(&pwm_obj);
//
//    	        // Delay for observing the output
//    	        cyhal_system_delay_ms(5000);
//    }
//}
//
//
