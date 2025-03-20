#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#define GPIO_INTERRUPT_PRIORITY (7u)

static void gpio_interrupt_handler_HAL(void *arg, cyhal_gpio_event_t event);
/* Semaphore from interrupt handler to background process */
volatile bool gpio_intr_flag = false;
volatile bool cond_flag = false;
/*This structure is used to initialize callback*/
cyhal_gpio_callback_data_t cb_data =
    {
        .callback = gpio_interrupt_handler_HAL,
        .callback_arg = NULL
 };

static void gpio_interrupt_handler_HAL(void *arg, cyhal_gpio_event_t event)
{
    gpio_intr_flag = true;
}
int main(){
	cy_rslt_t result;
		    /* Initialize the device and board peripherals */
	 result = cybsp_init();

		    /* Board init failed. Stop program execution */
	 if (result != CY_RSLT_SUCCESS)
		  {
		        CY_ASSERT(0);
		  }

		    /* Initialize the user LED  */
	 result = cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

		    /* Initialize the user button */
     result = cyhal_gpio_init(CYBSP_USER_BTN, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLUP, CYBSP_BTN_OFF);

		    /* Configure GPIO interrupt */
	cyhal_gpio_register_callback(CYBSP_USER_BTN, &cb_data);
    cyhal_gpio_enable_event(CYBSP_USER_BTN, CYHAL_GPIO_IRQ_RISE, GPIO_INTERRUPT_PRIORITY, true);

		    /* Enable global interrupts */
	  __enable_irq();
		   while(1){
			   if (true == gpio_intr_flag  &&  cond_flag == false)
			  		         {

			  		                 /* Turn ON user LED */
			  		                 cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
			  		                 cond_flag = true;
			  		                 cyhal_system_delay_ms(500);
			  		                 gpio_intr_flag=false;
			  		         }

			  		    else  if (true == gpio_intr_flag  &&  cond_flag == true)
			  		   		         {

			  		   		                 /* Turn OFF user LED */
			  		   		                 cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
			  		   		                 cond_flag = false;
			  		   		                 cyhal_system_delay_ms(500);
			  		   		                 gpio_intr_flag=false;
			  		   		         }

		   }
	}
