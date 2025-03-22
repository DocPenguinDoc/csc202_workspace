//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 7
//
//      FILE NAME:  lab7_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a demonstration of SysTick timer to manage an
//    operation (Flashing "SOS" on seven-segment display) while the main focus
//    is on creating scrolling messages for the LCD screen.
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

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab7_part1(void);
void run_lab7_part2(void);
void debounce(void);
void wait_for_PB2(void);
void lcd_string_parser( char* string, uint8_t start_lcd_addr, 
                        uint8_t max_lcd_addr);

#define MSPM0_CLOCK_FREQUENCY                                            (40E6)
#define SYST_TICK_PERIOD                                             (10.25E-3)
#define SYST_TICK_PERIOD_COUNT       (SYST_TICK_PERIOD * MSPM0_CLOCK_FREQUENCY)

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define NUM_STATES                                                           18

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
//defines the seven segment display for SOS
const uint8_t seg7_letter_code[] = 
{
    0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00,     //S
    0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x00,     //O
    0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00      //S
};

//These timings for morse code dots and dashes can vary slightly based on
//different implementations, but the 1:3 ratio for dots:dashes is a
//typuiical[sic] ration for Morse code. The delay times in terms of number
//SysTick interrups.
const uint8_t delay_count[] =
{
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x18,     //dots
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24,     //dash
    0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x30      //dots
};


// Define a structure to hold different data types

int main(void)
{
    
    clock_init_40mhz();
    launchpad_gpio_init();
    led_init();
    led_disable();
    seg7_init();
    sys_tick_init(SYST_TICK_PERIOD_COUNT);
    dipsw_init();
    I2C_init();
    lcd1602_init();
    lcd_clear();

    //PART 1
    run_lab7_part1();

    //PART 2
    run_lab7_part2();


    //Endless loop to prevent program from ending
    //while (1);

    //Disable SOS Message after program completion
    sys_tick_disable();
    seg7_off();

    //Wait here forever so SysTick can run
    for (; ;);
} /* main */

//-----------------------------------------------------------------------------
// PART 1: Scroll a specific message across the LCD
//-----------------------------------------------------------------------------
void run_lab7_part1(void)
{
    uint8_t lcd_address;
    uint8_t idx = 0;
    //Scrolling LCD Message
    char message[] = "Microcontrollers are fun.";   
    bool done = false;

    //Continious while loop scrolling message across LCD, until a set done flag
    while(!done){
        //Message scrolls from right to left, from end of line 2 to the start
        for (lcd_address = LCD_LINE2_ADDR + LCD_CHAR_POSITION_16; 
            (lcd_address > LCD_LINE2_ADDR) && (!done); lcd_address--) 
        {
            lcd_clear();
            //Check PB1 to set exit flag
            if (is_pb_down(PB1_IDX))
            {
                debounce();
                //Wait until PB2 is released
                while (is_pb_down(PB1_IDX)) {
                    debounce();  
                }
                done = true;
            }
            lcd_set_ddram_addr(lcd_address);
            lcd_write_string(message);
            //Delay controls scroll speed
            msec_delay(50);                     
            
        }

        //When the message reaches the left edge, continue scrolling
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        while ((message[idx] != NULL) && (!done))
        {
            lcd_clear();
            //Check PB1 to set exit flag
            if (is_pb_down(PB1_IDX))
            {
                debounce();
                //Wait until PB2 is released
                while (is_pb_down(PB1_IDX)) {
                    debounce();  
                }
                done = true;
            }
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            lcd_write_string(message + idx);
            //Delay controls scroll speed
            msec_delay(50);
            idx++;
        }
        //Reset LCD and index to restart message scroll
        idx = 0;
        lcd_clear();
    }
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("Part 1 Done");
}

