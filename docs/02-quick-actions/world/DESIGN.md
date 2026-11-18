# World — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

World contains four Actor-level options:

| Control | Meaning |
| --- | --- |
| In Main World Only | Restrict supported Actors to the main world context |
| Is Editor Only Actor | Exclude eligible Actor from cooked gameplay content |
| Is Spatially Loaded | Control World Partition spatial loading eligibility |
| Include Actor in HLOD | Permit participation in applicable HLOD generation |

## Use with care

These options affect how Unreal handles whole Actors, not merely the Outliner presentation. An editor-only Actor referenced by runtime content, or a spatial-loading choice made without understanding World Partition, can have consequences outside the current viewport.

For World Partition and HLOD options, confirm behavior in the actual world configuration.

## World: four Actor-level options

| Control | Purpose | When to use it carefully |
| --- | --- | --- |
| In Main World Only | Restricts supported Actors to their main-world context | World duplication and editor previews |
| Is Editor Only Actor | Marks an Actor as editor-only where supported | Do not leave gameplay references dependent on stripped Actors |
| Is Spatially Loaded | Controls World Partition spatial-loading eligibility | Relevant only in supported World Partition contexts |
| Include Actor in HLOD | Requests HLOD generation participation | Requires a suitable HLOD-enabled world |

## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
