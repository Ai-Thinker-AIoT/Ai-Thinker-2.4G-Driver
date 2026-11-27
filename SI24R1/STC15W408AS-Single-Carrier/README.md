# NF-02-PA (SI24R1) on STC15W408AS — Single-Carrier / Continuous Modulation Test

[![中文](https://img.shields.io/badge/中文-README-blue)](README.zh-CN.md)

> Official documentation: <https://docs.ai-thinker.com/2.4g_1/>

Derived from the factory example: on top of the same key-driven link it adds two certification test modes, **CW unmodulated carrier** and **MOD continuous modulated transmission**; the default is **MOD**.

- Chip: SI24R1 (software SPI, nRF24L01+ compatible)
- Project: Keil C51, `USER/SI24R1.uvproj`
- Clock: internal IRC **22.1184 MHz**; download needs a cold start

---

## 1. Layout

```
STC15W408AS-Single-Carrier/
├── USER/
│   ├── main.c            # branch entry for the three modes
│   └── SI24R1.uvproj     # Keil C51 project
├── SI24R1/
│   ├── SI24R1.c          # driver + CW / continuous modulation
│   └── SI24R1.h          # pin macros + SI24R1_TEST_MODE
├── FWLib/                # STC15 peripheral library
└── README.md
```

## 2. Test modes

In `SI24R1/SI24R1.h`:

```c
#define SI24R1_TEST_MODE  2   // 0 = normal link
                              // 1 = CW  unmodulated carrier
                              // 2 = MOD continuous modulated TX (default here)
```

| Mode | Behaviour | Parameters | Purpose |
|------|-----------|------------|---------|
| 0 | same key-driven link as `STC15W408AS-Demo` | 2440 MHz / 2 Mbps / +7 dBm | functional regression |
| 1 | `CONFIG=0x0E` → delay ≥1.5 ms → write `CONT_WAVE+PLL_LOCK` → set channel → CE high | 2475 MHz / `RF_SETUP = 0x9F` | carrier power, frequency tolerance, spurious |
| 2 | clear `EN_AA` / `SETUP_RETR`, CE high, keep feeding pseudo-random payloads into the TX FIFO | 2475 MHz / `RF_SETUP = 0x0F` | occupied bandwidth, 6 dB bandwidth, PSD |

- CW produces a single spectral line with an occupied bandwidth of only tens of kHz, so it **cannot** be used to measure OBW.
- Calling `SI24R1_TxPacket()` per packet creates on/off bursts that a swept analyser measures incorrectly; bandwidth measurements require mode 2.

## 3. Pins

| SI24R1 signal | STC15W408AS |
|---------------|-------------|
| CE   | P1.4 |
| CSN  | P1.5 |
| SCK  | P1.2 |
| MOSI | P1.3 |
| MISO | P1.0 |
| IRQ  | P1.1 |

Keys / LEDs: `KEY1 = P3.0`, `KEY2 = P3.1`, `LED3 = P1.6`, `LED4 = P3.5`.

## 4. Build and flash

1. Decide on `SI24R1_TEST_MODE` (default 2 = MOD).
2. Open `USER/SI24R1.uvproj` in Keil C51, Device = STC15W408AS, IRC 22.1184 MHz, Rebuild.
3. Download with STC-ISP, **cold start**.

## 5. Notes

Mode 0 behaves exactly like the factory example; modes 1 and 2 transmit continuously from power-up and ignore the keys, so switch back to mode 0 or power down after measuring.
