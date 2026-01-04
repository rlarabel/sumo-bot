#include "mcu_init.h"
#include "io.h"
#include "../common/assert_handler.h"
#include <msp430.h>

static void init_clocks(void)
{
    // Check to make sure calibration data has not been erased
    ASSERT(CALBC1_1MHZ != 0xFF && CALBC1_16MHZ != 0xFF);

    /* Configure internal oscillator to run at 16 MHz
     * Used as a reference to produce a more stable DCO
     */
    BCSCTL1 = CALBC1_16MHZ;

    // Sets the clock rate of the digitally controlled oscillator (DCO)
    DCOCTL = CALDCO_16MHZ;
}

/* Watchdog is set by default
 * and repeatedly reset mcu if not turned off.
 */
static void watchdog_stop(void)
{
    WDTCTL = WDTPW + WDTHOLD;
}

void mcu_init(void)
{
    // Must stop watchdog first
    watchdog_stop();
    io_init();
    init_clocks();
    // Enables globaly
    _enable_interrupts();
}