//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 9
//
//      FILE NAME:  lab9p2_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a simulation of a computer fan, testing the DC
//    motor. This is accomplished by reading an ADC value from the thermistor,
//    and using a defined threshold to determine the "fan speed".
//    An interrupt is used to end the program.
//
//-----------------------------------------------------------------------------
//
// HARDWARE REQUIREMENTS:
//    - MSPM0G3507 Microcontroller
//    - CSC202 Expansion Board
//    - Pushbuttons 1 & 2
//    - LCD1602 module
//    - L293D motor driver IC
//    - DC Motor
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
#define THERMISTOR               5       //Light Sensor Channel (7)
#define TEMP_THRESHOLD           80      //Temperature threshold

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
    
    ADC0_init(ADC12_MEMCTL_VRSEL_INTREF_VSSA);

    //Configure motor
    led_on(LED_BAR_LD1_IDX);
    led_off(LED_BAR_LD2_IDX);
    motor0_init();
    motor0_pwm_init(4000,0);
    motor0_pwm_enable();
    

    //Configure PB interrupts
    config_pb2_interrupts();
    config_pb1_interrupts();

    // PART 2
    run_lab9_part2();

    //Disable peripherals
    leds_off();
    motor0_pwm_disable();
    NVIC_DisableIRQ(GPIOB_INT_IRQn);
    NVIC_DisableIRQ(GPIOA_INT_IRQn);

    // Endless loop to prevent program from ending
    //while (1);
} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads an ADC value from the thermistor, and converts it to
//    Fahrenheit, before displaying it on the LCD.
//    Depending on whether the current temperature is above/below a certain
//    threshold, the "fan speed" is changed
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
void run_lab9_part2(void)
{
    uint8_t speed;
    int therm_adc_value;
    
    //Part 2 Begins
    lcd_write_string("Running Part 2");
    msec_delay(1000);
    lcd_clear();

    while (!pb1_pressed) {
        //Read thermistor, and convert reading to Fahrenheit
        therm_adc_value = ADC0_in(THERMISTOR);
        float temp_Celsius = thermistor_calc_temperature(therm_adc_value);
        float temp_Fahrenheit = (temp_Celsius * 9 / 5) + 32;

        //Display temperature on LCD
        lcd_set_ddram_addr(LCD_LINE1_ADDR + 1);
        lcd_write_string("TEMP = ");
        lcd_write_byte(temp_Fahrenheit);
        lcd_write_char(DEGREES);               
        lcd_write_string("F");

        //Determine, set,5 and display "fan speed"
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string("SPEED = ");
        if (temp_Fahrenheit > TEMP_THRESHOLD)
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
        
        //.25 second loop iteration delay
        msec_delay(250);
    }

    //Display "Program Stopped" when PB1 is pressed
    lcd_clear();
    lcd_write_string("Program Stopped");
} /* run_lab9_part2 */

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
} /* config_pb1_interrupts */

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
} /* config_pb2_interrupts */

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
} /* GROUP1_IRQHandler */

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
} /* debounce */