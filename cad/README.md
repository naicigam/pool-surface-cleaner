# CAD

Fusion 360 sources and exports for the floating chassis and electronics enclosure.

## Layout

```
cad/
├── exports/      # STL + STEP exports for printing / sharing
└── drawings/     # PDF technical drawings (dimensions, hole patterns)
```

## Parts list

| Part | File | Material | Notes |
|------|------|----------|-------|
| Floating ring chassis | `ring-chassis.f3d` / `ring-chassis.stl` | PETG, 100 % infill | Sealed pontoons, ~35 cm OD, weir cutout on one side |
| Weir lip insert | `weir-lip.f3d` / `weir-lip.stl` | PETG, 100 % infill | Sets the skim height ~5 mm below waterline |
| Debris chamber bottom | `chamber-bottom.f3d` | PETG, 100 % infill | Pump mount, basket seat, outlet boss |
| Mesh basket frame | `basket-frame.f3d` | PETG, 30 % infill | Holds the 200 µm filter bag |
| Electronics dome lid | `dome-lid.f3d` | PETG, 30 % infill | O-ring groove, 4× M3 inserts, 3× PG7 holes, solar panel mounting recess |
| Side outlet nozzle | `outlet-nozzle.f3d` | PETG, 100 % infill | Threaded onto chamber outlet boss with PTFE tape |

## Print settings

- Nozzle: 0.4 mm
- Layer height: 0.2 mm
- Walls: 4 perimeters
- Infill: per parts list above
- Material: PETG (Grilon3 / PrintaLot / equivalent)
- Build plate: 80 °C, glue stick or PEI
- Cooling: 30–50 % (PETG benefits from less cooling than PLA)

> **Note:** The Fusion 360 sources are not yet committed to this branch — only the README is. Add `*.f3d`, `*.stl`, and `*.step` files here as you create them. The slicer-cache files (`*.gcode`, `*.3mf`) are gitignored.
