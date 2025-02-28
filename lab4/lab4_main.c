//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  TBD
//
//       LAB NAME:  TBD
//
//      FILE NAME:  lab4_main.c
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

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab4_part2(void);
void run_lab4_part3(void);
void run_lab4_part4(void);
void run_lab4_part5(void);
void run_lab4_part6(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define LED_ON_TIME_MS      200     //Duration LED's stay on
#define PART_DELAY_MS       500     //Delay between parts
#define LED_BAR_L2_TO_LD5   0x3C    //Hexadecimal value to display "L" on 7-segment display
#define SEG7_L_VALUE        0x38    //Hexadecimal value to display "L" on 7-segment display
#define SEG7_4_VALUE        0x66    //Hexadecimal value to display "4" on 7-segment display

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

    
    //PART 1
    led_enable();                   //Enables anode side
    leds_on(LED_BAR_L2_TO_LD5);     //Enables parts of the cathode side
    msec_delay(500);                //Turn off LEDs after .5 seconds, ending Part 1
    leds_off();
    
    //PART 2
    msec_delay(PART_DELAY_MS*2);    //1-Second Delay before Part 2 starts
    run_lab4_part2();

    //PART 3
    msec_delay(PART_DELAY_MS);      //0.5-Second Delay before Part 3 starts
    run_lab4_part3();

    //PART 4
    msec_delay(PART_DELAY_MS);      //0.5-Second Delay before Part 4 starts
    run_lab4_part4();

    //PART 5
    msec_delay(PART_DELAY_MS);      //0.5-Second Delay before Part 5 starts
    run_lab4_part5();

    //PART 6
    msec_delay(PART_DELAY_MS);      //0.5-Second Delay before Part 6 starts
    run_lab4_part6();

    //COMPLETE
    leds_off();
    seg7_off();
    

    /*
    //PART 2 - ****NEED TO CHANGE TO A WHILE LOOP  &&&&&& *****Code for sequencing should be in a function called run_lab4_part2().
    msec_delay(1000);   //Pause for 1 second before starting Part 2

    for (counter = 0; counter < 5; counter++)   //Loop sequence 5 times
    {
        for (LDcounter = LED_BAR_LD0_IDX; LDcounter <= LED_BAR_LD7_IDX; LDcounter++) //Enable LED for .2 seconds. Cycle from LED 0 to 7
        {     
            led_on(LDcounter);
            msec_delay(200);
            leds_off();
        }

        for (LDcounter = LED_BAR_LD6_IDX; LDcounter > LED_BAR_LD0_IDX; LDcounter--)     //Enable LED for .2 seconds. Cycle from LED 6 to 0
        { 
            led_on(LDcounter);
            msec_delay(200);
            leds_off();
        }
    }
    led_on(LED_BAR_LD0_IDX);
    msec_delay(200);
    leds_off();

    
    //PART 3 - ********NOT A METHOD ETCC. DOES WORK THO
    msec_delay(500);   //Pause for .5 seconds before starting Part 3
    
    for (counter = 0; counter < 2; counter++)         //Loop sequence 2 times
    {
        for (LDcounter = 0; LDcounter < 256; LDcounter++)       //Counts to to 11111111 in binary
        {
            leds_on(LDcounter);
            msec_delay(100);        //Enable current binary value for 0.10 seconds
        }
    }

    

    //PART 4
    msec_delay(500);   //Pause for .5 seconds before starting Part 4
    leds_off();
    led_disable();
    seg7_init();
    seg7_on(0x38, SEG7_DIG0_ENABLE_IDX); // DIG0 will display the 'L'


    
    //PART 5
    msec_delay(500);   //Pause for .5 seconds before starting Part 4
    seg7_off();

    
    for (counter = 0; counter < 4; counter++)       //Loop 4 times
    {
        seg7_on(0x66, SEG7_DIG2_ENABLE_IDX);        //make 66 into a variable. Also could use hex 4
        msec_delay(3000);
        seg7_off();
        msec_delay(2000);
    }
    


    //PART 6          ****MUST USE VARIABLES NOT HEX NUMBERS> THIS GOES FOR EVERYTHING  ***ALSO YOU NEED ANOTHER LOOP WITH AN ARRAY. IT"S IN THE LECTURE SLIDES
    msec_delay(500);    //Pause for .5 seconds before starting Part 4

    for (counter = 0; counter < 200; counter++)
    {
        msec_delay(5);
        seg7_hex(0xC, SEG7_DIG0_ENABLE_IDX);
        msec_delay(5);
        seg7_hex(0xA, SEG7_DIG1_ENABLE_IDX);
        msec_delay(5);
        seg7_hex(0xF, SEG7_DIG2_ENABLE_IDX);
        msec_delay(5);
        seg7_hex(0xE, SEG7_DIG3_ENABLE_IDX);
    }

    
    leds_off();
    seg7_off();
    */




 // Endless loop to prevent program from ending
 //while (1);

} /* main */

