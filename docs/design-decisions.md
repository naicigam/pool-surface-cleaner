# Design Decisions

A running log of the choices behind this build, and what we explicitly rejected. Useful when you come back to the project six months later and wonder why something is the way it is.

## Why solar instead of tethered

A tether would solve power, but a cleaner that drifts freely all over the pool surface and a power cable in the water are an entanglement nightmare — for the cleaner, for swimmers, and for the pool's own skimmer/return plumbing. Surface cleaning is a "few hours per day" job, well within reach of a small solar panel + battery. Going off-grid lets the cleaner be a self-contained float you toss in and forget about.

## Why LiFePO4 instead of Li-ion 18650

Three reasons:

1. **Thermal safety in direct sun.** A black-domed pool float will easily reach 60 °C internal temperature on a summer afternoon. LiFePO4 cells stay safe to ~70 °C; Li-ion NMC starts losing cycle life rapidly above 45 °C and can enter thermal runaway in fault conditions. We don't want a battery fire on a pool.
2. **Cycle life.** LiFePO4 packs reach 2000+ cycles at 100 % DoD. The cleaner will charge-discharge daily; with Li-ion we'd be replacing the pack every two seasons.
3. **The voltage matches.** A 4S LiFePO4 nominal 12.8 V is a near-perfect match for a 12 V bilge pump. A 3S Li-ion (11.1 V) underdrives the pump.

The cost difference is small at this capacity (4 Ah).

## Why ESP32-C3 instead of ESP32-WROOM-32

The cleaner's compute load is trivial — 1 pump output, 2 ADC inputs, 1 button, optional WiFi. The C3 wins on the only metric that matters here: power.

- Pulls far less idle current (~20 µA in deep sleep vs. ~10 mA on a stock WROOM-32 dev board — a 500× factor that matters on a 4 Ah battery)
- Costs about half as much
- A single RISC-V core is plenty for this workload

If you only have a WROOM-32 in the parts bin, it works — just tune `WIFI_DUTY_CYCLE_S` down to compensate for the higher idle draw.

## Why a passive-drift design instead of motors

We considered adding paddlewheels to actively patrol the surface. Rejected for three reasons:

1. **Power.** A pair of motor-driven paddlewheels would consume far more than a single 350 GPH pump, blowing the solar budget.
2. **Coverage.** Pool surface currents (from the pool's circulation pump) and the cleaner's own pump-thrust nozzle do most of the work for free. Empirically, commercial solar pool skimmers (Solar Breeze, Betta) use the same passive-drift principle.
3. **Complexity.** A drive train, IMU, and bumpers would balloon the BOM and the firmware for marginal cleaning gains.

## Why a 200 µm filter mesh instead of finer

200 µm catches everything visible to the eye (leaves, bugs, hair, large pollen clumps) without restricting flow enough to stall the pump. We tested 100 µm and 50 µm — both reduced pump flow by > 50 % within an hour as fine algae and pollen built up. 200 µm clogs slowly enough that weekly basket rinses are sufficient.

## Why no autonomous "find the debris" navigation

A surface cleaner doesn't need to seek debris — debris floats to wherever the pool's circulation currents converge (typically near the skimmer return). The cleaner just needs to be in roughly the right area, and pool currents bring the debris to it. Adding active navigation would require GPS or visual odometry on a 35 cm chassis, which is wildly out of scope.

If the pool has no circulation, the cleaner's own outflow nozzle (angled tangent to the ring) gives it a slow rotational drift that covers the surface over hours.

## Why not pump straight into a hose to drain

We considered piping the pump output to the pool's deck drain to actually remove water (and debris with it). Rejected because:

1. It requires a tether, defeating the standalone solar design.
2. Pool water removal is the pool's own circulation system's job.
3. Catching debris in an on-board basket means the basket can be emptied without disturbing the cleaner.

## Open questions / future iterations

- **Basket-full detection.** The optional float switch on GPIO 3 is wired but unused in firmware v1. v2 will add it as a wake-and-blink-LED notification.
- **Storm mode.** No storm/wind detection yet. A pool full of leaves after a windstorm overwhelms the basket within an hour. A future version could use the IMU (if added) to detect rough water and just go to sleep until conditions calm.
- **Flow-rate sensing.** A small Hall-effect flow sensor on the outlet hose would let us detect a clogged basket before flow drops to zero. Considered, deferred.
