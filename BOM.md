# Bill of Materials

Estimated total: **~USD $130** / **~ARS $160.000** (at March 2026 rates)

> This BOM is adapted for availability in **Argentina**, with MercadoLibre links and local alternatives. The surface cleaner is intentionally cheaper and simpler than the [pool-vacumm](https://github.com/naicigam/pool-vacumm) — no drive train, no tether, no IMU.

## Skimming & Pump

The pump sits inside the floating ring, submerged below the waterline. Surface water flows over a weir lip into the debris chamber, through a 200 μm mesh basket, and out a side nozzle.

| Component | Qty | Est. Cost | Where to Buy | Notes |
|-----------|-----|-----------|-------------|-------|
| Bomba de achique 12V 350 GPH | 1 | ARS $25.000 | [MercadoLibre: Bomba achique 350 GPH](https://listado.mercadolibre.com.ar/bomba-de-achique-12-volts-350-gph) | Smaller than the vacuum's 1100 GPH — only needs to drive surface flow, not lift debris off the floor. |
| Bolsas filtro acuario (200 micron) | 5 | ARS $8.000 | MercadoLibre: "bolsa filtro acuario malla" | Removable debris basket, hand-washable. |
| Manguera silicona 19mm (~30 cm) | 1 | ARS $3.000 | MercadoLibre: "manguera silicona 19mm" | Pump outlet to side nozzle. |
| Abrazaderas inox (clamps) | 4 | ARS $2.000 | MercadoLibre: "abrazadera inox 16-25mm" | Hose clamps. |

**3D-printed parts** (included in filament cost):
- 1x floating ring chassis (donut shape, sealed pontoons, ~35 cm OD)
- 1x weir lip insert (sets the skimmer height ~5 mm below waterline)
- 1x debris chamber bottom (pump mount, basket seat)
- 1x mesh basket frame (snaps the filter bag in place)
- 1x **electronics dome + lid** — the rain-resistant enclosure for the battery, ESP32-C3, buck, MOSFET, BMS, etc. PETG at 100 % infill + 4 perimeters is essentially non-porous; sealing comes from the O-ring kit, PG7 glands, and marine epoxy listed under [Waterproofing & Hardware](#waterproofing--hardware) below. This is the project's IP-rated enclosure — there is no commercial box in the BOM by design.
- 1x side outlet nozzle

---

## Power (Solar + Battery)

This is the big difference from the vacuum: **no tether**. A small solar panel keeps a LiFePO4 pack topped up; the firmware's deep-sleep cycle plus a low-voltage cutoff keeps the system within the energy budget.

| Component | Qty | Est. Cost | Where to Buy | Notes |
|-----------|-----|-----------|-------------|-------|
| Panel solar 12V 10W (rígido o semiflexible) | 1 | ARS $20.000 | [MercadoLibre: Panel solar 10W](https://listado.mercadolibre.com.ar/panel-solar-10w-12v) | Mounts on top of the electronics dome. UV/water resistant. |
| Batería LiFePO4 12.8V 4Ah (con BMS) | 1 | ARS $35.000 | [MercadoLibre: bateria LiFePO4 12V 4Ah](https://listado.mercadolibre.com.ar/bateria-lifepo4-12v-4ah) | LiFePO4 preferred over Li-ion for thermal safety on a sun-baked pool. BMS integrated. |
| Controlador solar PWM 12V 5A | 1 | ARS $7.000 | [MercadoLibre: controlador solar PWM 5A](https://listado.mercadolibre.com.ar/controlador-de-carga-solar-pwm-5a) | Simple PWM is fine for 10 W. Upgrade to MPPT if you want the extra ~10 % efficiency. |
| Diodo Schottky 5A (SB560 / SR560) | 1 | ARS $1.000 | MercadoLibre: "diodo schottky 5a" | Reverse-current protection between panel and battery. |
| Fusible automotriz 5A + portafusible | 1 | ARS $2.000 | MercadoLibre: "portafusible automotriz" | Inline on battery positive. **Mandatory** — LiFePO4 packs can deliver hundreds of amps into a short. |

---

## Electronics

| Component | Qty | Est. Cost | Where to Buy | Notes |
|-----------|-----|-----------|-------------|-------|
| ESP32-C3 SuperMini | 1 | ARS $7.000 | [MercadoLibre: ESP32-C3 SuperMini](https://listado.mercadolibre.com.ar/esp32-c3-supermini) | Lower power than the WROOM-32. ~20 µA in deep sleep. WiFi for the dashboard. |
| Conversor DC-DC buck 12V→5V (MP1584 / LM2596) | 1 | ARS $3.000 | MercadoLibre: "buck mp1584" | Powers the ESP32-C3 from the 12 V bus. |
| Módulo MOSFET IRF520 | 1 | ARS $3.000 | [MercadoLibre: IRF520](https://listado.mercadolibre.com.ar/modulo-mosfet-irf520) | Switches the bilge pump on/off. Same module as the vacuum project. |
| Resistencias 100kΩ + 22kΩ (divisor de tensión) | 4 | ARS $500 | Any electronics store | 2x dividers for battery + solar voltage sensing on ADC pins. |
| Pulsador estanco IP67 (12mm) | 1 | ARS $4.000 | MercadoLibre: "pulsador estanco 12mm" | Waterproof start/stop / wake-from-sleep button. |
| Microswitch palanca (opcional) | 1 | ARS $2.000 | MercadoLibre: "microswitch palanca" | Optional float switch for "basket full" detection. |
| Perfboard (5x7 cm) | 1 | ARS $2.000 | MercadoLibre: "placa perforada 5x7" | Soldering practice board. |
| Kit conectores JST/Dupont | 1 | ARS $5.000 | MercadoLibre: "kit conectores dupont" | Modular wiring. |
| Cable 22 AWG (multi-color) | 1 | ARS $3.000 | MercadoLibre: "cable 22 awg" | Signal wiring. |

---

## Buoyancy & Chassis

| Component | Qty | Est. Cost | Where to Buy | Notes |
|-----------|-----|-----------|-------------|-------|
| Flotador pileta / pool noodle | 1 | ARS $3.000 | Any pool supply store or MercadoLibre: "flotador pileta fideos" | Cut to length, wraps the outside of the printed ring as a redundant buoyancy collar. Bright color = visibility. |
| Precintos plásticos UV (zip ties) | 1 pack | ARS $2.000 | MercadoLibre: "precintos plasticos uv" | Attach the noodle to the ring. UV-resistant only — they live in the sun. |

---

## Waterproofing & Hardware

| Component | Qty | Est. Cost | Where to Buy | Notes |
|-----------|-----|-----------|-------------|-------|
| Prensacables PG7 | 3 | ARS $3.000 | [MercadoLibre: Prensacable PG7](https://listado.mercadolibre.com.ar/prensacable-pg7) | Nylon, IP68. Pump cable + solar cable + button cable into the dome. |
| Kit O-rings NBR/EPDM | 1 set | ARS $8.000 | MercadoLibre: "kit oring goma nitrilo" | NBR or EPDM for chlorine resistance. Seals the dome lid. |
| Epoxi marino / sellador silicona | 1 | ARS $10.000 | MercadoLibre: "epoxi marino" | Waterproofing joints, sealing the panel edges, gluing nozzle threads. |
| Kit tornillos acero inoxidable M3 | 1 | ARS $5.000 | MercadoLibre: "kit tornillos inoxidable m3" | Bolts, nuts, washers. |
| Insertos roscados M3 (bronce) | 12 | ARS $3.000 | MercadoLibre: "inserto roscado m3 impresion 3d" | Heat-set into the dome and ring for the lid screws. |

---

## 3D Printing

| Component | Qty | Est. Cost | Where to Buy | Notes |
|-----------|-----|-----------|-------------|-------|
| Filamento PETG (1 kg) | 1 | ARS $15.000 | [MercadoLibre: PETG filamento](https://listado.mercadolibre.com.ar/filamento-petg) | UV + chlorine resistant. One spool is enough — the cleaner has far less printed volume than the vacuum. |

---

## Summary

| Category | Est. ARS | Est. USD |
|----------|----------|----------|
| Skimming & Pump | $38.000 | ~$31 |
| Power (panel + LiFePO4 + charger) | $65.000 | ~$53 |
| Electronics | $29.500 | ~$24 |
| Buoyancy & Chassis | $5.000 | ~$4 |
| Waterproofing & Hardware | $29.000 | ~$23 |
| 3D Printing | $15.000 | ~$12 |
| **Total** | **~ARS $181.500** | **~USD $147** |

## Notes

- Prices are estimates based on MercadoLibre Argentina (March 2026). Prices fluctuate.
- Most items offer **envío gratis** and **cuotas sin interés** on MercadoLibre.
- LiFePO4 is preferred over generic Li-ion 18650 packs for an outdoor, sun-exposed application: it tolerates higher temperatures and won't enter thermal runaway.
- A 10 W panel + 4 Ah battery sized for a 350 GPH pump running ~3 hours/day in an Argentine summer is comfortable; the firmware's deep-sleep + battery cutoff guards against winter underrun.
- Buy a spare ESP32-C3 — they cost almost nothing and you will brick at least one while bench-testing.
- Skip the WiFi dashboard if you want to stretch battery life further; the firmware already supports a WIFI_DISABLED build flag.

## Local Stores (Argentina)

**Electronics:**
- [Nubbeo](https://www.nubbeo.com.ar/) — Arduino, ESP32, sensors, modules
- [Electrocomponentes](https://www.electrocomponentes.com/) — General electronics
- [Patron](https://www.patron.com.ar/) — Electronic components
- [Sistemsoft](https://www.sistemsoft.com.ar/) — Mechatronics, motors, pumps

**Solar:**
- [Energe](https://www.energe.com.ar/) — Paneles solares, baterías LiFePO4, controladores
- [Solartec](https://www.solartec.com.ar/) — Paneles y accesorios
- MercadoLibre vendors: search "kit solar 10w 12v"

**Nautical (bombas de achique):**
- [Nautica Bigua](https://www.nauticabigua.com.ar/) — Aquafloat 350/750 GPH
- [Pedroni Nautica](https://www.pedroninautica.ar/) — Aquafloat 350 GPH
- [Jet Boats Market](https://www.jetboatsmarket.com.ar/) — Bombas de achique

## Useful References

- [Instructables: DIY solar pool skimmer](https://www.instructables.com/Solar-Powered-Pool-Skimmer/) — Earlier hobby builds for inspiration
- [Solar Pool Skimmer commercial product (Solar Breeze / Betta)](https://www.solar-breeze.com/) — Reference for proportions and intake geometry
- [LiFePO4 BMS 4S wiring](https://www.electrobotic.com.ar/blog/bms-lifepo4-4s) — Spanish-language guide
