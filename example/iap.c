#include "board.h"
#include <stdio.h>

#define TICKRATE_HZ (10)	/* 10 ticks per second */

/* Last sector address */
#define START_ADDR_LAST_SECTOR  0x00078000

/* Size of each sector */
#define SECTOR_SIZE             1024

/* LAST SECTOR */
#define IAP_LAST_SECTOR         29

/* Number of bytes to be written to the last sector */
#define IAP_NUM_BYTES_TO_WRITE  256

/* Number elements in array */
#define ARRAY_ELEMENTS          (IAP_NUM_BYTES_TO_WRITE / sizeof(uint32_t))

/* Data array to write to flash */
static uint32_t src_iap_array_data[ARRAY_ELEMENTS];

/**
 * @brief	Main program body
 * @return	Always returns 0
 */
int main(void)
{
    int i;
    uint8_t ret_code;
    uint32_t part_id;

    /* Generic Initialization */
    SystemCoreClockUpdate();
    Board_Init();

    /* Initialize the array data to be written to FLASH */
    for (i = 0; i < ARRAY_ELEMENTS; i++) {
      src_iap_array_data[i] = 0x11223340 + i;
    }

    /* Read Part Identification Number*/
    part_id = Chip_IAP_ReadPID();
    if(part_id != 0x281d3f47){
      for(;;);
    }

    /* Disable interrupt mode so it doesn't fire during FLASH updates */
    __disable_irq();

    /* IAP Flash programming */
    /* Prepare to write/erase the last sector */
    ret_code = Chip_IAP_PreSectorForReadWrite(IAP_LAST_SECTOR, IAP_LAST_SECTOR);

    /* Error checking */
    if (ret_code != IAP_CMD_SUCCESS) {
      for(;;);
    }

    /* Erase the last sector */
    ret_code = Chip_IAP_EraseSector(IAP_LAST_SECTOR, IAP_LAST_SECTOR);

    /* Error checking */
    if (ret_code != IAP_CMD_SUCCESS) {
      for(;;);
    }

    /* Prepare to write/erase the last sector */
    ret_code = Chip_IAP_PreSectorForReadWrite(IAP_LAST_SECTOR, IAP_LAST_SECTOR);

    /* Error checking */
    if (ret_code != IAP_CMD_SUCCESS) {
      for(;;);
    }

    /* Write to the last sector */
    ret_code = Chip_IAP_CopyRamToFlash(START_ADDR_LAST_SECTOR, src_iap_array_data, IAP_NUM_BYTES_TO_WRITE);

    /* Error checking */
    if (ret_code != IAP_CMD_SUCCESS) {
      for(;;);
    }

    /* Re-enable interrupt mode */
    __enable_irq();

    /* Start the signature generator for the last sector */
    Chip_FMC_ComputeSignatureBlocks(START_ADDR_LAST_SECTOR, (SECTOR_SIZE / 16));

    /* Check for signature geenration completion */
    while (Chip_FMC_IsSignatureBusy()) {}

    while (1) {
      __WFI();
    }
}
