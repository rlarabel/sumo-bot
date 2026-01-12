#include "assert_handler.h"
#include "defines.h"
#include "../drivers/uart.h"
#include "external/printf/printf.h"
#include <msp430.h>

/* The TI compiler provides supoort for calling an opcode.
 * Write __op_code(0x4343) to trigger a breakpoint with the TI toolchain.
 * This corresponds to the assembly instruction "CLR.B R3" that is
 * needed to trigger a breakpoint in the GCC toolchain.
 */
#define BREAKPOINT __asm volatile("CLR.B R3");

// Text + Program Counter + Null termination
#define ASSERT_STRING_MAX_SIZE (15u + 6u + 1u)

static void assert_trace(uint16_t program_counter)
{
    // UART Tx
    P1SEL |= BIT2;
    P1SEL2 |= BIT2;
    uart_init_assert();
    char assert_string[ASSERT_STRING_MAX_SIZE];
    snprintf(assert_string, sizeof(assert_string), "ASSERT 0x%x\n", program_counter);
    uart_trace_assert(assert_string);
}

static void assert_blink_led(void)
{
    // Configure TEST LED pin on LAUNCHPAD
    P1SEL &= ~(BIT0);
    P1SEL2 &= ~(BIT0);
    P1DIR |= BIT0;
    P1REN &= ~(BIT0);

    // Configure TEST LED pin on JR
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

void assert_handler(uint16_t program_counter)
{
    // TODO: Turn off motors
    BREAKPOINT
    assert_trace(program_counter);
    assert_blink_led();
}