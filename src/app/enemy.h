#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>

/* A software layer that converts the range measurements into discrete
 * enemy positions and distance to simplfy the application code
 */

typedef enum {
    ENEMY_POS_NONE,
    ENEMY_POS_LEFT,
    ENEMY_POS_MID,
    ENEMY_POS_RIGHT,
    ENEMY_POS_MID_LEFT,
    ENEMY_POS_MID_RIGHT,
    ENEMY_POS_ALL,
    ENEMY_POS_IMPOSSIBLE, // Keep for debugging purposes
} enemy_pos_e;

typedef enum {
    ENEMY_RANGE_NONE,
    ENEMY_RANGE_CLOSE,
    ENEMY_RANGE_MID,
    ENEMY_RANGE_FAR,
} enemy_range_e;

struct enemy
{
    enemy_pos_e position;
    enemy_range_e range;
};

void enemy_init(void);
struct enemy enemy_get(void);
bool enemy_detected(const struct enemy *enemy);
bool enemy_at_left(const struct enemy *enemy);
bool enemy_at_right(const struct enemy *enemy);
bool enemy_at_mid(const struct enemy *enemy);

const char *enemy_pos_str(enemy_pos_e pos);
const char *enemy_range_str(enemy_range_e range);

#endif // ENEMY_H