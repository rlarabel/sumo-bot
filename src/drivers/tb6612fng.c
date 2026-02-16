#include "io.h"
#include "tb6612fng.h"
#include "pwm.h"
#include "../common/assert_handler.h"
#include <assert.h>

struct cc_pins
{
    io_e cc1;
    io_e cc2;
};

static struct cc_pins tb6612fng_cc_pins[] = {
    [TB6612FNG_LEFT] = { IO_MOTORS_AIN_1, IO_MOTORS_AIN_2 },
#if defined(LAUNCHPAD)
    // Launchpad has no pins for right motor driver, duplicate left to avoid compile errors
    [TB6612FNG_RIGHT] = { IO_MOTORS_AIN_1, IO_MOTORS_AIN_2 },
#elif defined(JR)
    [TB6612FNG_RIGHT] = { IO_MOTORS_BIN_1, IO_MOTORS_BIN_2 },
#endif

};

void tb6612fng_set_mode(tb6612fng_e tb, tb6612fng_mode_e mode)
{
    switch (mode) {
    case TB6612FNG_MODE_STOP:
        io_set_out(tb6612fng_cc_pins[tb].cc1, IO_OUT_LOW);
        io_set_out(tb6612fng_cc_pins[tb].cc2, IO_OUT_LOW);
        break;
    case TB6612FNG_MODE_FORWARD:
        io_set_out(tb6612fng_cc_pins[tb].cc1, IO_OUT_HIGH);
        io_set_out(tb6612fng_cc_pins[tb].cc2, IO_OUT_LOW);
        break;
    case TB6612FNG_MODE_REVERSE:
        io_set_out(tb6612fng_cc_pins[tb].cc1, IO_OUT_LOW);
        io_set_out(tb6612fng_cc_pins[tb].cc2, IO_OUT_HIGH);
        break;
    }
}

static_assert(TB6612FNG_LEFT == (int)PWM_TB6612FNG_LEFT, "Enum mismatch");
static_assert(TB6612FNG_RIGHT == (int)PWM_TB6612FNG_RIGHT, "Enum mismatch");
void tb6612fng_set_pwm(tb6612fng_e tb, uint8_t duty_cycle)
{
    pwm_set_duty_cycle((pwm_e)tb, duty_cycle);
}

static void tb6612fng_assert_io_cfg(void)
{
    static const struct io_config cc_io_config = {
        .select = IO_SEL_GPIO, .resistor = IO_RES_DIS, .dir = IO_DIR_OUTPUT, .out = IO_OUT_LOW
    };
    struct io_config current_config;
    io_get_current_config(IO_MOTORS_AIN_1, &current_config);
    ASSERT(io_config_compare(&current_config, &cc_io_config));
    io_get_current_config(IO_MOTORS_AIN_2, &current_config);
    ASSERT(io_config_compare(&current_config, &cc_io_config));
#if defined(JR)
    io_get_current_config(IO_MOTORS_BIN_1, &current_config);
    ASSERT(io_config_compare(&current_config, &cc_io_config));
    io_get_current_config(IO_MOTORS_BIN_2, &current_config);
    ASSERT(io_config_compare(&current_config, &cc_io_config));
#endif
}

static bool initialized = false;
void tb6612fng_init(void)
{
    ASSERT(!initialized);
    tb6612fng_assert_io_cfg();
    pwm_init();
    initialized = true;
}