# Wiring Diagram

Single-page reference for the electronics. The whole system runs from one 12 V LiFePO4 battery; the ESP32-C3 sips through a buck converter; the pump switches via an IRF520 module.

## Block diagram

```
                           ┌──────── Solar Panel 12V 10W ─────────┐
                           │                                      │
                           ▼                                      │
                   ┌───────────────┐                              │
                   │ Schottky 5A   │                              │
                   │ (SB560)       │                              │
                   └───────┬───────┘                              │
                           │                                      │
                           ▼                                      │
                   ┌───────────────┐    PV+    ┌──────────────┐   │
                   │ PWM Charge    │──────────►│ LiFePO4 4S   │   │
                   │ Controller 5A │    PV-    │ 12.8V 4Ah    │   │
                   │   ──────────  │    BAT+   │ + BMS        │   │
                   │   (LOAD out   │◄──────────┤              │   │
                   │   not used —  │    BAT-   │              │   │
                   │   we tap BAT) │           └──────┬───────┘   │
                   └───────────────┘                  │           │
                                                      │ + 12V     │
                                              ┌───────┴───────┐   │
                                              │ Fuse 5A       │   │
                                              └───────┬───────┘   │
                                                      │           │
                                  ┌───────────────────┼───────────┘
                                  │                   │
                                  │                   ├──────────────────────┐
                                  │                   │                      │
                                  ▼                   ▼                      ▼
                          ┌──────────────┐   ┌──────────────┐        ┌──────────────┐
                          │ Buck 12→5V   │   │ IRF520 MOSFET│        │ Voltage div  │
                          │ MP1584       │   │   VIN  ──┬── │        │ 100k / 22k   │
                          └──────┬───────┘   │   GND   GND  │        │ → ADC0 (BAT) │
                                 │           │   SIG ──── ──┘        │ 100k / 22k   │
                                 │ 5V        │   V+   ──┐            │ → ADC1 (PV)  │
                                 │           │   V-   ──┘            └──────────────┘
                                 ▼           └──────┬───┬──┘
                          ┌──────────────┐          │   │
                          │ ESP32-C3     │          │   │   to pump +/-
                          │  SuperMini   │ ── GPIO 4 ┘   │
                          │              │              │
                          │  GPIO 0 (ADC)│◄── BAT divider
                          │  GPIO 1 (ADC)│◄── PV divider
                          │  GPIO 2 (IN) │◄── IP67 button (other side to GND)
                          │  GPIO 3 (BTN)│◄── (reserved: float switch / basket-full)
                          └──────────────┘
```

## Pin assignments (ESP32-C3 SuperMini)

| Pin | Direction | Purpose | Notes |
|-----|-----------|---------|-------|
| GPIO 0 | ADC IN | Battery voltage (post-divider) | Scale: V_bat = ADC × (122 / 22) × V_ref / 4095 |
| GPIO 1 | ADC IN | Solar/PV voltage (post-divider) | Same divider scaling |
| GPIO 2 | DIGITAL IN | Waterproof button | INPUT_PULLUP, button to GND, debounced 50 ms |
| GPIO 3 | DIGITAL IN | Optional basket-full float switch | INPUT_PULLUP, reserved |
| GPIO 4 | DIGITAL OUT (PWM) | Pump MOSFET gate signal | LEDC channel 0, 1 kHz, 0–255 |
| GPIO 8 | DIGITAL OUT | On-board LED | Heartbeat / status |

GPIO 9 is the bootstrap pin on the C3 SuperMini — leave it free.

## Power budget

| Mode | ESP32-C3 | Pump | Buck idle | Total | Battery (4 Ah, 12.8 V = ~51 Wh) |
|------|----------|------|-----------|-------|---------------------------------|
| Deep sleep | 20 µA | 0 | 1 mA | ~1 mA | months |
| Idle WiFi associated | 60 mA | 0 | 1 mA | 60 mA | ~35 h |
| Pumping (no WiFi) | 25 mA | ~2.5 A | 1 mA | ~2.5 A | ~1.6 h |
| Pumping + WiFi dashboard | 80 mA | ~2.5 A | 1 mA | ~2.6 A | ~1.5 h |

Solar generation (10 W panel, derate to 7 W average in sun, 5 sun-hours/day in Argentine summer) ≈ 35 Wh/day. That covers ~3 hours of pumping per day — comfortable. In winter the firmware's daylight detection (via PV voltage) automatically reduces runtime.

## Notes & gotchas

- **The PWM controller's LOAD output is not used.** It would cut power to the ESP32 along with the pump on low battery, dropping us off WiFi. We tap directly off BAT+ through our own fuse and rely on the firmware's low-voltage cutoff.
- **IRF520 is a logic-level-friendly MOSFET module**, not a bare IRF520. It includes a gate-driver circuit. If you sub a bare IRF520, add a 2N2222 driver — its V_GS(th) is too high for direct ESP32 drive.
- **The button on GPIO 2 also wakes from deep sleep.** That pin is RTC-capable on the C3.
- **Don't share the battery ground with USB ground while flashing.** Flash with the battery disconnected, or you'll back-feed 5 V into the buck output.
