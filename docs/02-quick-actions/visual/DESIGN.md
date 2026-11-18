# Visual — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

Visual contains **Hide in Editor**, **Hidden in Game**, and **Solo**. Those are three distinct choices, not three names for hiding. Hide in Editor changes the viewport presentation of an Actor, Hidden in Game controls the native gameplay property, and Solo isolates rendered primitive components in editor views.

## When to choose each

| You want to... | Use |
| --- | --- |
| Clear visual clutter while building a scene | Hide in Editor |
| Configure the Actor's ordinary in-game hidden property | Hidden in Game |
| Concentrate on a few Actors without hiding everything else one at a time | Solo |

You can click any of these on an Actor, folder or multi-selection; actions affect eligible **loaded** Actors. An actor already hidden in the editor will not reappear just because it is Soloed.

## Two independent visibility settings

**Hide in Editor** controls whether an Actor appears in editor viewports. It is useful for reducing visual noise without changing the Actor's gameplay visibility choice.

**Hidden in Game** changes the Actor's native Hidden in Game setting. Use it when you intentionally want that runtime property configured. Hiding an Actor in the editor does **not** automatically set Hidden in Game, and the reverse is also true.


## Use them

Select an Actor or folder in the Outliner and toggle the appropriate Focus visual segment. Hover first when unsure which is which. A mixed folder means some eligible Actors are hidden while others are not. Toggle mixed to apply one consistent state to the loaded scope.


## Example

You can hide decorative geometry in the editor while leaving it set to appear in game. Conversely, you can see a debug Actor while editing but mark it Hidden in Game.


## Saving matters

These two Focus controls have separate persistence checkboxes. Turning persistence off makes **Focus-initiated** changes temporary for the editor session. Native Details panel changes still save normally; use **Restore Temporary Changes** to discard eligible temporary Focus changes. Edit-Locked Actors are not changed by the Focus Hidden in Game toggle.

---

## Isolate without hiding the rest permanently

**Solo** makes the selected Actor or collection the focus of the editor viewport. Unlike selecting **Hide in Editor** on every other Actor, it does not require you to change all those Actors' visibility settings.


## Typical workflow

1. Solo a folder containing the area you are decorating.
2. Continue selecting and inspecting its loaded Actors.
3. Add another Solo scope if you need to compare several sections.
4. Turn off the Solo states when finished to restore the normal view.

You can Solo an Actor, attachment hierarchy, folder, group or multiple selection. Mixed state tells you that only part of the scope is Soloed.


## What Solo will and will not hide

Focus primarily isolates **rendered primitive components** in editor viewports. Lighting, volumes and other scene contributions can still influence what you see. An Actor explicitly hidden in the editor does not become visible simply because it is Soloed.


## Persistence

Solo can be persistent or session-only depending on Focus settings. It does not turn on **Hidden in Game**. When a persistent Solo state is saved, the related Actor package is part of the save workflow.

---

## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
