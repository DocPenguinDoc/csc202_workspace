//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 10
//
//      FILE NAME:  lab10p1_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a test of the UART to demonstrate serial
//    communication with the microcontroller. It does this by reading character
//    inputs, echoing them to the serial terminal, and building a string which
//    will be displayed to the LCD screen once ENTER is pressed. 
//
//-----------------------------------------------------------------------------
//
// HARDWARE REQUIREMENTS:
//    - MSPM0G3507 Microcontroller
//    - CSC202 Expansion Board
//    - LCD1602 module
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
#include "ti/devices/msp/peripherals/hw_oa.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab10_part1(void);             //Part 1 implementation

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define MAX_SIZE        16              //Max buffer (string) size
#define BAUD_RATE   115200              //UART Baud Rate

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
    I2C_init();
    lcd1602_init();
    UART_init(BAUD_RATE);
    lcd_clear();

    // PART 1
    run_lab10_part1();
 
    //Endless loop to prevent program from ending
    //while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function is as a test of the UART to demonstrate serial
//    communication with the microcontroller. It does this by reading character
//    inputs, echoing them to the serial terminal, and building a string which
//    will be displayed to the LCD screen once ENTER is pressed. 
//    The string is displayed after "NAME: " on line 1, while line 2 displays
//    "Program Stopped".
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
void run_lab10_part1(void)
{
    char character;                 //User input character variable
    char buffer[MAX_SIZE];          //String that will be filled & displayed
    uint8_t idx = 0;                //Index counter to navigate string
    bool done = false;              //Flag to end character inputs

    while(!done)
    {
        //Read and push charcter input to serial terminal
        character = UART_in_char();
        UART_out_char(character);

        //"ENTER" -> End While loop & Null terminate the string
        if (character == '\r')
        {
            done = true;
            buffer[idx] = '\0';
        }
        //"BACKSPACE" -> Decrement the string (if there is anything)
        else if (character == '\b')
        {
            if (idx > 0)
            {
                idx--;
            }
        }
        //Else -> Add character to string, and increment index
        else
        {
            buffer[idx++] = character;
        }
    }

    //Display "Name" (the input string) and "Program Stopped" to LCD
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("NAME:");
    lcd_write_string(buffer);
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Program Stopped");
}