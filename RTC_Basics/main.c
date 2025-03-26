///******************************************************************************
//* File Name:   main.c
//*
//* Description: This is the source code for the Infineon MCU HAL Real-Time Clock
//*              Basics example of ModusToolbox.
//*
//* Related Document: See README.md
//*
//*******************************************************************************
//* Copyright 2019-2024, Cypress Semiconductor Corporation (an Infineon company) or
//* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
//*
//* This software, including source code, documentation and related
//* materials ("Software") is owned by Cypress Semiconductor Corporation
//* or one of its affiliates ("Cypress") and is protected by and subject to
//* worldwide patent protection (United States and foreign),
//* United States copyright laws and international treaty provisions.
//* Therefore, you may use this Software only as provided in the license
//* agreement accompanying the software package from which you
//* obtained this Software ("EULA").
//* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
//* non-transferable license to copy, modify, and compile the Software
//* source code solely for use in connection with Cypress's
//* integrated circuit products.  Any reproduction, modification, translation,
//* compilation, or representation of this Software except as specified
//* above is prohibited without the express written permission of Cypress.
//*
//* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
//* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
//* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
//* reserves the right to make changes to the Software without notice. Cypress
//* does not assume any liability arising out of the application or use of the
//* Software or any product or circuit described in the Software. Cypress does
//* not authorize its products for use in any products where a malfunction or
//* failure of the Cypress product may reasonably be expected to result in
//* significant property damage, injury or death ("High Risk Product"). By
//* including Cypress's product in a High Risk Product, the manufacturer
//* of such system or application assumes all risk of such use and in doing
//* so agrees to indemnify Cypress against all liability.
//*******************************************************************************/
//
///******************************************************************************
// * Include header files
// ******************************************************************************/
//#include "cyhal.h"
//#include "cycfg.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//#include "string.h"
//
///*******************************************************************************
//* Macros
//*******************************************************************************/
//
//#define UART_TIMEOUT_MS (10u)      /* in milliseconds */
//#define INPUT_TIMEOUT_MS (120000u) /* in milliseconds */
//
//#define STRING_BUFFER_SIZE (80)
//
///* Available commands */
//#define RTC_CMD_SET_DATE_TIME ('1')
//#define RTC_CMD_CONFIG_DST ('2')
//
//#define RTC_CMD_ENABLE_DST ('1')
//#define RTC_CMD_DISABLE_DST ('2')
//#define RTC_CMD_QUIT_CONFIG_DST ('3')
//
//#define FIXED_DST_FORMAT ('1')
//#define RELATIVE_DST_FORMAT ('2')
//
///* Macro used for checking validity of user input */
//#define MIN_SPACE_KEY_COUNT (5)
//
///* Structure tm stores years since 1900 */
//#define TM_YEAR_BASE (1900u)
//
///* Maximum value of seconds and minutes */
//#define MAX_SEC_OR_MIN (60u)
//
///* Maximum value of hours definition */
//#define MAX_HOURS_24H (23UL)
//
///* Month per year definition */
//#define MONTHS_PER_YEAR (12U)
//
///* Days per week definition */
//#define DAYS_PER_WEEK (7u)
//
///* Days in month */
//#define DAYS_IN_JANUARY (31U)   /* Number of days in January */
//#define DAYS_IN_FEBRUARY (28U)  /* Number of days in February */
//#define DAYS_IN_MARCH (31U)     /* Number of days in March */
//#define DAYS_IN_APRIL (30U)     /* Number of days in April */
//#define DAYS_IN_MAY (31U)       /* Number of days in May */
//#define DAYS_IN_JUNE (30U)      /* Number of days in June */
//#define DAYS_IN_JULY (31U)      /* Number of days in July */
//#define DAYS_IN_AUGUST (31U)    /* Number of days in August */
//#define DAYS_IN_SEPTEMBER (30U) /* Number of days in September */
//#define DAYS_IN_OCTOBER (31U)   /* Number of days in October */
//#define DAYS_IN_NOVEMBER (30U)  /* Number of days in November */
//#define DAYS_IN_DECEMBER (31U)  /* Number of days in December */
//
///* Flags to indicate the if the entered time is valid */
//#define DST_DISABLED_FLAG (0)
//#define DST_VALID_START_TIME_FLAG (1)
//#define DST_VALID_END_TIME_FLAG (2)
//#define DST_ENABLED_FLAG (3)
//
///* Macro to validate seconds parameter */
//#define IS_SEC_VALID(sec) ((sec) <= MAX_SEC_OR_MIN)
//
///* Macro to validate minutes parameters */
//#define IS_MIN_VALID(min) ((min) <= MAX_SEC_OR_MIN)
//
///* Macro to validate hour parameter */
//#define IS_HOUR_VALID(hour) ((hour) <= MAX_HOURS_24H)
//
///* Macro to validate month parameter */
//#define IS_MONTH_VALID(month) (((month) > 0U) && ((month) <= MONTHS_PER_YEAR))
//
///* Macro to validate the year value */
//#define IS_YEAR_VALID(year) ((year) > 0U)
//
///* Checks whether the year passed through the parameter is leap or not */
//#define IS_LEAP_YEAR(year) \
//(((0U == (year % 4UL)) && (0U != (year % 100UL))) || (0U == (year % 400UL)))
//
///*******************************************************************************
//* Global Variables
//*******************************************************************************/
//cyhal_rtc_t rtc_obj;
//uint32_t dst_data_flag = 0;
//
///*******************************************************************************
//* Function Prototypes
//*******************************************************************************/
//
//static void set_new_time(uint32_t timeout_ms);
//static bool validate_date_time(int sec, int min, int hour,
//                                int mday, int month, int year);
//static int get_day_of_week(int day, int month, int year);
//static void set_dst_feature(uint32_t timeout_ms);
//static cy_rslt_t fetch_time_data(char *buffer,
//                             uint32_t timeout_ms, uint32_t *space_count);
//static int get_week_of_month(int day, int month, int year);
//
//
///*******************************************************************************
//* Function Name: handle_error
//********************************************************************************
//* Summary:
//* User defined error handling function
//*
//* Parameters:
//*  void
//*
//* Return:
//*  void
//*
//*******************************************************************************/
//void handle_error(void)
//{
//    /* Disable all interrupts. */
//    __disable_irq();
//
//    CY_ASSERT(0);
//}
//
///*******************************************************************************
//* Function Name: main
//********************************************************************************
//* Summary:
//*   This function:
//*  - Initializes the device and board peripherals
//*  - Initializes RTC
//*  - The loop checks for the user command and process the commands
//*
//* Parameters :
//*  void
//*
//* Return:
//*  int
//*
//*******************************************************************************/
//int main(void)
//{
//    cy_rslt_t rslt;
//    uint8_t cmd;
//    char buffer[STRING_BUFFER_SIZE];
//    struct tm date_time;
//
//    /* Initialize the device and board peripherals */
//    rslt = cybsp_init();
//    if (CY_RSLT_SUCCESS != rslt)
//    {
//        handle_error();
//    }
//
//    /* Initialize retargeting standard IO to the debug UART port */
//    rslt = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
//            CYBSP_DEBUG_UART_CTS,CYBSP_DEBUG_UART_RTS,CY_RETARGET_IO_BAUDRATE);
//
//    if (CY_RSLT_SUCCESS != rslt)
//    {
//        handle_error();
//    }
//
//    /* Enable global interrupts */
//    __enable_irq();
//
//    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
//    printf("\x1b[2J\x1b[;H");
//    printf("****************** HAL: RTC Basics ******************\r\n\n");
//
//    /* Initialize RTC */
//    rslt = cyhal_rtc_init(&rtc_obj);
//    if (CY_RSLT_SUCCESS != rslt)
//    {
//        handle_error();
//    }
//
//    /* Display available commands */
//    printf("Available commands \r\n");
//    printf("1 : Set new time and date\r\n");
//    printf("2 : Configure DST feature\r\n\n");
//
//    for (;;)
//    {
//        rslt = cyhal_rtc_read(&rtc_obj, &date_time);
//        if (CY_RSLT_SUCCESS == rslt)
//        {
//            strftime(buffer, sizeof(buffer), "%c", &date_time);
//            printf("\r%s", buffer);
//            memset(buffer, '\0', sizeof(buffer));
//        }
//
//        rslt = cyhal_uart_getc(&cy_retarget_io_uart_obj, &cmd, UART_TIMEOUT_MS);
//        if (rslt != CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT)
//        {
//            if (RTC_CMD_SET_DATE_TIME == cmd)
//            {
//                printf("\r[Command] : Set new time\r\n");
//                set_new_time(INPUT_TIMEOUT_MS);
//            }
//            else if (RTC_CMD_CONFIG_DST == cmd)
//            {
//                printf("\r[Command] : Configure DST feature\r\n");
//                set_dst_feature(INPUT_TIMEOUT_MS);
//            }
//        }
//    }
//}
//
///*******************************************************************************
//* Function Name: set_dst_feature
//********************************************************************************
//* Summary:
//*  This functions takes the user input ,sets the dst start/end date and time,
//*  and then enables the DST feature.
//*
//* Parameter:
//*  uint32_t timeout_ms : Maximum allowed time (in milliseconds) for the
//*  function
//*
//* Return:
//*  void
//*******************************************************************************/
//static void set_dst_feature(uint32_t timeout_ms)
//{
//    cy_rslt_t rslt;
//    uint8_t dst_cmd;
//    char dst_start_buffer[STRING_BUFFER_SIZE] = {0};
//    char dst_end_buffer[STRING_BUFFER_SIZE] = {0};
//    uint32_t space_count = 0;
//
//    /* Variables used to store DST start and end time information */
//    cyhal_rtc_dst_t dst_start_time, dst_end_time;
//
//    /* Variables used to store date and time information */
//    int mday = 0, month = 0, year = 0, sec = 0, min = 0, hour = 0;
//    uint8_t fmt = 0;
//    if (DST_ENABLED_FLAG == dst_data_flag)
//    {
//        if (cyhal_rtc_is_dst(&rtc_obj))
//        {
//            printf("\rCurrent DST Status :: Active\r\n\n");
//        }
//        else
//        {
//            printf("\rCurrent DST Status :: Inactive\r\n\n");
//        }
//    }
//    else
//    {
//        printf("\rCurrent DST Status :: Disabled\r\n\n");
//    }
//
//    /* Display available commands */
//    printf("Available DST commands \r\n");
//    printf("1 : Enable DST feature\r\n");
//    printf("2 : Disable DST feature\r\n");
//    printf("3 : Quit DST Configuration\r\n\n");
//
//    rslt = cyhal_uart_getc(&cy_retarget_io_uart_obj, &dst_cmd, timeout_ms);
//
//    if (rslt != CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT)
//    {
//        if (RTC_CMD_ENABLE_DST == dst_cmd)
//        {
//            /* Get DST start time information */
//            printf("Enter DST format \r\n");
//            printf("1 : Fixed DST format\r\n");
//            printf("2 : Relative DST format\r\n\n");
//
//            rslt = cyhal_uart_getc(&cy_retarget_io_uart_obj, &fmt, timeout_ms);
//            if (rslt != CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT)
//            {
//            printf("Enter DST start time in \"HH MM SS dd mm yyyy\" format\r\n");
//                rslt = fetch_time_data(dst_start_buffer, timeout_ms,
//                                                        &space_count);
//                if (rslt != CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT)
//                {
//                    if (space_count != MIN_SPACE_KEY_COUNT)
//                    {
//                        printf("\rInvalid values! Please enter "
//                        "the values in specified format\r\n");
//                    }
//                    else
//                    {
//                        sscanf(dst_start_buffer, "%d %d %d %d %d %d",
//                               &hour, &min, &sec,
//                               &mday, &month, &year);
//
//                    if((validate_date_time(sec, min, hour, mday, month, year))&&
//                    ((fmt == FIXED_DST_FORMAT) || (fmt == RELATIVE_DST_FORMAT)))
//                    {
//                        dst_start_time.format =
//                        (fmt == FIXED_DST_FORMAT) ? CYHAL_RTC_DST_FIXED :
//                                                        CYHAL_RTC_DST_RELATIVE;
//                        dst_start_time.hour = hour;
//                        dst_start_time.month = month;
//                        dst_start_time.dayOfWeek =
//                        (fmt == FIXED_DST_FORMAT) ? 1 :
//                                            get_day_of_week(mday, month, year);
//                        dst_start_time.dayOfMonth =
//                        (fmt == FIXED_DST_FORMAT) ? mday : 1;
//                        dst_start_time.weekOfMonth =
//                        (fmt == FIXED_DST_FORMAT) ? 1 :
//                                        get_week_of_month(mday, month, year);
//                        /* Update flag value to indicate that a
//                            valid DST start time information has been received*/
//                        dst_data_flag = DST_VALID_START_TIME_FLAG;
//                    }
//                    else
//                    {
//                        printf("\rInvalid values! Please enter the values"
//                                   " in specified format\r\n");
//                    }
//                    }
//                }
//                else
//                {
//                    printf("\rTimeout \r\n");
//                }
//
//                if (DST_VALID_START_TIME_FLAG == dst_data_flag)
//                {
//                    /* Get DST end time information,
//                    iff a valid DST start time information is received */
//                    printf("Enter DST end time "
//                    " in \"HH MM SS dd mm yyyy\" format\r\n");
//                    rslt = fetch_time_data(dst_end_buffer, timeout_ms,
//                                            &space_count);
//                    if (rslt != CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT)
//                    {
//                        if (space_count != MIN_SPACE_KEY_COUNT)
//                        {
//                            printf("\rInvalid values! Please"
//                            "enter the values in specified format\r\n");
//                        }
//                        else
//                        {
//                            sscanf(dst_end_buffer, "%d %d %d %d %d %d",
//                                   &hour, &min, &sec,
//                                   &mday, &month, &year);
//
//                            if((validate_date_time(sec,min,hour,mday,month,year))&&
//                                ((fmt == FIXED_DST_FORMAT) ||
//                                (fmt == RELATIVE_DST_FORMAT)))
//                            {
//                                dst_end_time.format = (fmt == FIXED_DST_FORMAT)?
//                                 CYHAL_RTC_DST_FIXED : CYHAL_RTC_DST_RELATIVE;
//                                dst_end_time.hour = hour;
//                                dst_end_time.month = month;
//                                dst_end_time.dayOfWeek =
//                                (fmt == FIXED_DST_FORMAT) ? 1 :
//                                get_day_of_week(mday, month, year);
//                                dst_end_time.dayOfMonth =
//                                (fmt == FIXED_DST_FORMAT) ? mday : 1;
//                                dst_end_time.weekOfMonth =
//                                (fmt == FIXED_DST_FORMAT) ? 1 :
//                                get_week_of_month(mday, month, year);
//                                /* Update flag value to indicate that a valid
//                                 DST end time information has been recieved*/
//                                dst_data_flag = DST_VALID_END_TIME_FLAG;
//                            }
//                            else
//                            {
//                                printf("\rInvalid values! Please enter the "
//                                       " values in specified format\r\n");
//                            }
//                        }
//                    }
//                    else
//                    {
//                        printf("\rTimeout \r\n");
//                    }
//                }
//
//                if (DST_VALID_END_TIME_FLAG == dst_data_flag)
//                {
//                    rslt = cyhal_rtc_set_dst(&rtc_obj, &dst_start_time,
//                                                        &dst_end_time);
//                    if (CY_RSLT_SUCCESS == rslt)
//                    {
//                        dst_data_flag = DST_ENABLED_FLAG;
//                        printf("\rDST time updated\r\n\n");
//                    }
//                    else
//                    {
//                        handle_error();
//                    }
//                }
//            }
//            else
//            {
//                printf("\rTimeout \r\n");
//            }
//        }
//        else if (RTC_CMD_DISABLE_DST == dst_cmd)
//        {
//            dst_end_time.format = CYHAL_RTC_DST_FIXED;
//            dst_end_time.hour = 0;
//            dst_end_time.month = 1;
//            dst_end_time.dayOfWeek = 1;
//            dst_end_time.dayOfMonth = 1;
//            dst_end_time.weekOfMonth = 1;
//            dst_start_time = dst_end_time;
//            rslt = cyhal_rtc_set_dst(&rtc_obj, &dst_start_time, &dst_end_time);
//            if (CY_RSLT_SUCCESS == rslt)
//            {
//                dst_data_flag = DST_DISABLED_FLAG;
//                printf("\rDST feature disabled\r\n\n");
//            }
//            else
//            {
//                handle_error();
//            }
//        }
//        else if (RTC_CMD_QUIT_CONFIG_DST == dst_cmd)
//        {
//            printf("\rExit from DST Configuration \r\n\n");
//        }
//    }
//    else
//    {
//        printf("\rTimeout \r\n");
//    }
//}
//
///*******************************************************************************
//* Function Name: set_new_time
//********************************************************************************
//* Summary:
//*  This functions takes the user input and sets the new date and time.
//*
//* Parameter:
//*  uint32_t timeout_ms : Maximum allowed time (in milliseconds) for the
//*  function
//*
//* Return :
//*  void
//*******************************************************************************/
//static void set_new_time(uint32_t timeout_ms)
//{
//    cy_rslt_t rslt;
//    char buffer[STRING_BUFFER_SIZE] = {0};
//    uint32_t space_count = 0;
//
//    /* Variables used to store date and time information */
//    int mday, month, year, sec, min, hour;
//    struct tm new_time = {0};
//
//    printf("\rEnter time in \"HH MM SS dd mm yyyy\" format \r\n");
//    rslt = fetch_time_data(buffer, timeout_ms, &space_count);
//    if (rslt != CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT)
//    {
//        if (space_count != MIN_SPACE_KEY_COUNT)
//        {
//            printf("\rInvalid values! Please enter the"
//                    "values in specified format\r\n");
//        }
//        else
//        {
//            sscanf(buffer, "%d %d %d %d %d %d",
//                   &hour, &min, &sec,
//                   &mday, &month, &year);
//
//            if (validate_date_time(sec, min, hour, mday, month, year))
//            {
//                new_time.tm_sec = sec;
//                new_time.tm_min = min;
//                new_time.tm_hour = hour;
//                new_time.tm_mday = mday;
//                new_time.tm_mon = month - 1;
//                new_time.tm_year = year - TM_YEAR_BASE;
//                new_time.tm_wday = get_day_of_week(mday, month, year);
//
//                rslt = cyhal_rtc_write(&rtc_obj, &new_time);
//                if (CY_RSLT_SUCCESS == rslt)
//                {
//                    printf("\rRTC time updated\r\n\n");
//                }
//                else
//                {
//                    handle_error();
//                }
//            }
//            else
//            {
//                printf("\rInvalid values! Please enter the values in specified"
//                       " format\r\n");
//            }
//        }
//    }
//    else
//    {
//        printf("\rTimeout \r\n");
//    }
//}
//
///*******************************************************************************
//* Function Name: fetch_time_data
//********************************************************************************
//* Summary:
//*  Function fetches data entered by the user through UART and stores it in the
//*  buffer which is passed through parameters. The function also counts number of
//*  spaces in the recieved data and stores in the variable, whose address are
//*  passsed as parameter.
//*
//* Parameter:
//*  char* buffer        : Buffer to store the fetched data
//*  uint32_t timeout_ms : Maximum allowed time (in milliseconds) for the function
//*  uint32_t* space_count : The number of spaces present in the fetched data.
//*
//* Return:
//*  Returns the status of the getc request
//*
//*******************************************************************************/
//static cy_rslt_t fetch_time_data(char *buffer, uint32_t timeout_ms,
//                                    uint32_t *space_count)
//{
//    cy_rslt_t rslt;
//    uint32_t index = 0;
//    uint8_t ch;
//    *space_count = 0;
//    while (index < STRING_BUFFER_SIZE)
//    {
//        if (timeout_ms <= UART_TIMEOUT_MS)
//        {
//            rslt = CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT;
//            break;
//        }
//
//        rslt = cyhal_uart_getc(&cy_retarget_io_uart_obj, &ch, UART_TIMEOUT_MS);
//
//        if (rslt != CY_RSLT_ERR_CSP_UART_GETC_TIMEOUT)
//        {
//            if (ch == '\n' || ch == '\r')
//            {
//                break;
//            }
//            else if (ch == ' ')
//            {
//                (*space_count)++;
//            }
//
//            buffer[index] = ch;
//            rslt = cyhal_uart_putc(&cy_retarget_io_uart_obj, ch);
//            index++;
//        }
//
//        timeout_ms -= UART_TIMEOUT_MS;
//    }
//    printf("\n\r");
//    return rslt;
//}
//
///*******************************************************************************
//* Function Name: get_day_of_week
//********************************************************************************
//* Summary:
//*  Returns a day of the week for a year, month, and day of month that are passed
//*  through parameters. Zeller's congruence is used to calculate the day of
//*  the week. See https://en.wikipedia.org/wiki/Zeller%27s_congruence for more
//*  details.
//*
//*  Note: In this algorithm January and February are counted as months 13 and 14
//*  of the previous year.
//*
//* Parameter:
//*  int day          : The day of the month, Valid range 1..31.
//*  int month        : The month of the year
//*  int year         : The year value. Valid range non-zero value.
//*
//* Return:
//*  Returns a day of the week (0 = Sunday, 1 = Monday, ., 6 = Saturday)
//*
//*******************************************************************************/
//static int get_day_of_week(int day, int month, int year)
//{
//    int ret;
//    int k = 0;
//    int j = 0;
//    if (month < CY_RTC_MARCH)
//    {
//        month += CY_RTC_MONTHS_PER_YEAR;
//        year--;
//    }
//
//    k = (year % 100);
//    j = (year / 100);
//    ret=(day+(13*(month+1)/5)+k+(k/4)+(j/4)+(5*j))%DAYS_PER_WEEK;
//    ret = ((ret + 6) % DAYS_PER_WEEK);
//    return ret;
//}
//
///*******************************************************************************
//* Function Name: get_week_of_month
//********************************************************************************
//* Summary:
//*  Returns week number of the month for a year and a month that are passed
//*  through parameters.
//*
//* Parameter:
//*  int day          : The day of the month, Valid range 1..31.
//*  int month        : The month of the year
//*  int year         : The year value. Valid range non-zero value.
//*
//* Return:
//*  Returns a week of the month (0 to 5)
//*
//*******************************************************************************/
//static int get_week_of_month(int day, int month, int year)
//{
//    int count = 0, day_of_week = 0, weekend_day = 0;
//
//    day_of_week = get_day_of_week(1, month, year);
//    weekend_day = 7 - day_of_week;
//    while (day > weekend_day)
//    {
//        count++;
//        weekend_day += 7;
//    }
//
//    return count;
//}
//
///*******************************************************************************
//* Function Name: validate_date_time
//********************************************************************************
//* Summary:
//*  This function validates date and time value.
//*
//* Parameters:
//*  uint32_t sec     : The second valid range is [0-59].
//*  uint32_t min     : The minute valid range is [0-59].
//*  uint32_t hour    : The hour valid range is [0-23].
//*  uint32_t date    : The date valid range is [1-31], if the month of February
//*                     is selected as the Month parameter, then the valid range
//*                     is [0-29].
//*  uint32_t month   : The month valid range is [1-12].
//*  uint32_t year    : The year valid range is [> 0].
//*
//* Return:
//*  false - invalid ; true - valid
//*
//*******************************************************************************/
//static bool validate_date_time(int sec, int min, int hour, int mday,
//                                    int month, int year)
//{
//    static const uint8_t days_in_month_table[MONTHS_PER_YEAR] =
//        {
//            DAYS_IN_JANUARY,
//            DAYS_IN_FEBRUARY,
//            DAYS_IN_MARCH,
//            DAYS_IN_APRIL,
//            DAYS_IN_MAY,
//            DAYS_IN_JUNE,
//            DAYS_IN_JULY,
//            DAYS_IN_AUGUST,
//            DAYS_IN_SEPTEMBER,
//            DAYS_IN_OCTOBER,
//            DAYS_IN_NOVEMBER,
//            DAYS_IN_DECEMBER,
//        };
//
//    uint8_t days_in_month;
//
//    bool rslt = IS_SEC_VALID(sec) & IS_MIN_VALID(min) &
//                IS_HOUR_VALID(hour) & IS_MONTH_VALID(month) &
//                IS_YEAR_VALID(year);
//
//    if (rslt)
//    {
//        days_in_month = days_in_month_table[month - 1];
//
//        if (IS_LEAP_YEAR(year) && (month == 2))
//        {
//            days_in_month++;
//        }
//
//        rslt &= (mday > 0U) && (mday <= days_in_month);
//    }
//
//    return rslt;
//}
//
///* [] END OF FILE */

