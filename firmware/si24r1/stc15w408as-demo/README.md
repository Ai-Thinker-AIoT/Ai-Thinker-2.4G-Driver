# NF-02-PA(SI24R1) on STC15W408AS —— 原厂默认收发示例

安信可 2.4G 测试板（STC15W408AS）上的 **SI24R1 原厂示例**：两块板各按按键，通过 2.4G 把键值发到对面，点亮对侧 LED。

- 芯片：SI24R1（NF-03 / NF-01-S / NF-02-PA / NF-02-SM / NF-02-PE），软件 SPI，nRF24L01+ 兼容
- 工程：Keil C51，`USER/SI24R1.uvproj`
- 时钟：内部 IRC **22.1184 MHz**，下载需冷启动

---

## 1. 目录结构

```
stc15w408as-demo/
├── USER/
│   ├── main.c            # 按键扫描 / 收发切换 / 指示灯
│   └── SI24R1.uvproj     # Keil C51 工程
├── SI24R1/
│   ├── SI24R1.c          # 寄存器初始化、软件 SPI、收发函数
│   └── SI24R1.h          # 引脚宏、命令与寄存器地址
├── FWLib/                # STC15 外设库
└── README.md
```

## 2. 引脚

| SI24R1 信号 | STC15W408AS |
|-------------|-------------|
| CE   | P1.4 |
| CSN  | P1.5 |
| SCK  | P1.2 |
| MOSI | P1.3 |
| MISO | P1.0 |
| IRQ  | P1.1 |

板载按键/指示灯：`KEY1 = P3.0`、`KEY2 = P3.1`、`LED3 = P1.6`、`LED4 = P3.5`（见 `USER/main.c`）。

改引脚只改 `SI24R1/SI24R1.h` 顶部的宏，驱动逻辑不用动。完整端口模式见仓库根目录 `docs/wiring.zh-CN.md`。

## 3. 射频参数

| 项目 | 值 |
|------|-----|
| 射频信道 | `RF_CH = 40` → 2440 MHz |
| 速率 / 功率 | 2 Mbps / +7 dBm（`RF_SETUP = 0x0F`） |
| 自动重发 | 250 us + 86 us × 10（`SETUP_RETR = 0x0A`） |
| Payload | 32 字节，5 字节地址 |
| CRC | 16 位 |

## 4. 操作说明

1. 上电后默认进入 **RX**（`SI24R1_RX_Mode()`）。
2. 按下 `KEY1` → 切 TX、发一包载荷 `0x55`、再切回 RX；对侧收到后 `LED3` 闪两下。
3. 按下 `KEY2` → 发 `0xAA`；对侧 `LED4` 闪两下。
4. 两块板角色对等，双向同步。

## 5. 编译与烧录

1. Keil C51 打开 `USER/SI24R1.uvproj`，Device = STC15W408AS，IRC 22.1184 MHz。
2. Rebuild 生成 HEX，用 STC-ISP 下载（**冷启动**：点下载后再给板子上电/复位）。

## 6. 备注

本程序与 nRF24L01+ 完全兼容，唯一区别是 SI24R1 最大输出功率可达 +7 dBm。
