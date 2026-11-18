# Navigation — Design

**Focus v2.1.0 · Quick Actions · User guide**

## Available controls

<!-- Media target: IMAGE | navigation-controls | docs/assets/media/navigation-controls.png -->
<figure class="doc-media" data-kind="image" data-media-id="navigation-controls">
  <img src="../../assets/media-placeholder.png" alt="Navigation Quick Actions displaying the navigation contribution and obstacle flags" loading="lazy">
  <figcaption>The Navigation category exposes native component settings that influence how Unreal builds and updates navigation data.</figcaption>
</figure>


| Quick Action | Appropriate target | Purpose |
| --- | --- | --- |
| **Can Ever Affect Navigation** | Eligible Actor Component | Permit contribution to navigation data |
| **Dynamic Obstacle** | Eligible Shape Component | Treat as a dynamic obstacle in supported nav generation |
| **Fill Collision Underneath for Navmesh** | Eligible Primitive Component | Fill beneath collision when supported by the nav system |
| **Rasterize as Filled Convex Volume** | Eligible Primitive Component | Rasterize geometry as a filled convex shape when supported |

## Workflow: prevent decorative collision from affecting routes

<!-- Media target: GIF | navigation-navmesh | docs/assets/media/navigation-navmesh.gif -->
<figure class="doc-media" data-kind="gif" data-media-id="navigation-navmesh">
  <img src="../../assets/media-placeholder.png" alt="Animation comparing navmesh results before and after changing navigation contribution" loading="lazy">
  <figcaption>Disabling navigation contribution on selected collision can remove its influence after Unreal updates the navigation data.</figcaption>
</figure>


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