//-----------------------------------------------------------------------------
// PART 2: Scroll a longer message
//-----------------------------------------------------------------------------
void run_lab7_part2(void)
{
    //Scrolling LCD message
    char long_message[] =   "Microcontrollers are fun. 
                            I love programming in MSPM0+ assembly code!!!";
    uint8_t start_pos = LCD_LINE2_ADDR + LCD_CHAR_POSITION_16;
    uint8_t idx = 0;
    uint8_t message_len = 0;
    bool done = false;

    //Start Part 2
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Press PB2");
    wait_for_PB2();
    lcd_clear();
    lcd_write_string("Running Part 2");
    msec_delay(1000);
    lcd_clear();

    //Calculate length of message
    while (long_message[message_len] != '\0') {
        message_len++;
    }

    while (!done)
    {
        //Message scrolls from right to left, from end of line 2 to the start
        for (uint8_t pos = start_pos; pos > LCD_LINE2_ADDR && !done; pos--)
        {
            lcd_clear();
            lcd_string_parser(&long_message[idx], pos, LCD_LINE2_ADDR + 15);

            //Check PB1 to set exit flag
            if (is_pb_down(PB1_IDX))
            {
                debounce();
                while (is_pb_down(PB1_IDX)) {
                    debounce();
                }
                done = true;
            }

            //Delay controls scroll speed
            msec_delay(50);
        }

        //When the message reaches the left edge, continue scrolling
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        while ((long_message[idx] != NULL) && !done)
        {
            lcd_clear();
            lcd_string_parser(&long_message[idx], LCD_LINE2_ADDR, LCD_LINE2_ADDR + 15);
            idx++;

            //Check PB1 to set exit flag
            if (is_pb_down(PB1_IDX))
            {
                debounce();
                while (is_pb_down(PB1_IDX)) {
                    debounce();
                }
                done = true;
            }

            //Delay controls scroll speed
            msec_delay(50);
        }
        //Reset index to restart message scroll
        idx = 0;
    }

    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("Part 2 Done");
}


//-----------------------------------------------------------------------------
// DESCRIPTION:
//   This function represents the ISR (Interrupt Service Routine) for
//   the SysTick timer. It is called at regulare intervals based on the
//   configured SysTick period. This ISR is responsible for managing the 
//   timing and display of the Morse code for "SOS" on the seven-segment
//   display. It controls the blink timing for the letters, with quick 
//   blinks for "S" (dot-dot-dot) and slower blinks for "O" (dash-dash-dash).
//   The sequence of letters is repeated indefinitely.
//
// INPUT PARAMETERS:
//   none
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
//-----------------------------------------------------------------------------
void SysTick_Handler(void)
{
    static uint16_t delay_time = 1;
    static uint16_t code_index = 0;

    delay_time--;
    if (delay_time == 0)
    {
        // delay time has expires so now move on to next letter to display
        seg7_on(seg7_letter_code[code_index], SEG7_DIG0_ENABLE_IDX);

        // get next delay time
        delay_time = delay_count[code_index];
        code_index++;

        if (code_index == NUM_STATES)
        {
            code_index = 0;
        } /* if */
    } /* if */
} /* SysTick_Handler */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   This function waits for the user to push PB2
//
// INPUT PARAMETERS:
//   none
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
//-----------------------------------------------------------------------------
void wait_for_PB2() {
    //Wait for input
    while (is_pb_up(PB2_IDX))
    { 
        debounce(); 
    } 
    //Wait for user to release input
    while (is_pb_down(PB2_IDX)) {
        debounce();
    }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   This function is a defined delay to prevent key bouncing
//
// INPUT PARAMETERS:
//   none
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
//-----------------------------------------------------------------------------
void debounce() {
    msec_delay(10);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   This function parses a string and displays it on the LCD.
//   Ideal for strings that would exceed a line maximum.
//
// INPUT PARAMETERS:
//   char* string               - Reference to string being displayed on LCD
//   uint8_t start_lcd_addr     - Starting address where string is displayed
//   uint8_t max_lcd_addr       - Maximum address to which string is displayed
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
//-----------------------------------------------------------------------------
void lcd_string_parser(char* string, uint8_t start_lcd_addr, uint8_t max_lcd_addr)
{
    uint8_t i = 0;
    uint8_t lcd_address = start_lcd_addr;
    
    //Loop through string, displaying each character on LCD
    while (string[i] != NULL && lcd_address <= max_lcd_addr)
    {
        lcd_set_ddram_addr(lcd_address);
        lcd_write_char(string[i]);
        lcd_address++;
        i++;
    }
}