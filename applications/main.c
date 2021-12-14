#include "board.h"
#include "mem_tests.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define DRAM_BASE_ADDRESS (uint32_t *)EMC_ADDRESS_DYCS0
#define DRAM_SIZE (8 * 1024 * 1024)

/* Read/write buffer (32-bit aligned) */
uint32_t buffer[64 / sizeof(uint32_t)];

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/**
 * @brief	Main program body
 * @return	Does not return
 */
int main(void)
{
    uint32_t addr = 0;
    uint32_t count = 0;
    lpc_nandflash_id_t nandId;
    uint16_t *buffer_ptr = (uint16_t *)buffer;

    SystemCoreClockUpdate();
    Board_Init();

    MEM_TEST_SETUP_T memSetup;

    /* Walking 0 test */
    memSetup.start_addr = DRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;
    count += mem_test_walking0(&memSetup);

    /* Walking 1 test */
    memSetup.start_addr = DRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;
    count += mem_test_walking1(&memSetup);

    /* Address test */
    memSetup.start_addr = DRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;
    count += mem_test_address(&memSetup);

    /* Inverse address test */
    memSetup.start_addr = DRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;
    count += mem_test_invaddress(&memSetup);

    /* Pattern test */
    memSetup.start_addr = DRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;
    count += mem_test_pattern(&memSetup);

    /* Seeded pattern test */
    memSetup.start_addr = DRAM_BASE_ADDRESS;
    memSetup.bytes = DRAM_SIZE;
    count += mem_test_pattern_seed(&memSetup, 0x12345678, 0x50005);

    if (count != 6)
    {
        /* SDRAM test failed, please check the timing. */
        for (;;)
        {
        }
    }

    /* Read data */
    addr = lpc_norflash_get_sector_offset(8);
    /* Erase flash */
    lpc_norflash_erase_sector(addr);
    /* Wait TSE (~25ms) */
    for (count = 0; count < 25 * 40000; count++)
    {
    }
    /* Fill data */
    for (count = 0; count < 32; count++)
        buffer_ptr[count] = count | (~count << 8);
    /* Write data to page */
    lpc_norflash_write_buffer(addr, (uint16_t *)buffer, 32);
    /* Read all data from flash */
    for (count = 0; count < 16; count++)
    {
        if (buffer_ptr[count] != lpc_norflash_read_word(addr + (count * 2)))
        {
            /* NOR test failed, please check the timing. */
            for (;;)
            {
            }
        }
    }

    lpc_nandflash_init();
    lpc_nandflash_get_id((uint8_t *)&nandId);
    if (nandId.MarkerCode != 0xEC && nandId.DeviceCode != 0xF1)
    {
        /* NAND test failed, please check the timing. */
        for (;;)
        {
        }
    }

    lpc_nandflash_erase_block(0);
    Board_NANDFLash_WaitReady();

    /* Check the result of erasing */
    if (lpc_nandflash_read_status() & NANDFLASH_STATUS_BLOCK_ERASE_FAIL)
    {
        /* NAND test failed, please check the IC. */
        for (;;)
        {
        }
    }

    for (count = 0; count < 16; count++)
        buffer[count] = count;

    /* Write data to page */
    lpc_nandflash_write_page(0, 0, (uint8_t *)buffer, 64);
    Board_NANDFLash_WaitReady();

    /* Check the result of writting */
    if (lpc_nandflash_read_status() & NANDFLASH_STATUS_PAGE_PROG_FAIL)
    {
        /* NAND test failed, please check the IC. */
        for (;;)
        {
        }
    }

    for (count = 0; count < 16; count++)
        buffer[count] = 0;

    /* Read all data from flash */
    lpc_nandflash_read_start(0, 0, 0);
    Board_NANDFLash_WaitReady();
    lpc_nandflash_read_data((uint8_t *)buffer, 64);

    for (count = 0; count < 16; count++)
    {
        if (buffer[count] != count)
        {
            /* NAND test failed, please check the IC. */
            for (;;)
            {
            }
        }
    }

    for (;;)
    {
    }
}
