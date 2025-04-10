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
#include "ti/devices/msp/peripherals/hw_oa.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab10_part1(void);              //Part 1 implementation

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


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
    UART_init(115200);
    lcd_clear();

    // PART 1
    run_lab10_part1();
 
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
void run_lab10_part1(void)
{
    char character;
    char buffer[16];
    uint8_t idx = 0;
    bool done = false;

    

    while(!done)
    {
        character = UART_in_char();
        UART_out_char(character);
        if (character == '\r')
        {
            done = true;
            buffer[idx] = '\0';
        }
        else if (character == '\b')
        {
            if (idx > 0)
            {
                idx--;
            }
        }
        else
        {
            buffer[idx++] = character;
        }
    }

    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("NAME:");
    lcd_write_string(buffer);
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Program Stopped");
}