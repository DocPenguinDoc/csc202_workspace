//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 7
//
//      FILE NAME:  lab7p3_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a demonstration of SysTick timer to manage an
//    operation (displaying count of active DIP switches on Seg7 display)
//    while the main focus is on a count of 0 to 99 on the LCD screen
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
void run_lab7_part3(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

#define MSPM0_CLOCK_FREQUENCY                                            (40E6)
#define SYST_TICK_PERIOD                                             (10.25E-3)
#define SYST_TICK_PERIOD_COUNT       (SYST_TICK_PERIOD * MSPM0_CLOCK_FREQUENCY)

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
const uint8_t delay_count = 0x0C;

// Define a structure to hold different data types

int main(void)
{
    //Configure Launchpad Boards
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
 
    // PART 3
    run_lab7_part3();

    // Endless loop to prevent program from ending
    // while (1);

} /* main */

//-----------------------------------------------------------------------------
// PART 3: Display a 0->99 counter.
//-----------------------------------------------------------------------------
void run_lab7_part3(void)
{
    uint8_t timer_count = 0;
    bool done = false;
    
    while (!done)
    {      
        //Display Countdown
        lcd_set_ddram_addr(6);                  //Line 1 center
        lcd_write_byte(timer_count);            //Print timer count
        msec_delay(200);                        //Decrement every .2 seconds
        lcd_clear();

        //Decrement timer or loop timer when finished.
        if (timer_count < 99) 
        {
            timer_count++;
        } 
        else 
        {
            timer_count = 0;
        }
    }
    lcd_clear();
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   This function represents the ISR (Interrupt Service Routine) for
//   the SysTick timer. It is called at regular intervals based on the
//   configured SysTick period. This ISR is responsible for montiroing
//   the DIP switches, and counting the number of switches that are on.
//   The result is displayed on the seven-segment display.
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

    delay_time--;
    if (delay_time == 0)
    {
        // delay time has expired so now check & display switch count
        switch (dipsw_read())
        {
            case 0: 
                seg7_hex(0x0, SEG7_DIG0_ENABLE_IDX);
                break;
            case 1: 
                seg7_hex(0x1, SEG7_DIG0_ENABLE_IDX);
                break;
            case 2: 
                seg7_hex(0x1, SEG7_DIG0_ENABLE_IDX);
                break;
            case 3:     
                seg7_hex(0x2, SEG7_DIG0_ENABLE_IDX);
                break;
            case 4: 
                seg7_hex(0x1, SEG7_DIG0_ENABLE_IDX);
                break;
            case 5: 
                seg7_hex(0x1, SEG7_DIG0_ENABLE_IDX);
                break;
            case 6: 
                seg7_hex(0x2, SEG7_DIG0_ENABLE_IDX);
                break;  
            case 7: 
                seg7_hex(0x3, SEG7_DIG0_ENABLE_IDX);
                break;
            case 8: 
                seg7_hex(0x1, SEG7_DIG0_ENABLE_IDX);
                break;
            case 9: 
                seg7_hex(0x2, SEG7_DIG0_ENABLE_IDX);
                break;
            case 10: 
                seg7_hex(0x2, SEG7_DIG0_ENABLE_IDX);
                break;
            case 12: 
                seg7_hex(0x2, SEG7_DIG0_ENABLE_IDX);
                break;
            case 13: 
                seg7_hex(0x3, SEG7_DIG0_ENABLE_IDX);
                break;
            case 14: 
                seg7_hex(0x3, SEG7_DIG0_ENABLE_IDX);
                break;
            case 15: 
                seg7_hex(0x4, SEG7_DIG0_ENABLE_IDX);
                break;
            //Unexpected State Error
            default: 
                seg7_hex(0xE, SEG7_DIG0_ENABLE_IDX);
        }
        
        // get next delay time
        delay_time = delay_count;
    } /* if */
} /* SysTick_Handler */