#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"

#include "lpc_norflash.h"
#include "lpc_nandflash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NANDFLASH_READY_PORT       2
#define NANDFLASH_READY_PIN        21

/**
 * @brief	Write on address lines and data lines which are being connected to NOR Flash
 * @param	addr	: value which will be placed on address lines
 * @param	data	: value which will be placed on data lines
 * @return	Nothing
 */
STATIC INLINE void Board_NorFlash_WriteCmd(uint32_t addr, uint16_t data)
{
	*((volatile uint16_t *) (EMC_ADDRESS_CS0 | (addr << 1))) = data;
}

/**
 * @brief	Read command data returned by NOR Flash
 * @param	addr	: value which will be placed on address lines
 * @return	Data returned by NOR Flash
 */
STATIC INLINE uint16_t Board_NorFlash_ReadCmdData(uint32_t addr)
{
	return *((volatile uint16_t *) (EMC_ADDRESS_CS0 | (addr << 1)));
}

/**
 * @brief	Write 16-bit data to NOR Flash
 * @param	addr	: Offset in NOR Flash
 * @param	data	: Data which will be written to NOR Flash
 * @return	Nothing
 */
STATIC INLINE void Board_NorFlash_WriteWord(uint32_t addr, uint16_t data)
{
	*((volatile uint16_t *) (EMC_ADDRESS_CS0 | addr)) = data;
}

/**
 * @brief	Read 16-bit data from NOR Flash
 * @param	addr	: Offset in NOR Flash
 * @return	Nothing
 */
STATIC INLINE uint16_t Board_NorFlash_ReadWord(uint32_t addr)
{
	return *((volatile uint16_t *) (EMC_ADDRESS_CS0 | addr));
}

/**
 * @brief	Poll NAND Ready/Busy signal
 * @return	true if ready, false if busy
 * @note	Polls the R/nB signal and returns the state
 */
STATIC INLINE bool Board_NANDFLash_GetReady(void)
{
	return (Chip_GPIO_ReadPortBit(LPC_GPIO, NANDFLASH_READY_PORT, NANDFLASH_READY_PIN)) ? true : false;
}

/**
 * @brief	Write a command to NAND Flash
 * @param	cmd	: Command byte
 * @return	Nothing
 */
STATIC INLINE void Board_NANDFLash_WriteCmd(uint8_t cmd)
{
	*((volatile uint8_t *) (EMC_ADDRESS_CS1 | (1 << 25))) = cmd;
}

/**
 * @brief	Write a address byte to NAND Flash
 * @param	addr	: Address byte
 * @return	Nothing
 */
STATIC INLINE void Board_NANDFLash_WriteAddr(uint8_t addr)
{
	*((volatile uint8_t *) (EMC_ADDRESS_CS1 | (1 << 24))) = addr;
}

/**
 * @brief	Write a byte to NAND Flash
 * @param	data	: Data byte
 * @return	Nothing
 */
STATIC INLINE void Board_NANDFLash_WriteByte(uint8_t data)
{
	*((volatile uint8_t *) (EMC_ADDRESS_CS1)) = data;
}

/**
 * @brief	Read data byte from Nand Flash
 * @return	Byte read from NAND FLASH
 */
STATIC INLINE uint8_t Board_NANDFLash_ReadByte(void)
{
	return *((volatile uint8_t *) (EMC_ADDRESS_CS1));
}

STATIC void Board_NANDFLash_WaitReady(void)
{
    if(Board_NANDFLash_GetReady()) {
        while(Board_NANDFLash_GetReady()){}
    }
    while(!Board_NANDFLash_GetReady()){}
}

#include "board_api.h"

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __BOARD_H_ */
