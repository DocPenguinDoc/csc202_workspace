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
#include <stdbool.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "LaunchPad.h"
#include "lcd1602.h"
#include "spi.h"
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "uart.h"
#include "ti/devices/msp/peripherals/hw_oa.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab11_part1(void);                     //Part 1 implementation
void uart_print_string(const char *string);     //UART String Printer
void shutdown_message(void);                    //Option 4 implementation
uint16_t get_spi_data(void);                    //Set Data to send
uint16_t string_to_uint16(const char *input_string);
char* uint16_to_string(uint16_t value);


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define BAUD_RATE      115200                   //UART Baud Rate
#define DECIMAL_BASE   10

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
    seg7_init();
    I2C_init();
    lcd1602_init();
    UART_init(BAUD_RATE);
    lcd_clear();
    spi1_init();
    IOMUX->SECCFG.PINCM[LP_SPI_CS0_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      IOMUX_PINCM23_PF_GPIOB_DIO06);
    GPIOB->DOE31_0 |= LP_SPI_CS0_MASK;
    GPIOB->DOUT31_0 &= ~LP_SPI_CS0_MASK;


    //ADC0_init(ADC12_MEMCTL_VRSEL_INTREF_VSSA);
    
    // PART 2
    run_lab11_part1();

    //leds_off();
    //seg7_off();
 
 
 // Endless loop to prevent program from ending
 //while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function displays a user to the menu, serving as a test of TBD
//          Option 1. TBD
//          Option 2. TBD
//          Option 3. TBD
//          Option 4. TBD
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
void run_lab11_part1(void)
{
    char input;                     //User input character variable
    uint16_t data_input = 0;
    uint8_t data_output = 0;
    bool done = false;              //Flag to end character inputs

    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("Program running");
    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string("See serial port");

    while (!done)
    {
        //Print Menu
        uart_print_string(
            "\n\nMENU OPTIONS\n"
            "   1. Set Data to Send\n"
            "   2. Send Data\n"
            "   3. Update LEDs\n"
            "   4. End Program\n"
            "Enter your selection: ");

 
        //Read & Display user input from Serial Console
        input = UART_in_char();
        UART_out_char(input);
            
        //User input 1-4 -> Run respective option
        if (input == '1')
        {
            //Display confirmation message to Serial Console
            uart_print_string("\n\nSet SPI xmit data menu selected.");
            //Set SPI data
            data_input = get_spi_data();
            //Display confirmation message to Serial Console
            uart_print_string("\nDPI data set to ");
            uart_print_string(uint16_to_string(data_input));
        }
        else if (input == '2')
        {
            //Send & Receive data from SPI shift register
            spi1_write_data(data_input);
            data_output = spi1_read_data();

            //Display sent & received data on LCD.
            lcd_clear();
            lcd_set_ddram_addr(LCD_LINE1_ADDR);
            lcd_write_string("SENT = ");
            lcd_write_string(uint16_to_string(data_input));

            lcd_set_ddram_addr(LCD_LINE2_ADDR);
            lcd_write_string("RCVD = ");
            lcd_write_string(uint16_to_string(data_output));

            //Display confirmation message to Serial Console
            uart_print_string("\nSent data.");
        }
        else if (input == '3')
        {
            //text
            GPIOB->DOUT31_0 |= LP_SPI_CS0_MASK;
            msec_delay(5);
            GPIOB->DOUT31_0 &= ~LP_SPI_CS0_MASK;
            //Display confirmation message to Serial Console
            uart_print_string("\nLEDs Updated");
        }
        else if (input == '4')
        {
            //Display confirmation message to Serial Console
            uart_print_string("\nThank you for using the program.");
            //Display shutdown message on LCD screen & set done flag
            shutdown_message();
            done = true;
        }
        //User input anything but 1-4 -> Print Invalid Option to terminal
        else
        {
            uart_print_string("\nInvalid Option");
        }
    }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    TBD
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
uint16_t get_spi_data(void)
{
    char input_string[4]; // Assuming a 3-digit max input (0-255) + null terminator
    int i = 0;
    char input;
    bool done = false;

    uart_print_string("\nEnter data to send (0-255): ");
    
    while (i < 3 && !done) // Limit to 3 characters
    {
        input = UART_in_char();
        

        if (input == '\r')  // Stop when user presses Enter
        {
            done = true;
        }
        else if (input >= '0' && input <= '9')  // Only accept numeric characters
        {
            UART_out_char(input);
            input_string[i++] = input;
        }
        else
        {
            uart_print_string("\nInvalid character, please enter a digit (0-9).");
        }
    }

    input_string[i] = '\0'; // Null-terminate the string

    uint16_t data = string_to_uint16(input_string);

    if (data >= 0 && data <= 255)
    {
        return data;  // Valid data between 0 and 255
    }
    else
    {
        uart_print_string("\nERROR: Invalid data entered.");
        return 0;  // Invalid input, return 0
    }
}


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function displays the message "Program Stopped" to the LCD screen.
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
void shutdown_message(void)
{
    //Clear LCD, and display termination message
    lcd_clear();
    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string("Program Stopped");
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function writes a string to the UART. Each character of the string
//    is written to the console one by one. This function performs no error 
//    checking to ensure the string is displayed properly on the UART.
//
// INPUT PARAMETERS:
//    string    - A pointer (address) to the null-terminated string to be 
//                written to the DDRAM
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
//------------------------------------------------------------------------------
void uart_print_string(const char *string)
{
    //Print characters until a \0 is reached
    while (*string != '\0')
    {
        //Print each character, and increment through string
        UART_out_char(*string++);
    }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    TBD
//
// INPUT PARAMETERS:
//    TBD
//
// OUTPUT PARAMETERS:
//    TBD
//
// RETURN:
//    none
//------------------------------------------------------------------------------
uint16_t string_to_uint16(const char *input_string)
{
    uint32_t result = 0;

    while (*input_string)
    {
        if (*input_string < '0' || *input_string > '9')  // Check if it's a digit
        {
            return 0;  // Return 0 if an invalid character is found
        }

        result = result * DECIMAL_BASE + (*input_string - '0');
        input_string++;
    }

    return (uint16_t)result;  // Return the converted value
}


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    TBD
//
// INPUT PARAMETERS:
//    TBD
//
// OUTPUT PARAMETERS:
//    TBD
//
// RETURN:
//    none
//------------------------------------------------------------------------------
char* uint16_to_string(uint16_t value)
{
    static char output_string[6]; // 5 digits max + null terminator
    uint8_t digit_count = 0;

    if (value == 0)
    {
        output_string[digit_count++] = '0';
    }
    else
    {
        // Convert each digit to a string
        while (value > 0)
        {
            output_string[digit_count++] = '0' + (value % DECIMAL_BASE);
            value /= DECIMAL_BASE;
        }
    }

    output_string[digit_count] = '\0'; // Null-terminate the string

    // Reverse the string to put digits in correct order
    for (uint8_t i = 0; i < digit_count / 2; i++)
    {
        char temp = output_string[i];
        output_string[i] = output_string[digit_count - 1 - i];
        output_string[digit_count - 1 - i] = temp;
    }

    return output_string;
}

//From board


