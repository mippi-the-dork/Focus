# Applicability, Mixed States and Undo — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Understand the colors and states

Focus distinguishes **On**, **Off**, **Mixed**, and **Not Applicable**.

- **On:** every eligible target in the row has the property enabled.
- **Off:** every eligible target has it disabled.
- **Mixed:** eligible targets differ; the next click normally enables them together.
- **Not Applicable:** there is no suitable editable target for that control. It is not the same as Off.

Hover a segment to see the property name, meaning, current row state, and number of eligible targets. A folder may contain many Actors but only some have a component to which a given quick-property control applies.

## What counts as a target

Simulation and most physics settings require eligible Primitive Components; some World and network flags belong to Actors directly; Mobility targets Scene Components; LOD targets mesh components. A property may also be restricted by Unreal's own edit conditions.

## Multi-selection example

Select one Static Mesh Actor and one Actor without a mesh. Forced LOD should apply only to eligible mesh components and not silently invent a mesh on the other Actor. If two eligible meshes have different settings, the row is Mixed.

## Edit Lock and history

Edit-Locked Actors are skipped when applying native quick-property controls. Eligible edits support Unreal's normal Undo/Redo transaction path. To reverse a mistaken batch, use Undo before proceeding.

## Why a dark segment is useful

Unavailable is an explanation, not a failure. It indicates there was no compatible instance-editable property for this row and the attempted action is disabled.

---

For editable C++ property targets, transaction details and native constraints, [see Technical](TECHNICAL.md).
