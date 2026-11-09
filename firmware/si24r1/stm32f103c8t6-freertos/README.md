# NF-02-PA(SI24R1) on STM32F103C8T6 + FreeRTOS

基于 **STM32F103C8T6 + FreeRTOS** 控制 **NF-02-PA(SI24R1)（基于 SI24R1 芯片的 2.4GHz 无线模组，兼容 nRF24L01+）** 的移植工程。

---

## 1. 目录结构

```
firmware/si24r1/stm32f103c8t6-freertos/
├── Project.uvprojx          # Keil 工程
├── Start/                   # 启动文件(core_cm3/system_stm32f10x/stm32f10x.h)
├── Library/                 # STM32F10x 标准外设库
├── System/                  # Delay(基于 SysTick)
├── Hardware/
│   ├── LED.c / LED.h        # LED1=PC13(板载), LED2=PA8
│   └── SI24R1/              # ★ 移植后的 SI24R1 驱动（软件 SPI）
│       ├── SI24R1.h
│       └── SI24R1.c
├── User/
│   ├── main.c               # ★ FreeRTOS 示例：心跳任务 + 无线任务
│   ├── stm32f10x_conf.h
│   └── stm32f10x_it.c/.h
├── FreeRTOS/                # FreeRTOS 内核(inc/port/src) + FreeRTOSConfig.h
├── DebugConfig/             # 调试配置（含 STM32F103C8 配置）
└── README.md
```

---

## 2. NF-02-PA(SI24R1) 引脚映射（软件 SPI，按 LoraKit 转接板实际接线）

| 信号  | 转接板丝印  | STM32 引脚 | 方向  | 说明 |
|-------|-------------|-----------|-------|------|
| CE    | CTR1        | PA11      | 输出  | 芯片使能（需飞线到 CTR1） |
| CSN   | SPI_NSS     | PA4       | 输出  | SPI 片选（低有效） |
| SCK   | SPI_SCK     | PA5       | 输出  | SPI 时钟 |
| MOSI  | SPI_MOSI    | PA7       | 输出  | 主机发 |
| MISO  | SPI_MISO    | PA6       | 输入  | 主机收 |
| IRQ   | LORA_DIO0   | PB0       | 输入  | 中断，开漏，已配置内部上拉 |

> 转接板上闲置信号（悬空不接）：`LORA_RESET(PB14)`、`LORA_DIO1(PB1)`、`LORA_DIO4(PA0)`、`CTR2(PA12)`。

> 如需改引脚，直接修改 `Hardware/SI24R1/SI24R1.h` 顶部的端口/引脚宏，无需改动驱动逻辑。
> 该映射避开 SWD(PA13/PA14) 与 JTAG(PB3/PB4)，无需重映射。

---

## 3. 发送 / 接收角色配置

SI24R1 同一时刻只能收或发，**两块板必须设为相反角色**才能通信：

- 在 `Hardware/SI24R1/SI24R1.h` 中修改：

```c
#define SI24R1_ROLE  SI24R1_ROLE_TX   // 本板作为发送端
// #define SI24R1_ROLE  SI24R1_ROLE_RX // 另一块板改为接收端
```

或在 Keil 的 `C` 预定义宏里定义 `SI24R1_ROLE=0` / `SI24R1_ROLE=1`。
源码中默认角色为 **RX**（`SI24R1_ROLE_RX`）。

`main.c` 中：
- **TX 任务**：每隔 1s 发送一包递增计数数据，发送成功翻转 LED2。
- **RX 任务**：轮询接收，收到数据翻转 LED2。
- **心跳任务**：每 500ms 翻转 LED1（PC13），指示 FreeRTOS 调度器在运行。
- LED2 = PA8，用于收发成功指示；USART1 = PA9(TX)/PA10(RX)，115200-8-N-1。

---

## 4. 移植要点说明

1. **软件 SPI**：本工程用 STM32 标准外设库
   `GPIO_SetBits / GPIO_ResetBits / GPIO_ReadInputDataBit` 实现位级 SPI，时序完全兼容 nRF24L01+。
2. **不占用 SysTick**：SPI 延时与初始化延时使用 **DWT 周期计数器**（`CoreDebug->DEMCR` / `DWT->CYCCNT`），
   因为 SysTick 已被 FreeRTOS 用于系统节拍（1kHz），这样可避免两者冲突。
3. **系统时钟**：`system_stm32f10x.c` 配置为 **HSE 8MHz → PLL → 72MHz (HCLK)**，与 `Delay.c` 及
   `FreeRTOSConfig.h (configCPU_CLOCK_HZ=72000000)` 一致。请确保板上焊接了 8MHz 晶振
   （Blue Pill / 多数 C8T6 最小系统板均满足）；若使用纯 HSI 无外部晶振的板子，
   需相应调整 `system_stm32f10x.c` 与 FreeRTOS 时钟配置。

---

## 5. 编译与烧录

1. 用 **Keil uVision**（需安装 `Keil.STM32F1xx_DFP` 器件包）打开 `Project.uvprojx`。
2. 确认 Target 选项：
   - Device = `STM32F103C8`
   - C/C++ → Define 包含 `STM32F10X_MD,USE_STDPERIPH_DRIVER`
   - 包含路径已含 `.\Hardware\SI24R1`
3. 点击 **Rebuild**；点击 **Download/Debug** 烧录（ST-Link）。
4. 上电后：LED1(PC13) 以 500ms 周期翻转（系统心跳）；TX 板每秒翻转 LED2(PA8)，
   RX 板收到数据时翻转 LED2。

---

## 6. 接线参考（模块 ↔ STM32）

```
SI24R1      LoraKit 丝印      STM32F103C8T6
VCC    →    3.3V              3.3V（务必 3.3V，不可 5V）
GND    →    GND               GND（共地）
CE     →    CTR1              PA11
CSN    →    SPI_NSS           PA4
SCK    →    SPI_SCK           PA5
MOSI   →    SPI_MOSI          PA7
MISO   →    SPI_MISO          PA6
IRQ    →    LORA_DIO0         PB0
```

完整接线（含串口、LED、端口模式）见仓库根目录 [`docs/wiring.zh-CN.md`](../../../docs/wiring.zh-CN.md)。

---

## 7. 射频参数

| 项目 | 值 |
|------|-----|
| 通信地址 | `0x0A 01 07 0E 01`（5 字节） |
| 射频信道 | `RF_CH = 100` → 2500 MHz |
| 空中速率 | 1 Mbps（`RF_SETUP = 0x07`，+7 dBm） |
| Payload | 32 字节 |
| 自动应答 | 使能（通道 0） |
| 自动重发 | 500 us × 15 次（`SETUP_RETR = 0x1F`） |
| CRC | 16 位 |

---

## 8. 可扩展方向

- 将 RX 改为 **EXTI 中断 + 二值信号量**（`xSemaphoreGiveFromISR`）唤醒接收任务，实现真正的事件驱动；
- 增加串口打印（`usart.c`）把收到的数据输出到上位机；
- 实现双向 Ping-Pong：通过切换 `RX/TX` 模式定时互发。
