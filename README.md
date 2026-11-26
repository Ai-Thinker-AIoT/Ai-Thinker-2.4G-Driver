<div align="center">

# Ai-Thinker 2.4G Driver

</div>

[![中文](https://img.shields.io/badge/中文-README-blue)](README.zh-CN.md)

**Ai-Thinker 2.4G Driver** — driver and example firmware collection for the Ai-Thinker 2.4G module family, covering **4 chips** (SI24R1, BK2425, CI24R1, NRF24L01+) and **12 module models**. Each chip has its own directory at the repository root.

---

## 1. Repository Layout

Firmware projects are grouped by chip, one directory per chip at the repository root.

### NF-03, NF-01-S, NF-02-PA, NF-02-SM, NF-02-PE (SI24R1)

**Chip directory**: [`SI24R1/`](SI24R1/)

| Directory | MCU / Toolchain | Contents | Project file |
|-----------|-----------------|----------|--------------|
| [`STC15W408AS-Demo`](SI24R1/STC15W408AS-Demo/) | STC15W408AS / Keil C51 | **Original factory demo** for the Ai-Thinker 2.4G test board: two boards control each other's LEDs via buttons | `USER/SI24R1.uvproj` |
| [`AI8051U-Demo`](SI24R1/AI8051U-Demo/) | AI8051U (8-bit) / Keil C51 | **Normal packet-link version** (baseline from the weekly-summary archive): UART logging, TX/RX self-test; no carrier test modes | `AI8051U-SI24R1.uvproj` |
| [`STM32F103C8T6-FreeRTOS`](SI24R1/STM32F103C8T6-FreeRTOS/) | STM32F103C8T6 / MDK-ARM + FreeRTOS | Software-SPI driver with heartbeat and radio tasks | `Project.uvprojx` |
| [`STM32F103C8T6-FreeRTOS-Single-Carrier`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/) | STM32F103C8T6 / Keil ARM-GNU (arm-none-eabi-gcc) + FreeRTOS | **Single-carrier (CW) test firmware** copied from the project above: boots straight into a continuous unmodulated 2475 MHz carrier (`RF_SETUP = 0x9F`), CE held high, heartbeat task keeps running; UART banner only, no packet link | `Project.uvprojx` |
| [`STC15W408AS-Single-Carrier`](SI24R1/STC15W408AS-Single-Carrier/) | STC15W408AS / Keil C51 | **Single-carrier test demo** branched from the factory example: CW unmodulated carrier and continuous modulated TX (defaults to MOD) | `USER/SI24R1.uvproj` |
| [`AI8051U-Single-Carrier`](SI24R1/AI8051U-Single-Carrier/) | AI8051U (8-bit) / Keil C51 | AI8051U port of the same single-carrier test framework; `SI24R1_TEST_MODE` = 0 / 1 / 2 (defaults to 1, CW) | `AI8051U-SI24R1.uvproj` |

**Test captures** — the two screenshots below are SI24R1 packet-link runs, captured from two serial terminals (one TX, one RX) at 115200-8-N-1.

**[`AI8051U-Demo`](SI24R1/AI8051U-Demo/)** — AI8051U, 32-byte incremental payload

![AI8051U SI24R1 TX/RX log](Assets/ScreenShot_2026-11-05_160920_306.png)

**[`STM32F103C8T6-FreeRTOS`](SI24R1/STM32F103C8T6-FreeRTOS/)** — STM32F103C8T6 + FreeRTOS, `SI24R1_ROLE = RX`

![STM32 FreeRTOS SI24R1 TX/RX log](Assets/ScreenShot_2026-11-05_174444_181.png)

**[`STM32F103C8T6-FreeRTOS-Single-Carrier`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/)** prints a one-shot boot banner over USART1 (`CW mode: carrier on, forever` / `RF = 2475 MHz, RF_SETUP = 0x9F`) and then emits the carrier; there is no packet log to capture — verify it on a spectrum analyser instead.

**Spectrum captures** — narrow-span swept measurements on a Keysight swept SA (Ref 30 dBm, 10 dB/div, 40 dB attenuation), both around the 2475 MHz carrier.

**[`STM32F103C8T6-FreeRTOS-Single-Carrier`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/)** — center 2.47498 GHz, span 25 kHz, RBW/VBW 240 Hz; marker `Mkr1 2.474976620 GHz` reads **20.50 dBm**. A single spectral line: unmodulated carrier, OBW only tens of kHz.

![STM32 CW carrier, 25 kHz span](Assets/3.jpg)

**[`AI8051U-Single-Carrier`](SI24R1/AI8051U-Single-Carrier/)** — center 2.47498 GHz, span 223.7 kHz, RBW/VBW 2.2 kHz (`PNO Wide`); marker reads **20.16 dBm**. Besides the main line there are discrete side lines, so this trace is **not** a clean single-line CW — check `SI24R1_TEST_MODE` on the board before using it as a CW reference.

![AI8051U carrier, 223.7 kHz span](Assets/4.jpg)

> The status bar in the first capture shows `RF Alignment Failure`; treat absolute power readings as indicative rather than calibrated.

### NF-04, NF-04-MI (BK2425)

**Chip directory**: [`BK2425/`](BK2425/)

| Directory | MCU / Toolchain | Contents | Project file |
|-----------|-----------------|----------|--------------|
| [`STC15W408AS-Demo`](BK2425/STC15W408AS-Demo/) | STC15W408AS / Keil C51 | BK2425 factory STC15 example (software-SPI driver, button/LED demo) | `USER/BK2425.uvproj` |

### NF-05, NF-05-S (CI24R1)

**Chip directory**: [`CI24R1/`](CI24R1/)

| Directory | MCU / Toolchain | Contents | Project file |
|-----------|-----------------|----------|--------------|
| [`STC15W408AS-Demo`](CI24R1/STC15W408AS-Demo/) | STC15W408AS / Keil C51 | CI24R1 factory STC15 example (software-SPI driver, button/LED demo) | `USER/CI24R1.uvproj` |

### NF-01-N, NF-02-PA, NF-02-PE (NRF24L01+)

**Chip directory**: [`NRF24L01P/`](NRF24L01P/)

| Directory | MCU / Toolchain | Contents | Project file |
|-----------|-----------------|----------|--------------|
| [`STC15W408AS-Demo`](NRF24L01P/STC15W408AS-Demo/) | STC15W408AS / Keil C51 | NRF24L01+ factory STC15 example (software-SPI driver, button/LED demo) | `USER/nRF24L01P.uvproj` |

---

## 2. Pin Mapping

Pin mapping below uses `NF-02-PA(SI24R1)` as the reference module (SI24R1 based, nRF24L01+ compatible, up to **+7 dBm** TX power).

| NF-02-PA(SI24R1) | STC15W408AS | AI8051U | STM32F103C8T6 |
|--------------------|-------------|---------|---------------|
| CE     | P1.4        | P1.4    | PA11 (CTR1) |
| CSN    | P1.5        | P1.5    | PA4 (SPI_NSS) |
| SCK    | P1.2        | P1.2    | PA5 (SPI_SCK) |
| MOSI   | P1.3        | P1.3    | PA7 (SPI_MOSI) |
| MISO   | P1.0        | P1.0    | PA6 (SPI_MISO) |
| IRQ    | P1.1        | P1.1    | PB0 (LORA_DIO0) |

- Both STM32 projects ([`STM32F103C8T6-FreeRTOS`](SI24R1/STM32F103C8T6-FreeRTOS/) and [`STM32F103C8T6-FreeRTOS-Single-Carrier`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/)) use the same pin map above; only the firmware behaviour differs.
- Debug UART: AI8051U = P3.7 (TX) / P3.6 (RX); STM32 = PA9 (TX) / PA10 (RX); both 115200-8-N-1.
- Full port-mode configuration: [`Docs/wiring.zh-CN.md`](Docs/wiring.zh-CN.md).

---

## 3. RF Parameters

| Project | RF_CH | Frequency | Rate / Power (`RF_SETUP`) | Auto retransmit | CRC |
|---------|-------|-----------|---------------------------|-----------------|-----|
| [`STC15W408AS-Demo`](SI24R1/STC15W408AS-Demo/) | 40 | 2440 MHz | 2 Mbps / +7 dBm (`0x0F`) | 250us+86us x10 (`0x0A`) | 16 bit |
| [`AI8051U-Demo`](SI24R1/AI8051U-Demo/) | 40 | 2440 MHz | 1 Mbps / +7 dBm (`0x07`) | 500us x10 (`0x1A`) | 16 bit |
| [`STM32F103C8T6-FreeRTOS`](SI24R1/STM32F103C8T6-FreeRTOS/) | 100 | 2500 MHz | 1 Mbps / +7 dBm (`0x07`) | 500us x15 (`0x1F`) | 16 bit |
| [`STM32F103C8T6-FreeRTOS-Single-Carrier`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/) | 75 | 2475 MHz | 2 Mbps / +7 dBm CW (`0x9F`) | disabled | 16 bit |
| [`AI8051U-Single-Carrier`](SI24R1/AI8051U-Single-Carrier/) (`TEST_MODE=0`) | 100 | 2500 MHz | 1 Mbps / +7 dBm (`0x07`) | 500us x15 (`0x1F`) | 16 bit |
| `*-Single-Carrier` (`TEST_MODE=1/2`, 8-bit projects only) | 75 | 2475 MHz | 2 Mbps / +7 dBm (`0x0F`; CW adds `CONT_WAVE+PLL_LOCK` -> `0x9F`) | disabled | 16 bit |

Common settings: address `0A 01 07 0E 01` (5 bytes), 32-byte payload, auto-ACK on pipe 0, 16-bit CRC.

---

## 4. Single-Carrier Test Modes (CW / MOD)

- [`AI8051U-Single-Carrier/Source/SI24R1.h`](SI24R1/AI8051U-Single-Carrier/Source/SI24R1.h) — `SI24R1_TEST_MODE` defaults to **1** (CW)
- [`STC15W408AS-Single-Carrier/SI24R1/SI24R1.h`](SI24R1/STC15W408AS-Single-Carrier/SI24R1/SI24R1.h) — `SI24R1_TEST_MODE` defaults to **2** (continuous modulated TX)
- [`STM32F103C8T6-FreeRTOS-Single-Carrier`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/) — no mode switch at all: it is a **CW-only** build (`main.c` calls `SI24R1_CW_Mode()` once and returns to the scheduler). Carrier is set by [`Hardware/SI24R1/SI24R1.h`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/Hardware/SI24R1/SI24R1.h) (`RF_CH_CW` 75 -> 2475 MHz, `RF_SETUP_CW` `0x9F`); change the channel/power there. For MOD, reuse the pseudo-random FIFO feeding loop from the AI8051U project and clear `EN_AA` / `SETUP_RETR`.

```c
#define SI24R1_TEST_MODE  0   // 0 = normal packet link
                              // 1 = CW  : unmodulated carrier, for power / frequency tolerance / spurious
                              // 2 = MOD : continuous modulated TX, for OBW / 6 dB BW / PSD
```

| Mode | Sequence | RF_SETUP | Purpose |
|------|----------|----------|---------|
| 1 CW | `CONFIG=0x0E` -> wait >=1.5 ms -> `CONT_WAVE+PLL_LOCK` -> set RF_CH -> hold CE high | `0x9F` | Single spectral line; OBW is only tens of kHz |
| 2 MOD | Clear EN_AA / SETUP_RETR, hold CE high, keep feeding pseudo-random payloads into TX FIFO | `0x0F` | A genuinely continuous modulated carrier |

- Calling `SI24R1_TxPacket()` per packet produces an on/off burst train that a swept spectrum analyser cannot measure correctly — use mode 2.

---

## 5. Building and Flashing

| Project | Environment | Steps |
|---------|-------------|-------|
| STC15 projects (SI24R1 / BK2425 / NRF24L01+) | Keil C51 | Open the matching `USER/*.uvproj` (`SI24R1.uvproj` / `BK2425.uvproj` / `nRF24L01P.uvproj`), Device = STC15W408AS, internal IRC 22.1184 MHz; flash with STC-ISP |
| STC15 project (CI24R1) | Keil C51 | Open `USER/CI24R1.uvproj`, internal IRC **11.0592 MHz** (different from the other three), cold-start when downloading; flash with STC-ISP |
| AI8051U projects | Keil C51 | Run `build.bat` or open `.uvproj`; clock 33.1776 MHz; 8-bit mode |
| [`STM32F103C8T6-FreeRTOS`](SI24R1/STM32F103C8T6-FreeRTOS/) | Keil MDK + `Keil.STM32F1xx_DFP` | Open `Project.uvprojx`, Define includes `STM32F10X_MD,USE_STDPERIPH_DRIVER`; flash `Objects/Project.axf` with ST-Link |
| [`STM32F103C8T6-FreeRTOS-Single-Carrier`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/) | Keil MDK with the **ARM-GNU** (arm-none-eabi-gcc) toolset | Open `Project.uvprojx`; startup comes from [`gcc/startup_stm32f103xb.s`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/gcc/startup_stm32f103xb.s), link map from [`gcc/stm32f103c8.ld`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/gcc/stm32f103c8.ld), [`gcc/syscalls.c`](SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/gcc/syscalls.c) supplies newlib stubs; `CreateHexFile=1` also emits `Project.hex` |

- AI8051U projects default to TX: set `SI24R1_ROLE` to `SI24R1_ROLE_RX` in `SI24R1.h` to build the receiver firmware (or override via a preprocessor define).
- [`AI8051U-Single-Carrier`](SI24R1/AI8051U-Single-Carrier/) defaults to `SI24R1_TEST_MODE = 1` (CW); set it to 0 for the normal packet link or 2 for MOD before building. [`STC15W408AS-Single-Carrier`](SI24R1/STC15W408AS-Single-Carrier/) defaults to MOD.
- STM32 defaults to RX: change `SI24R1_ROLE` in [`Hardware/SI24R1/SI24R1.h`](SI24R1/STM32F103C8T6-FreeRTOS/Hardware/SI24R1/SI24R1.h); one board must transmit, the other receive. No fromelf post-build step is configured, so the build produces only `.axf`; enable one under Options for Target -> User -> After Build if a `.hex` file is needed.
- The STM32 CW project ignores `SI24R1_ROLE` at runtime (it never enters RX/TX packet mode) and uses FreeRTOS heap `12 * 1024` instead of 17 KB; `configCHECK_HANDLER_INSTALLATION` is explicitly `0` because that firmware has not been re-validated on hardware. If you re-enable it, define `configASSERT()` as well.
