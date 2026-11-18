# Simulation and Physics Events — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Two related control groups

**Simulation** controls how a supported Actor or Primitive Component participates in physics. **Physics Events** controls which collision, overlap, and wake/sleep notifications that physics setup can generate. Neither group is a physics system of its own.

| Control | What it means | Important distinction |
| --- | --- | --- |
| Simulate Physics | Allows a supported primitive to respond to rigid-body physics | Requires suitable collision and mobility |
| Enable Gravity | Allows world gravity to affect a simulated primitive | Does **not** enable Simulate Physics for you |
| Ignore Radial Impulse | Ignores an instantaneous radial impulse | Separate from continuous radial force |
| Ignore Radial Force | Ignores a radial force | Separate from a single impulse |
| Apply Impulse on Damage | Permits supported damage events to produce a physical impulse | Does not itself enable damage handling |
| Async Physics Tick Enabled | Enables an Actor's async physics ticking where supported | Not the same as ordinary Actor Tick |
| Generate Hit Events | Allows hit notifications on eligible primitives | Collision responses still matter |
| Generate Overlap Events | Allows overlap notifications | Both participants must have compatible overlap settings |
| Generate Wake Events | Allows wake/sleep notifications | Meaningful only for compatible physics bodies |

## Using these controls

Enable the **Simulation** and **Physics Events** categories from Focus's column-header menu. Hover each part of the compact symbol before clicking. Apply a choice to an Actor, an attachment hierarchy, or a loaded folder. Component-targeted controls apply to *each eligible component* within that Actor.

For a physics prop, first make its collision and mobility suitable for simulation, then enable Simulate Physics. Enable Gravity if it should fall. If you need Blueprint hit events, enable Generate Hit Events, confirm collision channels and responses, and test in Play.

## Why nothing happens

An enabled flag can be valid even when the rest of Unreal's setup makes it ineffective. Gravity without physics simulation does not make a static object fall. Hit events without suitable contacts will not fire. Unavailable segments mean Focus could not find an eligible editable target.

---

For editable C++ property targets, transaction details and native constraints, [see Technical](TECHNICAL.md).
