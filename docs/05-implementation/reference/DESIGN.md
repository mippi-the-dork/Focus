# Complete Control Reference — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## What is covered?

Focus has **ten control categories**. The first two, Visual and Interaction, manage common Outliner workflows. The other eight groups expose **28 native quick properties** already present in Unreal Engine.

| Category | Controls at a glance |
| --- | --- |
| Visual | Hide in Editor, Hidden in Game, Solo |
| Interaction | Selection Lock; protection cycle Unlocked → Transform Lock → Edit Lock |
| Simulation | Simulate Physics, Gravity, radial-impulse/force responses, impulse-on-damage, async physics tick |
| Physics Events | Hit, overlap and wake/sleep event generation |
| Lighting | Three lighting channels and Cast Shadows |
| World | Main World Only, Editor Only, spatial loading and HLOD |
| Network | Net Load on Client, autonomous-proxy physics and Static Mesh movement replication |
| Navigation | Navigation contribution, dynamic obstacle, collision filling and convex rasterization |
| Mobility | Static, Stationary, Movable |
| LOD | Forced LOD, Never Distance Cull, Allow Cull Distance Volume |

## Choose by goal, not by icon

If you want to **avoid selecting geometry**, use Selection Lock. To prevent accidentally moving it, use Transform Lock. To prevent broader supported editing, use Edit Lock. If you only want to see fewer scene objects, Hide in Editor or Solo is more appropriate.

If you want to change physics, navigation, lighting, world or replication behavior, use a native quick-property group and verify the result in Unreal's actual systems.

## Understanding universal actions

The same control may act on a single Actor, its attachment hierarchy, loaded folder contents, native group, or multi-selection. Component-based properties only affect compatible editable components. An unavailable segment is not an error or an off state.

## Need a precise answer about a control?

The category-specific Design pages explain intended use and prerequisites. Their companion Technical pages identify the native property and scope, so users can compare Focus with Unreal's Details panel.

---

For implementation, native data and engine-integration details, [see Technical](TECHNICAL.md).
