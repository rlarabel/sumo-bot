#include "enemy.h"
#include "../drivers/vl53l0x.h"
#include "../common/assert_handler.h"
#include "../common/trace.h"

#define RANGE_DETECT_THRESHOLD (600u) // mm
#define INVALID_RANGE (UINT16_MAX)
#define RANGE_CLOSE (100u) // mm
#define RANGE_MID (200u) // mm
#define RANGE_FAR (300u) // mm

// Nice to have enum to str function, can be deleted for more flash space
const char *enemy_pos_str(enemy_pos_e pos)
{
    switch (pos) {
    case ENEMY_POS_NONE:
        return "NONE";
    case ENEMY_POS_LEFT:
        return "LEFT";
    case ENEMY_POS_MID:
        return "MID";
    case ENEMY_POS_RIGHT:
        return "RIGHT";
    case ENEMY_POS_MID_LEFT:
        return "MID LEFT";
    case ENEMY_POS_MID_RIGHT:
        return "MID RIGHT";
    case ENEMY_POS_ALL:
        return "ALL POS";
    case ENEMY_POS_IMPOSSIBLE:
        return "IMPOSSIBLE POS";
    }
    return "";
}

const char *enemy_range_str(enemy_range_e range)
{
    switch (range) {
    case ENEMY_RANGE_NONE:
        return "NONE";
    case ENEMY_RANGE_CLOSE:
        return "CLOSE";
    case ENEMY_RANGE_MID:
        return "MED";
    case ENEMY_RANGE_FAR:
        return "FAR";
    }
    return "";
}
struct enemy enemy_get(void)
{
    struct enemy enemy = { ENEMY_POS_NONE, ENEMY_RANGE_NONE };
    vl53l0x_ranges_t ranges;
    bool fresh_values = false;
    vl53l0x_result_e result = vl53l0x_read_range_multiple(ranges, &fresh_values);
    if (result) {
        TRACE("Read range failed %u", result);
        return enemy;
    }
    const uint16_t range_mid = ranges[VL53L0X_IDX_MID];
    const uint16_t range_left = ranges[VL53L0X_IDX_LEFT];
    const uint16_t range_right = ranges[VL53L0X_IDX_RIGHT];

    const bool mid = range_mid < RANGE_DETECT_THRESHOLD;
    const bool left = range_left < RANGE_DETECT_THRESHOLD;
    const bool right = range_right < RANGE_DETECT_THRESHOLD;

    uint16_t range = INVALID_RANGE;
    if (mid) {
        if (right && left) {
            enemy.position = ENEMY_POS_ALL;
            range = (((range_mid + range_left) / 2) + range_right) / 2;
        } else if (right) {
            enemy.position = ENEMY_POS_MID_RIGHT;
            range = (range_mid + range_right) / 2;
        } else if (left) {
            enemy.position = ENEMY_POS_MID_LEFT;
            range = (range_mid + range_left) / 2;
        } else {
            enemy.position = ENEMY_POS_MID;
            range = range_mid;
        }
    } else if (right) {
        if (left) {
            enemy.position = ENEMY_POS_IMPOSSIBLE;
        } else {
            enemy.position = ENEMY_POS_RIGHT;
            range = range_right;
        }
    } else if (left) {
        enemy.position = ENEMY_POS_LEFT;
        range = range_left;
    } else {
        enemy.position = ENEMY_POS_NONE;
    }

    if (range == INVALID_RANGE) {
        return enemy;
    }

    if (range < RANGE_CLOSE) {
        enemy.range = ENEMY_RANGE_CLOSE;
    } else if (range < RANGE_MID) {
        enemy.range = ENEMY_RANGE_MID;
    } else {
        enemy.range = ENEMY_RANGE_FAR;
    }

    return enemy;
}

bool enemy_detected(const struct enemy *enemy)
{
    return enemy->position != ENEMY_POS_NONE && enemy->position != ENEMY_POS_IMPOSSIBLE;
}

bool enemy_at_left(const struct enemy *enemy)
{
    return enemy->position == ENEMY_POS_LEFT || enemy->position == ENEMY_POS_MID_LEFT;
}

bool enemy_at_mid(const struct enemy *enemy)
{
    return enemy->position == ENEMY_POS_MID || enemy->position == ENEMY_POS_ALL;
}

bool enemy_at_right(const struct enemy *enemy)
{
    return enemy->position == ENEMY_POS_RIGHT || enemy->position == ENEMY_POS_MID_RIGHT;
}

static bool initialized = false;
void enemy_init(void)
{
    ASSERT(!initialized);
    vl53l0x_result_e result = vl53l0x_init();
    if (result) {
        TRACE("Failed to initialize vl53l0x %u", result);
        return;
    }

    initialized = true;
}