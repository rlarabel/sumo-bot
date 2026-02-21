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

// Configure pin to a low gpio output
#define GPIO_OUTPUT_LOW(port, bit)                                                                 \
    do {                                                                                           \
        P##port##SEL &= ~(BIT##bit);                                                               \
        P##port##SEL2 &= ~(BIT##bit);                                                              \
        P##port##DIR |= BIT##bit;                                                                  \
        P##port##REN &= ~(BIT##bit);                                                               \
        P##port##OUT &= ~(BIT##bit);                                                               \
    } while (0)

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
    GPIO_OUTPUT_LOW(1, 0); // TEST LED pin on LAUNCHPAD
    GPIO_OUTPUT_LOW(2, 6); // TEST LED pin on JR

    while (1) {
        // Blink LED on both targets, in case wrong target was flashed
        P1OUT ^= BIT0;
        P2OUT ^= BIT6;
        BUSY_WAIT_ms(500);
    };
}

static void assert_stop_motors(void)
{
    GPIO_OUTPUT_LOW(1, 6); // Left PWM (Launchpad)
    GPIO_OUTPUT_LOW(2, 1); // Right CC1 (JR)
    GPIO_OUTPUT_LOW(2, 2); // Right CC2 (JR)
    GPIO_OUTPUT_LOW(2, 4); // Left CC2 (JR & Launchpad)
    GPIO_OUTPUT_LOW(2, 5); // Left CC1 (JR & Launchpad)
    GPIO_OUTPUT_LOW(3, 5); // Left PWM (JR)
    GPIO_OUTPUT_LOW(3, 6); // Right PWM (JR)
}

void assert_handler(uint16_t program_counter)
{
    assert_stop_motors();
    BREAKPOINT
    assert_trace(program_counter);
    assert_blink_led();
}