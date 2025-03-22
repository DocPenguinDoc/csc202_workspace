//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 7
//
//      FILE NAME:  lab7p4_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a demonstration of configuring GPIO interrupts.
//    The main focus is a 0 to 99 counter on the LCD screen, with PB interrupts
//      PB1 interrupt: Terminates program/countdown
//      PB2 interrupt: Toggles message "PB2 PRESSED" on Line 2 of LCD
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
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/devices/msp/peripherals/hw_gpio.h"
#include "ti/devices/msp/peripherals/m0p/hw_cpuss.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab7_part4(void);
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);
void GROUP1_IRQHandler(void);
void debounce(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
bool pb1_pressed = false;
bool pb2_pressed = false;

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------


// Define a structure to hold different data types

int main(void)
{
    //Configure Launchpad Boards
    clock_init_40mhz();
    launchpad_gpio_init();
    led_init();
    led_disable();
    seg7_init();
    dipsw_init();
    I2C_init();
    lcd1602_init();
    lcd_clear();
    
    //Configure PB interrupts
    config_pb2_interrupts();
    config_pb1_interrupts();

    // PART 4
    run_lab7_part4();

    // Endless loop to prevent program from ending
    // while (1);

} /* main */

//-----------------------------------------------------------------------------
// PART 4: Display countdown while using GPIO interrupts.
//          PB1 terminates countdown. PB2 toggles message on line 2
//-----------------------------------------------------------------------------
void run_lab7_part4(void)
{
    uint8_t timer_count = 0;
    bool done = false;
    bool bottom_text = false;
    
    //Loop countdown until PB1 is pressed
    while (!pb1_pressed) 
    {      
        //Display Countdown
        lcd_set_ddram_addr(6);                  //Line 1 center
        lcd_write_byte(timer_count);            //Print timer count
        msec_delay(200);                        //Decrement every .2 seconds
        lcd_set_ddram_addr(LCD_LINE2_ADDR);

        //Increment timer or loop timer when finished.
        if (timer_count < 99) 
        {
            timer_count++;
        } 
        else 
        {
            timer_count = 0;
        }

        //When PB2 is pressed, toggle a message on the second line of LCD
        while (pb2_pressed) 
        {  
            lcd_set_ddram_addr(LCD_LINE2_ADDR);

            //Uses flag to toggle the message based on PB2 presses
            if (bottom_text)
            {
                lcd_write_string("PB2 PRESSED");
            }
            else
            {
                lcd_clear();
            }
            bottom_text = !bottom_text;
            debounce();

            //Reset PB2 flag
            pb2_pressed = false;
        }
    }

    //When PB1 is pressed, display message conveying end of program.
    lcd_clear();
    lcd_write_string("Program Stopped");
}



//-----------------------------------------------------------------------------
// Configures PB1 interrupt (GPIOB, DOI18)
//-----------------------------------------------------------------------------
void config_pb1_interrupts(void)
{
    //Set SW1 to rising edge (after inversion)
    GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;

    //Ensure bit is cleared
    GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;

    //Unmask SW1 to allow interrupt
    GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;

    NVIC_SetPriority(GPIOB_INT_IRQn, 2);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
}

//-----------------------------------------------------------------------------
// Configures PB2 interrupt (GPIOA, DOI15)
//-----------------------------------------------------------------------------
void config_pb2_interrupts(void)
{
    //Set SW1 to rising edge (after inversion)
    GPIOA->POLARITY15_0 = GPIO_POLARITY15_0_DIO15_RISE;

    //Ensure bit is cleared
    GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;

    //Unmask SW1 to allow interrupt
    GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;

    NVIC_SetPriority(GPIOA_INT_IRQn, 2);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

//-----------------------------------------------------------------------------
// GROUP1 Interrupt Handler (handles interrupts from GPIOA and GPIOB)
//-----------------------------------------------------------------------------
void GROUP1_IRQHandler(void)
{
    uint32_t group_iidx_status;
    uint32_t gpio_mis;
    debounce();
    
    do
    {
        group_iidx_status = CPUSS->INT_GROUP[1].IIDX;

        switch (group_iidx_status)
        {
            //Interrupt from GPIOA
            case (CPUSS_INT_GROUP_IIDX_STAT_INT0):
                gpio_mis = GPIOA->CPU_INT.MIS;
                if ((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) == GPIO_CPU_INT_MIS_DIO15_SET)
                {
                    pb2_pressed = true;
                    //Manually clear bit to acknowledge interrupt
                    GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
                }
                break;
            //Interrupt from GPIOB
            case (CPUSS_INT_GROUP_IIDX_STAT_INT1):
                gpio_mis = GPIOB->CPU_INT.MIS;
                if ((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) == GPIO_CPU_INT_MIS_DIO18_SET)
                {
                    pb1_pressed = true;
                    //Manually clear bit to acknowledge interrupt
                    GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
                }
                break;
            //Unexpected Interrupt
            default:
                break;
        }
    } while (group_iidx_status != 0);
}

//-----------------------------------------------------------------------------
// Debounce: 10ms delay to prevent key bouncing
//-----------------------------------------------------------------------------
void debounce() {
    msec_delay(10);
}