//
//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//#include "cyhal_rtc.h"
//#include <stdio.h>
//#include <time.h>
//
//#define TM_YEAR_BASE (1900u)  // Structure tm stores years since 1900
//
//cy_rslt_t result;
//cyhal_rtc_t my_rtc;
//char buffer[80];
//
//// Time settings (3rd March 2020, 08:10:10, Wednesday, DST off)
//const int mday = 3, month = 3, year = 2020; // Day of month, month and year
//const int hours = 8, minutes = 10, seconds = 10; // Hours, minutes and seconds
//const int wday = 3; // Days from Sunday. Sunday is 0, Monday is 1 and so on.
//const int dst = 0; // Daylight Savings. 0 - Disabled, 1 - Enabled
//
//// Setting the tm structure as 08 HRS:10 MIN:10 SEC; 3rd March 2020; Wednesday; DST off
//struct tm new_date_time =
//{
//    .tm_sec   = seconds,
//    .tm_min   = minutes,
//    .tm_hour  = hours,
//    .tm_mday  = mday,
//    .tm_mon   = month - 1,  // Month is 0-indexed (March is 2, i.e., month - 1)
//    .tm_year  = year - TM_YEAR_BASE,  // Year is offset by 1900 in tm structure
//    .tm_wday  = wday,  // Day of the week (Sunday = 0, Monday = 1, etc.)
//    .tm_isdst = dst  // Daylight savings time off
//};
//
//struct tm current_date_time = {0};  // Initialize the structure for reading the time
//
//// Function to initialize RTC
//void rtc_init(void)
//{
//    result = cyhal_rtc_init(&my_rtc);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        printf("RTC initialization failed, result: %d\n", result);
//        return;
//    }
//}
//
//// Function to set the RTC time
//void rtc_set_time(void)
//{
//    // Update the current time and date to the RTC peripheral
//    result = cyhal_rtc_write(&my_rtc, &new_date_time);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        printf("Failed to write date/time to RTC, result: %d\n", result);
//    }
//    else
//    {
//        printf("Successfully set time to RTC\n");
//    }
//}
//
//// Function to read the current time from RTC
//void rtc_read_time(void)
//{
//    // Get the current time and date from the RTC peripheral
//    result = cyhal_rtc_read(&my_rtc, &current_date_time);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        printf("Failed to read date/time from RTC, result: %d\n", result);
//        return;
//    }
//
//    // Use strftime to format the date and time into a string
//    strftime(buffer, sizeof(buffer), "%c", &current_date_time);  // "%c" gives the full date/time format
//
//    // Print the formatted date/time string to UART
//    printf("Current time from RTC: %s\n", buffer);
//}
//
//int main(void)
//{
//    // Initialize the board (e.g., LED, button, etc.)
//    result = cybsp_init();
//    if (result != CY_RSLT_SUCCESS)
//    {
//        printf("Board initialization failed, result: %d\n", result);
//        return -1;
//    }
//
//    // Initialize UART for retargeting (for debugging purposes)
//    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
//    if (result != CY_RSLT_SUCCESS)
//    {
//        printf("UART initialization failed, result: %d\n", result);
//        return -1;
//    }
//
//    // Initialize RTC
//    rtc_init();
//
//    // Set the RTC time to the configured values
//    rtc_set_time();
//
//    // Read and print the current time from the RTC
//    rtc_read_time();
//
//    // Main loop
//    while (1)
//    {}
//
//    return 0;
//}
//#include "cyhal.h"
//#include "cybsp.h"
//#include "cy_retarget_io.h"
//#include "ds1307.h"
//cyhal_i2c_t i2c_master_obj;
//int main(void)
//{
//	cy_rslt_t result;
//	result = cybsp_init();
//	if (result != CY_RSLT_SUCCESS)
//	{
//		CY_ASSERT(0);
//	}
//	__enable_irq();
//	result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
//			CYBSP_DEBUG_UART_CTS,CYBSP_DEBUG_UART_RTS,CY_RETARGET_IO_BAUDRATE);
//	if (result != CY_RSLT_SUCCESS)
//	{
//		CY_ASSERT(0);
//	}
//	cyhal_i2c_cfg_t i2c_master_config =
//	{
//			CYHAL_I2C_MODE_MASTER,
//			0, // address is not used for master mode
//			100000u
//	};
//
//	result = cyhal_i2c_init(&i2c_master_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
//	result = cyhal_i2c_configure(&i2c_master_obj, &i2c_master_config);
//
//	const char *DAYS_OF_WEEK[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
//	DS1307_Init();
//	DS1307_SetTimeZone(+8, 00);
//	DS1307_SetDate(14);
//	DS1307_SetMonth(2);
//	DS1307_SetYear(2025);
//	DS1307_SetDayOfWeek(5);
//	DS1307_SetHour(12);
//	DS1307_SetMinute(01);
//	DS1307_SetSecond(55);
//	for (;;)
//	{
//		uint8_t date = DS1307_GetDate();
//		uint8_t month = DS1307_GetMonth();
//		uint16_t year = DS1307_GetYear();
//		uint8_t dow = DS1307_GetDayOfWeek();
//		uint8_t hour = DS1307_GetHour();
//		uint8_t minute = DS1307_GetMinute();
//		uint8_t second = DS1307_GetSecond();
//		int8_t zone_hr = DS1307_GetTimeZoneHour();
//		uint8_t zone_min = DS1307_GetTimeZoneMin();
//		printf("ISO8601 FORMAT: %04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d  %s\n",
//				year, month, date, hour, minute, second, zone_hr, zone_min, DAYS_OF_WEEK[dow]);
//		cyhal_system_delay_ms(1000);
//	}
//}
//
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ds1307.h"  // Change to DS3231 RTC driver
cyhal_i2c_t i2c_master_obj;

