#include "ir_remote.h"
#include "io.h"
#include "led.h"
#include "../common/ring_buffer.h"
#include "../common/defines.h"
#include <msp430.h>
#include <assert.h>
#include <stdint.h>

#define TICKS_PER_ms (SMCLK / TIMER_INPUT_DIVIDER_3 / 1000u)
#define TIMER_INTERRUPT_ms (1u)
#define TIMER_INTERRUPT_TICKS (TICKS_PER_ms * TIMER_INTERRUPT_ms)
static_assert(TIMER_INTERRUPT_TICKS <= 0xFFFF, "Ticks too large");
#define TIMER_TIMEOUT_ms (150u)

#define IR_CMD_BUFFER_ELEM_CNT (10u)
static uint8_t buffer[IR_CMD_BUFFER_ELEM_CNT];
static struct ring_buffer ir_cmd_buffer = { .buffer = buffer, .size = IR_CMD_BUFFER_ELEM_CNT };
static union {
    struct
    {
        // cppcheck-suppress unusedStructMember
        uint8_t cmd_inverted;
        uint8_t cmd;
        // cppcheck-suppress unusedStructMember
        uint8_t addr_inverted;
        // cppcheck-suppress unusedStructMember
        uint8_t addr;
    } decoded;
    uint32_t raw;
} ir_message;
static uint8_t timer_ms = 0;
static uint16_t pulse_count = 0;

static void timer_init(void)
{
    // TASSEL_2: set SMCLK as clk source for the timer
    // ID_3: Input Divider 8
    TA1CTL = TASSEL_2 + ID_3;
    // Trigger Interrupt at a set interval
    TA1CCR0 = TIMER_INTERRUPT_TICKS;
    // Enable intterupt
    TA1CCTL0 = CCIE;
}

static void timer_start(void)
{
    // Restart the timer

    // Clear default MC configuration and add MC_1
    // MC_1 counts to CCR0
    // Clear any value in the current counter
    TA1CTL = (TA1CTL & ~TIMER_MC_MASK) | MC_1 | TACLR;
    timer_ms = 0;
}

static void timer_stop(void)
{
    // MC_0: Stops the timer
    TA1CTL = (TA1CTL & ~TIMER_MC_MASK) | MC_0;
}

static inline bool is_valid_pulse(uint16_t pulse, uint8_t ms)
{
    if (pulse == 1) {
        return ms == 0;
    } else if (pulse == 2) {
        return ms < 16;
    } else if (3 <= pulse && pulse <= 34) {
        return ms < 4;
    } else if (pulse == 35) {
        return ms < 50;
    } else if (pulse == 36) {
        return ms < 16;
    } else if (pulse >= 37 && IS_ODD(pulse)) {
        return ms < 110;
    } else if (pulse >= 37) {
        return ms < 16;
    } else {
        return false;
    }
}

static inline bool is_bit_pulse(uint16_t pulse)
{
    return 3 <= pulse && pulse <= 34;
}

static inline bool is_message_pulse(uint16_t pulse)
{
    return pulse == 34 || (pulse > 36 && IS_ODD(pulse));
}

static void isr_pulse(void)
{
    timer_stop();
    pulse_count++;
    // When you fat finger 2 buttons at once, it will send random messages to buffer
    if (!is_valid_pulse(pulse_count, timer_ms)) {
        pulse_count = 1;
        ir_message.raw = 0;
    } else if (is_bit_pulse(pulse_count)) {
        ir_message.raw <<= 1;
        ir_message.raw += (timer_ms >= 2) ? 1 : 0;
    }

    if (is_message_pulse(pulse_count)) {
        ring_buffer_put(&ir_cmd_buffer, ir_message.decoded.cmd);
    }

    timer_start();
}

INTERRUPT_FUNCTION(TIMER1_A0_VECTOR) isr_timer_a0(void)
{
    led_toggle(LED_TEST);
    if (timer_ms < TIMER_TIMEOUT_ms) {
        timer_ms++;

    } else {
        timer_stop();
        pulse_count = 0;
        timer_ms = 0;
        ir_message.raw = 0;
    }
}

ir_cmd_e ir_remote_get_cmd(void)
{
    io_disable_interrupt(IO_IR_REMOTE);
    ir_cmd_e cmd = IR_CMD_NONE;
    if (!ring_buffer_empty(&ir_cmd_buffer)) {
        cmd = ring_buffer_get(&ir_cmd_buffer);
    }
    io_enable_interrupt(IO_IR_REMOTE);
    return cmd;
}

void ir_remote_init(void)
{
    io_configure_interrupt(IO_IR_REMOTE, IO_TRIGGER_FALLING, isr_pulse);
    io_enable_interrupt(IO_IR_REMOTE);
    timer_init();
}

const char *ir_remote_cmd_to_string(ir_cmd_e cmd)
{
    switch (cmd) {
    case IR_CMD_0:
        return "0";
    case IR_CMD_1:
        return "1";
    case IR_CMD_2:
        return "2";
    case IR_CMD_3:
        return "3";
    case IR_CMD_4:
        return "4";
    case IR_CMD_5:
        return "5";
    case IR_CMD_6:
        return "6";
    case IR_CMD_7:
        return "7";
    case IR_CMD_8:
        return "8";
    case IR_CMD_9:
        return "9";
    case IR_CMD_LEFT:
        return "LEFT";
    case IR_CMD_RIGHT:
        return "RIGHT";
    case IR_CMD_UP:
        return "UP";
    case IR_CMD_DOWN:
        return "DOWN";
    case IR_CMD_OK:
        return "OK";
    case IR_CMD_POWER:
        return "POWER";
    case IR_CMD_NONE:
        return "NONE";
    }
    return "UNKNOWN";
}