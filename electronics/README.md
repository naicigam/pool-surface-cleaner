# Electronics

KiCad schematics and (optionally) PCB layout for the surface cleaner.

## Layout

```
electronics/
└── kicad/        # KiCad project: schematic + (future) PCB
```

## Schematic overview

See [../docs/wiring-diagram.md](../docs/wiring-diagram.md) for an ASCII version. The KiCad schematic mirrors that block diagram exactly.

### Sheets

1. **Power** — solar panel input, Schottky diode, PWM charge controller, LiFePO4 pack with BMS, fuse, buck-to-5V.
2. **MCU** — ESP32-C3 SuperMini with battery + PV voltage dividers and the IP67 button.
3. **Pump driver** — IRF520 module wired between 12 V rail and pump.

## PCB

For v1 the build uses a hand-soldered perfboard, not a custom PCB. A KiCad PCB layout is planned for v2 once the perfboard prototype has accumulated enough field hours that the design is stable.

> **Note:** No KiCad files are committed yet — only this README. Add `kicad/pool-surface-cleaner.kicad_pro` and friends here when you start the schematic capture.
