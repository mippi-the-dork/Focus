# 01. Getting Started

Focus is an **editor-only Unreal Engine plugin** that adds compact, configurable controls to the World Outliner. It helps you work with Actors and Components without opening the Details panel for every routine visibility, protection or native-property change.

## Is Focus right for your workflow?

Focus is useful when you frequently need to isolate level content, protect finished geometry from accidental edits, or update common native Actor/Component properties across many loaded Actors. You do not place a Focus Actor or build special data assets. The plugin operates on ordinary Unreal levels.

Focus is **not** a runtime gameplay system, a source-control permission layer, or a guarantee that every external editor tool obeys its locks.

## Your first session

1. [Install Focus](install/DESIGN.md) from a compatible release, Fab or source checkout.
2. Open a project in **Unreal Engine 5.8.x on Win64** and enable the plugin.
3. Open the World Outliner. Find the Focus column; its default groups are **Visual** and **Interaction**.
4. On a disposable test Actor, try Hide in Editor, Selection Lock and Solo.
5. Open the column-header category menu to see additional Quick Actions.
6. Review [Settings and Saving](../03-settings-and-saving/README.md) before using permanent and session-only controls on production levels.

## Where are the controls?

The controls live inside the World Outliner’s Focus column. Their icon shapes and colors describe the action and state. **On**, **Off**, **Mixed**, and **Not Applicable** are different; hover a segment for the current meaning and eligible target count.

Actor and folder operations affect eligible **currently loaded** Actors. Collapsing or filtering an Outliner row does not necessarily narrow the target set. Newly added Actors do not inherit previous folder operations.

## Choosing which guide to read

The **Design** page explains using Focus: actions, expected outcomes and examples. The **Technical** page explains native engine hooks, property types, state storage and implementation constraints.

## Getting Started topics

| Topic | What you'll learn | Design | Technical |
| --- | --- | --- | --- |
| **What Is Focus?** | Purpose, scope, limits and core workflows | [Design](overview/DESIGN.md) | [Technical](overview/TECHNICAL.md) |
| **Installation and Compatibility** | Fab, releases, source, activation and update requirements | [Design](install/DESIGN.md) | [Technical](install/TECHNICAL.md) |
| **First Use** | Test controls and read Outliner states | [Design](first-use/DESIGN.md) | [Technical](first-use/TECHNICAL.md) |

**Next:** [Quick Actions](../02-quick-actions/README.md) documents each of Focus's ten control categories.
