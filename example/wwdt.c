#include "board.h"

/**
 * @brief	watchdog timer Interrupt Handler
 * @return	Nothing
 * @note	Handles watchdog timer warning and timeout events
 */
void WDT_IRQHandler(void)
{
    uint32_t wdtStatus = Chip_WWDT_GetStatus(LPC_WWDT);

    /* The chip will reset before this happens, but if the WDT doesn't
       have WWDT_MOD_WDRESET enabled, this will hit once */
    if (wdtStatus & WWDT_WDMOD_WDTOF) {
        /* A watchdog feed didn't occur prior to window timeout */
        Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF);
    }
    /* Handle warning interrupt */
    if (wdtStatus & WWDT_WDMOD_WDINT) {
        /* A watchdog feed didn't occur prior to warning timeout */
        Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDINT);
        Chip_WWDT_Feed(LPC_WWDT);
    }
}

/**
 * @brief	Main entry point
 * @return	Nothing
 */
int main(void)
{
    SystemCoreClockUpdate();
    Board_Init();

    /* Initialize WWDT and event router */
    Chip_WWDT_Init(LPC_WWDT);
    
    /* Set watchdog feed time constant to 0.1s */
    Chip_WWDT_SetTimeOut(LPC_WWDT, SYSCTL_OSCEC / 10);

    /* Set watchdog warning time to 512 ticks after feed time constant
       Set watchdog window time to 0.9s */
    Chip_WWDT_SetWarning(LPC_WWDT, 512);
    Chip_WWDT_SetWindow(LPC_WWDT, WDT_OSC - (WDT_OSC / 10));

    /* Configure WWDT to reset on timeout */
    Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET);

    /* Clear watchdog warning and timeout interrupts */
    Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF | WWDT_WDMOD_WDINT);

    /* Start watchdog */
    Chip_WWDT_Start(LPC_WWDT);

    /* Enable watchdog interrupt */
    NVIC_ClearPendingIRQ(WDT_IRQn);
    NVIC_EnableIRQ(WDT_IRQn);
    
    for(;;);
}
