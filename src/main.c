#include <msp430.h>
#include "drivers/io.h"
#include "drivers/mcu_init.h"
static void test_setup(void)
{
    mcu_init();
}

// TODO: Put in test.c

/*
static void test_blink_led(void)
{
    test_setup();
    const struct io_config led_config =
    {
        .dir = IO_DIR_OUTPUT,
        .select = IO_SEL_GPIO,
        .resistor = IO_RES_DIS,
        .out = IO_OUT_LOW
    };
    io_configure(IO_TEST_LED, &led_config);
    io_out_e out = IO_OUT_LOW;
    while (1) {
        out = (out == IO_OUT_LOW) ? IO_OUT_HIGH : IO_OUT_LOW;
        io_set_out(IO_TEST_LED, out);
        __delay_cycles(2500000); // 2500 ms
    }
}

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
            __delay_cycles(10000);
            io_set_out(io, IO_OUT_LOW);
        }
    }
}
*/
/* Configure all pins except pin 1.0 (test led) internal pull up resistors.
 * Verify by pulling each pin down in increasing order with an
 * external pull-down resistor. LED state changes when the right pin is
 * pulled doen. Oncce all pins have been cerified, the LED blinks continously.
 */
static void test_launchpad_io_pins_input(void)
{
    test_setup();
    const struct io_config input_config = {
        .select = IO_SEL_GPIO,
        .resistor = IO_RES_EN,
        .dir = IO_DIR_INPUT,
        .out = IO_OUT_HIGH,
    };

    // TODO: Replace with LED driver
    const struct io_config led_config = {
        .select = IO_SEL_GPIO,
        .resistor = IO_RES_DIS,
        .dir = IO_DIR_OUTPUT,
        .out = IO_OUT_LOW,
    };

    // Configure test led and all other pins as inputs
    const io_generic_e io_led = IO_10;

    for (io_generic_e io = IO_10; io <= IO_27; io++) {
        io_configure(io, &input_config);
    }

    io_configure(io_led, &led_config);

    // Test inputs
    for (io_generic_e io = IO_10; io <= IO_27; io++) {
        if (io == io_led)
            continue;
        io_set_out(io_led, IO_OUT_HIGH);
        // wait for user to pull pin low
        while (io_get_input(io) == IO_IN_HIGH) {
            __delay_cycles(100000); // 100 ms
        }
        io_set_out(io_led, IO_OUT_LOW);
        // Wait for user to disconnect
        while (io_get_input(io) == IO_IN_LOW) {
            __delay_cycles(100000); // 100 ms
        }
    }

    // Blink LED when test is done
    while (1) {
        io_set_out(io_led, IO_OUT_HIGH);
        __delay_cycles(500000); // 500 ms
        io_set_out(io_led, IO_OUT_LOW);
        __delay_cycles(500000); // 500 ms
    }
}

int main(void)
{
    // test_blink_led();
    // test_launchpad_io_pins_output();
    test_launchpad_io_pins_input();
    return 0;
}