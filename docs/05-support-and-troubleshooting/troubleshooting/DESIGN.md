# Troubleshooting Focus — Design

**Focus v2.1.0 · User guide and workflow reference**

## Focus is missing from the World Outliner

Open **Edit → Plugins**, search for Focus, enable it if necessary and restart Unreal Editor. Verify UE 5.8.x on Win64 and that you are using the ordinary World Outliner Actor Browser. If the column itself is hidden, turn it back on through the Outliner column controls.

## A control is missing or unavailable

First open the Focus header menu and check whether the relevant category is visible. If the group exists but a segment is dark, inspect the hovered tooltip. Some controls require an eligible registered Primitive, Shape, Mesh or Scene Component, and Unreal may prevent editing an instance property.

A disabled control can be enabled; an unavailable control has no suitable editable target. Those are different conditions.

## A folder change did not reach an Actor

Verify the Actor is **currently loaded**, belongs to the intended folder/group/attachment hierarchy, and existed when the operation was applied. Search filters and collapsed rows do not normally narrow the scope, but unloaded World Partition Actors and newly added members are not inherited targets.

## A lock did not behave as expected

- **Selection Lock:** select through the Outliner, not viewport, to confirm the intended exception.
- **Transform Lock:** try a normal transform edit and an unrelated Details property to compare.
- **Edit Lock:** check the protected Actor and whether the attempted change was made through a native supported path or a custom editor/tool/script.
- **Hidden in Game:** Focus skips its quick toggle when the Actor is Edit Locked.

A script changing an Edit-Locked Actor does not necessarily mean the Focus native guard failed.

## A Focus setting did not survive reload

Check **Editor Preferences → Plugins → Focus → Visual & Interaction Persistence**. Session-only changes are deliberately temporary. When persistence is enabled, save the appropriate Actor/level package (including External Actor packages under OFPA) and reload.

Don't confuse a temporary Focus value with an ordinary native Details edit, which follows normal Unreal saving. Restore Temporary Changes also intentionally discards pending temporary values.

## Save or source-control checkout seems unexpected

Test in a disposable level with an otherwise clean package. Edit just one Focus control with persistence off, and observe whether the Actor was already dirty. Repeat through Save All and reload. Check other plugins, native property edits, and the source-control state. Focus's session-only system is designed to avoid saving temporary values, but it cannot prevent unrelated edits from requesting checkout.

## A quick property's effect is invisible

The toggle controls a native property, but the result may need additional Unreal settings: physics simulation and collision for gravity/hits, compatible light channels, World Partition or HLOD configuration, replication setup, a NavMesh rebuild, a Cull Distance Volume, or a mesh with sufficient LODs.

## Reporting an issue

Include Focus and Unreal versions, Actor/Component class, control, category visibility, hierarchy type, World Partition loaded state, persistence setting, reproduction steps and Output Log excerpts. Add screenshots or short video when relevant.

---

For diagnostics, native editor paths and compatibility, [see Technical](TECHNICAL.md).
