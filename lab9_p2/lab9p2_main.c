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
void run_lab9_part2(void);
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);
void GROUP1_IRQHandler(void);
void debounce(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define DEGREES                  0xDF    //Value for degree symbol on LCD

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool pb1_pressed = false;
bool pb2_pressed = false;


// Define a structure to hold different data types

int main(void)
{
    uint16_t adc_pot_value = 0;
    uint8_t switch_value = 0;
    uint8_t duty_cycle = 0;

    //Configure Launchpad Boards
    clock_init_40mhz();
    launchpad_gpio_init();
    dipsw_init();
    led_init();
    led_enable();
    I2C_init();
    lcd1602_init();
    lcd_clear();
    keypad_init();

    ADC0_init(ADC12_MEMCTL_VRSEL_INTREF_VSSA);

    motor0_init();
    motor0_pwm_init(4000,0);
    motor0_pwm_enable();

    //Configure PB interrupts
    config_pb2_interrupts();
    config_pb1_interrupts();


    run_lab9_part2();
 
 // Endless loop to prevent program from ending
 //while (1);

} /* main */

//-----------------------------------------------------------------------------
// PART 2: 
//-----------------------------------------------------------------------------
void run_lab9_part2(void)
{
    uint8_t speed;
    int therm_adc_value;
    
    lcd_write_string("Running Part 2");
    msec_delay(1000);
    lcd_clear();

    while (!pb1_pressed) {
        therm_adc_value = ADC0_in(5);
        // Calculate and display thermistor reading in Fahrenheit
        float temp_Celsius = thermistor_calc_temperature(therm_adc_value);
        float temp_Fahrenheit = (temp_Celsius * 9 / 5) + 32;
        lcd_set_ddram_addr(LCD_LINE1_ADDR + 1);
        lcd_write_string("TEMP = ");
        lcd_write_byte(temp_Fahrenheit);
        lcd_write_char(DEGREES);               
        lcd_write_string("F");

        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string("SPEED = ");
        if (temp_Fahrenheit > 80)
        {
            speed = 80;
        }
        else 
        {   
            speed = 25;
        }
        motor0_set_pwm_dc(speed);
        lcd_write_byte(speed);
        lcd_write_string("%");        
        
        msec_delay(250);
    }

    //Display "Program Stopped" when PB1 is pressed
    lcd_clear();
    lcd_write_string("Program Stopped");
}

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