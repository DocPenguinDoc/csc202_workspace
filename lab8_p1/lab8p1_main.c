//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 8
//
//      FILE NAME:  lab8p1_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a tester for the light sensor, utilizing the
//    analog-to-digital converter (ADC). The ADC reading is displayed on 
//    the LCD, while also displaying a "Status" messaging establishing if
//    it's reading "light" or "dark"
//    The program can be ended with PB1, which is using an interrupt.
//
//-----------------------------------------------------------------------------
//
// HARDWARE REQUIREMENTS:
//    - MSPM0G3507 microcontroller
//    - LCD 1602
//    - Pushbutton 1
//    - Pushbutton 2
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
void run_lab8_part1(void);              //Part 1 implementation
void OPA0_init(void);                   //Operational Amplifier initiliazation
void config_pb1_interrupts(void);       //Pushbutton 1 interrupt configuration
void config_pb2_interrupts(void);       //Pushbutton 2 interrupt configuration
void GROUP1_IRQHandler(void);           //Interrupt handler
void debounce(void);                    //Small 10ms delay

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define LIGHT_SENSOR 7                  //Light Sensor Channel (7)
#define LIGHT_THRESHOLD = 500;          //Light vs Dark threshold

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool pb1_pressed = false;               //Pushbutton 1 interrupt flag
bool pb2_pressed = false;               //Pushbutton 2 interrupt flag


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

    // PART 1
    run_lab8_part1();
 
    // Endless loop to prevent program from ending
    //while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads and displays the ADC value from the light sensor.
//    On the LCD screen, depending on the value read, user is informed if it
//    is considered light or dark.
//    The interrupt PB1, will end the program.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
//------------------------------------------------------------------------------
void run_lab8_part1(void)
{
    int adc_value;                              //Light sensor value from adc
    
    // Loop until PB1 is pressed (pb1_pressed flag is set)
    while (!pb1_pressed)
    {
        // Read and assign light sensor data (Channel 7)
        adc_value = ADC0_in(LIGHT_SENSOR);
        
        // Display "Status: Dark" or "Status: Light" based on threshold
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
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
        
        // Delay to prevent Prevent flashing
        msec_delay(200);
    }
    
    // Display "Program Stopped" when PB1 is pressed
    lcd_clear();
    lcd_write_string("Program Stopped");
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the operational amplifier (OPA0) on the 
//    MSPM0G3507 microcontroller. The configuration involves resetting the 
//    module, enabling power, setting the gain bandwidth, and configuring 
//    input and output pins. The OPA is then powered up and enabled for use
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
//-----------------------------------------------------------------------------
void OPA0_init(void)
{
    OPA0->GPRCM.RSTCTL = (  OA_RSTCTL_KEY_UNLOCK_W | OA_RSTCTL_RESETSTKYCLR_CLR
                            | OA_RSTCTL_RESETASSERT_ASSERT);

    OPA0->GPRCM.PWREN = (OA_PWREN_KEY_UNLOCK_W | OA_PWREN_ENABLE_ENABLE);

    //TIME FOR opa TO POWER UP
    clock_delay(24);

    OPA0->CFGBASE &= ~(OA_CFGBASE_RRI_MASK);

    OPA0->CFGBASE |= ((uint32_t) OA_CFGBASE_GBW_HIGHGAIN);

    OPA0->CFG |= (  OA_CFG_GAIN_MINIMUM | OA_CFG_MSEL_NC | OA_CFG_NSEL_EXTPIN0
                    | OA_CFG_PSEL_EXTPIN0 | OA_CFG_OUTPIN_ENABLED
                    | OA_CFG_CHOP_OFF);

    //Enable the OPA
    OPA0->CTL |= OA_CTL_ENABLE_ON;
} /* OPA0_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function configures the PB1 interrupt (GPIOB, DOI18)
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
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
// DESCRIPTION:
//    This function configures the PB2 interrupt (GPIOA, DOI15)
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
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
// DESCRIPTION:
//    This function handles interrupts from GPIOA and GPIOB
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
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
                if ((   gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) == 
                        GPIO_CPU_INT_MIS_DIO15_SET)
                {
                    pb2_pressed = true;
                    //Manually clear bit to acknowledge interrupt
                    GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
                }
                break;
            //Interrupt from GPIOB
            case (CPUSS_INT_GROUP_IIDX_STAT_INT1):
                gpio_mis = GPIOB->CPU_INT.MIS;
                if ((   gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) == 
                        GPIO_CPU_INT_MIS_DIO18_SET)
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
// DESCRIPTION:
//    This function is a small 10ms delayed with the intention of preventing
//    misread button presses as a result of bounding.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void debounce() {
    msec_delay(10);
}