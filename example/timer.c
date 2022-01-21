#include "board.h"
#include <stdio.h>

#define TICKRATE_HZ1 (11)	/* 11 ticks per second */

/**
 * @brief	Handle interrupt from 32-bit timer
 * @return	Nothing
 */
void TIMER0_IRQHandler(void)
{
    if (Chip_TIMER_MatchPending(LPC_TIMER0, 1)) {
      Chip_TIMER_ClearMatch(LPC_TIMER0, 1);
    }
}

/**
 * @brief	main routine for blinky example
 * @return	Function should not exit.
 */
int main(void)
{
    uint32_t timerFreq;

    /* Generic Initialization */
    SystemCoreClockUpdate();
    Board_Init();

    /* Enable timer 1 clock */
    Chip_TIMER_Init(LPC_TIMER0);

    /* Timer rate is system clock rate */
    timerFreq = Chip_Clock_GetSystemClockRate();

    /* Timer setup for match and interrupt at TICKRATE_HZ */
    Chip_TIMER_Reset(LPC_TIMER0);
    Chip_TIMER_MatchEnableInt(LPC_TIMER0, 1);
    Chip_TIMER_SetMatch(LPC_TIMER0, 1, (timerFreq / TICKRATE_HZ1));
    Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 1);
    Chip_TIMER_Enable(LPC_TIMER0);

    /* Enable timer interrupt */
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(TIMER0_IRQn);

    while (1) {
      __WFI();
    }
}
