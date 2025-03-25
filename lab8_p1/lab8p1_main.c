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
#include "adc.h"
#include "ti/devices/msp/peripherals/hw_oa.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab8_part1(void);
void OPA0_init(void);
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);
void GROUP1_IRQHandler(void);
void debounce(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool pb1_pressed = false;
bool pb2_pressed = false;
const uint16_t LIGHT_THRESHOLD = 2048;


// Define a structure to hold different data types

int main(void)
{
    //Configure Launchpad Boards
    clock_init_40mhz();
    launchpad_gpio_init();
    dipsw_init();
    OPA0_init();
    ADC0_init(ADC12_MEMCTL_VRSEL_INTREF_VSSA);
    I2C_init();
    lcd1602_init();
    lcd_clear();

    //Configure PB interrupts
    config_pb2_interrupts();
    config_pb1_interrupts();

    /*
    //temp
    while (1) {
        lcd_write_quadbyte(ADC0_in(7));
        msec_delay(100);
        lcd_clear();
    }
    */

    // PART 1
    run_lab8_part1();
 
    // Endless loop to prevent program from ending
    //while (1);

} /* main */

//-----------------------------------------------------------------------------
// Part 1: Display ADC value and toggle status based on light sensor
//-----------------------------------------------------------------------------
void run_lab8_part1(void)
{
    int adc_value;
    // Loop until PB1 is pressed (pb1_pressed flag is set)
    while (!pb1_pressed)
    {
        adc_value = 2000;    //read channel 7 - light sensor
        
        // Display "Status: Dark" or "Status: Light" based on threshold
        lcd_set_ddram_addr(LCD_LINE1_ADDR); // First line of LCD
        if (adc_value < LIGHT_THRESHOLD)
        {
            lcd_write_string("Status: Dark ");
        }
        else
        {
            lcd_write_string("Status: Light");
        }

        // Display ADC value on second line of LCD
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string("ADC = ");
        lcd_set_ddram_addr(LCD_LINE2_ADDR + 6);
        lcd_write_doublebyte(adc_value);
        
        // Prevent flashing
        msec_delay(200);
    }
    
    //Display "Program Stopped" when PB1 is pressed
    lcd_clear();
    lcd_write_string("Program Stopped");
}

//-----------------------------------------------------------------------------
// OPA0_init: Initialize OPA0
//-----------------------------------------------------------------------------
void OPA0_init(void)
{
    OPA0->GPRCM.RSTCTL = (OA_RSTCTL_KEY_UNLOCK_W | OA_RSTCTL_RESETSTKYCLR_CLR | OA_RSTCTL_RESETASSERT_ASSERT);

    OPA0->GPRCM.PWREN = (OA_PWREN_KEY_UNLOCK_W | OA_PWREN_ENABLE_ENABLE);

    //TIME FOR opa TO POWER UP
    clock_delay(24);

    OPA0->CFGBASE &= ~(OA_CFGBASE_RRI_MASK);

    OPA0->CFGBASE |= ((uint32_t) OA_CFGBASE_GBW_HIGHGAIN);

    OPA0->CFG |= (OA_CFG_GAIN_MINIMUM | OA_CFG_MSEL_NC | OA_CFG_NSEL_EXTPIN0 | OA_CFG_PSEL_EXTPIN0 | OA_CFG_OUTPIN_ENABLED | OA_CFG_CHOP_OFF);

    OPA0->CTL |= OA_CTL_ENABLE_ON;
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