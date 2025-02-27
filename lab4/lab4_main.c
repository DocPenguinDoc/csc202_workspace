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
    led_init();
    int counter;
    int LDcounter;

    
    //PART 1
    led_enable();       //Enables anode side
    leds_on(0x3C);      //Enables parts of the cathode side (00111100)
    
    msec_delay(500);    //Turn off LEDs after .5 seconds, ending Part 1
    leds_off();
    

    
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





 // Endless loop to prevent program from ending
 //while (1);

} /* main */


