/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "iap.h"
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = my_system.baud;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */


#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
		void _sys_exit(int x) 
		{ 
			x = x; 
		} 

//重定义fputc函数 
int fputc(int ch, FILE *f)
{      

	uint8_t temp[1];
	temp[0]=ch;
	HAL_UART_Transmit(&huart1,temp,1,2);
	return ch;
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(huart->Instance==USART1)
	{
		rx1_status++;		
		if(0==my_system.mode)	//指令模式	
		{			// adjust queue statusf
			if (*push_rx1 == 10)     //判断是不是换行符/n
			{
				 if( 13 == (*(push_rx1-1)) )//继续判断前一个接收到的字符是不是回车符/r
				 {
					*(push_rx1+1) = '\0';   //字符串末尾补结束符\0
		 
					gotCommand = 1;      //确认接收到完整命令以便usart1_Analysis()处理
					push_rx1 = rxbuf_UART1;    //接收指针归位起始位置
					rx1_status=0;
				 }
			}
			else
			{
					if (++push_rx1 >= (rxbuf_UART1+RX1BUFSIZE))	// handle buffer wraparound   数据溢出处理
					{
						push_rx1 = rxbuf_UART1;
						rx1_status=0;
					}
			}
			if('\0'==rxbuf_UART1[0])
			{
				 push_rx1 = rxbuf_UART1;
				rx1_status=0;
			}
		}
		else	//IAP模式
		{
			my_system.counter=0;
			switch(rxbuf_UART1[0])
			{
				case SOH:
					 if(rx1_status==(128+5))
					{
						gotData=1;//确认收到一包数据包
						push_rx1++;
						
					}
					else if(rx1_status<(128+5))
					{
						
						push_rx1++;
					}
					break;
				case STX:
					if(rx1_status==(1024+5))
					{
						gotData=1;//确认收到一包数据包
						push_rx1++;
					
					}
					else if(rx1_status<(1024+5))
					{
						
						push_rx1++;
					}
					break;
				case EOT:
					push_rx1++;//只管接收数据
					break;
				default:
					push_rx1 = rxbuf_UART1;
					rx1_status=0;
					break;
			
			}
		
		}
	
	}
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
