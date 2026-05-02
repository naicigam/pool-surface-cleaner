# Build & Deployment Checklist

A step-by-step list to take the project from a pile of parts on the bench to a cleaner happily skimming the pool. Tick items off as you go.

## Phase 1 — CAD & 3D Printing

- [ ] Floating ring chassis modelled in Fusion 360 with sealed pontoons
- [ ] Weir lip insert dimensioned so the lip sits ~5 mm below waterline at design displacement
- [ ] Debris chamber + basket frame + electronics dome modelled
- [ ] Buoyancy sanity-checked: total displacement > 1.5× total system mass
- [ ] All STLs sliced at 100 % infill, PETG, 0.2 mm layers, 4 perimeters
- [ ] Parts printed and trial-fitted (dome lid closes flush, basket drops in cleanly)
- [ ] Heat-set inserts installed in lid mounting bosses

## Phase 2 — Electronics Bench Build

- [ ] ESP32-C3 SuperMini blinks `LED_BUILTIN` from a fresh PlatformIO build
- [ ] Buck converter delivers a clean 5 V from 12 V (scope check, < 50 mV ripple)
- [ ] IRF520 module switches the bilge pump on a 12 V bench supply
- [ ] Voltage dividers calibrated against a multimeter (record the ADC scale factor in `config.h`)
- [ ] Solar panel charges the LiFePO4 pack via the PWM controller (panel in sun, watch battery V rise)
- [ ] Inline 5 A fuse installed on battery positive
- [ ] Reverse-current Schottky diode installed between panel and charger
- [ ] Low-voltage cutoff verified: discharge to 11.6 V → firmware refuses to start the pump

## Phase 3 — Soldering

- [ ] Perfboard layout sketched (power rail, ground rail, ESP32 socket, MOSFET, dividers)
- [ ] All joints solid (visual + tug test)
- [ ] Every external wire passes through a PG7 cable gland — no wires through bare holes
- [ ] Heat-shrink over every solder joint that will be inside the dome
- [ ] Pump power leads use marine-grade splice + epoxy (these get wet)

## Phase 4 — Firmware

- [ ] `pio run` builds without warnings
- [ ] WiFi dashboard reachable on the local network (battery V, solar V, last run, basket-full hint)
- [ ] Manual button toggles pump on/off with debounce
- [ ] Deep sleep current measured < 100 µA (multimeter in series with battery)
- [ ] Wake-on-button verified
- [ ] Wake-on-timer verified (default: every 30 min during daylight)
- [ ] Low-battery state refuses to run and goes back to deep sleep
- [ ] Over-temperature shutdown tested (hot-air gun on the ESP32 internal sensor)

## Phase 5 — Waterproofing

- [ ] Dome lid closes on its O-ring with no gap
- [ ] Cable glands hand-tightened, then ¼ turn with a wrench (not more — they crack)
- [ ] Marine epoxy bead around the solar panel perimeter, cured 24 h
- [ ] Dry-test: dome closed, paper towel inside, dunk in a bucket for 30 minutes → towel dry afterwards
- [ ] Pressure test (optional): 5 kPa via a bike pump on a sealed dome → leak-detection fluid on every joint

## Phase 6 — Float & Pool Test

- [ ] Bath tub float test: cleaner sits level, weir lip ~5 mm below water
- [ ] Trim weights added if needed (the printed ring rarely floats perfectly level on the first try)
- [ ] Pool test in calm conditions: drops some leaves upstream of the cleaner, verifies they end up in the basket within 10 minutes
- [ ] Pool test in mild wind: cleaner doesn't capsize, doesn't beach itself on the wall
- [ ] 24-hour autonomy test: leave it in the pool overnight + a day → battery still > 12.5 V at sunset
- [ ] Weekly maintenance routine documented (rinse basket, wipe panel, check fuse)

## Safety

- [ ] Pool occupants briefed: **don't** swim with the cleaner running — it's not a child-safe device, and the pump intake can grab hair
- [ ] Cleaner removed from the pool during pool chemical shocks (chlorine spikes can attack PETG over time)
- [ ] Battery never charged unattended for the first 5 charge cycles
- [ ] Fuse spec recorded on a sticker inside the dome
