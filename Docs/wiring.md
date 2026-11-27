# NF-02-PA (SI24R1) Wiring Guide

[中文](wiring.zh-CN.md)

This document covers only the `NF-02-PA (SI24R1)` module wiring on the AI8051U and STM32 projects under `SI24R1/`.

## AI8051U (`SI24R1/AI8051U-Demo/`, `SI24R1/AI8051U-Single-Carrier/`, Keil C51 8-bit)

Interface: software SPI (port 1) + UART1 (`P_SW1` group 1)

| NF-02-PA (SI24R1) pin | AI8051U pin | Direction | Note |
|-----------------------|-------------|-----------|------|
| VCC | 3.3V | — | must be 3.3V, never 5V |
| GND | GND | — | common ground |
| MOSI | P1.3 | MCU output | host sends data |
| MISO | P1.0 | MCU input | host receives data |
| SCK | P1.2 | MCU output | SPI clock |
| CSN | P1.5 | MCU output | chip select, active low |
| CE | P1.4 | MCU output | chip enable |
| IRQ | P1.1 | MCU input | interrupt, open drain, needs a pull-up |

**UART (debug printing, 115200-8-N-1)**

```
USB-TTL RXD  ->  AI8051U P3.7 (TxD, group 1)
USB-TTL TXD  ->  AI8051U P3.6 (RxD, group 1)
USB-TTL GND  ->  GND (common ground)
```

**LED indicators**

- P2.0: heartbeat / TX-RX indicator (active low)
- P2.0–P2.7: on-board LED bank

**Port mode configuration**

```
P1M1=0x03, P1M0=0x0F   P1.0/P1.1 input (MISO/IRQ), P1.2~P1.5 push-pull output
P2M1=0x00, P2M0=0xFF   P2 all push-pull (LEDs)
P3M1=0x00, P3M0=0x82   P3.1/P3.7 push-pull (TxD), P3.0/P3.6 quasi-bidirectional (RxD)
P4M1=0x00, P4M0=0x00   P4 quasi-bidirectional
```

---

## STM32F103C8T6 (`SI24R1/STM32F103C8T6-FreeRTOS-Demo/`, FreeRTOS)

> `SI24R1/STM32F103C8T6-FreeRTOS-Single-Carrier/` (CW) uses exactly this wiring; only the firmware behaviour changes to a continuous carrier.

Interface: software SPI

| NF-02-PA (SI24R1) pin | LoraKit silk | STM32 pin | Direction | Note |
|-----------------------|--------------|-----------|-----------|------|
| VCC | 3.3V | — | — | must be 3.3V, never 5V |
| GND | GND | — | — | common ground |
| CSN | SPI_NSS | PA4 | MCU output | chip select, active low |
| SCK | SPI_SCK | PA5 | MCU output | SPI clock |
| MISO | SPI_MISO | PA6 | MCU input | host receives data |
| MOSI | SPI_MOSI | PA7 | MCU output | host sends data |
| IRQ | LORA_DIO0 | PB0 | MCU input | interrupt, open drain, internal pull-up |
| CE | CTR1 | PA11 | MCU output | chip enable (needs a jumper wire to CTR1) |

**UART (debug printing, 115200-8-N-1)**

```
USB-TTL RXD  ->  STM32 PA9  (USART1_TX)
USB-TTL TXD  ->  STM32 PA10 (USART1_RX)
USB-TTL GND  ->  GND (common ground)
```

**LED indicators**

- PC13: heartbeat (on-board LED, toggles every 500 ms, active low)
- PA8: TX/RX success indicator (active low)
