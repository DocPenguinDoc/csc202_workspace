//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 10
//
//      FILE NAME:  lab10p2_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a ... 
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "LaunchPad.h"
#include "lcd1602.h"
#include "uart.h"
#include "adc.h"
#include "ti/devices/msp/peripherals/hw_oa.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab10_part2(void);              //Part 1 implementation
void inc_seg7(bool);                  //Option 1 implementation
void display_temp(void);                  //Option 2 implementation
void flash_leds(void);                  //Option 3 implementation
void shutdown_message(void);                  //Option 4 implementation
void uart_print_string(const char *string);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define DEGREES                  0xDF    //Value for degree symbol on LCD
#define THERMISTOR               5       //Thermistor Channel (5)
#define LED_BAR_L0_TO_LD7        0xFF    //Hex value enables full LED bar


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// Define a structure to hold different data types

int main(void)
{
    //Configure Launchpad Boards
    clock_init_40mhz();
    launchpad_gpio_init();
    led_init();
    seg7_init();
    I2C_init();
    lcd1602_init();
    UART_init(115200);
    lcd_clear();

    ADC0_init(ADC12_MEMCTL_VRSEL_INTREF_VSSA);
    
    // PART 2
    run_lab10_part2();

    leds_off();
    seg7_off();
 
    //Endless loop to prevent program from ending
    //while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    TBD
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
//------------------------------------------------------------------------------
void run_lab10_part2(void)
{
    char input;
    bool done = false;
    bool finished = false;

    while (!finished)
    {
        uart_print_string(
            "\n\nMENU OPTIONS\n"
            "   1. Increment count on 7-segment display\n"
            "   2. Show current temperature\n"
            "   3. Flash LEDs 3 times\n"
            "   4. End Program\n"
            "Enter your selection: ");
        inc_seg7(false);
        
        while(!done)
        {
            input = UART_in_char();
            
            if (input == '1')
            {
                UART_out_char(input);
                uart_print_string("\nIncremented Counter on 7-Segment Display.");
                inc_seg7(true);
            }
            else if (input == '2')
            {
                UART_out_char(input);
                display_temp();
            }
            else if (input == '3')
            {
                UART_out_char(input);
                seg7_off();
                led_enable();
                flash_leds();
                led_disable();
                inc_seg7(false);
            }
            else if (input == '4')
            {
                UART_out_char(input);
                shutdown_message();
                finished = true;
            }
            else
            {
                UART_out_char(input);
                uart_print_string("\nInvalid Option");
            }
            done = true;   //reset flag
        } /*End of While*/
        done = false;   //reset flag
    } /*End of Program*/
}

//------------------------------------------------------------------------------
void inc_seg7(bool enable_increment)
{
    static uint8_t seg7_counter = 0;

    if (enable_increment)
    {
        seg7_counter++;
        if (seg7_counter > 9)
        {
            seg7_counter = 0;
        }
    }
    seg7_hex(seg7_counter, SEG7_DIG0_ENABLE_IDX);
}

//------------------------------------------------------------------------------
void display_temp(void)
{
    int therm_adc_value = ADC0_in(THERMISTOR);
    float temp_Celsius = thermistor_calc_temperature(therm_adc_value);
    float temp_Fahrenheit = (temp_Celsius * 9 / 5) + 32;

    //Display temperature on LCD
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("TEMP = ");
    lcd_write_byte(temp_Fahrenheit);
    lcd_write_char(DEGREES);               
    lcd_write_string("F");

    uart_print_string("\nDisplayed current temperature on LCD screen.");
}

//------------------------------------------------------------------------------
void flash_leds(void)
{
    uart_print_string("\nFlashing LED Bar, 3 times.");

    uint8_t loop = 0;
    do
    {
        leds_on(LED_BAR_L0_TO_LD7);
        msec_delay(500);            
        leds_off();
        msec_delay(500); 
        loop++;
    } while (loop < 3);
}

//------------------------------------------------------------------------------
void run_option4(void)
{
    uart_print_string("\nThank you for using the program.");
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("Program Stopped");
}

//------------------------------------------------------------------------------
void uart_print_string(const char *string)
{
    while (*string != '\0')
    {
        UART_out_char(*string++);
    }
}