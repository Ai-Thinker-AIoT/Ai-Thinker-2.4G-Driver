# NF-02-PA (SI24R1) on STC15W408AS — Factory Default Transceiver Example

[![中文](https://img.shields.io/badge/中文-README-blue)](README.zh-CN.md)

> Official documentation: <https://docs.ai-thinker.com/2.4g_1/>

The **SI24R1 factory example** for the Ai-Thinker 2.4G test board (STC15W408AS): each board sends its key value over 2.4G and lights the LED on the other board.

- Chip: SI24R1 (NF-03 / NF-01-S / NF-02-PA / NF-02-SM / NF-02-PE), software SPI, nRF24L01+ compatible
- Project: Keil C51, `USER/SI24R1.uvproj`
- Clock: internal IRC **22.1184 MHz**; download needs a cold start

---

## 1. Layout

```
STC15W408AS-Demo/
├── USER/
│   ├── main.c            # key scan / TX-RX switching / LEDs
│   └── SI24R1.uvproj     # Keil C51 project
├── SI24R1/
│   ├── SI24R1.c          # register init, software SPI, TX/RX functions
│   └── SI24R1.h          # pin macros, commands and register addresses
├── FWLib/                # STC15 peripheral library
└── README.md
```

## 2. Pins

| SI24R1 signal | STC15W408AS |
|---------------|-------------|
| CE   | P1.4 |
| CSN  | P1.5 |
| SCK  | P1.2 |
| MOSI | P1.3 |
| MISO | P1.0 |
| IRQ  | P1.1 |

On-board keys / LEDs: `KEY1 = P3.0`, `KEY2 = P3.1`, `LED3 = P1.6`, `LED4 = P3.5` (see `USER/main.c`).

To change pins, edit only the macros at the top of `SI24R1/SI24R1.h`; the driver logic stays the same. Full port configuration: [`Docs/wiring.md`](../../Docs/wiring.md).

## 3. RF settings

| Item | Value |
|------|-------|
| Channel | `RF_CH = 40` → 2440 MHz |
| Rate / power | 2 Mbps / +7 dBm (`RF_SETUP = 0x0F`) |
| Auto retransmit | 250 us + 86 us × 10 (`SETUP_RETR = 0x0A`) |
| Payload | 32 bytes, 5-byte address |
| CRC | 16 bit |

## 4. How it works

1. After power-up the board enters **RX** (`SI24R1_RX_Mode()`).
2. Press `KEY1` → switch to TX, send one packet with payload `0x55`, switch back to RX; the other board's `LED3` blinks twice.
3. Press `KEY2` → send `0xAA`; the other board's `LED4` blinks twice.
4. Both boards are peers and work in both directions.

## 5. Build and flash

1. Open `USER/SI24R1.uvproj` in Keil C51, Device = STC15W408AS, IRC 22.1184 MHz.
2. Rebuild to produce the HEX, then download with STC-ISP (**cold start**: click download, then power-cycle / reset the board).

## 6. Notes

The program is fully compatible with nRF24L01+; the only difference is that SI24R1 can output up to +7 dBm.
