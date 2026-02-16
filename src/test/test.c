#include <msp430.h>
#include "../drivers/io.h"
#include "../drivers/mcu_init.h"
#include "../drivers/led.h"
#include "../drivers/uart.h"
#include "../drivers/ir_remote.h"
#include "../drivers/pwm.h"
#include "../drivers/tb6612fng.h"
#include "../common/assert_handler.h"
#include "../common/defines.h"
#include "../common/trace.h"
//#include "external/printf/printf.h"
//#include <stdio.h>

SUPPRESS_UNUSED
static void test_setup(void)
{
    mcu_init();
}

SUPPRESS_UNUSED
static void test_assert(void)
{
    test_setup();
    ASSERT(0);
}

SUPPRESS_UNUSED
static void test_blink_led(void)
{
    test_setup();
    led_init();
    led_state_e led_state = LED_OFF;
    while (1) {
        led_state = (led_state == LED_ON) ? LED_OFF : LED_ON;
        led_set(LED_TEST, led_state);
        BUSY_WAIT_ms(250);
    }
}

SUPPRESS_UNUSED
static void test_launchpad_io_pins_output(void)
{
    test_setup();
    const struct io_config output_config =
    {
        .select = IO_SEL_GPIO,
        .resistor = IO_RES_DIS,
        .dir = IO_DIR_OUTPUT,
        .out = IO_OUT_LOW
    };

    // Configure all pins as output
    for (io_generic_e io = IO_10; io <= IO_27; io++)
    {
        io_configure(io, &output_config);
    }
    while (1) {
        for (io_generic_e io = IO_10; io <= IO_27; io++)
        {
            io_set_out(io, IO_OUT_HIGH);
            BUSY_WAIT_ms(100);
            io_set_out(io, IO_OUT_LOW);
        }
    }
}

/* Configure all pins except pin 1.0 (test led) internal pull up resistors.
 * Verify by pulling each pin down in increasing order with an
 * external pull-down resistor. LED state changes when the right pin is
 * pulled down. Oncce all pins have been cerified, the LED blinks continously.
*/
SUPPRESS_UNUSED
static void test_launchpad_io_pins_input(void)
{
    test_setup();
    led_init();

    const struct io_config input_config = {
        .select = IO_SEL_GPIO,
        .resistor = IO_RES_EN,
        .dir = IO_DIR_INPUT,
        .out = IO_OUT_HIGH,
    };

    for (io_generic_e io = IO_11; io <= IO_27; io++) {
        io_configure(io, &input_config);
    }

    // Test inputs
    for (io_generic_e io = IO_10; io <= IO_27; io++) {
        if (io == (io_generic_e)IO_TEST_LED)
            continue;
        led_set(LED_TEST, LED_ON);
        // wait for user to pull pin low
        while (io_get_input(io) == IO_IN_HIGH) {
            BUSY_WAIT_ms(100); // 100 ms
        }
        led_set(LED_TEST, LED_OFF);
        // Wait for user to disconnect
        while (io_get_input(io) == IO_IN_LOW) {
            BUSY_WAIT_ms(100); // 100 ms
        }
    }

    // Blink LED when test is done
    while (1) {
        led_set(LED_TEST, LED_ON);
        BUSY_WAIT_ms(500); // 500 ms
        led_set(LED_TEST, LED_OFF);
        BUSY_WAIT_ms(500); // 500 ms
    }
}

SUPPRESS_UNUSED
static void io_11_isr(void)
{
    led_set(LED_TEST, LED_ON);
}

SUPPRESS_UNUSED
static void io_20_isr(void)
{
    led_set(LED_TEST, LED_OFF);
}

SUPPRESS_UNUSED
static void test_io_interrupt(void)
{
    test_setup();
    const struct io_config input_config = {
        .select = IO_SEL_GPIO,
        .resistor = IO_RES_EN,
        .dir = IO_DIR_INPUT,
        .out = IO_OUT_HIGH,
    };
    io_configure(IO_11, &input_config);
    io_configure(IO_20, &input_config);
    led_init();
    io_configure_interrupt(IO_11, IO_TRIGGER_FALLING, io_11_isr);
    io_configure_interrupt(IO_20, IO_TRIGGER_FALLING, io_20_isr);
    io_enable_interrupt(IO_11);
    io_enable_interrupt(IO_20);
    while (1);
    
}

SUPPRESS_UNUSED
static void test_uart(void)
{
    test_setup();
    uart_init();

    while (1) {
        _putchar('H');
        _putchar('e');
        _putchar('l');
        _putchar('l');
        _putchar('o');
        _putchar(',');
        _putchar(' ');
        _putchar('W');
        _putchar('o');
        _putchar('r');
        _putchar('l');
        _putchar('d');
        _putchar('!');
        _putchar('\n');
        BUSY_WAIT_ms(2000);
    }
}

SUPPRESS_UNUSED
static void test_trace(void)
{
    test_setup();
    trace_init();
    while(1){
        TRACE("Hello, %d", 2026);
        BUSY_WAIT_ms(2000);
    }
}

SUPPRESS_UNUSED
static void test_ir_remote(void)
{
    test_setup();
    trace_init();
    led_init();
    ir_remote_init();
    while(1) {
        TRACE("Command %s", ir_remote_cmd_to_string(ir_remote_get_cmd()));
        BUSY_WAIT_ms(250);
    }
}

SUPPRESS_UNUSED
static void test_pwm(void)
{
    test_setup();
    trace_init();
    pwm_init();
    const uint8_t duty_cycles[] = { 100, 75, 50, 25, 1, 0 };
    const uint16_t wait_time = 3000;
    while(1) {
        for (uint8_t i = 0; i < ARRAY_SIZE(duty_cycles); i++) {
            TRACE("Set duty cycle to %d for %d ms", duty_cycles[i], wait_time);
            pwm_set_duty_cycle(PWM_TB6612FNG_LEFT, duty_cycles[i]);
            pwm_set_duty_cycle(PWM_TB6612FNG_RIGHT, duty_cycles[i]);
            BUSY_WAIT_ms(wait_time);
        }
    }
}

SUPPRESS_UNUSED
static void test_tb6612fng(void)
{
    test_setup();
    trace_init();
    tb6612fng_init();
    const tb6612fng_mode_e modes[] =
    {
        TB6612FNG_MODE_FORWARD,
        TB6612FNG_MODE_REVERSE,
        TB6612FNG_MODE_FORWARD,
        TB6612FNG_MODE_REVERSE,
    };
    const uint8_t duty_cycles[] = { 100, 50, 25, 0 };
    while (1) {
        for (uint8_t i = 0; i < ARRAY_SIZE(duty_cycles); i++)
        {
            TRACE("Set mode %d and duty cycle %d", modes[i], duty_cycles[i]);
            tb6612fng_set_mode(TB6612FNG_LEFT, modes[i]);
            tb6612fng_set_mode(TB6612FNG_RIGHT, modes[i]);
            tb6612fng_set_pwm(TB6612FNG_LEFT, duty_cycles[i]);
            tb6612fng_set_pwm(TB6612FNG_RIGHT, duty_cycles[i]);
            BUSY_WAIT_ms(3000);
            tb6612fng_set_mode(TB6612FNG_LEFT, TB6612FNG_MODE_STOP);
            tb6612fng_set_mode(TB6612FNG_RIGHT, TB6612FNG_MODE_STOP);
            BUSY_WAIT_ms(1000);
        }
    }
}

int main()
{
    TEST();
    ASSERT(0);
}