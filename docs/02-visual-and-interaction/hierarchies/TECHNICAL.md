# Folders, Groups and Multi-Selection — Technical

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Scope resolution

`FFocusColumn::Gather` builds an Actor set from live Actor/Outliner relationships, rather than from materialized/filtered child rows. It traverses attachment descendants and native groups and handles folder path matching. `FFocusColumn::LoadedActors` constrains operations to available editor-world Actors.

## Multi-select semantics

Row actions resolve `ActionActors` from the activated item or from the entire selected set when the row is part of that selection. Results are aggregated in `RefreshRow`. Counts drive Off, On and Mixed states, including target-component eligibility for the quick-property groups.

## Duplicate prevention

Combining multiple sources into one action must deduplicate Actor targets. Native Group descendants and attachments may overlap. The UI represents each eligible Actor's state, not the number of Outliner rows visible after filtering.

## Limits

These are imperative bulk edits, not inheritance or folder metadata policies. A World Partition Actor not loaded in the editor cannot be traversed and modified like a loaded Actor.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)

For control workflows and examples, [read the Design page](DESIGN.md).

**Validation:** Implementation described from Focus 2.1.0 source. Project-specific live editor QA is separate from documentation review.
