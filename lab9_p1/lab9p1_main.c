//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 9
//
//      FILE NAME:  lab9p1_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a tester for the DC motor, using the keypad to
//    create a 0-100% speed based on the available 0-15 input.
//    The LCD screen is used to display the motor speed.
//    Interrupts are used to end the program, and change the motor direction
//
//-----------------------------------------------------------------------------
//
// HARDWARE REQUIREMENTS:
//    - MSPM0G3507 Microcontroller
//    - CSC202 Expansion Board
//    - Pushbuttons 1 & 2
//    - Keypad
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
void run_lab9_part1(void);
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);
void GROUP1_IRQHandler(void);
void debounce(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
typedef enum {                          //Defined motor states
    MOTOR_OFF1,                         //Motor Off
    MOTOR_CW,                           //Motor Clockwise
    MOTOR_OFF2,                         //Motor Off
    MOTOR_CCW                           //Motor Counterclockwise
} motor_state_t;

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
    keypad_init();

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

    // PART 1
    run_lab9_part1();

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
//    This function reads a value from the keypad (0-15) and converts it to a
//    percentage (0-100), and displays it to the screen.
//    This percentage represents the motor speed, which is based on the keypad
//    input.
//    The interrupt PB1, will change the state of the motor, looping through
//    the following states: Off, Clockwise, Off, Counterclockwise
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
void run_lab9_part1(void)
{
    uint8_t key;
    uint8_t duty_cycle;

    //Set initial motor state
    static motor_state_t motor_state = MOTOR_OFF1;
    led_off(LED_BAR_LD1_IDX);
    led_off(LED_BAR_LD2_IDX);

    //Part 1 Begins
    lcd_write_string("Running Part 1");
    msec_delay(1000);
    lcd_clear();

    while (!pb1_pressed) {
        //Display Heading
        lcd_set_ddram_addr(LCD_LINE1_ADDR + 2);
        lcd_write_string("MOTOR SPEED");

        //Calculate & Display Duty Cycle
        key = keypad_scan();
        if (key != 0x10) {         
            duty_cycle = key * 100 / 15;
            motor0_set_pwm_dc(duty_cycle);
            lcd_set_ddram_addr(LCD_LINE2_ADDR + 5);
            lcd_write_byte(duty_cycle);
            lcd_write_string("%");
            wait_no_key_pressed();
        }

        //Advance motor state when interrupt PB2 is pressed
        while (pb2_pressed) 
        {  
            switch (motor_state) {
                //Transition to clockwise state
                case MOTOR_OFF1:
                    led_on(LED_BAR_LD1_IDX);
                    led_off(LED_BAR_LD2_IDX);
                    motor_state = MOTOR_CW;     
                    break;

                //Transition to off state
                case MOTOR_CW:
                    led_off(LED_BAR_LD1_IDX);
                    led_off(LED_BAR_LD2_IDX);
                    motor_state = MOTOR_OFF2;   
                    break;

                //Transition to counterclockwise state
                case MOTOR_OFF2:
                    led_off(LED_BAR_LD1_IDX);
                    led_on(LED_BAR_LD2_IDX);
                    motor_state = MOTOR_CCW;    
                    break;

                //Transition back to off state
                case MOTOR_CCW:
                    led_off(LED_BAR_LD1_IDX);
                    led_off(LED_BAR_LD2_IDX);
                    motor_state = MOTOR_OFF1;   
                    break;

                default:
                    break;
            }
            // Delay to ensure smooth operation
            msec_delay(100);
            // Reset PB2 flag
            pb2_pressed = false;
        }
        
        
    }

    //Display "Program Stopped" when PB1 is pressed
    lcd_clear();
    lcd_write_string("Program Stopped");
} /* run_lab9_part1 */

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