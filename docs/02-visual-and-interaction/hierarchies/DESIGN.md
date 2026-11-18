# Folders, Groups and Multi-Selection — Design

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Scope follows Actors, not just visible rows

Focus actions on a **folder** affect loaded Actors in that folder and nested folders. Actions on an **Actor** also include its attached descendants. Native **Groups** include their members and nested groups. A multi-selection combines the eligible scope of the selected rows.

Collapsed Outliner rows and search filters do **not** exclude Actors from the operation.

## Example: lock a completed room

Organize finished room Actors under a folder. Toggle Transform Lock on that folder. Its loaded contents receive the protection even if the folder is collapsed or the Outliner currently filters the visible list. Check mixed state afterward to identify exceptions.

## Three key cautions

- Newly added Actors do **not** inherit an earlier folder change. Reapply when adding content.
- Unloaded World Partition Actors are not modified by bulk operations. Load them first.
- An Edit-Locked Actor can be skipped by other quick-property operations; not all bulk operations change every Actor.

## Reading mixed state

Off means all eligible Actors are off, On means all are on, and Mixed means the scope differs. Clicking a mixed toggle generally enables it across the eligible scope.

---

## Technical detail

[Read the Technical page](TECHNICAL.md) for engine integration, persistence and implementation boundaries.
