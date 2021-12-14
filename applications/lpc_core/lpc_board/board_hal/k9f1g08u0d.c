#include "board.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Get column address of a page in a block */
#define COLUMN_ADDR(block, page)      (block *  (64) /* Pages per block */ + page)

/* NAND information */
static const lpc_nandflash_size_t nandSize = {
	(2 << 10) /* Page size */,  (2 << 10) /* Spare start address */,  (64) /* Pages per block */,  (1024) /* The number of blocks */
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
/* Initialize flash */
void lpc_nandflash_init(void)
{}

/* De-initialize flash */
void lpc_nandflash_DeInit(void)
{}

/* Get the flash size */
const lpc_nandflash_size_t *lpc_nandflash_get_size(void)
{
	return &nandSize;
}

/* Get flash ID */
void lpc_nandflash_get_id(uint8_t *pData)
{
	uint8_t i = 0;
#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(true);
#endif
	Board_NANDFLash_WriteCmd(0x90);
	Board_NANDFLash_WriteAddr(0x00);

	for (i = 0; i < sizeof(lpc_nandflash_id_t); i++) {
		*pData = Board_NANDFLash_ReadByte();
		pData++;
	}

#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(false);
#endif
}

/* Read status */
uint8_t lpc_nandflash_read_status(void)
{
	uint8_t data;

#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(true);
#endif
	Board_NANDFLash_WriteCmd(0x70);
	data = Board_NANDFLash_ReadByte();
#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(false);
#endif

	return data;
}

/* Erase block */
void lpc_nandflash_erase_block(uint32_t block)
{
	uint32_t row = COLUMN_ADDR(block, 0);

#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(true);
#endif
	Board_NANDFLash_WriteCmd(0x60);

	/* Write address */
	Board_NANDFLash_WriteAddr(row & 0xFF);
	Board_NANDFLash_WriteAddr((row >> 8) & 0xFF);

	Board_NANDFLash_WriteCmd(0xD0);
#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(false);
#endif
}

/* Write buffer to flash */
uint32_t lpc_nandflash_write_page(uint32_t block, uint32_t page, uint8_t *data, uint32_t size)
{
	uint32_t i = 0;
	uint32_t row = COLUMN_ADDR(block, page);

#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(true);
#endif
	Board_NANDFLash_WriteCmd(0x80);

	/* Write address*/
	Board_NANDFLash_WriteAddr(0x00);
	Board_NANDFLash_WriteAddr(0x00);
	Board_NANDFLash_WriteAddr(row & 0xFF);
	Board_NANDFLash_WriteAddr((row >> 8) & 0xFF);

	/*Write data */
	for (i = 0; i < size; i++) {
		Board_NANDFLash_WriteByte(*data);
		data++;
	}

	Board_NANDFLash_WriteCmd(0x10);
#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(false);
#endif
	return i;
}

/* Start reading data from flash */
void lpc_nandflash_read_start(uint32_t block, uint32_t page, uint32_t ofs)
{
	uint32_t row = COLUMN_ADDR(block, page);

#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(true);
#endif
	Board_NANDFLash_WriteCmd(0x00);

	/* Write address */
	Board_NANDFLash_WriteAddr(ofs & 0xFF);
	Board_NANDFLash_WriteAddr((ofs >> 8) & 0xFF);
	Board_NANDFLash_WriteAddr(row & 0xFF);
	Board_NANDFLash_WriteAddr((row >> 8) & 0xFF);

	Board_NANDFLash_WriteCmd(0x30);
#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(false);
#endif
}

/* Read data from flash */
void lpc_nandflash_read_data(uint8_t *data, uint32_t size)
{
	uint32_t i = 0;

#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(true);
#endif
	for (i = 0; i < size; i++) {
		*data = Board_NANDFLash_ReadByte();
		data++;
	}
#if defined(BOARD_NAND_LOCKEDCS)
	Board_NANDFLash_CSLatch(false);
#endif
}

