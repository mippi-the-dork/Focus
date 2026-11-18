# Interaction — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

Interaction has **Selection Lock**, plus a cycling **Protection Lock** with three levels: Unlocked → Transform Lock → Edit Lock → Unlocked. Selection Lock is independent of that cycle.

Choose Selection Lock to avoid picking something accidentally in the viewport. Choose Transform Lock to prevent ordinary position/rotation/scale edits while keeping unrelated properties editable. Choose Edit Lock to block broader supported native Actor/Component edits.

**Not a permission system:** custom editor tools and scripts can still change locked Actors.

## Choose the protection you need

| Control | What it protects | What remains possible |
| --- | --- | --- |
| Selection Lock | Accidental viewport picking | Select from World Outliner; inspect or unlock |
| Transform Lock | Normal position, rotation and scale editing | Edit unrelated properties |
| Edit Lock | Supported Actor and Component edits, including transforms | Inspect Details, use search and category navigation |

Selection Lock is independent of the other two. **Transform Lock** and **Edit Lock** are protection levels: Edit Lock includes transform protection.


## Practical examples

Use Selection Lock on dense background meshes so clicking through the scene is easier. Transform Lock finished architecture without preventing an artist from adjusting its material. Use Edit Lock on a reviewed assembly when accidental modifications would be costly.


## A locked Actor is not inaccessible

Select it in the Outliner to inspect and unlock it. Some specialized tools, scripts or custom editor plugins may still change it: Focus guards supported Unreal editing paths, not arbitrary code.


## Hierarchy behavior

A mixed protection icon indicates different eligible Actors have different levels. Applying a level affects eligible loaded Actors in that scope. Before changing many Actors, confirm you selected the right folder or selection set.

---

## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
