#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include <string.h>
#include <uart.h>
#define DBG_TAG "uart"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
UART_HandleTypeDef huart2,huart1;
uint8_t lcd_rx_buf[64];
uint8_t rx485_rx_buf[64];
rt_mq_t uart2_rx_mq,uart1_rx_mq;
uart2msg msg,msg_uart1;
rt_err_t  s;
void MX_USART2_UART_Init(void)
{

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {

    }
    __HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);//接收中断
    __HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);//空闲中断
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&lcd_rx_buf, 64);
    uart2_rx_mq = rt_mq_create("uart_mq", 70, 48, RT_IPC_FLAG_FIFO);
}

void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {


    }
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);//接收中断
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);//空闲中断
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx485_rx_buf, 64);
    uart1_rx_mq = rt_mq_create("uart1_mq", 70, 48, RT_IPC_FLAG_FIFO);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
      GPIO_InitTypeDef GPIO_InitStruct = {0};
          if(huart->Instance==USART2)
          {
          /* USER CODE BEGIN USART2_MspInit 0 */

          /* USER CODE END USART2_MspInit 0 */
            /* USART2 clock enable */
            __HAL_RCC_USART2_CLK_ENABLE();

            __HAL_RCC_GPIOA_CLK_ENABLE();
            /**USART2 GPIO Configuration
            PA2     ------> USART2_TX
            PA3     ------> USART2_RX
            */
            GPIO_InitStruct.Pin = GPIO_PIN_2;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = GPIO_PIN_3;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            /* USART2 interrupt Init */
            HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(USART2_IRQn);
          /* USER CODE BEGIN USART2_MspInit 1 */

          /* USER CODE END USART2_MspInit 1 */
          }
          if(huart->Instance==USART1)
        {
        /* USER CODE BEGIN USART2_MspInit 0 */

        /* USER CODE END USART2_MspInit 0 */
          /* USART2 clock enable */
          __HAL_RCC_USART1_CLK_ENABLE();

          __HAL_RCC_GPIOA_CLK_ENABLE();
          /**USART2 GPIO Configuration
          PA2     ------> USART2_TX
          PA3     ------> USART2_RX
          */
          GPIO_InitStruct.Pin = GPIO_PIN_9;
          GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
          HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

          GPIO_InitStruct.Pin = GPIO_PIN_10;
          GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
          GPIO_InitStruct.Pull = GPIO_NOPULL;
          HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

          /* USART2 interrupt Init */
          HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
          HAL_NVIC_EnableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART2_MspInit 1 */

        /* USER CODE END USART2_MspInit 1 */
        }
}


void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);

    if(RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))
    {
       __HAL_UART_CLEAR_IDLEFLAG(&huart2);

       HAL_UART_RxIdleCpltCallback(&huart2);
    }
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);

    if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
    {
       __HAL_UART_CLEAR_IDLEFLAG(&huart1);

       HAL_UART_RxIdleCpltCallback(&huart1);
    }
}
void HAL_UART_RxIdleCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        memcpy(msg.send_uart,lcd_rx_buf,(huart->RxXferSize)-(huart->RxXferCount));
        msg.len = (huart->RxXferSize)-(huart->RxXferCount);
        //LOG_D("msg.len == %d",msg.len);
        rt_mq_send(uart2_rx_mq, &msg,sizeof(msg));
        rt_memset(&lcd_rx_buf, 0, sizeof(lcd_rx_buf));
        huart->RxXferCount = huart->RxXferSize;
        huart->pRxBuffPtr = &lcd_rx_buf;
        //LOG_D("msg.len == %d",msg.len);
    }

    if (huart->Instance == USART1)
    {
        memcpy(msg_uart1.send_uart,rx485_rx_buf,(huart->RxXferSize)-(huart->RxXferCount));
        msg_uart1.len = (huart->RxXferSize)-(huart->RxXferCount);
        //LOG_D("msg.len == %d",msg.len);
        rt_mq_send(uart1_rx_mq, &msg_uart1,sizeof(msg_uart1));
        rt_memset(&rx485_rx_buf, 0, sizeof(rx485_rx_buf));
        huart->RxXferCount = huart->RxXferSize;
        huart->pRxBuffPtr = &rx485_rx_buf;
        //LOG_D("msg.len == %d",msg.len);
    }
}
