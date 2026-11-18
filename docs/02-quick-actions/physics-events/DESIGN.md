# Physics Events — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

| Control | Purpose | Requires |
| --- | --- | --- |
| Generate Hit Events | Receive supported rigid-body collision hit notifications | Compatible collision/contacts |
| Generate Overlap Events | Receive component overlap notifications | Both objects' collision responses permit overlap |
| Generate Wake Events | Receive physics wake/sleep event notifications | A compatible body participating in simulation |

## Workflow: checking an overlap

Enable **Generate Overlap Events** on an eligible Primitive Component through the Outliner. Verify its collision profile and the other object's collision response. Then test the Blueprint or C++ event in Play.

This category configures event generation; it does not author event handlers or create collision geometry. An enabled flag alone does not guarantee a callback.



## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
