#include "board.h"

/* EMC clock delay */
#define CLK0_DELAY 0

/* Keil SDRAM timing and chip Config */
STATIC const IP_EMC_DYN_CONFIG_T IS42S32800D_Config = {
	EMC_NANOSECOND(64000000 / 4096),
	0x01,				/* Command Delayed */
	3,					/* tRP */
	7,					/* tRAS */
	EMC_NANOSECOND(70),	/* tSREX */
	EMC_CLOCK(0x01),	/* tAPR */
	EMC_CLOCK(0x05),	/* tDAL */
	EMC_NANOSECOND(12),	/* tWR */
	EMC_NANOSECOND(60),	/* tRC */
	EMC_NANOSECOND(60),	/* tRFC */
	EMC_NANOSECOND(70),	/* tXSR */
	EMC_NANOSECOND(12),	/* tRRD */
	EMC_CLOCK(0x02),	/* tMRD */
	{
		{
			EMC_ADDRESS_DYCS0,	/* EA Board uses DYCS0 for SDRAM */
			2,	/* RAS */

			EMC_DYN_MODE_WBMODE_PROGRAMMED |
			EMC_DYN_MODE_OPMODE_STANDARD |
			EMC_DYN_MODE_CAS_2 |
			EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL |
			EMC_DYN_MODE_BURST_LEN_4,

			EMC_DYN_CONFIG_DATA_BUS_32 |
			EMC_DYN_CONFIG_LPSDRAM |
			EMC_DYN_CONFIG_8Mx16_4BANKS_12ROWS_9COLS |
			EMC_DYN_CONFIG_MD_SDRAM
		},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	}
};

/* NorFlash timing and chip Config */
STATIC const IP_EMC_STATIC_CONFIG_T SST39VF3201B_Config = {
	0,
	EMC_STATIC_CONFIG_MEM_WIDTH_16 |
	EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW |
	EMC_STATIC_CONFIG_BLS_HIGH /* |
							      EMC_CONFIG_BUFFER_ENABLE*/,

	EMC_NANOSECOND(0),
	EMC_NANOSECOND(35),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(40),
	EMC_CLOCK(4)
};

/* NandFlash timing and chip Config */
STATIC const IP_EMC_STATIC_CONFIG_T K9F1G08U0D_Config = {
	1,
	EMC_STATIC_CONFIG_MEM_WIDTH_8 |
	EMC_STATIC_CONFIG_CS_POL_ACTIVE_LOW |
	EMC_STATIC_CONFIG_BLS_HIGH /* |
							      EMC_CONFIG_BUFFER_ENABLE*/,

	EMC_NANOSECOND(0),
	EMC_NANOSECOND(35),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(70),
	EMC_NANOSECOND(40),
	EMC_CLOCK(4)
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Setup system clocking */
void Board_SetupClocking(void)
{
	/* Enable PBOOST for CPU clock over 100MHz */
	Chip_SYSCTL_EnableBoost();

	Chip_SetupXtalClocking();
	
	/* SPIFI clocking will be derived from Main PLL with a divider of 2 (60MHz) */
	Chip_Clock_SetSPIFIClockDiv(2);
	Chip_Clock_SetSPIFIClockSource(SYSCTL_SPIFICLKSRC_MAINPLL);

}

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	int i, j;

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_EMC);
	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_IOCON);

	/* Setup data, address, and EMC control pins with high slew rate */
	for (i = 3; i <= 4; i++) {
		for (j = 0; j <= 31; j++) {
			Chip_IOCON_PinMuxSet(LPC_IOCON, (uint8_t) i, (uint8_t) j, (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
		}
	}
	for (i = 16; i <= 31; i++) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, (uint8_t) i, (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
	}

        Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 0, (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
        Chip_IOCON_PinMuxSet(LPC_IOCON, 5, 1 , (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
}

/* Setup external memories */
void Board_SetupExtMemory(void)
{
	/* Setup EMC Delays */
	/* Move all clock delays together */
	LPC_SYSCTL->EMCDLYCTL = (CLK0_DELAY) | (CLK0_DELAY << 8) | (CLK0_DELAY << 16 | (CLK0_DELAY << 24));

	/* Setup EMC Clock Divider for divide by 2 */
	/* Setup EMC clock for a divider of 2 from CPU clock. Enable EMC clock for
	   external memory setup of DRAM. */
	Chip_Clock_SetEMCClockDiv(SYSCTL_EMC_DIV2);
	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_EMC);

	/* Init EMC Controller -Enable-LE mode- clock ratio 1:1 */
	Chip_EMC_Init(1, 0, 0);

	/* Init EMC Dynamic Controller */
	Chip_EMC_Dynamic_Init((IP_EMC_DYN_CONFIG_T *) &IS42S32800D_Config);

	/* Init EMC Static Controller CS0 */
	Chip_EMC_Static_Init((IP_EMC_STATIC_CONFIG_T *) &SST39VF3201B_Config);
        
	/* Init EMC Static Controller CS1 */
	Chip_EMC_Static_Init((IP_EMC_STATIC_CONFIG_T *) &K9F1G08U0D_Config);

	/* EMC Shift Control */
	LPC_SYSCTL->SCS |= 1;
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	Board_SetupMuxing();
	Board_SetupClocking();
	Board_SetupExtMemory();
}
