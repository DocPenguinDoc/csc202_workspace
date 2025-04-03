//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 8
//
//      FILE NAME:  lab8p3_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a tester for the thermistor, utilizing the
//    analog-to-digital converter (ADC). When PB2 is pushed, an interrupt
//    reads the current temperature, and then it is displayed on line 2 of
//    the LCD.
//    This program also serves as a tester for the potentiometer, utilizing the
//    analog-to-digital converter (ADC). The ADC reading is displayed on 
//    the LCD, while the LED bar lights up representing the percentage the
//    potentiometer has been rotated.
//    The program can be ended with PB1, which is using an interrupt.
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
void run_lab8_part3(void);
void config_pb1_interrupts(void);
void config_pb2_interrupts(void);
void GROUP1_IRQHandler(void);
void debounce(void);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define POTENTIOMETER            7       //Potentiometer Channel (7)
#define ADC_LED_DIVISOR          455     //ADC value range per LED (4096 / 9)
#define LCD_ADDR_OFFSET          6       //Offset for LCD display of ADC value
#define DEGREES                  0xDF    //Value for degree symbol on LCD

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool pb1_pressed = false;
bool pb2_pressed = false;
int therm_adc_value;


// Define a structure to hold different data types

int main(void)
{
    //Configure Launchpad Boards
    clock_init_40mhz();
    launchpad_gpio_init();
    led_init();
    led_enable();
    dipsw_init();
    ADC0_init(ADC12_MEMCTL_VRSEL_INTREF_VSSA);
    I2C_init();
    lcd1602_init();
    lcd_clear();

    //Configure PB interrupts
    config_pb2_interrupts();
    config_pb1_interrupts();

    // PART 3
    run_lab8_part3();
 
    // Endless loop to prevent program from ending
    //while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function reads and displays the ADC value from the potentiometer.
//    On the LCD screen, the ADC values displated.
//    On the LED bar, the amount of lights lit correstponds to the percentage
//    the potentiometer is turned.
//    The interrupt PB1, will end the program.
//    The interrupt PB2 will display the value of read using the ADC of the
//    thermistor, and calculate & display the temperature in Fahrenheit
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
void run_lab8_part3(void)
{
    int adc_value;
    // Loop until PB1 is pressed
    while (!pb1_pressed)
    {
        // Should implement interrupt disable?
        adc_value = ADC0_in(POTENTIOMETER);
        // Should implement interrupt re-enable?

        // Display ADC value on first line of LCD
        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string("ADC = ");
        lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_ADDR_OFFSET);
        lcd_write_doublebyte(adc_value);

        uint8_t led_count = adc_value / ADC_LED_DIVISOR;
        // Loop through the LEDs and turn on appropriate amount
        for (uint8_t i = 0; i <= led_count; i++)
        {
            led_on(i);  // Turn on LED i
        }
        // Turn off remaining LEDs
        for (uint8_t i = led_count; i < MAX_NUM_LEDS; i++)
        {
            led_off(i);  // Turn off LED i
        }

        while (pb2_pressed) 
        {  
            //therm_adc_value = ADC0_in(5);      //Moved to Interrupt

            // Calculate and display thermistor reading in Fahrenheit
            float temp_Celsius = thermistor_calc_temperature(therm_adc_value);
            float temp_Fahrenheit = (temp_Celsius * 9 / 5) + 32;
            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            lcd_write_string("Temp = ");
            lcd_set_ddram_addr(LCD_LINE2_ADDR + LCD_ADDR_OFFSET);
            lcd_write_doublebyte(temp_Fahrenheit);
            lcd_write_char(DEGREES);               
            lcd_write_string("F");
            debounce();

            // Reset PB2 flag
            pb2_pressed = false;
        }

        // Prevent flashing
        msec_delay(200);
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
                    therm_adc_value = ADC0_in(5);
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