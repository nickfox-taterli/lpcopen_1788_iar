#include "board.h"

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
static volatile uint8_t channelTC, dmaChannelNum;
uint32_t DMAbuffer[255];
uint16_t dataADC;

/**
 * @brief	DMA interrupt handler sub-routine
 * @return	Nothing
 */
void DMA_IRQHandler(void)
{
    if (Chip_GPDMA_Interrupt(LPC_GPDMA, dmaChannelNum) == SUCCESS) {
      channelTC++;
    }
    else {
            /* Process error here */
    }
}

/* DMA routine for ADC example */
static void App_DMA_Test(void)
{
    static ADC_CLOCK_SETUP_T ADCSetup;

    Chip_IOCON_PinMux(LPC_IOCON,0, 25, IOCON_ADMODE_EN, IOCON_FUNC1);
    Chip_ADC_Init(LPC_ADC, &ADCSetup);

    ADCSetup.burstMode = true;
    Chip_ADC_SetSampleRate(LPC_ADC, &ADCSetup, 8000);
    Chip_ADC_EnableChannel(LPC_ADC, ADC_CH2, ENABLE);

    /* Initialize GPDMA controller */
    Chip_GPDMA_Init(LPC_GPDMA);
    /* Setting GPDMA interrupt */
    NVIC_DisableIRQ(DMA_IRQn);
    NVIC_SetPriority(DMA_IRQn, ((0x01 << 3) | 0x01));
    NVIC_EnableIRQ(DMA_IRQn);
    /* Setting ADC interrupt, ADC Interrupt must be disable in DMA mode */
    NVIC_DisableIRQ(ADC_IRQn);
    Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH2, ENABLE);
    /* Get the free channel for DMA transfer */
    dmaChannelNum = Chip_GPDMA_GetFreeChannel(LPC_GPDMA, GPDMA_CONN_ADC);
    /* Enable burst mode if any, the AD converter does repeated conversions
       at the rate selected by the CLKS field in burst mode automatically */
    Chip_ADC_SetBurstCmd(LPC_ADC, ENABLE);
    /* Get  adc value until get 'x' character */

    /* Start A/D conversion if not using burst mode */
    // Chip_ADC_SetStartMode(LPC_ADC, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
    
    channelTC = 0;
    Chip_GPDMA_Transfer(LPC_GPDMA, dmaChannelNum,
                                      GPDMA_CONN_ADC,
                                      (uint32_t) &DMAbuffer[0],
                                      GPDMA_TRANSFERTYPE_P2M_CONTROLLER_DMA,
                                      256);

    /* Waiting for reading ADC value completed */
    while (channelTC == 0) {}

    /* Get the ADC value fron Data register*/
    dataADC = 0;
    for(uint8_t i = 0;i < 255;i++)
      dataADC += ADC_DR_RESULT(DMAbuffer[i]);
    
    dataADC = dataADC / 255;
    
    /* Disable interrupts, release DMA channel */
    Chip_GPDMA_Stop(LPC_GPDMA, dmaChannelNum);
    NVIC_DisableIRQ(DMA_IRQn);
    /* Disable burst mode if any */
    Chip_ADC_SetBurstCmd(LPC_ADC, DISABLE);
    
    Chip_ADC_DeInit(LPC_ADC);
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
          App_DMA_Test();
    }
}
