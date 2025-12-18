#ifndef LED_H

// simple driver configuring GPIO pins that are connected to LEDS

typedef enum {
    LED_TEST
} led_e;

typedef enum {
    LED_OFF,
    LED_ON
} led_state_e;
void led_init(void);
void led_set(led_e led, led_state_e state);

#endif