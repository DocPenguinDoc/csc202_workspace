//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 6
//
//      FILE NAME:  lab6_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a tester for the LCD screen
//    There are 4 tests:
//      - Test 1: Display the alphabet, demonstrating text-wrapping
//      - Test 2: Display numbers, demonstrating positioning & functions
//      - Test 3: Display countdown. Manipulate with PB1 and PB2
//      - Test 4: Display user inputs from keypad. Manipulate with PB1 and PB2
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
void run_lab6_part1(void);
void run_lab6_part2(void);
void run_lab6_part3(void);
void run_lab6_part4(void);
void debounce(void);
void wait_for_PB1(void);
void wait_for_PB2(void);

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
    clock_init_40mhz();
    launchpad_gpio_init();
    dipsw_init();
    I2C_init();
    lcd1602_init();
    lcd_clear();

    //PART 1
    run_lab6_part1();

    //PART 2
    run_lab6_part2();

    //PART 3
    run_lab6_part3();

    //PART 4
    keypad_init();
    run_lab6_part4();
 
    // Endless loop to prevent program from ending
    // while (1);

} /* main */

//-----------------------------------------------------------------------------
// PART 1: Display the alphabet, demonstrating text-wrapping
//-----------------------------------------------------------------------------
void run_lab6_part1(void)
{
    char letter = 'A';
    for (letter = 'A'; letter <= 'Z'; letter++) {
        lcd_write_char(letter);
        msec_delay(50);
        //Spill to line 2 when line 1 fills
        if (letter == ('A' + CHARACTERS_PER_LCD_LINE - 1)) {
		    lcd_set_ddram_addr(LCD_LINE2_ADDR);
        }
    }
}

//-----------------------------------------------------------------------------
// PART 2: Display numbers, demonstrating positioning & functions
//-----------------------------------------------------------------------------
void run_lab6_part2(void)
{
    uint32_t num32 = 1234567890;
    uint16_t num16 = 1234;
    uint8_t num8 = 123;
    
    //Start Part 2
    wait_for_PB2();
    lcd_clear();
    lcd_write_string("Running Part 2");
    msec_delay(1000);
    lcd_clear();
    
    //Disaply 32-bit value
    lcd_set_ddram_addr(3);                      //Line 1 Center
    lcd_write_quadbyte(num32);                  //Print 32-bit
    wait_for_PB1();                             //Wait for user
    debounce();
    
    //Display 16-bit value
    lcd_set_ddram_addr(LCD_LINE2_ADDR + 5);     //Line 2 Center
    lcd_write_doublebyte(num16);                //Print 16-bit
    wait_for_PB1();                             //Wait for user
    debounce();
    lcd_clear();                                //Clear
    
    //Display 8-bit value
    lcd_set_ddram_addr(6);                      //Line 1 Center
    lcd_write_byte(num8);                       //Print 8-bit
    wait_for_PB1();                             //Wait for user
    debounce();             
    lcd_clear();                                //Clear
    
    //End Part 2
    lcd_write_string("Part 2 Done");        
    msec_delay(500);
}

//-----------------------------------------------------------------------------
// PART 3: Display countdown. Manipulate with PB1 and PB2
//-----------------------------------------------------------------------------
void run_lab6_part3(void)
{
    uint8_t timer_count = 100;
    bool done = false;

    //Start Part 3
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Press PB2");
    wait_for_PB2();
    lcd_clear();
    lcd_write_string("Running Part 3");
    msec_delay(1000);
    lcd_clear();
    
    //Continious Countdown (until user exits)
    while (!done) {            
        //PB1 is pressed -> reset the timer
        if (is_pb_down(PB1_IDX)) {
            timer_count = 100;
            debounce();
            //Wait until PB1 is released
            while (is_pb_down(PB1_IDX)) {
                debounce();  
            }
        }

        //Display Countdown
        lcd_set_ddram_addr(6);                  //Line 1 center
        lcd_write_byte(timer_count);            //Print timer count
        msec_delay(200);                        //Decrement every .2 seconds
        lcd_clear();

        //Decrement timer or loop timer when finished.
        if (timer_count > 0) 
        {
            timer_count--;
        } 
        else 
        {
            timer_count = 100;
        }

        //PB2 is pressed -> stop the countdown
        if (is_pb_down(PB2_IDX)) {
            done = true;
            debounce();
            //Wait until PB2 is released
            while (is_pb_down(PB2_IDX)) {
                debounce();  
            }
        }
    }

    //End Part 3
    lcd_clear();
    lcd_write_string("Part 3 Done");
    msec_delay(500);
}

//-----------------------------------------------------------------------------
// PART 4: Display user inputs from keypad. Manipulate with PB1 and PB2
//-----------------------------------------------------------------------------
void run_lab6_part4(void)
{
    bool done = false;
    uint8_t key;
    uint8_t row = 0;
    uint8_t col = 0;
    
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Press PB2");
    wait_for_PB2();
    lcd_clear();
    lcd_write_string("Running Part 4");
    msec_delay(1000);
    lcd_clear();

    while (!done) {
        key = keypad_scan();                //Scan for keypress
        if (key != 0x10) {                  //Only runs after key presses         
            if (row >= 2)                   //Display full -> Clear
            {
                lcd_clear();
                row = 0;
                col = 0;
            }

            hex_to_lcd(key);                //Print keypress
            col++;                          //Increment character counter

            wait_no_key_pressed();          //Wait until key is released
        }

        //Row fills -> Next Row
        if (col >= 16) {
            col = 0;
            row++; 
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
        }
    
        //PB1 is pressed -> Clear & Reset Display
        if (is_pb_down(PB1_IDX)) {
            lcd_clear();
            row = 0;
            col = 0;
            debounce();
        }

        //PB2 pressed -> Exit the loop and End Program
        if (is_pb_down(PB2_IDX)) {
            done = true;
            lcd_clear();
            lcd_write_string("Program Stopped");
            debounce();
        }

        debounce();
    }
}

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
// Hold for PB1: Basic function that waits for a user to push PB1
//-----------------------------------------------------------------------------
void wait_for_PB1() {
    //Wait for input
    while (is_pb_up(PB1_IDX))
    { 
        debounce(); 
    }
    //Wait for user to release input
    while (is_pb_down(PB1_IDX)) {
        debounce();
    }
}


//-----------------------------------------------------------------------------
// Debounce: 10ms delay to prevent key bouncing
//-----------------------------------------------------------------------------
void debounce() {
    msec_delay(10);
}
