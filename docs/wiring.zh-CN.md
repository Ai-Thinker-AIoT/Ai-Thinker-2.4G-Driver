# SI24R1 接线说明

## AI8051U（`firmware/ai8051u-demo/`、`firmware/ai8051u-single-carrier/`，Keil C51 8 位）

通信接口：软件 SPI（P1 口） + UART1（`P_SW1` 切组 1）

| SI24R1 引脚 | AI8051U 引脚 | 方向 | 说明 |
|-------------|--------------|------|------|
| VCC | 3.3V | — | 必须接 3.3V，不可接 5V |
| GND | GND | — | 共地 |
| MOSI | P1.3 | MCU 输出 | 主机发数据 |
| MISO | P1.0 | MCU 输入 | 主机收数据 |
| SCK | P1.2 | MCU 输出 | SPI 时钟 |
| CSN | P1.5 | MCU 输出 | 片选，低有效 |
| CE | P1.4 | MCU 输出 | 芯片使能 |
| IRQ | P1.1 | MCU 输入 | 中断，开漏，需上拉 |

**串口（调试打印，115200-8-N-1）**

```
USB转TTL RXD  ->  AI8051U P3.7（TxD，组 1）
USB转TTL TXD  ->  AI8051U P3.6（RxD，组 1，可不接）
USB转TTL GND  ->  GND（共地）
```

**LED 指示**

- P2.0：心跳 / 收发指示（低电平点亮）
- P2.0~P2.7：板载 LED 组

**端口模式配置（代码内）**

```
P1M1=0x03, P1M0=0x0F   P1.0/P1.1 输入（MISO/IRQ），P1.2~P1.5 推挽输出
P2M1=0x00, P2M0=0xFF   P2 全推挽（LED）
P3M1=0x00, P3M0=0x82   P3.1/P3.7 推挽（TxD），P3.0/P3.6 准双向（RxD）
P4M1=0x00, P4M0=0x00   P4 准双向
```

---

## STM32F103C8T6（`firmware/stm32f103c8t6-freertos/`，FreeRTOS）

通信接口：软件 SPI（LoraKit 转接板引脚）

| SI24R1 引脚 | LoraKit 丝印 | STM32 引脚 | 方向 | 说明 |
|-------------|--------------|------------|------|------|
| VCC | 3.3V | — | — | 必须接 3.3V，不可接 5V |
| GND | GND | — | — | 共地 |
| CSN | SPI_NSS | PA4 | MCU 输出 | 片选，低有效 |
| SCK | SPI_SCK | PA5 | MCU 输出 | SPI 时钟 |
| MISO | SPI_MISO | PA6 | MCU 输入 | 主机收数据 |
| MOSI | SPI_MOSI | PA7 | MCU 输出 | 主机发数据 |
| IRQ | LORA_DIO0 | PB0 | MCU 输入 | 中断，开漏，内部上拉 |
| CE | CTR1 | PA11 | MCU 输出 | 芯片使能（需飞线到 CTR1） |

转接板上 SI24R1 用不到的信号（悬空不接）：
`LORA_RESET(PB14)`、`LORA_DIO1(PB1)`、`LORA_DIO4(PA0)`、`CTR2(PA12)`。

**串口（调试打印，115200-8-N-1）**

```
USB转TTL RXD  ->  STM32 PA9（USART1_TX）
USB转TTL TXD  ->  STM32 PA10（USART1_RX，可不接）
USB转TTL GND  ->  GND（共地）
```

**LED 指示**

- PC13：心跳（板载灯，500ms 翻转，低电平点亮）
- PA8：收发成功指示（低电平点亮）
