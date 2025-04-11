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
//    This program serves as another test of the UART to demonstrate serial
//    communication with the microcontroller. This test involves displaying a
//    menu to the user, who can input their option choice, which run relevent
//    functions.
//          Option 1. Increment counter on 7-segment display
//          Option 2. Show current temperature, in F, using thermistor
//          Option 3. Flash LED bar 3 times
//          Option 4. End Program
//
//-----------------------------------------------------------------------------
//
// HARDWARE REQUIREMENTS:
//    - MSPM0G3507 Microcontroller
//    - CSC202 Expansion Board
//    - LCD1602 module
//    - 7-Segment Display
//    - LED Bar
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
void run_lab10_part2(void);                     //Part 1 implementation
void inc_seg7(bool);                            //Option 1 implementation
void display_temp(void);                        //Option 2 implementation
void flash_leds(void);                          //Option 3 implementation
void shutdown_message(void);                    //Option 4 implementation
void uart_print_string(const char *string);     //UART String Printer

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define DEGREES                  0xDF    //Value for degree symbol on LCD
#define THERMISTOR               5       //Thermistor Channel (5)
#define LED_BAR_LD0_TO_LD7       0xFF    //Hex value enables full LED bar
#define BAUD_RATE                115200  //UART Baud Rate



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
    UART_init(BAUD_RATE);
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
//    This function displays a user to the menu, serving as a test of serial
//    communication with the Launchpad boad. The menu displays the following
//    four options to the user:
//          Option 1. Increment counter on 7-segment display
//          Option 2. Show current temperature, in F, using thermistor
//          Option 3. Flash LED bar 3 times
//          Option 4. End Program
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
    char input;                     //User input character variable
    bool done = false;              //Flag to end character inputs

    while (!done)
    {
        //Print Menu
        uart_print_string(
            "\n\nMENU OPTIONS\n"
            "   1. Increment count on 7-segment display\n"
            "   2. Show current temperature\n"
            "   3. Flash LEDs 3 times\n"
            "   4. End Program\n"
            "Enter your selection: ");
        //Start displaying 7-Segment counter at 0
        inc_seg7(false);
 
        //Read & Display user input from Serial Console
        input = UART_in_char();
        UART_out_char(input);
            
        //User input 1-4 -> Run respective option
        if (input == '1')
        {
            //Increment 7-Segment counter
            inc_seg7(true);
            //Display confirmation message to Serial Console
            uart_print_string("\nIncremented Counter on 7-Segment Display.");
        }
        else if (input == '2')
        {
            //Read & display temperature on LCD screen
            display_temp();
            //Display confirmation message to Serial Console
            uart_print_string("\nDisplayed current temperature on LCD screen.");
        }
        else if (input == '3')
        {
            //Display confirmation message to Serial Console
            uart_print_string("\nFlashing LED Bar, 3 times.");

            //Swap to LED Bar (from seg7)
            seg7_off();
            led_enable();
            //Flash LED bar
            flash_leds();
            //Swap back to seg7 and redisplay count (but do not increment)
            led_disable();
            inc_seg7(false);
        }
        else if (input == '4')
        {
            //Display confirmation message to Serial Console
            uart_print_string("\nThank you for using the program.");
            //Display shutdown message on LCD screen & set done flag
            shutdown_message();
            done = true;
        }
        //User input anything but 1-4 -> Print Invalid Option to terminal
        else
        {
            uart_print_string("\nInvalid Option");
        }
    }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function manages the counter, which is displayed on the 7-Segment
//    display. When called initially, it initializes a static variable to
//    store the count. If the passed argument is true, it will increment
//    that count before displaying it. Otherwise, it will display the count
//    as is.
//
// INPUT PARAMETERS:
//    enable-increment    - A boolean value that determines if the 7-Segment
//                          display is incrementing the count, or just
//                          displaying the current count.
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
//------------------------------------------------------------------------------
void inc_seg7(bool enable_increment)
{
    //Static counter variable
    static uint8_t seg7_counter = 0;

    //Optionally increment counter (Loops at 9, back to 0)
    if (enable_increment)
    {
        seg7_counter++;
        if (seg7_counter > 9)
        {
            seg7_counter = 0;
        }
    }

    //Display coutner variable to DIG0 on the 7-segment display
    seg7_hex(seg7_counter, SEG7_DIG0_ENABLE_IDX);
}

//------------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads an ADC value from the thermistor, and converts it to
//    Fahrenheit, before displaying it on the LCD.
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
void display_temp(void)
{
    //Read ADC value, and convert it to Fahrenheit
    int therm_adc_value = ADC0_in(THERMISTOR);
    float temp_Celsius = thermistor_calc_temperature(therm_adc_value);
    float temp_Fahrenheit = (temp_Celsius * 9 / 5) + 32;

    //Display Fahrenheit temperature on LCD
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("TEMP = ");
    lcd_write_byte(temp_Fahrenheit);
    lcd_write_char(DEGREES);               
    lcd_write_string("F");
}

//------------------------------------------------------------------------------
// DESCRIPTION:
//    This function flashes the LED bar three times, using a for loop
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
void flash_leds(void)
{
    //Flash LED bar 3 times
    for (uint8_t loop = 0; loop < 3; loop++)
    {
        //Enable all LEDs for .5 seconds
        leds_on(LED_BAR_LD0_TO_LD7);
        msec_delay(500);         
        //Disable all LEDs for .5 seconds   
        leds_off();
        msec_delay(500); 
    }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function displays the message "Program Stopped" to the LCD screen.
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
void shutdown_message(void)
{
    //Clear LCD, and display termination message
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("Program Stopped");
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function writes a string to the UART. Each character of the string
//    is written to the console one by one. This function performs no error 
//    checking to ensure the string is displayed properly on the UART.
//
// INPUT PARAMETERS:
//    string    - A pointer (address) to the null-terminated string to be 
//                written to the DDRAM
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
//------------------------------------------------------------------------------
void uart_print_string(const char *string)
{
    //Print characters until a \0 is reached
    while (*string != '\0')
    {
        //Print each character, and increment through string
        UART_out_char(*string++);
    }
}