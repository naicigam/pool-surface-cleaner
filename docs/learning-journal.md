# Learning Journal

A loose log of things learned, mistakes made, and rabbit holes worth (or not worth) going down. Append-only — the dated entries stay even when the design moves on.

## Entry 1 — Picking the pump (March 2026)

Started by trying a 1100 GPH bilge pump from the parts bin. Bench-tested it floating in a 200 L drum with a 3D-printed weir lip — way too much suction, the cleaner dipped its weir under the surface and started swallowing whole leaves into the chamber faster than the basket could pass them. Backed off to a 350 GPH pump and the skim flow looks right: leaves drift over the lip without being yanked.

Lesson: for a *skimmer*, you want flow rate, not lift. The 350 GPH pump barely lifts water, but it moves a lot of it horizontally, which is exactly what surface skimming needs.

## Entry 2 — PETG vs. PLA (March 2026)

Printed the first ring in PLA "just to test the geometry". Left it floating in a chlorinated bucket overnight. By morning the surface was crazed and the part had slightly warped at one of the layer-line junctions. Reprinted in PETG; no change after a week in the same bucket.

Lesson: chlorine + UV + sustained water contact = PETG, end of discussion. PLA is for prototypes that live indoors.

## Entry 3 — The buck converter that wasn't (April 2026)

Spent an evening chasing a "phantom reset" where the ESP32 would brown out every few seconds when the pump kicked on. Scope showed the 5 V rail dipping to ~3.7 V at pump start. Turned out the buck I'd grabbed was a sketchy MP1584 clone with no input capacitor populated. Soldered a 470 µF 25 V electrolytic to the input pads, problem gone.

Lesson: when buying $1 buck modules, expect to add the input cap they "forgot" to populate.

## Entry 4 — Deep sleep current was higher than spec (April 2026)

Datasheet says the C3 should pull < 50 µA in deep sleep. I was measuring 4 mA. Two culprits:

1. The buck converter's quiescent current (the MP1584 pulls ~1.5 mA even with no load — replaced with a TPS62203 module for ~50 µA quiescent).
2. The IRF520 module has a 10 kΩ pull-down on the gate driver's input, but also a status LED that's *on* whenever VIN is present. Cut the trace to the LED, dropped 2 mA.

Final figure: ~80 µA in deep sleep. Good enough.

Lesson: spec sheet quiescent currents are aspirational. Always measure with a multimeter in series with the battery before declaring victory.

## Entry 5 — Where leaves actually float (April 2026)

The cleaner doesn't need to chase leaves. After a couple of afternoons of watching the pool, I realised the pool's own circulation return pushes everything to one corner within 30 minutes. Just letting the cleaner sit near that corner gathers ~80 % of the floating debris with no movement at all. The remaining 20 % comes in as the cleaner's own pump-thrust slowly rotates it around its convergence zone.

Confirms the design decision to skip active drive. Score one for laziness.

## Entry 6 — The basket clogs faster than expected with pollen (May 2026)

In late spring the local pollen drop is heavy. The 200 µm basket clogs in ~3 hours, not the days I expected with autumn leaves. Added a daily "rinse the basket" item to the maintenance README. Considered an auto-backwash mechanism — rejected as too much complexity for a "small cleaner" project.

Lesson: filter sizing is seasonal. The cleaner needs more attention in pollen season than in leaf season.

## Things to try next

- A second mesh layer (1 mm pre-filter in front of the 200 µm) to keep the pollen on the *outside* of the basket, where rinsing is easier.
- A reflective sticker pattern on top so the cleaner's position is visible at night.
- Logging battery V to the WiFi dashboard's history endpoint, then a tiny SwiftUI widget on my phone.
