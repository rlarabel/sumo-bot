#include "assert_handler.h"
#include "defines.h"
#include <msp430.h>

/* The TI compiler provides supoort for calling an opcode.
 * Write __op_code(0x4343) to trigger a breakpoint with the TI toolchain.
 * This corresponds to the assembly instruction "CLR.B R3" that is
 * needed to trigger a breakpoint in the GCC toolchain.
 */
#define BREAKPOINT __asm volatile("CLR.B R3");

void assert_handler(void)
{
    // TODO: Turn off motors
    // TODO: Trace console
    // Breakpoint
    BREAKPOINT

    // Configure TEST LED pin on LAUNCHPAD
    P1SEL &= ~(BIT0);
    P1SEL2 &= ~(BIT0);
    P1DIR |= BIT0;
    P1REN &= ~(BIT0);

    // COnfigure TEST LED pin on JR
    P2SEL &= ~(BIT6);
    P2SEL2 &= ~(BIT6);
    P2DIR |= (BIT6);
    P2REN &= ~(BIT6);

    while (1) {
        // Blink LED on both targets, in case wrong target was flashed
        P1OUT ^= BIT0;
        P2OUT ^= BIT6;
        BUSY_WAIT_ms(500);
    };
}