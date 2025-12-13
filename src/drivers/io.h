#ifndef IO_H
#define IO_H
/* IO pin handling including pinmapping, initialization, and configuration.
 * This wraps the lower register defines provided in the headers from
 * Texas Instruments
 */

// TODO: Improve hardware handling
#define LAUNCHPAD

/********************** ENUMS ************************/
// clang-format off
typedef enum {
    IO_10, IO_11, IO_12, IO_13, IO_14, IO_15, IO_16, IO_17,
    IO_20, IO_21, IO_22, IO_23, IO_24, IO_25, IO_26, IO_27,
#if defined(JR)
    IO_30, IO_31, IO_32, IO_33, IO_34, IO_35, IO_36, IO_37,
#endif 
} io_generic_e;
// clang-format on

typedef enum {
#if defined(LAUNCHPAD)
    IO_TEST_LED = IO_10,
    IO_UART_RXD = IO_11,
    IO_UART_TXD = IO_12,
    IO_UNUSED_0 = IO_13,
    IO_UNUSED_1 = IO_14,
    IO_UNUSED_2 = IO_15,
    IO_UNUSED_3 = IO_16,
    IO_UNUSED_4 = IO_17,
    IO_UNUSED_5 = IO_20,
    IO_UNUSED_6 = IO_21,
    IO_UNUSED_7 = IO_22,
    IO_UNUSED_8 = IO_23,
    IO_UNUSED_9 = IO_24,
    IO_UNUSED_10 = IO_25,
    IO_UNUSED_11 = IO_26,
    IO_UNUSED_12 = IO_27,
#elif defined(JR)
    IO_ADC_CHANNEL_0 = IO_10,
    IO_UART_RX = IO_11,
    IO_UART_TX = IO_12,
    IO_ADC_CHANNEL_3 = IO_13,
    IO_ADC_CHANNEL_4 = IO_14,
    IO_ADC_CHANNEL_5 = IO_15,
    IO_SCL = IO_16,
    IO_SDA = IO_17,
    IO_PWM_MOTOR_A = IO_20,
    IO_PWM_MOTOR_B = IO_21,
    IO_BIN_2 = IO_22,
    IO_BIN_1 = IO_23,
    IO_AIN_2 = IO_24,
    IO_AIN_1 = IO_25,
    IO_UNUSED_0 = IO_26,
    IO_UNUSED_1 = IO_27,
    IO_UNUSED_2 = IO_30,
    IO_XSHUT_LEFT = IO_31,
    IO_XSHUT_RIGHT = IO_32,
    IO_XSHUT_MID = IO_33,
    IO_TIMER_IR_RECEIVER = IO_34,
    IO_INT_MID = IO_35,
    IO_UNUSED_3 = IO_36,
    IO_UNUSED_4 = IO_37
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

/********************** STRUCTS ************************/
struct io_config
{
    io_sel_e select;
    io_res_e resistor;
    io_dir_e dir;
    io_out_e out;
};

/********************** FUNC ************************/
void io_init(void);
void io_configure(io_e io, const struct io_config *config);
void io_set_select(io_e io, io_sel_e select);
void io_set_direction(io_e io, io_dir_e direction);
void io_set_resistor(io_e io, io_res_e resistor);
void io_set_out(io_e io, io_out_e out);
io_in_e io_get_input(io_e io);

#endif