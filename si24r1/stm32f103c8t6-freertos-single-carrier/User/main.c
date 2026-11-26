#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "SI24R1/SI24R1.h"
#include "FreeRTOS.h"                  // FreeRTOS 内核头文件
#include "task.h"                      // Task 头文件

#include <string.h>
#include <stdio.h>

//==============================================================================
// 单载波（CW）连续发射固件 —— 由 TX/RX demo 工程复制改造
//   上电后 SI24R1 进入 CW 模式（2475MHz / 2Mbps / +7dBm / 连续载波），
//   CE 常高，持续发射未调制载波，用于频偏/功率/频谱/杂散测试。
//   FreeRTOS 心跳任务照常运行（LED1 500ms 翻转指示调度器存活）。
//==============================================================================

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

// USART1 单字节发送（供 newlib _write / fputc 共用）
void USART1_Putc(uint8_t ch)
{
    USART_SendData(USART1, ch);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

// printf 重定向到 USART1
int fputc(int ch, FILE *f)
{
    (void)f;
    USART1_Putc((uint8_t)ch);
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
// 无线任务：上电进入单载波（CW）连续发射，永不返回。
//   - 初始化 SI24R1 -> 进 CW 模式 -> CE 常高持续发射
//   - 参数与 AI8051U 单载波工程一致：2475MHz / 2Mbps / +7dBm
//   - 串口打印一次性启动信息，之后 LED1 心跳由独立任务维持
//==============================================================================
void vRadioTask(void *pvParameters)
{
    (void)pvParameters;

    SI24R1_Init();
    SI24R1_CW_Mode();

    printf("================================\r\n");
    printf(" STM32F103C8 + FreeRTOS + SI24R1\r\n");
    printf(" CW mode: carrier on, forever\r\n");
    printf(" RF = 2475 MHz, RF_SETUP = 0x9F\r\n");
    printf(" (2Mbps, +7dBm, CONT_WAVE, PLL_LOCK)\r\n");
    printf("================================\r\n");

    // 载波由硬件持续产生，任务进入空闲；心跳任务继续调度。
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
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
