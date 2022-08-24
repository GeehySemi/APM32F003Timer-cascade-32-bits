/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.1
 *
 * @date        2022-04-11
 *
 * @attention
 *
 *  Copyright (C) 2018-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#include "main.h"
#include "apm32f00x_tmr1.h"
#include "apm32f00x_tmr2.h"
#include "apm32f00x_usart.h"
#include "stdio.h"


/** TMR2 Init */
void TMR2Init(void);
/** TMR1 Init */
void TMR1Init(void);
/* USART Init */
void USARTInit(void);

 /*!
 * @brief       Redirect C Library function printf to serial port.
 *              After Redirection, you can use printf function.
 *
 * @param       ch:  The characters that need to be send.
 *
 * @param       *f:  pointer to a FILE that can recording all information
 *              needed to control a stream
 *
 * @retval      The characters that need to be send.
 *
 * @note
 */

int fputc(int ch, FILE *f)
{
   while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);//等待先前的字符发送完成   
   USART_TxData8(USART1, (uint8_t) ch);    //发送字符
   return ch;
}


/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
int main(void)
{
		USARTInit();  
		TMR2Init();					// TMR2 is Master and needs to be before TMR1 
		TMR1Init();
		uint32_t temp = 0;
    while(1)
    {
			temp = TMR1_ReadCounter(TMR1);
			temp <<= 16;
			temp |= TMR2_ReadCounter();
      printf("Count Value: 0x%8x\r\n",temp);
    }
}

/*!
 * @brief       USART Init   
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void USARTInit(void)
{
    USART_Config_T usartConfig;
    USART_SyncClockConfig_T usartSyncClockConfig;
    
    /**  Enable USART Clock  */
    usartSyncClockConfig.enable = USART_CLKEN_ENABLE;
    /**  Enable last bit clock  */
    usartSyncClockConfig.lastBitClock = USART_LBCP_ENABLE;
    /**  Clock phase is 2 edge */
    usartSyncClockConfig.phase = USART_CLKPHA_2EDGE;
    /**  Clock polarity is high */
    usartSyncClockConfig.polarity = USART_CLKPOL_HIGH;
    /**  Synchronous communication clock configuration */
    USART_ConfigSyncClock(USART1, &usartSyncClockConfig);
    
    /**  BaudRate is 115200 */
    usartConfig.baudRate = 115200;
    /**  No interrupt */
    usartConfig.interrupt = USART_INT_NONE;
    /**  Enable RX and TX */
    usartConfig.mode = USART_MODE_TX_RX;
    /**  Parity disable */
    usartConfig.parity = USART_PARITY_NONE;
    /**  One stop bit */
    usartConfig.stopBits = USART_STOP_BIT_1;
    /**  Word length is 8bit */
    usartConfig.wordLength = USART_WORD_LEN_8B;
    /**  USART1 configuration */
    USART_Config(USART1, &usartConfig);
    
    /**  Enable USART1 */
    USART_Enable(USART1);
}

/*!
 * @brief       TMR1 Init
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void TMR1Init(void)
{
    TMR1_TimeBaseConfig_T timeBaseConfig;

    /** Up-counter */
    timeBaseConfig.cntMode = TMR1_CNT_MODE_UP;
    /** Set divider */
    timeBaseConfig.divider = 0;
    /** Repetition counter = 0x0 */
    timeBaseConfig.repetitionCount = 0X0;
		/** specifies the counter */
	  timeBaseConfig.count = 0xFFFF;
    TMR1_ConfigTimerBase(TMR1, &timeBaseConfig);
    /** The update event is selected as trigger Input */
    TMR1_ConfigInputTrigger(TMR1,TMR1_ITC_TMR2);
		/**  Set TMR1 Slave mode  */
    TMR1_ConfigSlaveMode(TMR1,TMR1_SLAVE_MODE_EXTERNAL1);
    /**  Enable TMR1  */
    TMR1_Enable(TMR1);
}

/*!
 * @brief       TMR2 Init
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void TMR2Init(void)
{	
	  /**  Divider = 0xFF, counter = 0xFFFF  */
    TMR2_ConfigTimerBase(0xFF, 0XFFFF);
		/**  Master mode Configuration  */
    TMR2_ConfigMasterMode(TMR2_MASTER_MODE_UPDATE);
	  /**  Enable TMR2 Master mode  */
    TMR2_EnableMasterMode();	
	  /**  Enable TMR2  */
    TMR2_Enable();	
}
