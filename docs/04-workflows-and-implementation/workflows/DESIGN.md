# Practical Workflows for Dense Levels — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Workflow 1: Protect finished architecture

Imagine a level where a hallway is approved but adjacent rooms remain under construction.

1. Put finished meshes under a clearly named Outliner folder.
2. Use **Selection Lock** on that folder to prevent those meshes intercepting normal viewport clicks.
3. Use **Transform Lock** when their placement is finalized but material or component properties may still need editing.
4. Use **Edit Lock** only when supported property changes should be prevented entirely.
5. Inspect mixed indicators after a bulk action to find exceptions.

Do not assume the lock is automatically inherited by Actors added to the folder tomorrow. Reapply when needed.

## Workflow 2: Focus on one area

Solo the Actors or folder for your current environment dressing task. Other rendered primitives are filtered from that editor view, but lights or volume contributions can remain. Clear Solo before final scene review. If you want isolation only for the current work session, turn off Solo persistence first.

## Workflow 3: Configure a physics test area

Enable the Simulation and Physics Events category groups. On appropriate props, set Simulate Physics, Enable Gravity, and Generate Hit Events. Confirm the collision profiles, mobility and event binding in Unreal. On Actors without eligible Primitive Components, those segments may remain unavailable.

## Workflow 4: Work across a multi-selection

Select several Actors or folders in the Outliner. Activate a Focus control on one of the selected rows to apply it to the combined selection. Hover first to read eligible counts; a mixed value means not all selected eligible targets agree.

## Workflow 5: Avoid unwanted saved changes

Set an appropriate Visual or Interaction category to session-only in Editor Preferences, then temporarily hide or protect objects while inspecting a level. Restore Temporary Changes when finished instead of accidentally saving an interim workspace configuration.

## When to stop and verify

After changing World Partition, HLOD, navigation, physics or networking flags, verify in the native engine tool or runtime simulation that actually owns the result. Focus simplifies authoring; it does not validate every downstream engine system.

---

For implementation, native data and engine-integration details, [see Technical](TECHNICAL.md).
