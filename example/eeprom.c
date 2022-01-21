#include "board.h"

static void App_EEPROM_Test(void){
    uint32_t buffer;
  
    /* Init EEPROM */
    Chip_EEPROM_Init(LPC_EEPROM);

    /* Read data from EEPROM page */
    Chip_EEPROM_Read(LPC_EEPROM, 0, 0, (uint8_t *) &buffer, EEPROM_RWSIZE_8BITS, 4);

    /* Erase page */
    Chip_EEPROM_Erase(LPC_EEPROM, 0);
    
    buffer++;

    /* Write data to page */
    Chip_EEPROM_Write(LPC_EEPROM, 0, 0, (uint8_t *) &buffer, EEPROM_RWSIZE_8BITS, 4);
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
          App_EEPROM_Test();
    }
}
