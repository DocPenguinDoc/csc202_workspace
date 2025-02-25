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
        for (LDcounter = 0; LDcounter < 8; LDcounter++){     //Enable LED for .2 seconds. Cycle from LED 0 to 7
            led_on(LDcounter);
            msec_delay(200);
            led_off(LDcounter);
        }

        for (LDcounter = 6; LDcounter > 0; LDcounter--){     //Enable LED for .2 seconds. Cycle from LED 6 to 0
            led_on(LDcounter);
            msec_delay(200);
            led_off(LDcounter);
        }
    }


    //PART 3 - ********EXTREMELY BAD AND ONLY COUNTS TO 8
    msec_delay(500);   //Pause for .5 seconds before starting Part 3

    for (counter = 0; counter < 5; counter++)   //Loop sequence 2 times
    {
        led_on(0);
        msec_delay(5);
        led_off(0);
        led_on(1);
        msec_delay(5);
        led_on(0);
        msec_delay(5);
        led_on(2);
        led_off(1);
        led_off(0);
        msec_delay(5);
        led_on(0);
        msec_delay(5);
        led_on(1);
        msec_delay(5);
        led_on(3);
        led_off(2);
        led_off(1);
        led_off(0);
    }



 // Endless loop to prevent program from ending
 //while (1);

} /* main */

