//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  TBD
//
//       LAB NAME:  TBD
//
//      FILE NAME:  lab7_main.c
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

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab7_part1(void);
void run_lab7_part2(void);
void debounce(void);
void wait_for_PB2(void);
void lcd_string_parser(char* string, uint8_t start_lcd_addr, uint8_t max_lcd_addr);  // Declare the function prototype

#define MSPM0_CLOCK_FREQUENCY        (40E6)
#define SYST_TICK_PERIOD             (10.25E-3)
#define SYST_TICK_PERIOD_COUNT       (SYST_TICK_PERIOD * MSPM0_CLOCK_FREQUENCY)

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define NUM_STATES                      18

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

    
    sys_tick_disable();
    seg7_off();

    //Wait here forever so SysTick can run
    for (; ;);
} /* main */

//-----------------------------------------------------------------------------
// PART 1:
//-----------------------------------------------------------------------------
void run_lab7_part1(void)
{
    uint8_t lcd_address;
    uint8_t idx = 0;
    char message[] = "Microcontrollers are fun.";
    bool done = false;

    while(!done){
        for (lcd_address = LCD_LINE2_ADDR + LCD_CHAR_POSITION_16; 
            (lcd_address > LCD_LINE2_ADDR) && (!done); lcd_address--) 
        {
            lcd_clear();
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
            msec_delay(50);
            
        }

        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        while ((message[idx] != NULL) && (!done))
        {
            lcd_clear();
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
            msec_delay(50);
            idx++;
        }
        idx = 0;
        lcd_clear();
    }
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("Part 1 Done");
}

//-----------------------------------------------------------------------------
// PART 2:
//-----------------------------------------------------------------------------
void run_lab7_part2(void)
{
    //Start Part 2
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Press PB2");
    wait_for_PB2();
    lcd_clear();
    lcd_write_string("Running Part 2");
    msec_delay(1000);
    lcd_clear();

    char long_message[] = "Microcontrollers are fun. I love programming in MSPM0+ assembly code!!!";
    uint8_t start_pos = LCD_LINE2_ADDR + LCD_CHAR_POSITION_16;
    uint8_t idx = 0;
    uint8_t message_len = 0;
    bool done = false;

    while (long_message[message_len] != '\0') {
        message_len++;
    }

    while (!done)
    {
        // Scroll the message to the left, starting at the far-right position
        for (uint8_t pos = start_pos; pos > LCD_LINE2_ADDR && !done; pos--)
        {
            lcd_clear();
            lcd_string_parser(&long_message[idx], pos, LCD_LINE2_ADDR + 15);
            msec_delay(50);

            // Check for PB1 press to exit
            if (is_pb_down(PB1_IDX))
            {
                debounce();
                while (is_pb_down(PB1_IDX)) {
                    debounce();
                }
                done = true;
            }
        }

        // When the message reaches the left edge, continue scrolling
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        while ((long_message[idx] != NULL) && !done)
        {
            lcd_clear();
            lcd_string_parser(&long_message[idx], LCD_LINE2_ADDR, LCD_LINE2_ADDR + 15);
            idx++;

            if (is_pb_down(PB1_IDX))
            {
                debounce();
                while (is_pb_down(PB1_IDX)) {
                    debounce();
                }
                done = true;
            }

            msec_delay(50);
        }
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
// Hold for PB2: Basic function that waits for a user to push PB1
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
// Debounce: 10ms delay to prevent key bouncing
//-----------------------------------------------------------------------------
void debounce() {
    msec_delay(10);
}

//-----------------------------------------------------------------------------
// make this good TBDTBD
//-----------------------------------------------------------------------------
void lcd_string_parser(char* string, uint8_t start_lcd_addr, uint8_t max_lcd_addr)
{
    uint8_t i = 0;
    uint8_t lcd_address = start_lcd_addr;
    
    while (string[i] != NULL && lcd_address <= max_lcd_addr)
    {
        lcd_set_ddram_addr(lcd_address);
        lcd_write_char(string[i]);
        lcd_address++;
        i++;
    }
}