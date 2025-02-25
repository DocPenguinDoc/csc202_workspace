//*****************************************************************************
//*****************************    Assembly Code    ***************************
//*****************************************************************************
//
//  DESIGNER NAME:  Connor Blum
//
//       LAB NAME:  Lab 3, Part 2
//
//      FILE NAME:  lab3p2.s
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    Assembly Bitwise Operations which are implemented in lab3p2_main.c
//
//*****************************************************************************
//*****************************************************************************

//Routine names made available outside of this file (globals)
.global my_asm_16bitset
.global my_asm_16bitclr
.global my_asm_16bitcheck

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   Set a specific bit in the register value
// INPUT PARAMETERS:
//   R0: register value
//   R1: bit mask
// RETURNS:
//   R0: modified register value
//-----------------------------------------------------------------------------

my_asm_16bitset:
    ORRS R0, R0, R1     //OR operation on register value
    BX LR               //'Branch and Exchange to Link Register' - (Returns to main.c)

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   Clear a specific bit in the register value
// INPUT PARAMETERS:
//   R0: register value
//   R1: bit mask
// RETURNS:
//   R0: modified register value
//-----------------------------------------------------------------------------

my_asm_16bitclr:
    MVNS R1, R1         //NOT operation on Bitmask
    ANDS R0, R0, R1     //AND operation on register value
    BX LR               //'Branch and Exchange to Link Register' - (Returns to main.c)

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   Check if a specific bit is set in the register value
// INPUT PARAMETERS:
//   R0: register value
//   R1: bit mask
// RETURNS:
//   R0: 1 if the bit is set, otherwise 0
//-----------------------------------------------------------------------------

my_asm_16bitcheck:
    ANDS R0, R0, R1     //AND operation on register value
    BEQ bit_not_set     //BEQ - "Branch if Equal" meaning *if* the AND above == 0, *else* it keeps going.
    MOVS R0, #1         //"Move" R0 value to 1
    BX LR               //'Branch and Exchange to Link Register' - (Returns to main.c)
bit_not_set:
    MOVS R0, #0         //"Move" R0 value to 0
    BX LR               //'Branch and Exchange to Link Register' - (Returns to main.c)

