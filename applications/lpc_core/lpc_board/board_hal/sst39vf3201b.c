#include "board.h"
#include "lpc_norflash.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Total size */
#define FLASH_SIZE                   (4 << 20)		/* 2Mx16 */
/* Sector size */
#define SECTOR_SIZE                  (4 << 10)		/* 2K words */
/* Block size */
#define BLOCK_SIZE                   (64 << 10)		/* 32K words */
/* Toggle bit */
#define TOGGLE_BIT                   (1 << 6)		/* DQ6 */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/* Init nor flash */
void lpc_norflash_init(void)
{}

/* Get the flash size */
void lpc_norflash_get_size(UNS_32 *size, UNS_32 *sector_count)
{
	*size = FLASH_SIZE;
	*sector_count = FLASH_SIZE / SECTOR_SIZE;	/* Uniform 2 KWord sectors*/
}

/* Get offset of a sector in flash */
UNS_32 lpc_norflash_get_sector_offset(UNS_32 sector)
{
	return SECTOR_SIZE * sector;
}

void lpc_norflash_get_id(UNS_16 *manu_id, UNS_16 *device_id)
{
	volatile uint32_t i;

	/* Enter Product Identification Mode */
	Board_NorFlash_WriteCmd(0x5555, 0xAAAA);
	Board_NorFlash_WriteCmd(0x2AAA, 0x5555);
	Board_NorFlash_WriteCmd(0x5555, 0x9090);

	/* Wait tIDA (~150ns) */
	// FIXME - this needs to be done with a more accurate delay
	for (i = 0; i < 10; i++) {}

	/* Read Manufacturer ID. It should be 0xBF */
	*manu_id = Board_NorFlash_ReadCmdData(0x0000);
	/* Read Device ID. It should be 0x235B for SST39VF3201 and 0x235A for SST39VF3202*/
	*device_id = Board_NorFlash_ReadCmdData(0x0001);

	/* Exit Product Identification Mode */
	Board_NorFlash_WriteCmd(0x5555, 0xAAAA);
	Board_NorFlash_WriteCmd(0x2AAA, 0x5555);
	Board_NorFlash_WriteCmd(0x5555, 0xF0F0);

	/* Wait tIDA (~150ns) */
	// FIXME - this needs to be done with a more accurate delay
	for (i = 0; i < 10; i++) {}
}

/* Toggle Bit check */
bool lpc_norflash_toggle_bit_check(UNS_32 addr)
{
	UNS_16 tmp1 = lpc_norflash_read_word(addr);
	UNS_16 tmp2 = lpc_norflash_read_word(addr);

	if ((tmp1 & TOGGLE_BIT) == (tmp2 & TOGGLE_BIT)) {
		return true;
	}
	return false;
}

/* Erase sector */
void lpc_norflash_erase_sector(UNS_32 sec_addr)
{
	Board_NorFlash_WriteCmd(0x5555, 0xAAAA);
	Board_NorFlash_WriteCmd(0x2AAA, 0x5555);
	Board_NorFlash_WriteCmd(0x5555, 0x8080);
	Board_NorFlash_WriteCmd(0x5555, 0xAAAA);
	Board_NorFlash_WriteCmd(0x2AAA, 0x5555);
	Board_NorFlash_WriteWord(sec_addr, 0x3030);
}

/* Write data to flash */
void lpc_norflash_write_word(UNS_32 addr, UNS_16 data)
{
	Board_NorFlash_WriteCmd(0x5555, 0xAAAA);
	Board_NorFlash_WriteCmd(0x2AAA, 0x5555);
	Board_NorFlash_WriteCmd(0x5555, 0xA0A0);
	Board_NorFlash_WriteWord(addr, data);
}

/* Write buffer to flash */
UNS_32 lpc_norflash_write_buffer(UNS_32 addr, UNS_16 *data, UNS_32 size)
{
	UNS_32 i = 0;
	for (i = 0; i < size; i += 2) {
		lpc_norflash_write_word(addr + i, *data);
		while (!lpc_norflash_toggle_bit_check(addr + i)) {}
		data++;
	}
	return i;
}

/* Read 16-bit data from flash */
UNS_16 lpc_norflash_read_word(UNS_32 addr)
{
	return Board_NorFlash_ReadWord(addr);
}

