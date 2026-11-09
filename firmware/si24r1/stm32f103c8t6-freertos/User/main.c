#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "SI24R1/SI24R1.h"
#include "FreeRTOS.h"                  // FreeRTOS 内核头文件
#include "task.h"                      // Task 头文件

#include <string.h>
#include <stdio.h>

//==============================================================================
// USART1 串口（PA9=TXD, PA10=RXD）: 115200-8-N-1, printf 重定向
//==============================================================================
void USART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // PA9 = TXD 复用推挽
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PA10 = RXD 浮空输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = 115200;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

// printf 重定向到 USART1
int fputc(int ch, FILE *f)
{
    (void)f;
    USART_SendData(USART1, (uint8_t)ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return ch;
}

//==============================================================================
// 任务句柄
//==============================================================================
TaskHandle_t xHeartbeatTaskHandler = NULL;
TaskHandle_t xRadioTaskHandler     = NULL;

//==============================================================================
// 心跳任务：每 500ms 翻转 LED1，用于指示 FreeRTOS 调度器正常运行
//==============================================================================
void vHeartbeatTask(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        LED1_Turn();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

//==============================================================================
// 无线任务：根据 SI24R1_ROLE 决定发送或接收
//   - TX 角色：每隔 1s 发送一包递增计数数据，发送成功翻转 LED2
//   - RX 角色：轮询接收，收到数据翻转 LED2
// 两块板需设为相反角色（见 SI24R1.h 中的 SI24R1_ROLE 配置）
//==============================================================================
void vRadioTask(void *pvParameters)
{
    (void)pvParameters;

    uint8_t txbuf[TX_PLOAD_WIDTH];
    uint8_t rxbuf[TX_PLOAD_WIDTH];
    uint32_t counter = 0;

    SI24R1_Init();
    printf("================================");
    printf(" STM32F103C8T6 + FreeRTOS + SI24R1");
#if (SI24R1_ROLE == SI24R1_ROLE_TX)
    printf(" Role = TX");
    printf("================================");
    SI24R1_TX_Mode();
    printf(">> TX mode ready, sending...");
    for (;;)
    {
        counter++;
        memset(txbuf, 0, TX_PLOAD_WIDTH);
        txbuf[0] = (uint8_t)(counter & 0xFF);
        txbuf[1] = (uint8_t)((counter >> 8) & 0xFF);
        txbuf[2] = (uint8_t)((counter >> 16) & 0xFF);
        txbuf[3] = (uint8_t)((counter >> 24) & 0xFF);

        uint8_t sta = SI24R1_TxPacket(txbuf);
        if (sta == TX_DS)
        {
            LED2_Turn();   // 发送成功指示
            printf("TX ok [%08X]", (unsigned int)counter);
        }
        else if (sta == MAX_RT)
        {
            printf("TX max_retry (no ACK)");
        }
        else
        {
            printf("TX fail (0x%02X)", sta);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));   // 每秒发送一次
    }
#else
    printf(" Role = RX");
    printf("================================");
    SI24R1_RX_Mode();
    printf(">> RX mode ready, waiting...");
    for (;;)
    {
        if (SI24R1_RxPacket(rxbuf) == 0)   // 收到一包数据
        {
            LED2_Turn();   // 接收指示
            printf("RX: %02X %02X %02X %02X %02X %02X %02X %02X ...",
                   rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3],
                   rxbuf[4], rxbuf[5], rxbuf[6], rxbuf[7]);
        }
        vTaskDelay(pdMS_TO_TICKS(10));     // 轮询间隔
    }
#endif
}

//==============================================================================
// 主函数
//==============================================================================
int main(void)
{
    LED_Init();
    USART1_Init();

    // 创建心跳任务（低优先级）
    xTaskCreate(vHeartbeatTask, "Heartbeat", 128, NULL, 1, &xHeartbeatTaskHandler);

    // 创建无线任务（较高优先级）
    xTaskCreate(vRadioTask, "Radio", 256, NULL, 2, &xRadioTaskHandler);

    // 启动调度器，之后由任务接管
    vTaskStartScheduler();

    while (1)
    {
        // 调度器启动失败才会到这里
    }
}