void run_lab4_part2(void)
{
    uint8_t counter = 0;    //Counter variable for while loop   - Tracks # of times part 2 is repeated
    uint8_t LEDcounter;     //Counter varaible for for loops    - Tracks which LED to enable

    while (counter < 5)  // Repeat 5 times
    {
        //Cycle LEDs up from LD0 to LD7
        for (LEDcounter = LED_BAR_LD0_IDX; LEDcounter <= LED_BAR_LD7_IDX; LEDcounter++)
        {     
            led_on(LEDcounter);
            msec_delay(LED_ON_TIME_MS);
            leds_off();
        }

        //Cycle LEDs down from LD6 to LD0
        for (LEDcounter = LED_BAR_LD6_IDX; LEDcounter > LED_BAR_LD0_IDX; LEDcounter--)
        { 
            led_on(LEDcounter);
            msec_delay(LED_ON_TIME_MS);
            leds_off();
        }

        //Increment counter
        counter++;      
    }

    // End Part 2 with LD0 on
    led_on(LED_BAR_LD0_IDX);        //After final downward cycle, ensure LD0 enables
    msec_delay(LED_ON_TIME_MS);
    leds_off();
}

void run_lab4_part3(void)
{
    uint8_t counter = 0;    //Counter variable for while loop   - Tracks # of times part 3 is repeated
    uint16_t LEDcounter;    //Counter varaible for for loop     - Tracks which LED to enable
    uint8_t max_binary = 255;   //Maximum count of binary ("11111111") which is used for the counter

    while (counter < 2)  // Repeat 2 times
    {
        for (LEDcounter = 0; LEDcounter <= max_binary; LEDcounter++)    //Binary count from 00000000 to 11111111
        {
            leds_on(LEDcounter);  
            msec_delay(LED_ON_TIME_MS/2);   //Display binary numbers for 0.1 seconds
        }

        //Increment counter
        counter++;
    }
}

void run_lab4_part4(void)
{    
    leds_off();         //Ensure LEDs are off
    led_disable();      //Disable LEDs
    seg7_init();        //Initiate 7-segment display

    seg7_on(SEG7_L_VALUE, SEG7_DIG0_ENABLE_IDX);    //Display "L" on DIG0
}

void run_lab4_part5(void)
{
    uint8_t counter;

    msec_delay(PART_DELAY_MS);  // Delay before Part 5 starts

    for (counter = 0; counter < 4; counter++)  // Repeat 4 times
    {
        seg7_on(SEG7_4_VALUE, SEG7_DIG2_ENABLE_IDX);    //Display "4" on DIG2 for 3 seconds
        msec_delay(LED_ON_TIME_MS*15);
        
        seg7_off();                                     //Display nothing for 2 seconds
        msec_delay(LED_ON_TIME_MS*10);
    }
}

void run_lab4_part6(void)
{
    uint8_t counter = 0;    //Counter variable for while loop   - Tracks # of times part 3 is repeated
    uint8_t SEGcounter;     //Counter varaible for for loop     - Tracks which segment to enable
    uint8_t CAFE_values[] = {0xC, 0xA, 0xF, 0xE};   //Array of hex values for "CAFE"

    while (counter < 200)   //Display "CAFE" 200 times
    {
        for (SEGcounter = 0; SEGcounter < 4; SEGcounter++)  //Loop through each letter, and associated digit to display
        {
            msec_delay(5);
            seg7_hex(CAFE_values[SEGcounter], SEGcounter);
        }

        //Increment counter
        counter++;
    }
}