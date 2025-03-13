//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab #5: Interfacing to Input Device
//
//      FILE NAME:  lab5_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a tester for the Pushbuttons, Dipswitch, & Keypad.
//    There are 4 tests:
//      - Test 1: Toggle seg7 display with Pushbutton 1, 3 times
//      - Test 2: Use dipswitch to read a number displayed to Seg7, 3 times
//      - Test 3: Use keypad to display a binary number on LED bar, 8 times
//      - Test 4: Flash LED bar, quantity based on keypad button, 4 times
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "LaunchPad.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab5_part1(void);
void run_lab5_part2(void);
void run_lab5_part3(void);
void run_lab5_part4(void);
void debounce(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define PART_DELAY_MS       500     //Delay between parts
#define SEG7_3_VALUE        0x4F    //Hex value to display "3" on 7-SEG display
typedef enum { GET_LOW, GET_HIGH, DISPLAY } state_t;
                                    //Defined datatype that assigns state names

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// Define a structure to hold different data types

int main(void)
{
    clock_init_40mhz();
    launchpad_gpio_init();
    led_init();
    seg7_init();
    lpsw_init();
    dipsw_init();
    


    //PART 1
    run_lab5_part1();
    
    //PART 2
    msec_delay(PART_DELAY_MS);          //0.5-Second Delay before Part 2 starts
    run_lab5_part2();

    keypad_init();                      //Enable keypad & LED bar before pt 3&4
    led_enable();

    //PART 3
    msec_delay(PART_DELAY_MS);          //0.5-Second Delay before Part 3 starts
    run_lab5_part3();

    //PART 4
    msec_delay(PART_DELAY_MS);          //0.5-Second Delay before Part 4 starts
    run_lab5_part4();


    // Endless loop to prevent program from ending
    // while (1);
} /* main */

//-----------------------------------------------------------------------------
// PART 1: Toggle seg7 display 3 times with Pushbutton 1
//-----------------------------------------------------------------------------
void run_lab5_part1(void)
{
    uint8_t loop_count = 0;             //Loop counter for While loop
    bool display_on = false;            //Boolean value toggles between states

    while (loop_count < 3) {            //Toggle digit 3 times
        if (is_pb_down(PB1_IDX))
        {
            if (display_on)             //if-else to act as display toggle
            {
                seg7_off();             //Turning off display iterates loop
                display_on = false;
                loop_count++;
            }
            else 
            {
                seg7_on(SEG7_3_VALUE, SEG7_DIG0_ENABLE_IDX);
                display_on = true;      //Turning on display shows "3" on DIG0
            }

            debounce();                 //Small delay to confirm button state
            while (is_pb_down(PB1_IDX)) {
                debounce();             //PB1 remains pushed -> Wait
            }
        }
        debounce();                     //Delay to prevent double-read btn push
    }
} /* part 1 */


//-----------------------------------------------------------------------------
// PART 2: Use dipswitch to read a binary number displayed to Seg7, 3 times
//-----------------------------------------------------------------------------
void run_lab5_part2(void)
{
    uint8_t low_value = 0;              //Lower 4 digits that are displayed
    uint8_t high_value = 0;             //Upper 4 digits that are displayed
    uint8_t display_value = 0;          //Combined high & low value for display
    uint8_t loop_count = 0;             //Loop counter for While loop
    state_t current_state = GET_LOW;    //State initializes at "GET_LOW"

    while (loop_count < 3) {            //Read & Display 3 Numbers
        switch (current_state) {        //Constantly reading current state
            case GET_LOW:               //Reads the dip switch states for dcba
                low_value = dipsw_read(); 
                debounce();             //Small delay to ensure swiches are set
                while (is_lpsw_down(LP_SW2_IDX)) {
                    debounce();         //Delay confirms if btn remains pushed
                    current_state = GET_HIGH;
                }                       //SW2 pushed -> Move to GET_HIGH
                break;

            case GET_HIGH:              //Reads the dip switch states for hgfe
                high_value = dipsw_read(); 
                debounce();             //Small delay to ensure swiches are set
                while (is_lpsw_down(LP_SW2_IDX)) {
                    debounce();         //Delay confirms if btn remains pushed
                    current_state = DISPLAY;
                }                       //SW2 pushed -> Move to DISPLAY
                break;

            case DISPLAY:               //Set and Display the combined value
                display_value = (high_value << 4) | (low_value);

                if (is_pb_down(PB1_IDX))//PB1 Down -> Display on Digit 2
                {
                    seg7_on(display_value, SEG7_DIG2_ENABLE_IDX);
                }
                else                    //Otherwise -> Display on Digit 0
                {
                    seg7_on(display_value, SEG7_DIG0_ENABLE_IDX);
                }

                debounce();             //Small delay to ensure value displayed
                while (is_lpsw_down(LP_SW2_IDX)) {
                    debounce();         //Delay confirms if btn remains pushed
                    current_state = GET_LOW;
                                        //SW2 pushed -> Move to DISPLAY
                    if (is_lpsw_up(LP_SW2_IDX)) 
                    {
                        loop_count++;   //SW2 lifted -> Read new number
                    }
                    
                }
                
                break;
        }
        debounce();                     //Debounce between all state changes
    }
    leds_off();
    seg7_off();
} /* part 2 */

//-----------------------------------------------------------------------------
// PART 3: Use keypad to display a binary number on LED bar, 8 times
//-----------------------------------------------------------------------------
void run_lab5_part3(void)
{
    uint8_t key = 0;                    //Value of the pushed key
    uint8_t loop_count = 0;             //Loop counter for While loop
    
    while (loop_count < 8) {            //Display 8 Numbers
        key = getkey_pressed();

        if (key < 16)
        {
            leds_on(key);               //Enable LED's based on the pressed key
            wait_no_key_pressed();      //Pauses until no keys are pressed
            loop_count++;               //Move onto next number
        }
        debounce();                     //Delay to ensure doesn't double-read #
    }
    leds_off();                         //Turn off LED's when finished
} /* part 3 */

//-----------------------------------------------------------------------------
// PART 4: Flash LED bar a # of times based on pushed keypad button, 4 times
//-----------------------------------------------------------------------------
void run_lab5_part4(void)
{
    uint8_t key = 0;                    //Value of the pushed key
    uint8_t flash_count = 0;            //Quantity of required flashes
    uint8_t loop_count = 0;             //Loop counter for While loop

    while (loop_count < 4) {            //Flash 4 different quantities
        key = keypad_scan(); 

        if (key != 0x10) {              //Ensure a valid key press
            flash_count = key;          //Set # of flashes
                                        //Flash all LED's for # of flashes
            for (uint8_t i = 0; i < flash_count; i++) {
                leds_on(0xFF);  
                msec_delay(500);        //LED's are on for .5 seconds
                leds_off();      
                msec_delay(500);        //LED's are off for .5 seconds
            }
            wait_no_key_pressed();
            loop_count++;               //Move onto next number
        }
        debounce();                     //Delay to ensure doesn't double-read #
    }
} /* part 4 */

//-----------------------------------------------------------------------------
// Debounce: 10ms delay to prevent key bouncing
//-----------------------------------------------------------------------------
void debounce() {
    msec_delay(10);
}
