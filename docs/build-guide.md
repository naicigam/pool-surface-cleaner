# Build Guide

Step-by-step assembly for the pool surface cleaner. Plan on a long weekend if you're starting from zero, or an evening if you already have the printed parts and the perfboard soldered.

## 1. Print the chassis

1. Slice all parts in `cad/exports/` at:
   - PETG, 245 °C nozzle, 80 °C bed
   - 0.2 mm layers, **100 % infill** for the ring (water resistance), 30 % for the dome
   - 4 perimeters everywhere
2. Print order:
   1. Floating ring (longest print, ~12 h)
   2. Debris chamber bottom + basket frame
   3. Weir lip insert
   4. Electronics dome + lid
   5. Side outlet nozzle
3. After printing the ring, fill any visible layer lines on the **outside surface** with a thin coat of marine epoxy. PETG at 100 % infill is very water resistant but not water-proof; the epoxy seals the last 1 % of porosity.

## 2. Heat-set the threaded inserts

The dome lid bolts onto the dome with M3 stainless screws into brass heat-set inserts. Use a soldering iron at ~250 °C and press the insert in flush. Aim for square — a leaning insert will pull the lid out of plane and break the O-ring seal.

## 3. Mount the pump

1. The pump sits in the centre well of the debris chamber, intake facing up.
2. Connect the silicone hose to the pump outlet, route it through the chamber wall, and clamp it to the side outlet nozzle.
3. Use stainless hose clamps; nylon zip ties will UV-degrade in a year.
4. Splice the pump's stock leads to a longer pair using marine-grade butt splices, then dip the splice in marine epoxy. This joint will sit underwater 24/7.

## 4. Build the electronics on perfboard

See [wiring-diagram.md](wiring-diagram.md) for the full schematic. In short:

- 12 V from battery → fuse → buck → 5 V to ESP32-C3
- 12 V from battery → IRF520 drain → pump positive
- IRF520 gate ← ESP32 GPIO 4 (PWM-capable, not strictly needed, but lets us soft-start the pump)
- Battery + → 100 kΩ + 22 kΩ divider → ESP32 GPIO 0 (ADC)
- Solar + (post-controller load output) → 100 kΩ + 22 kΩ divider → ESP32 GPIO 1 (ADC)
- IP67 button → GPIO 2 with INPUT_PULLUP, the other terminal to GND
- Pump cable, solar cable, button cable each enter the dome through their own PG7 gland

Check every solder joint with a magnifier. Bridge a pair of joints once and you'll smoke an ESP32 the first time you connect the battery.

## 5. Assemble the buoyancy collar

1. Cut the pool noodle into a length matching the outer circumference of the printed ring.
2. Slit it lengthwise with a kitchen knife so it wraps around the ring.
3. Zip-tie it on every ~10 cm with UV-resistant zip ties. Trim the zip-tie tails flush.

## 6. Mount the solar panel

1. The panel sits on top of the dome lid with its long axis aligned to the cleaner's natural drift direction (so it self-shades less when tilted by waves).
2. Run the panel's leads into the dome through a dedicated PG7 gland.
3. Bead marine epoxy around the panel's perimeter where it meets the lid. Cure 24 h before water testing.

## 7. Flash the firmware

```bash
cd firmware
cp include/secrets.example.h include/secrets.h
# edit include/secrets.h with your WiFi credentials, or leave WIFI_SSID empty for offline mode
pio run -t upload
pio device monitor
```

Watch the serial monitor as the cleaner boots: it should report battery voltage, solar voltage, and either a WiFi IP or "WIFI_DISABLED".

## 8. Bench leak test

1. Dome closed, paper towel inside.
2. Submerge to lid level in a bucket for 30 minutes.
3. Open and inspect — the towel must be bone dry. Any moisture means a gland or O-ring needs attention.

## 9. Bath-tub float test

1. Drop the cleaner in the bath. Note the waterline.
2. The weir lip should be ~5 mm below the surface. If it's high, add ballast (a few stainless M3 nuts in the dome). If it's swamped, you printed something at the wrong density — recheck infill.

## 10. Pool deployment

1. Charge the battery to full from the panel in the sun for one clear day before first deployment.
2. Drop the cleaner in the pool, press the button, watch it run.
3. After 10 minutes, fish it out, open the basket — there should be visible debris. If not, the weir lip is sitting too high (skim isn't engaging) or the pump isn't priming.

## Maintenance

- **Weekly:** rinse the basket, wipe the solar panel.
- **Monthly:** check the inline fuse, inspect the pump cable splice, look for UV damage on the noodle.
- **Yearly:** replace the noodle and zip ties, re-bead the panel's epoxy if it shows lifting at the edges.
