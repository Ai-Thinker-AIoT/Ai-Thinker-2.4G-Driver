<div align="center">

# Ai-Thinker 2.4G Driver (SI24R1)

</div>

[![English](https://img.shields.io/badge/English-README-blue)](README.md)

**SI24R1** 2.4 GHz 无线收发模块（兼容 nRF24L01+，最大发射功率 **+7 dBm**）的多平台驱动与示例集合。
包含原厂 STC15 测试板示例、AI8051U 与 STM32F103C8T6 + FreeRTOS 的正常收发链路工程，以及两个用于射频认证测试的**单载波（single-carrier）**工程（未调制 CW / 连续调制 MOD 发射）。

---

## 1. 仓库结构

| 目录 | MCU / 工具链 | 内容 | 工程文件 |
|------|--------------|------|----------|
| `firmware/stc15w408as-demo/` | STC15W408AS / Keil C51 | 安信可 2.4G 测试板**原厂默认示例**：双板按键互控 LED | `USER/SI24R1.uvproj` |
| `firmware/ai8051u-demo/` | AI8051U（8-bit）/ Keil C51 | **正常收发链路版**（取自周总结归档基线）：串口日志、TX/RX 自测；不含载波测试模式 | `AI8051U-SI24R1.uvproj` |
| `firmware/stm32f103c8t6-freertos/` | STM32F103C8T6 / MDK-ARM + FreeRTOS | 软件 SPI 驱动 + 心跳任务 / 无线任务 | `Project.uvprojx` |
| `firmware/stc15w408as-single-carrier/` | STC15W408AS / Keil C51 | 从原厂示例派生的**单载波测试 demo**：未调制单载波 CW、连续调制发射 MOD（默认 MOD） | `USER/SI24R1.uvproj` |
| `firmware/ai8051u-single-carrier/` | AI8051U（8-bit）/ Keil C51 | 同一套单载波测试框架在 AI8051U 上的移植；`SI24R1_TEST_MODE` = 0 / 1 / 2（默认 0，正常链路） | `AI8051U-SI24R1.uvproj` |

---

## 2. 引脚对照

| SI24R1 | STC15W408AS | AI8051U | STM32F103C8T6 |
|--------|-------------|---------|-------------------------------|
| CE     | P1.4        | P1.4    | PA11（CTR1） |
| CSN    | P1.5        | P1.5    | PA4（SPI_NSS） |
| SCK    | P1.2        | P1.2    | PA5（SPI_SCK） |
| MOSI   | P1.3        | P1.3    | PA7（SPI_MOSI） |
| MISO   | P1.0        | P1.0    | PA6（SPI_MISO） |
| IRQ    | P1.1        | P1.1    | PB0（LORA_DIO0） |

- 调试串口：AI8051U = P3.7(TX)/P3.6(RX)，STM32 = PA9(TX)/PA10(RX)，均 115200-8-N-1。
- 完整端口模式配置见 `docs/wiring.zh-CN.md`。

---

## 3. 射频参数

| 工程 | RF_CH | 频率 | 速率 / 功率（`RF_SETUP`） | 自动重发 | CRC |
|------|-------|------|---------------------------|----------|-----|
| `firmware/stc15w408as-demo/` | 40 | 2440 MHz | 2 Mbps / +7 dBm (`0x0F`) | 250us+86us ×10 (`0x0A`) | 16 bit |
| `firmware/ai8051u-demo/` | 40 | 2440 MHz | 1 Mbps / +7 dBm (`0x07`) | 500us ×10 (`0x1A`) | 16 bit |
| `firmware/stm32f103c8t6-freertos/` | 100 | 2500 MHz | 1 Mbps / +7 dBm (`0x07`) | 500us ×15 (`0x1F`) | 16 bit |
| `firmware/ai8051u-single-carrier/`（`TEST_MODE=0`） | 100 | 2500 MHz | 1 Mbps / +7 dBm (`0x07`) | 500us ×15 (`0x1F`) | 16 bit |
| `firmware/*-single-carrier/`（`TEST_MODE=1/2`） | 75 | 2475 MHz | 2 Mbps / +7 dBm (`0x0F`；CW 再加 `CONT_WAVE+PLL_LOCK` → `0x9F`) | 关闭 | 16 bit |

公共配置：地址 `0x0A 0x01 0x07 0x0E 0x01`（5 字节）、Payload 32 字节、通道 0 自动应答、16 位 CRC。

---

## 4. 单载波测试模式（CW / 连续调制 MOD）

- `firmware/ai8051u-single-carrier/Source/SI24R1.h` — `SI24R1_TEST_MODE` 默认为 **0**（正常收发链路）
- `firmware/stc15w408as-single-carrier/SI24R1/SI24R1.h` — `SI24R1_TEST_MODE` 默认为 **2**（连续调制发射）

```c
#define SI24R1_TEST_MODE  0   // 0 = 正常收发链路
                              // 1 = CW  未调制单载波：载波功率 / 频率容限 / 杂散
                              // 2 = MOD 连续调制发射：占用带宽 / 6dB 带宽 / PSD
```

| 模式 | 做法 | RF_SETUP | 用途 |
|------|------|----------|------|
| 1 CW | `CONFIG=0x0E` → 延时 ≥1.5 ms → `CONT_WAVE+PLL_LOCK` → 设 RF_CH → CE 常高 | `0x9F` | 单根谱线，OBW 仅几十 kHz |
| 2 MOD | EN_AA / SETUP_RETR 清零、CE 常高、持续向 TX FIFO 喂伪随机载荷 | `0x0F` | 真正的连续调制载波 |

- 逐包调用 `SI24R1_TxPacket()` 会产生通断突发，扫频式频谱仪测不准，必须用模式 2。

---

## 5. 编译与烧录

| 工程 | 环境 | 步骤 |
|------|------|------|
| STC15工程 | Keil C51 | 打开 `USER/SI24R1.uvproj`，Device = STC15W408AS，内部 IRC 22.1184 MHz；STC-ISP 烧录|
| AI8051U工程 | Keil C51 | 运行 `build.bat` 或打开 `.uvproj`；主频 33.1776 MHz；8bit |
| STM32 工程 | Keil MDK + `Keil.STM32F1xx_DFP` | 打开 `Project.uvprojx`，Define 含 `STM32F10X_MD,USE_STDPERIPH_DRIVER`；ST-Link 烧录 |

- AI8051U 工程默认角色均为 TX：改 `SI24R1.h` 中 `SI24R1_ROLE` 为 `SI24R1_ROLE_RX` 编出接收端固件（或用编译预定义覆盖）。
- STM32 默认角色 RX：改 `Hardware/SI24R1/SI24R1.h` 中 `SI24R1_ROLE`；两块板必须一发一收。