# NF-01-N / NF-02-PA / NF-02-PE (NRF24L01+) on STC15W408AS —— 原厂收发示例

[![English](https://img.shields.io/badge/English-README-green)](README.md)

> 官方文档：<https://docs.ai-thinker.com/2.4g_1/>

安信可 2.4G 测试板上的 **nRF24L01+ 原厂 STC15 示例**：双板按键互控 LED。

- 芯片：nRF24L01+（NF-01-N / NF-02-PA / NF-02-PE）
- 工程：Keil C51，`USER/nRF24L01P.uvproj`
- 时钟：内部 IRC **22.1184 MHz**，下载需冷启动

---

## 1. 目录结构

```
STC15W408AS-Demo/
├── USER/
│   ├── main.c            # 按键扫描 / 收发切换 / 指示灯
│   └── nRF24L01P.uvproj  # Keil C51 工程
├── 24L01/
│   ├── nRF24L01P.c       # 寄存器初始化、软件 SPI、收发函数
│   └── nRF24L01P.h       # 引脚宏、命令与寄存器地址
├── FWLib/                # STC15 外设库
└── README.md
```

## 2. 引脚

| nRF24L01+ 信号 | STC15W408AS |
|----------------|-------------|
| CE   | P1.4 |
| CSN  | P1.5 |
| SCK  | P1.2 |
| MOSI | P1.3 |
| MISO | P1.0 |
| IRQ  | P1.1 |

按键/指示灯：`KEY1 = P3.0`、`KEY2 = P3.1`、`LED3 = P1.6`、`LED4 = P3.5`。

## 3. 射频参数

| 项目 | 值 |
|------|-----|
| 射频信道 | `RF_CH = 40` → 2440 MHz |
| 速率 / 功率 | 2 Mbps / 0 dBm（`RF_SETUP = 0x0F`，源码注释值） |
| 自动重发 | 250 us + 86 us × 10（`SETUP_RETR = 0x0A`） |
| Payload | 32 字节，5 字节地址 |
| CRC | 16 位 |

## 4. 操作说明

上电默认 RX；按 `KEY1` 发 `0x55`、对侧 `LED3` 闪两下；按 `KEY2` 发 `0xAA`、对侧 `LED4` 闪两下。

## 5. 编译与烧录

Keil C51 打开 `USER/nRF24L01P.uvproj`（Device = STC15W408AS，IRC 22.1184 MHz）→ Rebuild → STC-ISP 下载，**冷启动**。

## 6. 备注

本程序不需要任何修改即可直接用于 SI24R1 芯片，唯一差别是此时 SI24R1 输出功率为 +7 dBm 而不是 0 dBm。
