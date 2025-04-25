//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 11
//
//      FILE NAME:  lab11p1_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program uses a serial communication with the microcontroller to 
//    create a synchronous serial interface. The Serial Peripheral Interface
//    sends an 8-bit value (0-255) to a shift register which is represented
//    on an LED bar. The user is presented with 4 options in the Serial Console
//    which they can select:
//          Option 1. Set Data to Send
//          Option 2. Send Data
//          Option 3. Update LEDs
//          Option 4. End Program
//
//-----------------------------------------------------------------------------
//
// HARDWARE REQUIREMENTS:
//    - MSPM0G3507 Microcontroller
//    - CSC202 Expansion Board
//    - LCD1602 module
//    - Serial Peripheral Interface
//    - 74HC595 shift register
//    - LED Bar (connected to shift register)
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
void run_lab11_part1(void);                         //Part 1 implementation
void uart_print_string(const char *string);         //UART String Printer
void shutdown_message(void);                        //Option 4 implementation
uint16_t get_spi_data(void);                        //Set and verify data
uint16_t string_to_uint16(const char *input_string);//Convert string to uint16
char* uint16_to_string(uint16_t value);             //Convert uint16 to string


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define BAUD_RATE           115200                  //UART Baud Rate
#define DECIMAL_BASE        10                      //Using base-10 for inputs
#define INPUT_STRING_SIZE   4                       //Size of the input_string
#define OUTPUT_STRING_SIZE  6                       //Size of the output_string
#define DATA_MIN            0                       //Minimum size of data
#define DATA_MAX            255                     //Maximum size of data

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

    //Modify initialization of SPI
    IOMUX->SECCFG.PINCM[LP_SPI_CS0_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      IOMUX_PINCM23_PF_GPIOB_DIO06);
    GPIOB->DOE31_0 |= LP_SPI_CS0_MASK;
    GPIOB->DOUT31_0 &= ~LP_SPI_CS0_MASK;

    // PART 1
    run_lab11_part1(); 
 
    // Endless loop to prevent program from ending
    //while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function displays a user to the menu, demonstrating the Serial 
//    Peripheral Interface, and shift register. The user is presented with
//    four options:
//          Option 1. Set Data to Send
//          Option 2. Send Data
//          Option 3. Update LEDs
//          Option 4. End Program
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
    uint16_t data_input = 0;        //Data input variable
    uint8_t data_output = 0;        //Data output variable
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
            //Display instructional message to Serial Console
            uart_print_string("\n\nSet SPI xmit data menu selected.");
            //Set SPI data
            data_input = get_spi_data();
            //Display confirmation message to Serial Console
            uart_print_string("\nSPI data set to: ");
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
            //Toggle chip select to trigger LED latch update
            GPIOB->DOUT31_0 |= LP_SPI_CS0_MASK;
            msec_delay(INPUT_STRING_SIZE);
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
//    This function reads and validates a user input number of 0-255.
//    They are presented an error message if they enter an invalid number or
//    characters.
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    uint16_t - Validated 0-255 value which will be 0 if invalid.
//
// RETURN:
//    uint16_t
//------------------------------------------------------------------------------
uint16_t get_spi_data(void)
{
    //input_string has room for 3 digits and a null terminator
    char input_string[INPUT_STRING_SIZE];                   
    int idx = 0;
    char input;
    bool done = false;

    //Display instruction to Serial Console
    uart_print_string("\nEnter data to send (0-255): ");
    
    //Stop reading after 3 characters, or ENTER is pushed
    while (idx < (INPUT_STRING_SIZE-1) && !done)
    {
        input = UART_in_char();
        
        //ENTER -> Stop taking inputs
        if (input == '\r')
        {
            done = true;
        }
        //Numerical digit -> place in input_string
        else if (input >= '0' && input <= '9')
        {
            UART_out_char(input);
            input_string[idx++] = input;
        }
        //Otherwise, it's an invalid character
        else
        {
            uart_print_string(
                "\nInvalid character, please enter a digit (0-9).");
        }
    }

    //Null-terminate the string, and convert it to a uint16_t
    input_string[idx] = '\0'; 
    uint16_t data = string_to_uint16(input_string);

    //Validate the data is within bounds (0-255), and return it.
    if (data >= DATA_MIN && data <= DATA_MAX)
    {
        return data;
    }
    //Invalid data returns 0
    else
    {
        uart_print_string("\nERROR: Invalid data entered.");
        return 0;
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
    //Print characters until a '\0' is reached
    while (*string != '\0')
    {
        //Print each character, and increment through string
        UART_out_char(*string++);
    }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function takes an string and converts it into a uint16_t value
//
// INPUT PARAMETERS:
//    string    - A pointer (address) to the null-terminated string to be 
//                written to the DDRAM.   
//
// OUTPUT PARAMETERS:
//	  uint16_t - An unsigned integer value which comes from a string
//
// RETURN:
//    uint16_t
//------------------------------------------------------------------------------
uint16_t string_to_uint16(const char *input_string)
{
    uint32_t result = 0;

    //Iterate through input_string
    while (*input_string)
    {
        //Specific char isn't a digit -> Return 0
        if (*input_string < '0' || *input_string > '9')
        {
            return 0;
        }

        //Push result up 1 decimal place value, and add to 1's place
        result = result * DECIMAL_BASE + (*input_string - '0');
        input_string++;
    }

    //Convert result to a uint16_t
    return (uint16_t)result;
}


//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function takes an uint16_t value and converts it into a
//    null-terminating string, which has 5 digits max.
//
// INPUT PARAMETERS:
//		uint16_t value - The provided value to be converted to a string
//
// OUTPUT PARAMETERS:
//    string    - A pointer (address) to the null-terminated string to be 
//                written to the DDRAM.
//
// RETURN:
//    string
//------------------------------------------------------------------------------
char* uint16_to_string(uint16_t value)
{
    //output_string has room for 5 digits and a null terminator
    static char output_string[OUTPUT_STRING_SIZE];
    uint8_t digit_count = 0;

    //value is 0 -> output_string is 0
    if (value == 0)
    {
        output_string[digit_count++] = '0';
    }
    //Otherwise, convert value to string digit by digit
    else
    {
        while (value > 0)
        {
            //Convert final digit to a char
            output_string[digit_count++] = '0' + (value % DECIMAL_BASE);
            //Remove final digit
            value /= DECIMAL_BASE;
        }
    }
    //Append a null terminator
    output_string[digit_count] = '\0';

    //Correct for string being in reverse order
    for (uint8_t i = 0; i < digit_count / 2; i++)
    {
        char temp = output_string[i];
        output_string[i] = output_string[digit_count - 1 - i];
        output_string[digit_count - 1 - i] = temp;
    }

    return output_string;
}


