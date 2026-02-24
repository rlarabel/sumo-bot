#include "qre1113.h"
#include "adc.h"
#include "io.h"
#include "../common/assert_handler.h"
#include <stdbool.h>

static bool initialized = false;
void qre1113_init(void)
{
    ASSERT(!initialized);
    adc_init();
    initialized = true;
}

void qre1113_get_voltages(struct qre1113_voltages *voltages)
{
    adc_channel_values_t values;
    adc_get_channel_values(values);
    voltages->front_left = values[io_to_adc_idx(IO_ADC_CHANNEL_0)];
#if defined(JR)
    voltages->back_left = values[io_to_adc_idx(IO_ADC_CHANNEL_3)];
    voltages->front_right = values[io_to_adc_idx(IO_ADC_CHANNEL_4)];
    voltages->back_right = values[io_to_adc_idx(IO_ADC_CHANNEL_5)];
#endif
}