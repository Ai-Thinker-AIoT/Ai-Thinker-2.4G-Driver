#include "stm32f10x.h"                  // Device header

/**
  * LED 引脚（原 PB0/PB1 与 LoraKit 的 DIO0/DIO1 冲突，已迁移）
  *   LED1 = PC13  （Blue Pill 板载 LED，低电平点亮）
  *   LED2 = PA8   （空闲引脚，低电平点亮，可外接 LED）
  */
#define LED1_PORT   GPIOC
#define LED1_PIN    GPIO_Pin_13
#define LED2_PORT   GPIOA
#define LED2_PIN    GPIO_Pin_8

/**
  * 函    数：LED初始化
  */
void LED_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
	                       RCC_APB2Periph_GPIOA, ENABLE);

	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LED1_PIN;
	GPIO_Init(LED1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED2_PIN;
	GPIO_Init(LED2_PORT, &GPIO_InitStructure);

	/*默认高电平（灭）*/
	GPIO_SetBits(LED1_PORT, LED1_PIN);
	GPIO_SetBits(LED2_PORT, LED2_PIN);
}

void LED1_ON(void)  { GPIO_ResetBits(LED1_PORT, LED1_PIN); }
void LED1_OFF(void) { GPIO_SetBits(LED1_PORT, LED1_PIN); }
void LED1_Turn(void)
{
	if (GPIO_ReadOutputDataBit(LED1_PORT, LED1_PIN) == 0)
		GPIO_SetBits(LED1_PORT, LED1_PIN);
	else
		GPIO_ResetBits(LED1_PORT, LED1_PIN);
}

void LED2_ON(void)  { GPIO_ResetBits(LED2_PORT, LED2_PIN); }
void LED2_OFF(void) { GPIO_SetBits(LED2_PORT, LED2_PIN); }
void LED2_Turn(void)
{
	if (GPIO_ReadOutputDataBit(LED2_PORT, LED2_PIN) == 0)
		GPIO_SetBits(LED2_PORT, LED2_PIN);
	else
		GPIO_ResetBits(LED2_PORT, LED2_PIN);
}
