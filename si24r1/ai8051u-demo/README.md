# NF-02-PA(SI24R1) on AI8051U —— 正常收发链路

AI8051U 上的 SI24R1 收发示例（取自周总结归档基线）：串口日志 + TX/RX 自测，不含载波测试模式。

- 芯片：SI24R1（软件 SPI，nRF24L01+ 兼容）
- MCU：AI8051U，32 位模式（C351 编译器，8-bit 内核配置 `STC51U_8BIT`）
- 主频 **33.1776 MHz**，UART1 = 115200-8-N-1

---

## 1. 目录结构

```
ai8051u-demo/
├── Source/
│   ├── main.c                # 初始化、UART 日志、TX/RX 任务循环
│   ├── main_full.c           # 整理时保留的完整参考版，未加入工程编译
│   ├── SI24R1.c / SI24R1.h   # 驱动 + 引脚宏 + 角色宏
│   └── XSFR_STC8051U_8Bit.H  # AI8051U 寄存器/端口定义
├── Toolchain/                # 本地 Keil C51 工具链缓存（CI 不含二进制）
├── AI8051U-SI24R1.uvproj     # Keil C51 工程
├── build.bat                 # 命令行编译脚本
└── README.md
```

## 2. 引脚

| SI24R1 信号 | AI8051U |
|-------------|---------|
| CE   | P1.4 |
| CSN  | P1.5 |
| SCK  | P1.2 |
| MOSI | P1.3 |
| MISO | P1.0 |
| IRQ  | P1.1 |

- 调试串口：P3.7（TX）/ P3.6（RX），115200-8-N-1。
- 状态 LED：`P2^0`（低电平点亮，见 `main.c` 的 `sbit LED`）。
- 与 STC15 版同口同序，接线可直接沿用 `docs/wiring.zh-CN.md`。

## 3. 角色配置

`Source/SI24R1.h` 中 `SI24R1_ROLE` 默认 **TX**：

```c
#define SI24R1_ROLE  SI24R1_ROLE_TX   // 发送端
// #define SI24R1_ROLE  SI24R1_ROLE_RX // 接收端
```

也可用编译预定义 `-DSI24R1_ROLE=0`（RX）/ `=1`（TX）覆盖。**两块板必须一收一发**，地址一致才能互通。

## 4. 射频参数

| 项目 | 值 |
|------|-----|
| 信道 | `RF_CH = 40` → 2440 MHz |
| 速率 / 功率 | 1 Mbps / +7 dBm（`RF_SETUP = 0x07`） |
| 自动重发 | 500 us × 10（`SETUP_RETR = 0x1A`） |
| Payload | 32 字节递增载荷，5 字节地址 |
| CRC | 16 位 |

## 5. 编译与烧录

- 双击 `build.bat`（内部调用 `UV4.exe -b AI8051U-SI24R1.uvproj`，产物 `Objects/AI8051U-SI24R1.Hex`），或直接用 Keil C51 打开 `AI8051U-SI24R1.uvproj` 后 Rebuild。
- 用 STC-ISP 下载 HEX，AI8051U 需按提示冷启动/重新上电。
- 实测双板收发日志见仓库根目录 `README.md` 的截图。

## 6. 备注

本工程不提供 CW / MOD 测试模式；需要做认证测试请用 `../ai8051u-single-carrier/`。
