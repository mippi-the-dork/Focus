# Navigation — Design

**Focus v2.1.0 · Quick Actions · User guide**

## Available controls

| Quick Action | Appropriate target | Purpose |
| --- | --- | --- |
| **Can Ever Affect Navigation** | Eligible Actor Component | Permit contribution to navigation data |
| **Dynamic Obstacle** | Eligible Shape Component | Treat as a dynamic obstacle in supported nav generation |
| **Fill Collision Underneath for Navmesh** | Eligible Primitive Component | Fill beneath collision when supported by the nav system |
| **Rasterize as Filled Convex Volume** | Eligible Primitive Component | Rasterize geometry as a filled convex shape when supported |

## Workflow: prevent decorative collision from affecting routes

1. Inspect the level's current NavMesh so you have a baseline.
2. Enable **Navigation** in the Focus header.
3. Select the loaded Actor that has the relevant registered, editable component.
4. Disable **Can Ever Affect Navigation** for the intended component(s).
5. Rebuild or update navigation using Unreal's normal workflow, then inspect the resulting navmesh and AI pathing.
6. Undo or restore the previous value if the geometry should contribute after all.

## Workflow: configure a dynamic obstacle

For a compatible Shape Component, enable **Dynamic Obstacle** and verify its collision/navigation configuration. The native property is only one prerequisite; it does not create a navmesh or an AI move request.

## Why a toggle doesn't change the visible NavMesh

Navigation data is built and updated by Unreal's navigation system; Focus only changes native configuration fields. Check navmesh generation settings, collision geometry, editor/PIE update policy and whether the component is registered. **Not Applicable** typically means the required component/property isn't editable.

## Target scope, state and saving

These Quick Actions operate on **currently loaded, eligible Actors**, including compatible attached descendants, nested folders, groups and selected Outliner rows. Hover for the state and number of eligible targets. **Mixed** means values differ across those targets; **Not Applicable** means no editable target exists.

Native-property changes respect Edit Lock, support Unreal transactions/Undo and follow normal Actor/Component save behavior. They are not part of Focus's separate Visual/Interaction session-only persistence system. See [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For exact component scopes and native setter behavior, [read Technical](TECHNICAL.md).
