# Mobility, LOD and Distance Culling — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Mobility: Static, Stationary or Movable

Focus presents **S**, **T**, and **M** for eligible Scene Components. Hover each choice to confirm the selection. Static is for nonmoving content, Stationary allows certain restricted changes, and Movable supports dynamic movement where Unreal allows it. The correct choice depends on the component, physics and lighting setup.

To change a group, select the appropriate Actor/folder and click the desired mobility option. This affects eligible Scene Components, not just an Actor's root in all cases.

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
