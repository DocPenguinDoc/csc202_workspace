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
//      - Test 2: Display numbers, demonstrating positioning & methods
//      - Test 3: Use keypad to display a binary number on LED bar, 8 times
//      - Test 4: Flash LED bar, quantity based on keypad button, 4 times
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
// PART 1:
//-----------------------------------------------------------------------------
void run_lab6_part1(void)
{
    char letter = 'A';
    for (letter = 'A'; letter <= 'Z'; letter++) {
        lcd_write_char(letter);
        msec_delay(50);
        if (letter == ('A' + CHARACTERS_PER_LCD_LINE - 1)) {
		    lcd_set_ddram_addr(LCD_LINE2_ADDR);
        }
    }
}

//-----------------------------------------------------------------------------
// PART 2:
//-----------------------------------------------------------------------------
void run_lab6_part2(void)
{
    uint32_t num32 = 1234567890;
    uint16_t num16 = 1234;
    uint8_t num8 = 123;
    
    wait_for_PB2();
    lcd_clear();
    lcd_write_string("Running Part 2");
    msec_delay(1000);
    lcd_clear();
    
    lcd_set_ddram_addr(3);       //center?
    lcd_write_quadbyte(num32);      //Need to center
    wait_for_PB1();
    debounce();
    
    lcd_set_ddram_addr(LCD_LINE2_ADDR + 5);       //center?
    lcd_write_doublebyte(num16);    //Need to center
    wait_for_PB1();
    debounce();
    lcd_clear();
    
    lcd_set_ddram_addr(6);       //center?
    lcd_write_byte(num8);           //Need to center
    wait_for_PB1();
    debounce();
    lcd_clear();
    
    lcd_write_string("Part 2 Done");
    msec_delay(500);
}

//-----------------------------------------------------------------------------
// PART 3:
//-----------------------------------------------------------------------------
void run_lab6_part3(void)
{
    uint8_t timer_count = 100;
    bool done = false; 
    uint8_t display_value = 0;

    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("Press PB2");

    wait_for_PB2();
    lcd_clear();
    lcd_write_string("Running Part 3");
    msec_delay(1000);
    lcd_clear();
    
    while (!done) {            
        //PB1 is pressed -> reset the timer
        if (is_pb_down(PB1_IDX)) {
            timer_count = 100;  // Reset the timer to 100
            debounce();  // Debounce PB1
            while (is_pb_down(PB1_IDX)) {
                debounce();  // Wait until PB1 is released
            }
        }

        lcd_set_ddram_addr(6);       //center?
        lcd_write_byte(timer_count);
        msec_delay(200);
        lcd_clear();

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
            done = true;  // Stop the loop
            debounce();  // Debounce PB2
            while (is_pb_down(PB2_IDX)) {
                debounce();  // Wait until PB2 is released
            }
        }
    }

    lcd_clear();
    lcd_write_string("Part 3 Done");
    msec_delay(500);
}

//-----------------------------------------------------------------------------
// PART 4:
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
        key = keypad_scan();
        
        

        if (key != 0x10) {  //Ignore if no key is pressed (key = 0x10 means no key)            
            if (row >= 2) 
            {
                lcd_clear();
                row = 0;
                col = 0;
            }

            hex_to_lcd(key);  // Write the key OLD
            col++; //Needs to be 5 not 16 if using bytes

            /*
            //OLD - TURN INTO METHOD AND COMMENT IT OUT
            //Printing Keypress - NEED TO CHANGE OUT FOR hex_to_lcd(); STUFF - Problem is the col counter - 
            //DO NOT NEED DOUBLE DIGIT NUMBERS - SHOULD PRINT A, B, C, ETC.
            switch (key) {
                case 0:
                    lcd_write_char('0');
                    col++;
                    break;
                case 1:
                    lcd_write_char('1');
                    col++;
                    break;
                case 2:
                    lcd_write_char('2');
                    col++;
                    break;
                case 3:
                    lcd_write_char('3');
                    col++;
                    break;
                case 4:
                    lcd_write_char('4');
                    col++;
                    break;
                case 5:
                    lcd_write_char('5');
                    col++;
                    break;
                case 6:
                    lcd_write_char('6');
                    col++;
                    break;
                case 7:
                    lcd_write_char('7');
                    col++;
                    break;
                case 8:
                    lcd_write_char('8');
                    col++;
                    break;
                case 9:
                    lcd_write_char('9');
                    col++;
                    break;
                case 10:
                    lcd_write_char('1');
                    col++;
                    if (col >= 16) {
                        col = 0;
                        row++; 
                        lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    }
                    if (row >= 2) {
                        lcd_clear();
                        row = 0;
                        col = 0;
                    }
                    lcd_write_char('0');
                    col++;
                    break;
                case 11:
                    lcd_write_char('1');
                    col++;
                    if (col >= 16) {
                        col = 0;
                        row++; 
                        lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    }
                    if (row >= 2) {
                        lcd_clear();
                        row = 0;
                        col = 0;
                    }
                    lcd_write_char('1');
                    col++;
                    break;
                case 12:
                    lcd_write_char('1');
                    col++;
                    if (col >= 16) {
                        col = 0;
                        row++; 
                        lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    }
                    if (row >= 2) {
                        lcd_clear();
                        row = 0;
                        col = 0;
                    }
                    lcd_write_char('2');
                    col++;
                    break;
                case 13:
                    lcd_write_char('1');
                    col++;
                    if (col >= 16) {
                        col = 0;
                        row++; 
                        lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    }
                    if (row >= 2) {
                        lcd_clear();
                        row = 0;
                        col = 0;
                    }
                    lcd_write_char('3');
                    col++;
                    break;
                case 14:
                    lcd_write_char('1');
                    col++;
                    if (col >= 16) {
                        col = 0;
                        row++; 
                        lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    }
                    if (row >= 2) {
                        lcd_clear();
                        row = 0;
                        col = 0;
                    }
                    lcd_write_char('4');
                    col++;
                    break;
                case 15:
                    lcd_write_char('1');
                    col++;
                    if (col >= 16) {
                        col = 0;
                        row++; 
                        lcd_set_ddram_addr(LCD_LINE2_ADDR);
                    }
                    if (row >= 2) {
                        lcd_clear();
                        row = 0;
                        col = 0;
                    }
                    lcd_write_char('5');
                    col++;
                    break;
            }
            */

            wait_no_key_pressed();
        }

        if (col >= 16) {
            col = 0;
            row++; 
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
        }
        
    
        //If PB1 is pressed, clear the LCD and reset positions
        if (is_pb_down(PB1_IDX)) {
            lcd_clear();
            row = 0;
            col = 0;
            debounce();  // Debounce PB1
        }

        //If PB2 is pressed, exit the loop and display "Program Stopped"
        if (is_pb_down(PB2_IDX)) {
            done = true;
            lcd_clear();
            lcd_write_string("Program Stopped");
            debounce();  // Debounce PB2
        }

        debounce();
    }
}

//-----------------------------------------------------------------------------
// tbd
//-----------------------------------------------------------------------------
void wait_for_PB2() {
    while (is_pb_up(PB2_IDX))
    { 
        debounce(); 
    } 
    while (is_pb_down(PB2_IDX)) {
        debounce();
    }
}

//-----------------------------------------------------------------------------
// tbd
//-----------------------------------------------------------------------------
void wait_for_PB1() {
    while (is_pb_up(PB1_IDX))
    { 
        debounce(); 
    } 
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
