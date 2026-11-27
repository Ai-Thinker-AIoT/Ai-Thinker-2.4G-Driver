# NF-05 / NF-05-S (CI24R1) on STC15W408AS — Factory Transceiver Example

[![中文](https://img.shields.io/badge/中文-README-blue)](README.zh-CN.md)

> Official documentation: <https://docs.ai-thinker.com/2.4g_1/>

The **CI24R1 factory STC15 example** for the Ai-Thinker 2.4G test board: two boards exchange key presses and light each other's LEDs.

- Chip: CI24R1 (NF-05 / NF-05-S)
- Project: Keil C51, `USER/CI24R1.uvproj`
- Clock: internal IRC **11.0592 MHz** (the other three chips use 22.1184 MHz); download needs a cold start

---

## 1. Layout

```
STC15W408AS-Demo/
├── USER/
│   ├── main.c            # key scan / TX-RX switching / LEDs
│   └── CI24R1.uvproj     # Keil C51 project
├── CI24R1/
│   ├── CI24R1.c          # register init, timing, TX/RX functions
│   └── CI24R1.h          # pin macros, commands and register addresses
├── FWLib/                # STC15 peripheral library
└── README.md
```

## 2. Interface and pins

CI24R1 does not use the same physical interface as nRF24L01+: the data line is shared and CE is controlled by commands, so `CI24R1/CI24R1.h` is the reference for wiring.

| CI24R1 signal | STC15W408AS | Note |
|---------------|-------------|------|
| CSN  | P1.5 | chip select, active low |
| SCK  | P1.2 | clock |
| DATA | P1.0 | single line shared by MOSI / IRQ, switched by the `SELSPI` / `SELIRQ` commands |
| CE   | — | no dedicated pin; enabled by `CE_ON` / `CE_OFF` commands |

Keys / LEDs: `KEY1 = P3.0`, `KEY2 = P3.1`, `LED3 = P1.6`, `LED4 = P3.5`.

> CI24R1 therefore cannot be plugged into the four-wire SI24R1/nRF24L01+ socket for a point-to-point test; both boards must be CI24R1.

## 3. RF settings

| Item | Value |
|------|-------|
| Channel | `RF_CH = 80` → 2480 MHz |
| Rate / power | 2 Mbps / +9 dBm (`RF_SETUP = 0x0D`, value as commented in the source) |
| Payload | 32 bytes |
| CRC | 16 bit |

Auto-ACK, retransmit and the remaining registers are configured in `CI24R1_RX_Mode` / `CI24R1_TX_Mode` inside `CI24R1/CI24R1.c`.

## 4. How it works

After power-up the board is in RX. Pressing `KEY1` sends `0x55` and the other board's `LED3` blinks twice; pressing `KEY2` sends `0xAA` and `LED4` blinks twice.

## 5. Build and flash

Open `USER/CI24R1.uvproj` in Keil C51 (Device = STC15W408AS, IRC **11.0592 MHz**) → Rebuild → download with STC-ISP, **cold start**.

A wrong clock setting breaks UART and timing immediately, so always double-check this item.
