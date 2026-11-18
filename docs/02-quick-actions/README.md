# 02. Quick Actions

## Purpose

Quick Actions are the compact controls Focus places inside Unreal Engine's **World Outliner**, grouped into the same **ten categories** that appear in Focus's category visibility menu. Every category has one complete user-facing **Design** guide and one source-grounded **Technical** reference.

## How to use the controls

1. Open the World Outliner and find the Focus column.
2. Use the column-header category menu to display the categories you need. **Visual** and **Interaction** are displayed by default; the other eight are optional.
3. Hover a symbol to read its exact action, current state, and number of eligible targets. A control can be On, Off, Mixed or Not Applicable.
4. Click to apply to the relevant currently loaded Actor, attachment hierarchy, folder, group or current multi-selection.
5. Use Unreal's Undo if you need to reverse an unintended edit.

## Understanding categories

Visual adjusts editor/game visibility and Solo. Interaction protects against accidental selection or editing. The other eight categories expose **28 native Unreal properties**, rather than new Focus gameplay systems. Native property effects can depend on collision, World Partition, network setup, NavMesh generation or rendering configuration.

## Choose a category

| Category | Covers | Design | Technical |
| --- | --- | --- | --- |
| **Visual** | Editor visibility, in-game visibility and Solo | [Design](visual/DESIGN.md) | [Technical](visual/TECHNICAL.md) |
| **Interaction** | Selection, Transform and Edit Lock | [Design](interaction/DESIGN.md) | [Technical](interaction/TECHNICAL.md) |
| **Simulation** | Simulation native controls | [Design](simulation/DESIGN.md) | [Technical](simulation/TECHNICAL.md) |
| **Physics Events** | Physics Events native controls | [Design](physics-events/DESIGN.md) | [Technical](physics-events/TECHNICAL.md) |
| **Lighting** | Lighting native controls | [Design](lighting/DESIGN.md) | [Technical](lighting/TECHNICAL.md) |
| **World** | World native controls | [Design](world/DESIGN.md) | [Technical](world/TECHNICAL.md) |
| **Network** | Network native controls | [Design](network/DESIGN.md) | [Technical](network/TECHNICAL.md) |
| **Navigation** | Navigation native controls | [Design](navigation/DESIGN.md) | [Technical](navigation/TECHNICAL.md) |
| **Mobility** | Mobility native controls | [Design](mobility/DESIGN.md) | [Technical](mobility/TECHNICAL.md) |
| **LOD** | Mesh LOD and distance-culling flags | [Design](lod/DESIGN.md) | [Technical](lod/TECHNICAL.md) |

## Saving and protection

Visual and Interaction controls can use Focus-specific session-only persistence, configured under **Editor Preferences → Plugins → Focus**. The other eight categories modify native Unreal properties using normal transactions and save behavior. Native quick-property changes skip Edit-Locked Actors. Focus locks are safeguards for supported editor workflows, not access-control permissions.
