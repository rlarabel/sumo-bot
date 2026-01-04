#include <msp430.h>
#include "../drivers/io.h"
#include "../drivers/mcu_init.h"
#include "../drivers/led.h"
#include "../drivers/uart.h"
#include "../common/assert_handler.h"
#include "../common/defines.h"

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
        uart_print_interrupt("Hello, World\n");
        BUSY_WAIT_ms(2000);
    }
}

int main()
{
    TEST();
    ASSERT(0);
}