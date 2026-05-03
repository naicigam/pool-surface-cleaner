# Pool Surface Cleaner

An open-source, DIY solar-powered floating pool surface cleaner. A simple device that floats on the pool, runs off a small solar panel and battery, and uses a single submersible pump to draw floating leaves and small debris into a removable mesh basket.

A hands-on learning project covering mechanical design (Fusion 360), 3D printing, electronics, soldering, and embedded firmware. No drive motors, no tether, no navigation — it just drifts and skims.

## Features

- Floats on the pool surface with the intake at the waterline (skimmer-style weir)
- Single 12V submersible bilge pump pulls water + floating debris into a removable mesh basket
- Small solar panel (10 W) keeps a 12 V LiFePO4 battery topped up — no tether, no plug-in
- ESP32-C3 controller with deep sleep — wakes on a timer, runs the pump while battery + light allow, sleeps again
- Waterproof start/stop button on top — tap to override and run on demand
- Optional WiFi web dashboard (battery voltage, solar voltage, runtime, basket-full hint)
- Fully 3D-printable floating ring chassis (PETG) with foam-noodle buoyancy collar
- Drifts passively with pool currents — the pump's outflow nudges it around the surface
- Safe-by-design: low-voltage cutoff, over-temperature shutdown, no exposed mains

## How it works

The cleaner is a floating ring. A weir lip on the side sits ~5 mm below the waterline, so surface water (and whatever is floating on it — leaves, pollen, bugs, hair) flows over the lip into a debris chamber. A submerged 12 V bilge pump in the bottom of the chamber pumps the now-debris-laden water down through a 200 μm mesh basket and out a side nozzle. The mesh catches the debris; the clean water exits and creates a gentle thrust that slowly nudges the cleaner around the pool surface, exposing it to new debris.

## Architecture

```
                 ┌──────── Solar Panel (10 W) ────────┐
                 │                                     │
                 ▼                                     │
        ┌──────────────────┐                           │
        │ MPPT Charger     │   ┌──────────────────┐    │
        │ (CN3791 / TP4056 │──►│ LiFePO4 4S 4 Ah  │    │
        │  + 12 V boost,   │   │ + BMS            │    │
        │  or 12 V PWM)    │   └────────┬─────────┘    │
        └──────────────────┘            │              │
                                        │ 12 V         │
                                        ▼              │
                            ┌────────────────────────┐ │
                            │  ESP32-C3   ◄──ADC──── ┼─┘  battery / solar V
                            │  + button              │
                            │  + MOSFET (IRF520)─────┼──► 12 V Bilge Pump
                            │  + WiFi (optional)     │
                            └────────────────────────┘
                                                          │
                                                          ▼
            ┌──────────────────── floating ring chassis ─────────────┐
            │  weir lip ────► debris chamber ────► mesh basket ──────┼──► clean water out
            └────────────────────────────────────────────────────────┘
                                  ▲
                          ~ waterline
```

## Project Structure

```
pool-surface-cleaner/
├── README.md              # This file
├── LICENSE                # MIT License
├── BOM.md                 # Bill of materials
├── CHECKLIST.md           # Build / test / pool-deployment checklist
├── docs/                  # Build documentation
│   ├── build-guide.md     # Step-by-step assembly
│   ├── cad-guide.md       # Step-by-step Fusion 360 modelling tutorial
│   ├── wiring-diagram.md  # Electronics wiring
│   ├── design-decisions.md
│   └── learning-journal.md
├── cad/                   # Fusion 360 & exports
│   ├── exports/           # STL/STEP files
│   └── drawings/          # Technical drawings
├── firmware/              # ESP32-C3 firmware (PlatformIO)
│   ├── platformio.ini
│   ├── src/main.cpp
│   └── include/config.h
└── electronics/           # Schematics & PCB
    └── kicad/
```

## Quick Start (Firmware)

```bash
cd firmware
# Install PlatformIO CLI: https://platformio.org/install/cli
pio run              # Compile
pio run -t upload    # Flash to ESP32-C3
pio device monitor   # Serial monitor
```

## Build Phases

1. **CAD Design** — Model the floating ring, weir lip, debris chamber, basket holder, and electronics dome in Fusion 360 (see [docs/cad-guide.md](docs/cad-guide.md) for a step-by-step tutorial)
2. **Electronics Prototyping** — Breadboard ESP32-C3 + solar charger + MOSFET pump driver
3. **Soldering** — Solder perfboard, build battery pack, waterproof every joint
4. **3D Printing** — Print PETG parts at 100 % infill; add a foam pool-noodle ring for redundant buoyancy
5. **Firmware** — Pump scheduler, battery monitor, deep-sleep cycle, optional WiFi dashboard
6. **Assembly & Testing** — Bench leak test, bath tub float test, then pool deployment

## Component Summary

| System | Component | Notes |
|--------|-----------|-------|
| Skimming | Weir lip + debris chamber + 200 μm mesh basket | Removable basket, washable |
| Pump | 12 V submersible bilge pump (350 GPH) | Submerged inside the chamber |
| Power | 10 W solar panel + LiFePO4 12.8 V 4 Ah + MPPT charger | No tether — fully off-grid |
| Brain | ESP32-C3 SuperMini | Low-power WiFi MCU, deep sleep ~20 µA |
| Sensing | Battery + solar voltage divider, IP67 button | Optional float switch for debris-full |
| Buoyancy | 3D-printed sealed pontoons + pool-noodle collar | Redundant flotation |

See [BOM.md](BOM.md) for the full bill of materials with links and prices.

## License

MIT — see [LICENSE](LICENSE) for details.