int main(void)
{
    cy_rslt_t result;

    // Initialize the board (e.g., LED, button, etc.)
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    // Initialize UART for retargeting (for debugging purposes)
    result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                                     CYBSP_DEBUG_UART_CTS, CYBSP_DEBUG_UART_RTS, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // I2C Master configuration
    cyhal_i2c_cfg_t i2c_master_config = {
        CYHAL_I2C_MODE_MASTER,  // Set I2C mode as master
        0,                      // Address is not used for master mode
        100000u                 // Set frequency for I2C communication (100kHz)
    };

    // Initialize I2C master
    result = cyhal_i2c_init(&i2c_master_obj, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    result = cyhal_i2c_configure(&i2c_master_obj, &i2c_master_config);

    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    // Days of the week array
    const char *DAYS_OF_WEEK[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

    // Initialize the DS3231 RTC (ensure your DS3231 driver is set up correctly)
    DS1307_Init();

    // Set time and date (example values)
    DS1307_SetTimeZone(+8, 00);   // Set timezone to GMT+8
    DS1307_SetDate(14);           // Set day of the month
    DS1307_SetMonth(2);          // Set month (February)
    DS1307_SetYear(2025);        // Set year (2025)
    DS1307_SetDayOfWeek(5);      // Set day of the week (Friday)
    DS1307_SetHour(12);          // Set hour (12 PM)
    DS1307_SetMinute(01);        // Set minute (01)
    DS1307_SetSecond(55);        // Set second (55)

    for (;;)
    {
        // Retrieve current time and date from DS3231
        uint8_t date = DS1307_GetDate();
        uint8_t month =DS1307_GetMonth();
        uint16_t year = DS1307_GetYear();
        uint8_t dow = DS1307_GetDayOfWeek();
        uint8_t hour = DS1307_GetHour();
        uint8_t minute = DS1307_GetMinute();
        uint8_t second = DS1307_GetSecond();
        int8_t zone_hr = DS1307_GetTimeZoneHour();
        uint8_t zone_min = DS1307_GetTimeZoneMin();

        // Print time in ISO8601 format
        printf("ISO8601 FORMAT: %04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d  %s\r\n",
                year, month, date, hour, minute, second, zone_hr, zone_min, DAYS_OF_WEEK[dow]);

        // Delay for 1 second before updating the time again
        cyhal_system_delay_ms(1000);
    }
}
