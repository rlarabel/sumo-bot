#include "mcu_init.h"
#include "io.h"
#include <msp430.h>

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
    // Enables globaly
    _enable_interrupts();
}