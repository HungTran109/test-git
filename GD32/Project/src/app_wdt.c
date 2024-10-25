#include "app_wdt.h"
#include "gd32e23x.h"

#define APP_WDT_ENABLE      1

static void irc40k_config(void);

/**
 * @brief Start app watchdog
 */
void app_wdt_start(void)
{
#if APP_WDT_ENABLE
    /* Enable write access to FWDGT_PSC and FWDGT_RLD registers.
        FWDGT counter clock: 40KHz(IRC40K) / 256 = 156,25 Hz */
    fwdgt_config(4095, FWDGT_PSC_DIV256);
    fwdgt_enable();
    
    /* check if the system has resumed from FWDGT reset */
    if (RESET != rcu_flag_get(RCU_FLAG_FWDGTRST))
    {
        rcu_all_reset_flag_clear();
    }
#endif
}

void app_wdt_feed(void)
{
#if APP_WDT_ENABLE
    fwdgt_counter_reload();
#endif
}

static void irc40k_config(void)
{
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    rcu_osci_stab_wait(RCU_IRC40K);
}