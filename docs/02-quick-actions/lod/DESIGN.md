# LOD — Design

**Focus v2.1.0 · Quick Actions · User guide**

## Available controls

| Quick Action | Meaning | What it doesn't guarantee |
| --- | --- | --- |
| **Forced LOD Model** | Override a compatible mesh's automatic LOD choice | Cannot create missing mesh LODs |
| **Never Distance Cull** | Keep eligible primitives from distance-culling systems that honor this flag | Cannot stop all streaming, occlusion or manual hiding |
| **Allow Cull Distance Volume** | Permit applicable Cull Distance Volumes to affect the primitive | No effect without a suitable volume |

## How to operate the Forced LOD number

| Input | Result |
| --- | --- |
| **Left-click** | Increase value by 1 (slightly delayed to distinguish double-click) |
| **Alt + left-click** | Decrease value by 1 |
| **Mouse wheel** | Up/down changes by ±1 |
| **Double left-click** | Open a small popup and type a numeric value |
| **Right-click** | Leave native Outliner context-menu behavior available |

Numeric entry accepts values **0–255**, but that does **not** mean every mesh has 255 LODs. An out-of-range requested value is indicated with the **Alert** color.

| Focus number | Result |
| --- | --- |
| **0** | Automatic selection |
| **1** | Force mesh LOD **0** |
| **2** | Force mesh LOD **1** |
| **n** | Force mesh LOD **n−1** |

## Workflow: diagnose an unexpected mesh change

1. Enable **LOD**, select an Actor containing an eligible Static or Skinned Mesh Component and hover the numeric control.
2. Double-click the number and enter **1**, forcing mesh LOD 0.
3. Increase cautiously and watch for the Alert color when the mesh lacks the requested level. On multiple eligible meshes, the common valid range can be smaller.
4. Return to **0** for automatic LOD selection once you've finished comparing.
5. If a distant Actor disappears, separately inspect **Never Distance Cull** and **Allow Cull Distance Volume**. They control different native behavior from Forced LOD.

## Common problems

**The number shows a dash:** mixed current values or no applicable target can prevent a single numeric value from being displayed; hover to distinguish Mixed from Not Applicable.

**The Actor still disappears in the distance:** streaming and other visibility systems can hide it regardless of distance-cull flags.

**A click changed multiple meshes:** when an Outliner selected row is activated, Focus can apply to the combined selected loaded scope.

## Target scope, state and saving

These Quick Actions operate on **currently loaded, eligible Actors**, including compatible attached descendants, nested folders, groups and selected Outliner rows. Hover for the state and number of eligible targets. **Mixed** means values differ across those targets; **Not Applicable** means no editable target exists.

Native-property changes respect Edit Lock, support Unreal transactions/Undo and follow normal Actor/Component save behavior. They are not part of Focus's separate Visual/Interaction session-only persistence system. See [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For mesh-specific setters, LOD counts and native constraints, [read Technical](TECHNICAL.md).
