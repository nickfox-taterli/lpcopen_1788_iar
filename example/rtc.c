#include "board.h"

/**
 * @brief	RTC interrupt handler
 * @return	Nothing
 */
void RTC_IRQHandler(void)
{
	RTC_EV_TIMESTAMP_T timestamp;
	uint8_t i = 0;

	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE)) {
              /* Clear pending interrupt */
              Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);

              /* display timestamp */
              Chip_RTC_GetTime(LPC_RTC, RTC_TIMETYPE_SECOND);
	}

	for (i = 0; i < RTC_EV_CHANNEL_NUM; i++) {
              if (Chip_RTC_EV_GetChannelStatus(LPC_RTC, (RTC_EV_CHANNEL_T) i)) {
                    // 单步观察
                    Chip_RTC_EV_GetFirstTimeStamp(LPC_RTC, (RTC_EV_CHANNEL_T) i, &timestamp);
                    Chip_RTC_EV_GetLastTimeStamp(LPC_RTC, (RTC_EV_CHANNEL_T) i, &timestamp);
                    Chip_RTC_EV_GetCounter(LPC_RTC, (RTC_EV_CHANNEL_T) i);
                    Chip_RTC_EV_ClearChannelStatus(LPC_RTC, (RTC_EV_CHANNEL_T) i);
              }
	}

	if (Chip_RTC_EV_GetStatus(LPC_RTC)) {
              Chip_RTC_EV_ClearStatus(LPC_RTC, Chip_RTC_EV_GetStatus(LPC_RTC));
	}
        
        /* Check for alarm match */
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM)) {
              /* Clear pending interrupt */
              Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM);
	}

}

/* Pin mux for Event Monitor/Recorder */
void Board_RTC_EV_Init(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 7, (IOCON_FUNC4 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8, (IOCON_FUNC4 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, (IOCON_FUNC4 | IOCON_MODE_INACT));
}

/**
 * @brief	Main entry point
 * @return	Nothing
 */
int main(void)
{
      RTC_TIME_T FullTime;

      SystemCoreClockUpdate();
      Board_Init();

      Board_RTC_EV_Init();

      Chip_RTC_Init(LPC_RTC);

      /* Set current time for RTC 2:00:00PM, 2012-10-05 */
      FullTime.time[RTC_TIMETYPE_SECOND]  = 0;
      FullTime.time[RTC_TIMETYPE_MINUTE]  = 0;
      FullTime.time[RTC_TIMETYPE_HOUR]    = 14;
      FullTime.time[RTC_TIMETYPE_DAYOFMONTH]  = 5;
      FullTime.time[RTC_TIMETYPE_DAYOFWEEK]   = 5;
      FullTime.time[RTC_TIMETYPE_DAYOFYEAR]   = 279;
      FullTime.time[RTC_TIMETYPE_MONTH]   = 10;
      FullTime.time[RTC_TIMETYPE_YEAR]    = 2012;

      Chip_RTC_SetFullTime(LPC_RTC, &FullTime);

      /* Set ALARM time for 17 seconds from time */
      FullTime.time[RTC_TIMETYPE_SECOND]  = 17;
      Chip_RTC_SetFullAlarmTime(LPC_RTC, &FullTime);
      
      /* Set the RTC to generate an interrupt on each second */
      Chip_RTC_CntIncrIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, ENABLE);
      
      /* Enable matching for alarm for second, minute, hour fields only */
      Chip_RTC_AlarmIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC | RTC_AMR_CIIR_IMMIN | RTC_AMR_CIIR_IMHOUR, ENABLE);

      /* Clear interrupt pending */
      Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);

      /* Enable RTC (starts increase the tick counter and second counter register) */
      Chip_RTC_Enable(LPC_RTC, ENABLE);

      /* Initialize Event Monitor/recorder */
      Chip_RTC_EV_Config(LPC_RTC, RTC_EV_CHANNEL_1, RTC_ERCTRL_POL_POSITIVE | RTC_ERCTRL_INPUT_EN);	/* Event on Negative edge */
      Chip_RTC_EV_Config(LPC_RTC, RTC_EV_CHANNEL_2, RTC_ERCTRL_POL_POSITIVE);	/* Event on Positive edge */
      Chip_RTC_EV_Config(LPC_RTC, RTC_EV_CHANNEL_3, RTC_ERCTRL_POL_NEGATIVE | RTC_ERCTRL_INPUT_EN);/* Event on Negative edge */

      /* Enable Event Monitor/Recorder */
      Chip_RTC_EV_SetMode(LPC_RTC, RTC_EV_MODE_ENABLE_64HZ);

      /* Enable RTC interrupt in NVIC */
      NVIC_EnableIRQ((IRQn_Type) RTC_IRQn);

      /* Loop forever */
      while (1) {
          /* read and display time */
          Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
      }
}
