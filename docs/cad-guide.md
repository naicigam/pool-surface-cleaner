# Fusion 360 CAD Guide

This is a step-by-step tutorial that builds every printable part in Fusion 360 from a blank file. It's written for someone who has installed Fusion 360 and watched maybe one introductory video — every click is spelled out, and every design choice has a "why" attached. By the time you finish, you'll have the full set of STLs ready to slice, *and* you'll have practiced the core Fusion 360 workflow (sketches → features → parameters → bodies → components → joints → export).

If you've never opened Fusion 360 before, work through Autodesk's free [Fusion 360 for Hobbyists](https://www.autodesk.com/learn/ondemand/learning-path/fusion-for-hobbyists) primer first — it's about 90 minutes and covers the navigation cube, sketch constraints, and the Timeline. This guide assumes you can find the **Create**, **Modify**, and **Sketch** menus on the toolbar.

---

## 0. Set up the project

**Why a project before any sketches:** Fusion 360 organises everything into projects. If you start sketching in the Untitled file that opens by default, you'll regret it the first time the cloud sync gets confused about where things live.

1. **File → New Project**, name it `pool-surface-cleaner`.
2. Inside that project, **New Design**. **File → Save As**, name it `01-ring-chassis`. We'll save each part as its own design file, which is the convention for printed parts that you want to slice individually.
3. **Modify → Change Parameters → User Parameters → +**. Add the following. *Why now:* every dimension we'll sketch is going to reference one of these, so when (not if) the first float test shows the ring sitting too high in the water, we change one number here and the whole assembly re-builds itself. This is the single most important habit to learn in Fusion 360.

   | Name | Unit | Expression | Comment |
   |------|------|------------|---------|
   | `D_ring_outer` | mm | 350 | Floating ring outer diameter |
   | `D_ring_inner` | mm | 240 | Floating ring inner / chamber opening |
   | `H_ring`       | mm | 70  | Ring overall height |
   | `H_waterline`  | mm | 35  | Designed waterline above ring bottom |
   | `H_weir`       | mm | 30  | Weir lip top above ring bottom (= waterline − 5) |
   | `D_pump_outer` | mm | 55  | Bilge pump body diameter |
   | `D_basket_top` | mm | 200 | Basket frame top diameter |
   | `H_basket`     | mm | 90  | Basket frame height |
   | `D_dome_outer` | mm | 180 | Electronics dome diameter |
   | `H_dome`       | mm | 60  | Electronics dome interior height |
   | `D_panel`      | mm | 145 | Solar panel short-axis fit |
   | `t_wall`       | mm | 4   | Default wall thickness |

4. Close the dialog. Save.

---

## Part 1 — Floating ring chassis

The big donut that holds everything else up. We're going to build it in this order: outer shape → hollow it out for buoyancy → cut a notch for the weir → fillets. Each step introduces one Fusion 360 concept.

### 1.1 The first sketch — outer profile

**Concept introduced:** sketches and the difference between a sketch and a body.

A *sketch* is 2D and lives on a plane. A *body* is 3D and gets created by features (extrude, revolve, sweep, loft) that consume sketches.

1. **Solid → Create Sketch**. Pick the **XY plane** (the horizontal one in the navigation cube — fingers crossed Fusion 360 picked the orientation you expect; if not, click the home icon).
2. **Create → Circle → Center Diameter Circle**. Click the origin (the little triangle marker), then type `350` and press Enter. *Why type instead of drag:* in CAD, dragging is for humans drawing on napkins; typing is for engineers. We always set dimensions explicitly.
3. Hover over the circle, press `D` (the shortcut for Sketch Dimension), click the circle, click somewhere off to the side. In the popup, type `D_ring_outer` (not `350` — we want the parameter, not a hard-coded number). Press Enter.
4. Repeat steps 2–3 for an inner circle, dimension `D_ring_inner`.
5. **Finish Sketch** (top right).

What you should see: two concentric circles on the XY plane, both linked to user parameters (visible as `fx:` in the dimension labels). Saving now isn't a bad habit.

### 1.2 First feature — extrude the donut

**Concept introduced:** features, bodies, the Timeline.

1. **Create → Extrude** (shortcut `E`). The pointer turns into a "select profile" cursor.
2. Click the *annular region* between the two circles — that's the shape we want to push up into 3D. (Click the inner disc and you get a solid puck instead. Click outside both and Fusion 360 yells at you.)
3. In the dialog: **Distance** = `H_ring`, **Operation** = New Body. Click OK.

Look at the bottom of the screen — the **Timeline**. There's now a tiny icon for the sketch and another for the extrude. *Why this matters:* you can right-click any timeline icon → Edit Feature, change a value, and Fusion 360 will rebuild every later step that depends on it. Parametric design only works if you trust the Timeline.

In the Browser (left), expand **Bodies** — there's `Body1`. Right-click → **Rename** → `RingBody`. Naming bodies as you create them saves your future self.

### 1.3 Hollow the pontoons

**Why:** PETG at 100 % infill is dense — about 1.27 g/cm³, denser than water. A solid PETG ring would sink. We hollow it into a sealed donut so the trapped air provides buoyancy. The walls stay 4 mm thick (= `t_wall`) for waterproofness.

**Concept introduced:** sketching on a face (instead of a plane).

1. **Create Sketch**. This time, don't pick a plane — click the **top face** of `RingBody`. Fusion 360 creates a sketch *on that face*.
2. **Create → Project → Project**. Click the inner and outer circular edges of the top face. *Why project:* now we have those two circles available in the new sketch as references, instead of guessing at their location.
3. **Sketch → Offset** (shortcut `O`). Click the outer projected circle, drag inward, type `t_wall` (= 4 mm), Enter.
4. Repeat for the inner circle, offsetting outward by `t_wall`.
5. Finish Sketch.
6. **Create → Extrude**, click the *new* annular region (between the two offset circles). **Distance** = `-(H_ring - 2*t_wall)` — type that whole expression in the field; Fusion 360 evaluates it. With our values that's `-62`. **Operation** = Cut.

You now have a hollow donut with 4 mm walls and a 4 mm floor and ceiling.

> **Stop and inspect.** Hit `Q` (the section-analysis shortcut) and slice through the model on the YZ plane. You should see a rectangular cross-section with a hollow centre — the pontoon air pocket. If it's solid, the offset went the wrong way.

### 1.4 The pontoon access slots

**Why:** A closed-top hollow torus has nowhere for the printer to bridge. The slicer can do it, but the bridges sag. We add four small slots on the *bottom* face that we'll seal with marine epoxy after printing.

1. **Create Sketch** on the bottom face of `RingBody`.
2. **Create → Rectangle → Center Rectangle**. Place at a point on the bottom annulus midline (the centre between inner and outer wall). Dimension 4 × 6 mm.
3. **Sketch → Circular Pattern**. Pick the rectangle, axis = the Z axis (or origin point), quantity = 4.
4. Finish Sketch. Extrude the four rectangles **distance** = `-1` (downward), **Operation** = Cut. *Why downward:* this creates a slot through the floor; "Cut" subtracts from the body.

### 1.5 The weir notch

**Why:** the weir lip insert (Part 2) is a separate printed part that drops into a 60° opening on one side of the ring. Two reasons to make it removable: (a) you can iterate the lip angle without reprinting the ring; (b) printing both parts together would require horrendous support structure.

**Concept introduced:** construction geometry.

1. **Create Sketch** on the XY plane. We're sketching where the cut will go.
2. Sketch a vertical construction line (Sketch → Line, then check the **Construction** box on the right) from the origin straight up the +Y axis, length 200. *Why construction:* construction lines don't get extruded — they exist purely to anchor real geometry.
3. Around that construction line, sketch two lines forming a 60° wedge (30° on either side of +Y). Dimension the angle with the angular dimension tool.
4. Trim the wedge to the inner and outer projected circles (use **Project** to bring them in, then **Trim**, shortcut `T`).
5. Finish Sketch.
6. **Extrude** the wedge **From** = top face (Object), **Distance** = `-(H_ring - H_weir)` = `-40`, **Operation** = Cut. The cut leaves a 30-mm-tall floor in the wedge — that's where the weir lip will sit.

### 1.6 Fillets

**Why:** sharp top edges on a printed part chip easily and feel terrible to handle wet. Sharp bottom edges, on the other hand, shed water faster — no fillet there.

1. **Modify → Fillet** (shortcut `F`). Click each top edge of the ring (use Ctrl-click to add to selection). Radius = 2 mm.
2. Fillet the four corners of the weir cutout with radius 5 mm.

### 1.7 Export

1. **File → Save**.
2. Right-click `RingBody` in the Browser → **Save As Mesh**. Format = STL. Refinement = High. Save to `cad/exports/ring-chassis.stl`.
3. Bonus: **File → Export** the whole `.f3d` to your cloud project (in case Fusion 360's local cache misbehaves).

---

## Part 2 — Weir lip insert

A short, scoop-shaped wedge that drops into the ring's weir notch. The top edge of the scoop sits at the design waterline minus 5 mm — surface water flows over the edge into the chamber.

**Why a J-shaped scoop instead of a flat wall:** a flat wall would skim water but also bounce floating debris off it. A J-curve bends the incoming water *down* into the chamber, dragging leaves with it — same principle as a kitchen sink's strainer overflow.

### 2.1 New design file

**File → New Design**. **File → Save As → `02-weir-lip`**.

In **Modify → Change Parameters → User Parameters**, click the link icon and **Link** the parameters from the ring-chassis design — *why:* now `D_ring_outer` and `H_weir` mean the same thing in both files. Change them in the master spot and both parts update.

### 2.2 Sketch the arc footprint

1. **Create Sketch** on XY plane.
2. Sketch a 60° arc with **Create → Arc → 3-Point Arc**, snapping to the parameters: arc radius matches ring outer/inner (use `D_ring_outer/2 - 24` for the inner arc and `D_ring_outer/2` for the outer arc — match the ring's notch).
3. Close it with two radial lines. Finish Sketch.

### 2.3 Extrude

Extrude the arc region upward `H_weir`. New Body, name it `LipBody`.

### 2.4 The J-curve scoop

**Concept introduced:** Sweep features (sketch a profile, sweep it along a path).

1. **Create Sketch** on the YZ plane (or another plane that's perpendicular to the arc — use the construction plane that bisects the arc).
2. Sketch the J-curve cross-section: a vertical line from the bottom going up `H_weir - 8`, then a tangent arc curling outward 6 mm. Use **Tangent** constraint between the line and the arc — Fusion 360 will auto-suggest it.
3. Finish Sketch.
4. **Create Sketch** on the bottom face. Project the inner arc edge — that's the path the scoop follows. Finish.
5. **Create → Sweep**. Profile = the J-curve. Path = the projected arc. Operation = Cut.

Now the inner face of the lip is a smooth J. Run **Q** section analysis along the radial direction to confirm the curve looks like a J.

### 2.5 Bayonet tabs

**Why:** the lip needs to lock into the ring's notch positively, but be removable. A bayonet (drop in vertically, twist a few degrees to lock) is the simplest mechanical approach.

1. Sketch on the bottom face: two 6 × 4 mm rectangles, opposite sides of the arc, pointing radially outward.
2. Extrude 3 mm down. Operation = Join.

### 2.6 Export STL.

---

## Part 3 — Debris chamber bottom

A flat disc that drops into the ring's centre opening. It carries the pump in a recessed well and seats the basket above it. The disc has a side outlet boss for the hose.

### 3.1 New design, link parameters, sketch the disc

1. New design `03-chamber-bottom`. Link parameters.
2. Sketch on XY plane: circle, diameter `D_ring_inner - 4`. *Why minus 4:* leaves 2 mm radial clearance all around for assembly tolerance and an O-ring seal between the disc and the ring's inner wall.
3. Extrude 8 mm.

### 3.2 Pump well

**Why a recessed well rather than just placing the pump on top:** the well lowers the pump's centre of mass below the chamber floor, which (a) lets debris drop *over* the pump rather than into it, and (b) lowers the cleaner's overall centre of gravity — important for stability on choppy water.

1. Sketch on top face: circle diameter `D_pump_outer + 1` = 56, centred on origin.
2. Extrude **distance** = -4 (a 4 mm-deep well), **Operation** = Cut.

### 3.3 Outlet boss

**Concept introduced:** modelling threads.

1. Sketch on a side construction plane (or on the cylindrical face — use **Create Sketch** and pick the curved face; Fusion 360 auto-creates a tangent plane).
2. Sketch a 22 mm circle, centred 4 mm above the disc bottom. Extrude **Symmetric**, total 24 mm. Operation = Join. You now have a small cylinder sticking out the side.
3. **Modify → Hole** through the centre of the boss: 19 mm diameter, simple hole, all the way through.
4. **Modify → Thread** on the inner cylindrical face of that hole. Type = ISO Metric Profile, designation G3/4". *Why threaded:* the printed thread mates with the outlet nozzle (Part 6) using PTFE tape. Glue would also work, but threads are removable for maintenance.

### 3.4 Basket seat

A 5-mm-tall lip on the top face that the basket frame drops into.

1. Sketch on top face: circle diameter `D_basket_top + 2` = 202.
2. Extrude 5 mm up. Operation = Join.
3. Add four notches: sketch four 6 × 3 mm tabs on the lip (90° apart, use **Circular Pattern** in the sketch). Extrude as cuts, depth 3 mm.

### 3.5 Heat-set insert pockets

**Why heat-set inserts:** screwing M3 stainless screws directly into PETG strips the threads after a few cycles. Brass heat-set inserts are pressed into the print with a soldering iron and give you proper steel-on-brass threads with hundreds of cycles of life.

1. Sketch on top face: four 4.0 mm circles, 90° apart, at radius `D_ring_inner/2 - 6` = 114. Use **Circular Pattern** in the sketch (axis = origin, quantity = 4).
2. Extrude as cut, depth 6 mm.

### 3.6 Export STL.

---

## Part 4 — Mesh basket frame

A tapered open-topped cup that holds the 200 µm aquarium filter bag. It needs lots of side openings (the filter bag is what catches debris, not the plastic frame).

**Why tapered, not cylindrical:** a tapered cup lets you lift the basket out of the chamber straight up without it catching on the chamber walls — and the taper makes emptying debris easier (it slides out).

### 4.1 Loft

**Concept introduced:** Loft features (blend two or more profiles into a smooth body).

1. New design `04-basket-frame`.
2. Two sketches:
   - On XY plane: circle diameter 120 (the bottom of the basket).
   - **Construct → Offset Plane** at +90 mm. Sketch on it: circle diameter `D_basket_top` = 200 (the top of the basket).
3. **Create → Loft**. Pick the two circles as profiles. Operation = New Body.

You should have a tapered cup. Solid for now; we'll make it open-topped next.

### 4.2 Shell

**Concept introduced:** the Shell feature.

1. **Modify → Shell**. Select the **top face** of the cup as the face to remove. Inside thickness = 2 mm.

The basket is now a 2-mm-walled cup with an open top.

### 4.3 Side windows

**Why:** the plastic frame should be mostly empty space — flow has to go through the filter bag, and the frame is just there to keep the bag's shape.

1. Sketch on the side wall: a 25 × 60 mm rounded rectangle (use **Create → Rectangle → Center Rectangle**, then **Modify → Sketch Fillet** the corners with 5 mm).
2. Finish Sketch. Extrude as a cut, **distance = through-all** (or a generous negative number).
3. **Create → Pattern → Circular Pattern**. Pattern type = Features. Pick the cut feature in the Timeline. Axis = Z axis. Quantity = 8.

### 4.4 Top rim with drawstring channel

The filter bag has a drawstring that needs a place to sit so it doesn't slip off.

1. Sketch on top face: an annular ring 4 mm wide at the top edge.
2. Extrude up 6 mm. Operation = Join.
3. Sketch on the *outer* face of the new rim: a 2 × 3 mm rectangular channel running all the way around. Extrude as a cut, depth 1.5 mm. *Why a continuous channel:* a single notch would let the drawstring pop out at the wrong angle; a continuous channel lets the drawstring pull tight against the whole rim.

### 4.5 Keying tabs

**Why:** the basket's bottom outer wall has four 5 × 2 mm tabs that drop into the chamber bottom's notches (Part 3.4). This stops the basket from rotating when the pump's torque hits it.

Sketch on the outer bottom edge — four tabs, 90° apart, extrude 2 mm outward.

### 4.6 Export STL.

---

## Part 5a — Electronics dome base

The waterproof box that holds the battery, ESP32-C3, buck, MOSFET, and BMS. Sits above the waterline on top of the floating ring; the lid screws down with an O-ring seal.

### 5a.1 New design, the dome cylinder

1. `05-dome-base`.
2. Sketch on XY plane: circle diameter `D_dome_outer` = 180.
3. Extrude `H_dome` = 60. New Body.
4. **Modify → Shell**. Remove the *bottom* face. Wall thickness `t_wall` = 4. *Why bottom:* the dome opens downward onto the floating ring, so the lid sits on top.

### 5a.2 Mounting flange

**Why:** the dome bolts to the ring with M3 screws. Without a flange, you'd have to drill into the dome's vertical wall, which weakens the seal. A horizontal flange around the dome's bottom gives you a flat surface to bolt through.

1. Sketch on bottom edge: outer ring, OD 200, ID 180.
2. Extrude **upward** 6 mm. *Why upward not downward:* downward would interfere with the floating ring's top. Upward gives the screws room to engage.
3. Sketch four 3.4 mm holes (M3 clearance) on the flange, 90° apart, at radius 95.
4. Extrude as cuts, through-all.

### 5a.3 Cable gland holes

Three PG7 cable gland holes around the wall — one each for the pump cable, the solar panel cable, and the IP67 button cable.

1. Sketch on the cylindrical face: three 12.0 mm circles at 120° apart, centred 20 mm above the bottom. (Use a construction plane tangent to the cylinder at one point; sketch one hole; circular-pattern it.)
2. Extrude as cuts, through-all.
3. On the *outer* face, sketch a 14 mm-OD circle around each hole. Extrude as cut, depth 1.5 mm. *Why this recess:* the PG7 gland's flange needs a flat seat — a recessed pocket guarantees the gland sits flush against a flat surface even on a curved wall.

### 5a.4 Heat-set insert pockets on the flange top

Four 4.0 mm × 6 mm-deep pockets on the top face of the flange, aligned with the bolt holes — same logic as Part 3.5.

### 5a.5 Export STL.

---

## Part 5b — Electronics dome lid

Closes the dome. Has the O-ring groove on the bottom face and the solar panel recess on the top face.

### 5b.1 New design, lid disc

1. `06-dome-lid`. Link parameters.
2. Sketch on XY plane: circle diameter `D_dome_outer + 8` = 188. *Why 8 mm bigger:* the lid overhangs the dome wall by 4 mm all around, which sheds rain off the seal line — same logic as a roof's eaves.
3. Extrude 8 mm.

### 5b.2 O-ring groove

**Why a groove and not a flat gasket:** a flat gasket needs even pressure at every screw to seal. A groove captures the O-ring and only needs the O-ring squished by 0.5 mm to seal — the screws don't have to be perfectly torqued.

1. Sketch on the **bottom** face: an annular groove, centred at radius 88, width 2.5 mm. *(For a 175 mm ID × 3.0 mm cross-section O-ring from the BOM kit.)*
2. Extrude as cut, depth 2.0 mm. *Why 2.0 not 3.0:* the O-ring is 3 mm cross-section, so leaving 1 mm above the groove gives the seal 0.5 mm of compression once the lid is bolted down. Less and the seal is loose; more and the lid won't sit flat.

### 5b.3 Bolt clearance holes

Four 3.4 mm holes at radius 95, 90° apart — M3 clearance. Through-all.

### 5b.4 Solar panel recess

**Why a recess instead of just glueing flat:** the panel sits in a 3-mm-deep pocket so its edges are protected from snags and side splashes. Marine epoxy seals the perimeter (BOM line). Without the recess, a single corner getting nicked could lift the whole panel.

1. Sketch on the **top** face: a `D_panel × D_panel` square, centred. Adjust to your actual panel's footprint if it's not square.
2. Extrude as cut, depth 3 mm.

### 5b.5 Solar cable pass-through

A 7 mm hole through the lid, somewhere inside the panel recess where the panel's terminal box is. Extrude as cut, through-all. Then add a PG7 gland recess on the *bottom* face around the hole — same as 5a.3.

### 5b.6 Export STL.

---

## Part 6 — Side outlet nozzle

Threads into the chamber bottom's outlet boss. Bent 15° tangent to the ring so the outflow gives the cleaner a slow rotational drift around the pool.

**Why a separate part:** trying to print the chamber bottom + nozzle as one piece would require either supports inside the boss (terrible to remove) or printing the nozzle in mid-air. Separating them is the print-friendly choice.

### 6.1 New design, the threaded plug

1. `07-outlet-nozzle`.
2. Sketch on XY plane: circle, diameter 19. Extrude 14 mm.
3. **Modify → Thread** on the *outer* face. Type = ISO Metric Profile, designation G3/4" external. Mate this to the chamber's internal thread.

### 6.2 The bent body

**Concept introduced:** Sweep with a sketched path.

1. **Construct → Offset Plane** from the threaded end face, 0 mm offset. Sketch on it: circle diameter 22. (Slightly larger than the thread end — gives a flange the silicone hose clamp can grab.)
2. **Construct → Sketch on Path** or just sketch on a perpendicular plane: a 30 mm path bent 15° from straight. Use a 3-point arc to define the bend.
3. **Create → Sweep**. Profile = the 22 mm circle. Path = the bent line. Operation = Join.

### 6.3 Hollow it out

**Modify → Shell**. Remove both end faces. Wall thickness 2.5 mm.

### 6.4 Export STL.

---

## Final assembly check

Now we'll bring all six parts into one design to verify they fit.

### A.1 Master assembly file

1. **File → New Design**, save as `00-assembly`.
2. **Insert → Insert Derive** (or **File → Open from this Project** for each part, then **Save As Component**). Drop in each of the six parts as **components** (not bodies — components are how Fusion 360 handles assemblies). *Why components:* components have their own origin and can be moved/jointed independently, which is what we need.

### A.2 Position and joint

**Concept introduced:** Joints (the Assemblies workflow).

1. **Assemble → Joint** (shortcut `J`). Pick the bottom face of the ring chassis as the *first* component, the world origin as the *second*. Joint type: Rigid. The ring is now grounded.
2. Joint the chamber bottom to the ring: bottom face of chamber → top face of ring (inner lip). Joint type: Rigid.
3. Joint the basket to the chamber: basket bottom keying tabs → chamber's notches. Joint type: Rigid.
4. Joint the dome base to the ring: dome flange bottom → ring top face. Joint type: Rigid.
5. Joint the lid to the dome: lid bottom O-ring groove face → dome top face. Joint type: Rigid.
6. Joint the weir lip into the ring's notch.
7. Joint the outlet nozzle into the chamber's outlet boss.

### A.3 Interference check

**Inspect → Interference**. Pick all components, check "include coincident". Click Compute. Expected result: zero interferences. If something interferes, check that you used the correct parameter on both sides — usually the culprit is a hard-coded number creeping into one part.

### A.4 Waterline check

**Inspect → Section Analysis**. Pick the YZ plane. Look at the cross-section: imagine the waterline at `Z = H_waterline` = 35. The weir lip top should be at Z = 30 — exactly 5 mm below the surface. If it's not, you have a parameter mismatch somewhere; find it now, before printing.

### A.5 Mass and centre of mass

**Inspect → Section Properties** on the whole assembly (with everything set to PETG in **Modify → Physical Material**). Confirm:
- Mass < ~1.5 kg (otherwise the buoyancy-to-weight ratio is too tight).
- Centre of mass below the waterline (otherwise the cleaner is top-heavy and will capsize — this is the most common first-iteration failure).

If the COM is too high, your options are:
- Move the battery lower in the dome (stick it to the dome floor, not the ceiling).
- Add ballast to the chamber bottom (a couple of stainless M3 nuts, glued in place).
- Reduce dome wall thickness.

---

## Slicer settings reference

Once the STLs are exported, slice with these settings (works on most prosumer slicers — Cura, PrusaSlicer, Bambu Studio):

| Part | Infill | Walls | Layer | Supports |
|------|--------|-------|-------|----------|
| `ring-chassis` | 100 % | 4 | 0.2 mm | None |
| `weir-lip` | 100 % | 4 | 0.2 mm | Tree under the J-scoop |
| `chamber-bottom` | 100 % | 4 | 0.2 mm | None |
| `basket-frame` | 30 % | 3 | 0.2 mm | None |
| `dome-base` | 30 % | 4 | 0.2 mm | On the flange overhang |
| `dome-lid` | 30 % | 4 | 0.2 mm | None (panel recess up) |
| `outlet-nozzle` | 100 % | 4 | 0.2 mm | Tree on the angle |

PETG, 245 °C nozzle, 80 °C bed, 30–50 % cooling. Don't substitute PLA — it cracks under sustained chlorine exposure.

## What you've learned in Fusion 360

If you followed every step, you've now used:

- **User Parameters** (single source of truth for dimensions)
- **Sketches**: lines, circles, arcs, dimensioning, projecting edges, offsetting, trimming, construction geometry, sketch patterns
- **Features**: Extrude (Join, Cut, New Body), Shell, Hole, Thread, Loft, Sweep, Fillet
- **The Timeline** (parametric history) and how to edit upstream features
- **Bodies vs. components**, naming, and the Browser
- **Assemblies and Joints** (Rigid joint, grounding the first component)
- **Inspection tools**: Section Analysis, Interference, Section Properties
- **STL export** for slicing

That's the core of the Fusion 360 workflow. Every new printable part you design from here will use the same handful of tools.

## Iteration tips

- **Trust User Parameters.** When the first float test shows the ring sitting too high or too low, change `H_waterline` and `H_weir` in the parameters table, not in individual sketches. Re-export only the affected STLs.
- **Print the chamber bottom first** (it's the smallest). It's a quick test of whether your printer can hit the M3 insert pocket dimensions accurately, before you commit to a 12-hour ring print.
- **Pontoon air-tightness is the most common failure.** After printing the ring, seal the four bottom slots with marine epoxy and let it cure 24 h. Then submerge the ring upside-down in the bathtub for 30 minutes — no bubbles should escape. If they do, it's almost always a layer-line porosity issue: re-print at a higher extrusion multiplier (1.03 instead of 1.00).
