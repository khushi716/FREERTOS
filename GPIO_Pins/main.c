



#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/******************************************************************************
 * Macros for both examples
 *****************************************************************************/
#define GPIO_INTERRUPT_PRIORITY (7u)

/*
 * This code example assumes that the Device Configurator will automatically configure
 * all GPIO pins of the device. To see how PDL drivers are used to manually
 * configure GPIO pins, set the PDL_PIN_CONFIGURATION #define to 1, otherwise leave
 * set to 0.
 */
#define PDL_PIN_CONFIGURATION 0

/******************************************************************************
 * Definitions and functions for PDL example
 *****************************************************************************/
#if PDL_PIN_CONFIGURATION
/* This structure is used to initialize a single GPIO pin using PDL configuration. */
const cy_stc_gpio_pin_config_t P0_4_Pin_Init =
    {
        .outVal = 1u,                    /* Pin output state */
        .driveMode = CY_GPIO_DM_PULLUP,  /* Drive mode */
        .hsiom = HSIOM_SEL_GPIO,         /* HSIOM selection */
        .intEdge = CY_GPIO_INTR_FALLING, /* Interrupt Edge type */
        .intMask = CY_GPIO_INTR_EN_MASK, /* Interrupt enable mask */
        .vtrip = CY_GPIO_VTRIP_CMOS,     /* Input buffer voltage trip type */
        .slewRate = CY_GPIO_SLEW_FAST,   /* Output buffer slew rate */
        .driveSel = CY_GPIO_DRIVE_FULL,  /* Drive strength */
        .vregEn = 0u,                    /* SIO pair output buffer mode */
        .ibufMode = 0u,                  /* SIO pair input buffer mode */
        .vtripSel = 0u,                  /* SIO pair input buffer trip point */
        .vrefSel = 0u,                   /* SIO pair reference voltage for input buffer trip point */
        .vohSel = 0u                     /* SIO pair regulated voltage output level */
};
#endif

/* This structure is used to initialize a full GPIO Port using PDL configuration */
const cy_stc_gpio_prt_config_t port5_Init =
    {
        .out = 0x000000FFu,        /* Initial output data for the IO pins in the port */
        .intrMask = 0x00000000u,   /* Interrupt enable mask for the port interrupt */
        .intrCfg = 0x00000000u,    /* Port interrupt edge detection configuration */
        .cfg = 0xEEEEEEEEu,        /* Port drive modes and input buffer enable configuration */
        .cfgIn = 0x00000000u,      /* Port input buffer configuration */
        .cfgOut = 0x00000000u,     /* Port output buffer configuration */
        .cfgSIO = 0x00000000u,     /* Port SIO configuration */
        .sel0Active = 0x00000000u, /* HSIOM selection for port pins 0,1,2,3 */
        .sel1Active = 0x00000000u, /* HSIOM selection for port pins 4,5,6,7 */
};

/* This structure initializes the Port0 interrupt for the NVIC */
cy_stc_sysint_t intrCfg =
    {
        .intrSrc = ioss_interrupts_gpio_0_IRQn, /* Interrupt source is GPIO port 0 interrupt */
        .intrPriority = 2UL                     /* Interrupt priority is 2 */
};
uint32 pinState = 0ul;

/*******************************************************************************
* Function Prototypes
********************************************************************************/
static void pdl_code_example();
static void gpio_interrupt_handler_PDL();

/*******************************************************************************
* Global Variables
********************************************************************************/
volatile bool gpio_intr_flag = false;
volatile bool cond_flag = false;

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  System entrance point. This function runs the PDL example.
*
* Return: int
*
*******************************************************************************/
int main(void)
{
    pdl_code_example();
}

/*******************************************************************************
* Function Name: gpio_interrupt_handler_PDL
********************************************************************************
*
*  Summary:
*  GPIO interrupt handler for the PDL example.
*
*  Parameters:
*  None
*
*  Return:
*  None
*
**********************************************************************************/
static void gpio_interrupt_handler_PDL()
{
    gpio_intr_flag = true;

    /* Clear pin interrupt logic. Required to detect next interrupt */
    Cy_GPIO_ClearInterrupt(CYHAL_GET_PORTADDR(CYBSP_USER_BTN), CYHAL_GET_PIN(CYBSP_USER_BTN));
}

/*******************************************************************************
* Function Name: pdl_code_example
********************************************************************************
* Summary:
*   PDL version of the GPIO code example.
*
* Parameters:
*  None
*
*******************************************************************************/
static void pdl_code_example()
{
    cy_rslt_t result;
    volatile bool read_val = false;
    uint32 portReadValue = 0ul;

#if PDL_PIN_CONFIGURATION
    GPIO_PRT_Type *CYBSP_USER_LED_PORT = P0_4_PORT;
    uint8_t CYBSP_USER_LED_PIN = P0_4_PIN;
#else
    GPIO_PRT_Type *CYBSP_USER_BTN_PORT = CYHAL_GET_PORTADDR(CYBSP_USER_BTN);
    uint8_t CYBSP_USER_BTN_PIN = CYHAL_GET_PIN(CYBSP_USER_BTN);
#endif

    /* Port and pin translations for the USER_LED */
    GPIO_PRT_Type *CYBSP_USER_LED_PORT = CYHAL_GET_PORTADDR(CYBSP_USER_LED);
    uint8_t CYBSP_USER_LED_PIN = CYHAL_GET_PIN(CYBSP_USER_LED);

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Enable global interrupts */
    __enable_irq();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* The most code efficient method to configure all attributes for a full port of pins */
    Cy_GPIO_Port_Init(GPIO_PRT5, &port5_Init);

    /* Initialize USER_LED */
    Cy_GPIO_Pin_FastInit(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN, CY_GPIO_DM_STRONG, 1UL, HSIOM_SEL_GPIO);

    /* Initialize the user button */
    Cy_GPIO_Pin_FastInit(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_PIN, CY_GPIO_DM_STRONG, 1UL, HSIOM_SEL_GPIO);

    /* Pin Interrupts */
    /* Configure GPIO pin to generate interrupts */
    Cy_GPIO_SetInterruptEdge(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_PIN, CY_GPIO_INTR_RISING);
    Cy_GPIO_SetInterruptMask(CYBSP_USER_BTN_PORT, CYBSP_USER_BTN_PIN, CY_GPIO_INTR_EN_MASK);

    /* Configure CM4+ CPU GPIO interrupt vector for Port 0 */
    Cy_SysInt_Init(&intrCfg, gpio_interrupt_handler_PDL);
    NVIC_ClearPendingIRQ(intrCfg.intrSrc);
    NVIC_EnableIRQ((IRQn_Type)intrCfg.intrSrc);

    for (;;)
    {


        /* Check the interrupt status */
        if (true == gpio_intr_flag && cond_flag == false  )
        {
            /* Reset interrupt flag */
            gpio_intr_flag = false;
             Cy_GPIO_Write(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN, CYBSP_LED_STATE_ON);
            Cy_SysLib_Delay(500);
            cond_flag = true;

        }
        else if (true == gpio_intr_flag && cond_flag == true )
             {
                 /* Reset interrupt flag */
                 gpio_intr_flag = false;
                  Cy_GPIO_Write(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN, CYBSP_LED_STATE_OFF);
                 Cy_SysLib_Delay(500);
                 cond_flag = false;

             }

    }
}
