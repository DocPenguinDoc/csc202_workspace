//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 9
//
//      FILE NAME:  lab9p3_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a tester for the servomotor, the position of which
//    is determined by an ADC value retrieved from the potentiometer, which is
//    converted to 10 bits, then converted to the necessary count value to
//    manipulate the servomotor.
//    Interrupts are used to end the program, and change the motor direction
//
//-----------------------------------------------------------------------------
//
// HARDWARE REQUIREMENTS:
//    - MSPM0G3507 Microcontroller
//    - CSC202 Expansion Board
//    - Pushbutton 1
//    - LCD1602 module
//    - Potentiometer
//    - L293D motor driver IC
//    - Servomotor
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
void run_lab9_part3(void);
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);
void GROUP1_IRQHandler(void);
void debounce(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define DEGREES                  0xDF    //Value for degree symbol on LCD
#define MAX_ADC_VALUE            1023    //Maximum 10-bit ADC value
#define MIN_SERVO_COUNT          100     //Minimum count for Servo
#define MAX_SERVO_COUNT          500     //Maximum count for Servo
#define POTENTIOMETER            7       //Potentiometer Channel (7)

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool pb1_pressed = false;
bool pb2_pressed = false;


// Define a structure to hold different data types

int main(void)
{
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

    // PART 3
    run_lab9_part3();

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
//    This function reads and displays the ADC value from the potentiometer,
//    converted to 10 bits (from 12), on the LCD screen.
//    This reading is converted to a count value which is used to manipulate
//    the servo, and is also displayed on the LCD screen.
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
void run_lab9_part3(void)
{
    uint16_t adc_value = 0;
    uint16_t servo_count = 0;

    lcd_write_string("Running Part 3");
    msec_delay(1000);
    lcd_clear();

    while (!pb1_pressed) {
        //Read potentiometer, Calculate Servo Count
        adc_value = ADC0_in(POTENTIOMETER);
        adc_value >>= 2;
        servo_count =   adc_value * (MAX_SERVO_COUNT - MIN_SERVO_COUNT) 
                        / MAX_ADC_VALUE + MIN_SERVO_COUNT;
        
        //Display ADC value
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string("ADC VAL = ");
        lcd_write_doublebyte(adc_value);

        //Display servo count
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string("SERVO # = ");
        lcd_write_doublebyte(servo_count);

        motor0_set_pwm_count(servo_count);

        //Wait 0.25 seconds
        msec_delay(250);
    }

    // Display "Program Stopped" when PB1 is pressed
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("Program Stopped");
} /* run_lab9_part3 */

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