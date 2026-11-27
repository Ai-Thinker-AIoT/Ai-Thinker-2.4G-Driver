# NF-01-N / NF-02-PA / NF-02-PE (NRF24L01+) on STC15W408AS — Factory Transceiver Example

[![中文](https://img.shields.io/badge/中文-README-blue)](README.zh-CN.md)

> Official documentation: <https://docs.ai-thinker.com/2.4g_1/>

The **nRF24L01+ factory STC15 example** for the Ai-Thinker 2.4G test board: two boards exchange key presses and light each other's LEDs.

- Chip: nRF24L01+ (NF-01-N / NF-02-PA / NF-02-PE)
- Project: Keil C51, `USER/nRF24L01P.uvproj`
- Clock: internal IRC **22.1184 MHz**; download needs a cold start

---

## 1. Layout

```
STC15W408AS-Demo/
├── USER/
│   ├── main.c            # key scan / TX-RX switching / LEDs
│   └── nRF24L01P.uvproj  # Keil C51 project
├── 24L01/
│   ├── nRF24L01P.c       # register init, software SPI, TX/RX functions
│   └── nRF24L01P.h       # pin macros, commands and register addresses
├── FWLib/                # STC15 peripheral library
└── README.md
```

## 2. Pins

| nRF24L01+ signal | STC15W408AS |
|------------------|-------------|
| CE   | P1.4 |
| CSN  | P1.5 |
| SCK  | P1.2 |
| MOSI | P1.3 |
| MISO | P1.0 |
| IRQ  | P1.1 |

Keys / LEDs: `KEY1 = P3.0`, `KEY2 = P3.1`, `LED3 = P1.6`, `LED4 = P3.5`.

## 3. RF settings

| Item | Value |
|------|-------|
| Channel | `RF_CH = 40` → 2440 MHz |
| Rate / power | 2 Mbps / 0 dBm (`RF_SETUP = 0x0F`, value as commented in the source) |
| Auto retransmit | 250 us + 86 us × 10 (`SETUP_RETR = 0x0A`) |
| Payload | 32 bytes, 5-byte address |
| CRC | 16 bit |

## 4. How it works

After power-up the board is in RX. Pressing `KEY1` sends `0x55` and the other board's `LED3` blinks twice; pressing `KEY2` sends `0xAA` and `LED4` blinks twice.

## 5. Build and flash

Open `USER/nRF24L01P.uvproj` in Keil C51 (Device = STC15W408AS, IRC 22.1184 MHz) → Rebuild → download with STC-ISP, **cold start**.

## 6. Notes

The program also runs on SI24R1 without any change; the only difference is that SI24R1 then outputs +7 dBm instead of 0 dBm.
