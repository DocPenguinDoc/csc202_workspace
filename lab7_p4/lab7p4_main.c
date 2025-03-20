//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  TBD
//
//       LAB NAME:  TBD
//
//      FILE NAME:  main.c
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
    clock_init_40mhz();
    launchpad_gpio_init();
    led_init();
    led_disable();
    seg7_init();
    dipsw_init();
    I2C_init();
    lcd1602_init();
    lcd_clear();
    
    config_pb2_interrupts();
    config_pb1_interrupts();

    // PART 4
    run_lab7_part4();

 // Endless loop to prevent program from ending
 // while (1);

} /* main */

//-----------------------------------------------------------------------------
// PART 4:
//-----------------------------------------------------------------------------
void run_lab7_part4(void)
{
    uint8_t timer_count = 0;
    bool done = false;
    bool bottom_text = false;
    
    while (!pb1_pressed) 
    {      
        //Display Countdown
        lcd_set_ddram_addr(6);                  //Line 1 center
        lcd_write_byte(timer_count);            //Print timer count
        msec_delay(200);                        //Decrement every .2 seconds
        lcd_set_ddram_addr(LCD_LINE2_ADDR);

        //Decrement timer or loop timer when finished.
        if (timer_count < 99) 
        {
            timer_count++;
        } 
        else 
        {
            timer_count = 0;
        }

        while (pb2_pressed) 
        {  
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            if (bottom_text)
            {
                lcd_write_string("PB2 PRESSED");
                bottom_text = !bottom_text;
            }
            else
            {
                lcd_clear();
                bottom_text = !bottom_text;
            }
            debounce();
            pb2_pressed = false;
        }
    }
    lcd_clear();
    lcd_write_string("Program Stopped");
}



//1
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

    //clear previously set flags within RIS (Raw Interrupt Status) register by writing to the ICLR (Interrupt Clear Register)
        //How to write to ICLR
    //Configure interrupt polarity for each port pin that will generate an interrupt by setting the appropriate bits in the POLARITYx (Polarity Select Register)
        //Use POLARITY31_16 and POLARITY15_0 registers for edge detection
    //Enable the interrupt for the desired port pins by setting the corresponding bits in the Interrupt Mask (IMASK) Register.
    //Enable the interrupt using the NVIC_EnableIRQ(xxx) function. xxx is either GPIOA_INT_IRQn or GPIOB_INT_IRQn depending on the GPIO module you are working with
        //ex:
        //NVIC_EnableIRQ(GPIOA_INT_IRQn)
        //NVIC_EnableIRQ(GPIOB_INT_IRQn)
    //NVIC_EnableIRQ(GPIOB_INT_IRQn);

    //Inside interrupt handler, read the IIDX for the grouping to determine source (GPIOA or GPIOB). Then read MIS (Masked Interrupt Status) register from the corresponding GPIO and check each bit you are interested in to see if it is set.
        //Read CPUSS IIDX ex:
        // group_gpio_iidx = CPUSS->INT_GROUP[1].IIDX;
        //Check set bit ex:
        //CPUSS_INT_GROUP_IIDX_STAT_INT0
        //CPUSS_INT_GROUP_IIDX_STAT_INT1
    //If bit is set, it indicates that the button was pressed, and you should set a global flag indicating the button was pressed.
    //Cannot use is_pb_down etc. in the ISR
}

//2
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