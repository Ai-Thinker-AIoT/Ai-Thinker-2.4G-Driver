<div align="center">

# Ai-Thinker 2.4G Driver

</div>

[![English](https://img.shields.io/badge/English-README-blue)](README.md)

**Ai-Thinker 2.4G Driver** —— 安信可 2.4G 系列模组的驱动与示例固件集合。覆盖 **4 颗芯片**（SI24R1、BK2425、CI24R1、NRF24L01+）与 **12 个模组型号**，固件工程统一放在 `firmware/` 下。

---

## 1. 仓库结构

固件工程统一放在 `firmware/` 下、按芯片分目录。

### NF-03、NF-01-S、NF-02-PA、NF-02-SM、NF-02-PE(SI24R1)

**芯片目录**：`firmware/si24r1/`

| 目录 | MCU / 工具链 | 内容 | 工程文件 |
|------|--------------|------|----------|
| `firmware/si24r1/stc15w408as-demo/` | STC15W408AS / Keil C51 | 安信可 2.4G 测试板**原厂默认示例**：双板按键互控 LED | `USER/SI24R1.uvproj` |
| `firmware/si24r1/ai8051u-demo/` | AI8051U（8-bit）/ Keil C51 | **正常收发链路版**（取自周总结归档基线）：串口日志、TX/RX 自测；不含载波测试模式 | `AI8051U-SI24R1.uvproj` |
| `firmware/si24r1/stm32f103c8t6-freertos/` | STM32F103C8T6 / MDK-ARM + FreeRTOS | 软件 SPI 驱动 + 心跳任务 / 无线任务 | `Project.uvprojx` |
| `firmware/si24r1/stc15w408as-single-carrier/` | STC15W408AS / Keil C51 | 从原厂示例派生的**单载波测试 demo**：未调制单载波 CW、连续调制发射 MOD（默认 MOD） | `USER/SI24R1.uvproj` |
| `firmware/si24r1/ai8051u-single-carrier/` | AI8051U（8-bit）/ Keil C51 | 同一套单载波测试框架在 AI8051U 上的移植；`SI24R1_TEST_MODE` = 0 / 1 / 2（默认 0，正常链路） | `AI8051U-SI24R1.uvproj` |

### NF-04、NF-04-MI(BK2425)

**芯片目录**：`firmware/bk2425/`

| 目录 | MCU / 工具链 | 内容 | 工程文件 |
|------|--------------|------|----------|
| `firmware/bk2425/stc15w408as-demo/` | STC15W408AS / Keil C51 | BK2425 原厂 STC15 示例（软件 SPI 驱动、按键控灯） | `USER/BK2425.uvproj` |

### NF-05、NF-05-S(CI24R1)

**芯片目录**：`firmware/ci24r1/`

| 目录 | MCU / 工具链 | 内容 | 工程文件 |
|------|--------------|------|----------|
| `firmware/ci24r1/stc15w408as-demo/` | STC15W408AS / Keil C51 | CI24R1 原厂 STC15 示例（软件 SPI 驱动、按键控灯） | `USER/CI24R1.uvproj` |

### NF-01-N、NF-02-PA、NF-02-PE(NRF24L01+)

**芯片目录**：`firmware/nrf24l01p/`

| 目录 | MCU / 工具链 | 内容 | 工程文件 |
|------|--------------|------|----------|
| `firmware/nrf24l01p/stc15w408as-demo/` | STC15W408AS / Keil C51 | NRF24L01+ 原厂 STC15 示例（软件 SPI 驱动、按键控灯） | `USER/nRF24L01P.uvproj` |

---

## 2. 引脚对照

以下以 `NF-02-PA(SI24R1)` 模组为参考（基于 SI24R1 芯片，兼容 nRF24L01+，最大发射功率 **+7 dBm**）。

| NF-02-PA(SI24R1) | STC15W408AS | AI8051U | STM32F103C8T6 |
|--------------------|-------------|---------|-------------------------------|
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
| `firmware/si24r1/stc15w408as-demo/` | 40 | 2440 MHz | 2 Mbps / +7 dBm (`0x0F`) | 250us+86us ×10 (`0x0A`) | 16 bit |
| `firmware/si24r1/ai8051u-demo/` | 40 | 2440 MHz | 1 Mbps / +7 dBm (`0x07`) | 500us ×10 (`0x1A`) | 16 bit |
| `firmware/si24r1/stm32f103c8t6-freertos/` | 100 | 2500 MHz | 1 Mbps / +7 dBm (`0x07`) | 500us ×15 (`0x1F`) | 16 bit |
| `firmware/si24r1/ai8051u-single-carrier/`（`TEST_MODE=0`） | 100 | 2500 MHz | 1 Mbps / +7 dBm (`0x07`) | 500us ×15 (`0x1F`) | 16 bit |
| `firmware/si24r1/*-single-carrier/`（`TEST_MODE=1/2`） | 75 | 2475 MHz | 2 Mbps / +7 dBm (`0x0F`；CW 再加 `CONT_WAVE+PLL_LOCK` → `0x9F`) | 关闭 | 16 bit |

公共配置：地址 `0x0A 0x01 0x07 0x0E 0x01`（5 字节）、Payload 32 字节、通道 0 自动应答、16 位 CRC。

---

## 4. 单载波测试模式（CW / 连续调制 MOD）

- `firmware/si24r1/ai8051u-single-carrier/Source/SI24R1.h` — `SI24R1_TEST_MODE` 默认为 **0**（正常收发链路）
- `firmware/si24r1/stc15w408as-single-carrier/SI24R1/SI24R1.h` — `SI24R1_TEST_MODE` 默认为 **2**（连续调制发射）

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
| STC15 工程（SI24R1 / BK2425 / NRF24L01+） | Keil C51 | 打开对应 `USER/*.uvproj`（`SI24R1.uvproj` / `BK2425.uvproj` / `nRF24L01P.uvproj`），Device = STC15W408AS，内部 IRC 22.1184 MHz；STC-ISP 烧录 |
| STC15 工程（CI24R1） | Keil C51 | 打开 `USER/CI24R1.uvproj`，内部 IRC **11.0592 MHz**（与其余三颗不同），下载需冷启动；STC-ISP 烧录 |
| AI8051U 工程 | Keil C51 | 运行 `build.bat` 或打开 `.uvproj`；主频 33.1776 MHz；8bit 模式 |
| STM32 工程 | Keil MDK + `Keil.STM32F1xx_DFP` | 打开 `Project.uvprojx`，Define 含 `STM32F10X_MD,USE_STDPERIPH_DRIVER`；ST-Link 烧录 `Objects/Project.axf` |

- AI8051U 工程默认角色均为 TX：改 `SI24R1.h` 中 `SI24R1_ROLE` 为 `SI24R1_ROLE_RX` 编出接收端固件（或用编译预定义覆盖）。
- `firmware/si24r1/ai8051u-single-carrier/` 默认 `SI24R1_TEST_MODE = 0`，要出单载波需先改成 1(CW) 或 2(MOD)；`firmware/si24r1/stc15w408as-single-carrier/` 默认即为 MOD。
- STM32 默认角色 RX：改 `Hardware/SI24R1/SI24R1.h` 中 `SI24R1_ROLE`；两块板必须一发一收。该工程未配置 fromelf 后处理，编译只生成 `.axf`，需要 `.hex` 时请在 Options for Target → User → After Build 中自行启用。