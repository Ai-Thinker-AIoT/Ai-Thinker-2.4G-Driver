# NF-02-PA(SI24R1) on STC15W408AS —— 单载波 / 连续调制测试

[![English](https://img.shields.io/badge/English-README-green)](README.md)

> 官方文档：<https://docs.ai-thinker.com/2.4g_1/>

从原厂示例派生：同一套按键收发逻辑之外，增加 **CW 未调制单载波** 与 **MOD 连续调制发射** 两种认证测试模式，默认 **MOD**。

- 芯片：SI24R1（软件 SPI，nRF24L01+ 兼容）
- 工程：Keil C51，`USER/SI24R1.uvproj`
- 时钟：内部 IRC **22.1184 MHz**，下载需冷启动

---

## 1. 目录结构

```
STC15W408AS-Single-Carrier/
├── USER/
│   ├── main.c            # 三种模式的分支入口
│   └── SI24R1.uvproj     # Keil C51 工程
├── SI24R1/
│   ├── SI24R1.c          # 驱动 + CW / 连续调制发射
│   └── SI24R1.h          # 引脚宏 + SI24R1_TEST_MODE
├── FWLib/                # STC15 外设库
└── README.md
```

## 2. 测试模式

`SI24R1/SI24R1.h` 中：

```c
#define SI24R1_TEST_MODE  2   // 0 = 正常收发链路
                              // 1 = CW  未调制单载波
                              // 2 = MOD 连续调制发射（本工程默认）
```

| 模式 | 行为 | 参数 | 用途 |
|------|------|------|------|
| 0 | 与 `STC15W408AS-Demo` 相同的按键收发 | 2440 MHz / 2 Mbps / +7 dBm | 功能回归 |
| 1 | `CONFIG=0x0E` → 延时 ≥1.5 ms → 写 `CONT_WAVE+PLL_LOCK` → 设信道 → CE 常高 | 2475 MHz / `RF_SETUP = 0x9F` | 载波功率、频率容限、杂散 |
| 2 | 清 `EN_AA` / `SETUP_RETR`，CE 常高，持续向 TX FIFO 喂伪随机载荷 | 2475 MHz / `RF_SETUP = 0x0F` | 占用带宽、6 dB 带宽、PSD |

- CW 只有单根谱线，占用带宽仅几十 kHz，**不能**用来测 OBW。
- 逐包调用 `SI24R1_TxPacket()` 是通断突发，扫频式频谱仪测不准，测带宽必须用模式 2。

## 3. 引脚

| SI24R1 信号 | STC15W408AS |
|-------------|-------------|
| CE   | P1.4 |
| CSN  | P1.5 |
| SCK  | P1.2 |
| MOSI | P1.3 |
| MISO | P1.0 |
| IRQ  | P1.1 |

按键/指示灯：`KEY1 = P3.0`、`KEY2 = P3.1`、`LED3 = P1.6`、`LED4 = P3.5`。

## 4. 编译与烧录

1. 先确定 `SI24R1_TEST_MODE`（默认 2 = MOD）。
2. Keil C51 打开 `USER/SI24R1.uvproj`，Device = STC15W408AS，IRC 22.1184 MHz，Rebuild。
3. STC-ISP 下载，**冷启动**。

## 5. 备注

模式 0 的行为与原厂示例一致；模式 1/2 上电即持续发射，不再响应按键，测完请改回模式 0 或断电。
