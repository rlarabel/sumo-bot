#include "drive.h"
#include "../drivers/tb6612fng.h"
#include "../common/assert_handler.h"
#include "../common/defines.h"
#include <assert.h>
#include <stdbool.h>

struct drive_speeds
{
    uint8_t left;
    uint8_t right;
};

/* Drive direction come in pairs. To save flash space, only save the speeds
 * for one direction (primary), and create a macro to get the corresponding
 * primary direction for every direction and inverse the speeds when its not
 * a primary direction
 */
#define DRIVE_PRIMARY_DIRECTION(dir) (dir - MODULO_2(dir))
static_assert(DRIVE_PRIMARY_DIRECTION(DRIVE_DIR_REVERSE) == DRIVE_DIR_FORWARD);
static const struct drive_speeds drive_primary_speeds[][4] =
{
    [DRIVE_DIR_FORWARD] = {
        [DRIVE_SPEED_SLOW] = {25, 25},
        [DRIVE_SPEED_MED] = {45, 45}, 
        [DRIVE_SPEED_FAST] = {55, 55}, 
        [DRIVE_SPEED_MAX] = {100, 100}, 
    },
    [DRIVE_DIR_ROTATE_LEFT] = {
        [DRIVE_SPEED_SLOW] = {-25, 25},
        [DRIVE_SPEED_MED] = {-45, 45}, 
        [DRIVE_SPEED_FAST] = {-55, 55}, 
        [DRIVE_SPEED_MAX] = {-100, 100}, 
    },
    [DRIVE_DIR_ARCTURN_SHARP_LEFT] = {
        [DRIVE_SPEED_SLOW] = {0, 25},
        [DRIVE_SPEED_MED] = {0, 45}, 
        [DRIVE_SPEED_FAST] = {0, 55}, 
        [DRIVE_SPEED_MAX] = {0, 100}, 
    },
    [DRIVE_DIR_ARCTURN_MID_LEFT] = {
        [DRIVE_SPEED_SLOW] = {8, 25},
        [DRIVE_SPEED_MED] = {15, 45}, 
        [DRIVE_SPEED_FAST] = {18, 55}, 
        [DRIVE_SPEED_MAX] = {33, 100}, 
    },
    [DRIVE_DIR_ARCTURN_WIDE_LEFT] = {
        [DRIVE_SPEED_SLOW] = {16, 25},
        [DRIVE_SPEED_MED] = {30, 45}, 
        [DRIVE_SPEED_FAST] = {36, 55}, 
        [DRIVE_SPEED_MAX] = {66, 100}, 
    }
};

static void drive_inverse_speeds(int8_t *speed_left, int8_t *speed_right)
{
    if (*speed_left == *speed_right) {
        // reverse
        *speed_left = -*speed_left;
        *speed_right = -*speed_right;
    } else {
        // swap
        const int8_t temp = *speed_left;
        *speed_left = *speed_right;
        *speed_right = temp;
    }
}

void drive_set(drive_dir_e direction, drive_speed_e speed)
{
    drive_dir_e primary_direction = DRIVE_PRIMARY_DIRECTION(direction);
    int8_t speed_left = drive_primary_speeds[primary_direction][speed].left;
    int8_t speed_right = drive_primary_speeds[primary_direction][speed].right;
    if (direction != primary_direction) {
        drive_inverse_speeds(&speed_left, &speed_right);
    }
    ASSERT(speed_left != 0 && speed_right != 0);
    const tb6612fng_mode_e mode_left =
        speed_left > 0 ? TB6612FNG_MODE_FORWARD : TB6612FNG_MODE_REVERSE;
    tb6612fng_set_mode(TB6612FNG_LEFT, mode_left);
    tb6612fng_set_pwm(TB6612FNG_LEFT, ABS(speed_left));
    const tb6612fng_mode_e mode_right =
        speed_right > 0 ? TB6612FNG_MODE_FORWARD : TB6612FNG_MODE_REVERSE;
    tb6612fng_set_mode(TB6612FNG_RIGHT, mode_right);
    tb6612fng_set_pwm(TB6612FNG_RIGHT, ABS(speed_right));
}

void drive_stop(void)
{
    tb6612fng_set_mode(TB6612FNG_LEFT, TB6612FNG_MODE_STOP);
    tb6612fng_set_mode(TB6612FNG_RIGHT, TB6612FNG_MODE_STOP);
    tb6612fng_set_pwm(TB6612FNG_LEFT, 0);
    tb6612fng_set_pwm(TB6612FNG_RIGHT, 0);
}

static bool initialized = false;
void drive_init(void)
{
    ASSERT(!initialized);
    tb6612fng_init();
}