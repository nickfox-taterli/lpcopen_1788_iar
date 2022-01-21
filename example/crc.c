#include "board.h"

static void App_CRC_Test(void)
{
    uint8_t BlockData[8] = {0xAA,0x55,0xA5,0x5A,0x00,0xFF,0xF0,0x0F};
    uint32_t result;      
    
    Chip_CRC_Init();
    
    result = Chip_CRC_CRC8((const uint8_t *) BlockData, 8);
    if (result != 0x6963) {
       for(;;);
    }
    
    result = Chip_CRC_CRC16((const uint16_t *) BlockData, 4);
    if (result != 0xcefb) {
       for(;;);
    }
    
    result = Chip_CRC_CRC32((const uint32_t *) BlockData, 2);
    if (result != 0x2eab7dcb) {
       for(;;);
    } 
}

/**
 * @brief	Main program body
 * @return	Does not return
 */
int main(void)
{    
    SystemCoreClockUpdate();
    Board_Init();
                
    for (;;)
    {
          App_CRC_Test();
    }
}
