# NF-05 / NF-05-S (CI24R1) on STC15W408AS —— 原厂收发示例

安信可 2.4G 测试板上的 **CI24R1 原厂 STC15 示例**：双板按键互控 LED。

- 芯片：CI24R1（NF-05 / NF-05-S）
- 工程：Keil C51，`USER/CI24R1.uvproj`
- 时钟：内部 IRC **11.0592 MHz**（与其它三颗芯片的 22.1184 MHz 不同），下载需冷启动

---

## 1. 目录结构

```
STC15W408AS-Demo/
├── USER/
│   ├── main.c            # 按键扫描 / 收发切换 / 指示灯
│   └── CI24R1.uvproj     # Keil C51 工程
├── CI24R1/
│   ├── CI24R1.c          # 寄存器初始化、时序、收发函数
│   └── CI24R1.h          # 引脚宏、命令与寄存器地址
├── FWLib/                # STC15 外设库
└── README.md
```

## 2. 接口与引脚

CI24R1 与 nRF24L01+ 不是同一套物理接口：数据线复用、CE 由命令控制，接线以 `CI24R1/CI24R1.h` 为准。

| CI24R1 信号 | STC15W408AS | 说明 |
|-------------|-------------|------|
| CSN  | P1.5 | 片选，低有效 |
| SCK  | P1.2 | 时钟 |
| DATA | P1.0 | 单线复用，兼 MOSI / IRQ，靠 `SELSPI` / `SELIRQ` 命令切换角色 |
| CE   | — | 无独立引脚，用 `CE_ON` / `CE_OFF` 命令开关 |

按键/指示灯：`KEY1 = P3.0`、`KEY2 = P3.1`、`LED3 = P1.6`、`LED4 = P3.5`。

> 因此 CI24R1 不能直接插在 SI24R1/nRF24L01+ 的四线座子上做点对点测试，两块板必须是同型号 CI24R1。

## 3. 射频参数

| 项目 | 值 |
|------|-----|
| 射频信道 | `RF_CH = 80` → 2480 MHz |
| 速率 / 功率 | 2 Mbps / +9 dBm（`RF_SETUP = 0x0D`，源码注释值） |
| Payload | 32 字节 |
| CRC | 16 位 |

自动应答、重发等其余寄存器配置见 `CI24R1/CI24R1.c` 中 `CI24R1_RX_Mode` / `CI24R1_TX_Mode`。

## 4. 操作说明

上电默认 RX；按 `KEY1` 发 `0x55`、对侧 `LED3` 闪两下；按 `KEY2` 发 `0xAA`、对侧 `LED4` 闪两下。

## 5. 编译与烧录

Keil C51 打开 `USER/CI24R1.uvproj`（Device = STC15W408AS，IRC **11.0592 MHz**）→ Rebuild → STC-ISP 下载，**冷启动**。

时钟选错会直接导致串口/时序异常，务必核对这一项。
