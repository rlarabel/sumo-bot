#ifndef IO_H
#define IO_H
/* IO pin handling including pinmapping, initialization, and configuration.
 * This wraps the lower register defines provided in the headers from
 * Texas Instruments
 */

// TODO: Improve hardware handling
#define LAUNCHPAD

// TODO: enums
typedef enum {
#if defined(LAUNCHPAD)
    IO_TEST_LED,
    IO_UART_RXD,
    IO_UART_TXD,
    IO_UNUSED_0,
    IO_UNUSED_1,
    IO_UNUSED_2,
    IO_UNUSED_3,
    IO_UNUSED_4,
    IO_UNUSED_5,
    IO_UNUSED_6,
    IO_UNUSED_7,
    IO_UNUSED_8,
    IO_UNUSED_9,
    IO_UNUSED_10,
    IO_UNUSED_11,
    IO_UNUSED_12,
    IO_UNUSED_13
#elif defined(JR)
    IO_ADC_CHANNEL_0,
    IO_UART_RX,
    IO_UART_TX,
    IO_ADC_CHANNEL_3,
    IO_ADC_CHANNEL_4,
    IO_ADC_CHANNEL_5,
    IO_SCL,
    IO_SDA,
    IO_PWM_MOTOR_A,
    IO_PWM_MOTOR_B,
    IO_BIN_2,
    IO_BIN_1,
    IO_AIN_2,
    IO_AIN_1,
    IO_UNUSED_0,
    IO_UNUSED_1,
    IO_UNUSED_2,
    IO_XSHUT_LEFT,
    IO_XSHUT_RIGHT,
    IO_XSHUT_MID,
    IO_TIMER_IR_RECEIVER,
    IO_INT_MID,
    IO_UNUSED_3,
    IO_UNUSED_4
#endif
} io_e;

typedef enum {
    IO_SEL_GPIO,
    IO_SEL_ALT1,
    IO_SEL_ALT2,
    IO_SEL_ALT3,
} io_sel_e;

typedef enum {
    IO_DIR_OUTPUT,
    IO_DIR_INPUT,
} io_dir_e;

typedef enum {
    IO_RES_DIS,
    IO_RES_EN,
} io_res_e;

typedef enum {
    IO_OUT_LOW, // Pull Down
    IO_OUT_HIGH, // Pull Up
} io_out_e;

typedef enum {
    IO_IN_LOW,
    IO_IN_HIGH,
} io_in_e;

// TODO: stucts
void io_set_select(io_e io, io_sel_e select);
void io_set_direction(io_e io, io_dir_e direction);
void io_set_resistor(io_e io, io_res_e resistor);
void io_set_out(io_e io, io_out_e out);
io_in_e io_get_input(io_e io);

#endif