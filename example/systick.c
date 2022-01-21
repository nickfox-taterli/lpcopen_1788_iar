#include "board.h"
#include "intrinsics.h"

/* 1000Hz Interrupt */
void SysTick_Handler(void)
{
    asm("NOP");
}

int main(void)
{
    /* Generic Initialization */
    SystemCoreClockUpdate();
    Board_Init();

    /* Enable and setup SysTick Timer at a periodic rate */
    SysTick_Config(SystemCoreClock / 1000);

    /* LEDs toggle in interrupt handlers */
    while (1) {
      __WFI();
    }
}