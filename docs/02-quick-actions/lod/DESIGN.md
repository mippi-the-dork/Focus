# LOD — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

**Forced LOD Model** overrides mesh LOD selection, while **Never Distance Cull** and **Allow Cull Distance Volume** control different distance-culling behaviors.

| Forced LOD entry | Meaning |
| --- | --- |
| 0 | Let Unreal choose automatically |
| 1 | Force mesh LOD 0 |
| 2 | Force mesh LOD 1 |
| n | Force mesh LOD n-1 |

## Workflow

Use Forced LOD when inspecting mesh LOD quality or debugging a distant object. Check the mesh has enough LODs; an out-of-range request is highlighted with the Alert color.

Never Distance Cull only affects systems respecting that property. Allow Cull Distance Volume requires a suitable volume to exist. Neither can disable every form of streaming or occlusion.

## Forced LOD: read the index carefully

- **0** means Unreal selects the mesh LOD automatically.
- **1** forces mesh **LOD 0**.
- **2** forces mesh **LOD 1**, and so on.

The numeric entry is a request, not proof that the mesh contains that many LODs. An out-of-range value can be highlighted with the alert color. Use a reasonable common LOD across a multi-selection.


## Distance-culling flags

**Never Distance Cull** disables culling in systems that respect that native property; it does not disable every visibility or streaming system. **Allow Cull Distance Volume** permits compatible Cull Distance Volumes to influence primitives when such a volume exists in the level.


## Example

When investigating disappearing props, temporarily set Never Distance Cull on a few loaded primitives and examine the result. If streaming or another visibility mechanism is responsible, changing this one flag may not affect the symptom.


## Common confusions

Mobility and physics simulation are distinct. Forced LOD and distance culling are distinct. A valid property can have no visible effect in a scene whose required engine feature is absent.

---

For editable C++ property targets, transaction details and native constraints, [see Technical](TECHNICAL.md).

## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
