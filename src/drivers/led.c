#include "led.h"
#include "../common/defines.h"
#include "../common/assert_handler.h"
#include "io.h"
#include <stdbool.h>

static const struct io_config led_config = {
    .select = IO_SEL_GPIO, .resistor = IO_RES_DIS, .dir = IO_DIR_OUTPUT, .out = IO_OUT_LOW
};

static bool initialized = false;
void led_init(void)
{
    ASSERT(!initialized);
    struct io_config current_config;
    io_get_current_config(IO_TEST_LED, &current_config);
    ASSERT(io_config_compare(&led_config, &current_config))
    initialized = true;
}

void led_set(led_e led, led_state_e state)
{
    ASSERT(initialized);
    const io_out_e out = (state == LED_ON) ? IO_OUT_HIGH : IO_OUT_LOW;
    switch (led) {
    case LED_TEST:
        io_set_out(IO_TEST_LED, out);
        break;
    }
}