//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 4
//
//      FILE NAME:  lab4_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a tester for the LED Bar and 7-Segment display.
//    There are 6 tests:
//      - Test 1: Enable LED Bar LD2 - LD5
//      - Test 2: Sequence up and down from LD0 to LD7, 4 times
//      - Test 3: Binary count from 00000000 to 11111111, 2 times
//      - Test 4: Display "L" on 7-segment display
//      - Test 5: Display "4" on 7-segment display, 4 times
//      - Test 6: DIsplay "CAFE" on 7-segment display
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
#define LED_BAR_L2_TO_LD5   0x3C    //Hex value enable LD2-LD5 on the Light Bar
#define SEG7_L_VALUE        0x38    //Hex value to display "L" on 7-SEG display
#define SEG7_4_VALUE        0x66    //Hex value to display "4" on -7SEG display

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
    msec_delay(500);                //Turn off LEDs after .5 seconds
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
    

 // Endless loop to prevent program from ending
 //while (1);

} /* main */

//-----------------------------------------------------------------------------
// PART 2: Sequence LED Bar from LD0 up to LD7 and back down to LD0 5 times
//-----------------------------------------------------------------------------
void run_lab4_part2(void)
{
    uint8_t counter = 0;    //Tracks # of times part 2 is repeated (While Loop)
    uint8_t LEDcntr;        //Tracks which LED to enable (For Loops)

    while (counter < 5)  // Repeat 5 times
    {
        //Cycle LEDs up from LD0 to LD7
        for (LEDcntr = LED_BAR_LD0_IDX; LEDcntr <= LED_BAR_LD7_IDX; LEDcntr++)
        {     
            led_on(LEDcntr);
            msec_delay(LED_ON_TIME_MS);
            leds_off();
        }

        //Cycle LEDs down from LD6 to LD0
        for (LEDcntr = LED_BAR_LD6_IDX; LEDcntr > LED_BAR_LD0_IDX; LEDcntr--)
        { 
            led_on(LEDcntr);
            msec_delay(LED_ON_TIME_MS);
            leds_off();
        }

        //Increment counter
        counter++;      
    }

    // End Part 2 with LD0 on
    led_on(LED_BAR_LD0_IDX);    //After final down cycle, ensure LD0 enables
    msec_delay(LED_ON_TIME_MS);
    leds_off();
} /* part 2 */

//-----------------------------------------------------------------------------
// PART 3: Count to 255 in binary on the LED bar, twice
//-----------------------------------------------------------------------------
void run_lab4_part3(void)
{
    uint8_t counter = 0;    //Tracks # of times part 3 is repeated (While Loop)
    uint16_t LEDcounter;    //Tracks which LED to enable (For Loop)
    uint8_t max_binary = 255;   //Maximum count of binary ("11111111")

    while (counter < 2)  // Repeat 2 times
    {
        for (LEDcounter = 0; LEDcounter <= max_binary; LEDcounter++)
        {
            leds_on(LEDcounter);            //Count to 255 in binary
            msec_delay(LED_ON_TIME_MS/2);   //Display each # for 0.1 seconds
        }

        //Increment counter
        counter++;
    }
} /* part 3 */

//-----------------------------------------------------------------------------
// PART 4: Display "L" on the seven-segment display
//-----------------------------------------------------------------------------
void run_lab4_part4(void)
{    
    leds_off();         //Ensure LEDs are off
    led_disable();      //Disable LEDs
    seg7_init();        //Initiate 7-segment display

    seg7_on(SEG7_L_VALUE, SEG7_DIG0_ENABLE_IDX);    //Display "L" on DIG0
} /* part 4 */

//-----------------------------------------------------------------------------
// PART 4: Display "4" on the seven-segment display, 4 times
//-----------------------------------------------------------------------------
void run_lab4_part5(void)
{
    uint8_t counter;

    msec_delay(PART_DELAY_MS);  // Delay before Part 5 starts

    for (counter = 0; counter < 4; counter++)  // Repeat 4 times
    {
        seg7_on(SEG7_4_VALUE, SEG7_DIG2_ENABLE_IDX);    //Display "4" on DIG2
        msec_delay(LED_ON_TIME_MS*15);                  //for 3 seconds
        
        seg7_off();                                     //Display nothing
        msec_delay(LED_ON_TIME_MS*10);                  //for 2 seconds
    }
} /* part 5 */

//-----------------------------------------------------------------------------
// PART 4: Display "CAFE" on the seven-segment display
//-----------------------------------------------------------------------------
void run_lab4_part6(void)
{
    uint8_t counter = 0;    //Tracks # of times part 3 is repeated (While loop)
    uint8_t SEGcounter;     //Tracks which segment to enable (For loop)
    uint8_t CAFE_values[] = {0xC, 0xA, 0xF, 0xE};   //Array containing "CAFE"

    while (counter < 200)   //Display "CAFE" 200 times
    {
        for (SEGcounter = 0; SEGcounter < 4; SEGcounter++)
        {
            msec_delay(5);  //Multiplexes through each digit
            seg7_hex(CAFE_values[SEGcounter], SEGcounter);  //Display letter
        }

        //Increment counter
        counter++;
    }
} /* part 6 */