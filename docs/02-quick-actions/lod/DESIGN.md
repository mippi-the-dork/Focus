# LOD — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

The LOD micro-widget contains a central **Forced LOD Model number** and two separate distance-culling toggles. Forced LOD changes which mesh LOD is displayed; the culling options change whether eligible primitives participate in supported distance-culling mechanisms.

| Control | What it does | What it does not do |
| --- | --- | --- |
| Forced LOD Model | Requests automatic or forced mesh LOD selection | It does not generate missing LODs |
| Never Distance Cull | Prevents distance culling by systems that honor this native setting | It does not override every visibility, occlusion or streaming system |
| Allow Cull Distance Volume | Allows an eligible primitive to respond to a Cull Distance Volume | It does nothing without a suitable volume |

## Exactly how to operate the Forced LOD number

The compact numeric control supports more than typing a number. The gestures are:

| Input | Result |
| --- | --- |
| **Left-click once** | Increase the displayed value by one |
| **Alt + left-click** | Decrease the value by one |
| **Mouse wheel up or down** | Increase or decrease by one |
| **Double left-click** | Open a small text-entry popup to enter a specific number |
| **Right-click** | Retains the normal Outliner context-menu interaction |

A single-click increment is delayed briefly so a double-click can open text entry without accidentally changing the value first. When typing, enter a number from **0 to 255** and commit it. The popup is deliberately separate from the narrow Outliner column.

### How the number maps to mesh LOD

| Focus entry | Meaning |
| --- | --- |
| **0** | Automatic: Unreal chooses LOD |
| **1** | Force mesh LOD **0** |
| **2** | Force mesh LOD **1** |
| **n** | Force mesh LOD **n-1** |

This is Unreal's one-based *forced* LOD setting, not the mesh asset's zero-based LOD index.

## Practical workflow: diagnose a disappearing mesh

1. Enable the **LOD** category in the Focus column-header menu.
2. Find an Actor with an eligible Static Mesh or Skeletal/Skinned Mesh Component.
3. Double-click the LOD number and enter **1** to inspect the first authored LOD.
4. Try higher values only if the mesh actually contains those LODs. Hover to inspect the common valid range when several meshes are selected.
5. Restore **0** afterward to return to automatic selection.
6. If the Actor disappears with distance, separately inspect **Never Distance Cull** and **Allow Cull Distance Volume**. Do not assume that Forced LOD controls actor streaming.

## Out-of-range and mixed values

The input can accept values beyond an individual mesh's authored LOD count. Focus can flag an invalid request with its **Alert** color. A multi-selection may contain meshes with different available LOD ranges; an empty or mixed display indicates the eligible targets don't share the same current value.

## Target scope and saving

Changes affect eligible components on loaded Actors in the activated row, attachments, folder/group, or selected rows. Edit-Locked Actors are skipped for native quick-property writes. LOD settings use native Unreal property persistence, not Focus's Visual/Interaction session-only options. Use Undo for an unintended batch edit.

For exact mesh component setters and eligibility checks, [read Technical](TECHNICAL.md). For common states and hierarchy rules, see [Quick Actions overview](../README.md).
