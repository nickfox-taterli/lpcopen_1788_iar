#include "board.h"
#include "string.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System oscillator rate and RTC oscillator rate */
const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 32768;

/*****************************************************************************
 * Private functions
 ****************************************************************************/
static void Board_NANDFLash_Init(void)
{
    /* NANDFLASH Ready pin */
    Chip_IOCON_PinMuxSet(LPC_IOCON, NANDFLASH_READY_PORT, NANDFLASH_READY_PIN, (IOCON_FUNC0 | IOCON_MODE_INACT));
    Chip_GPIO_WriteDirBit(LPC_GPIO, NANDFLASH_READY_PORT, NANDFLASH_READY_PIN, false);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
    /* Initializes GPIO */
    Chip_GPIO_Init(LPC_GPIO);
    Chip_IOCON_Init(LPC_IOCON);
    Board_NANDFLash_Init();
}
