# Lighting — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

Lighting contains **Lighting Channel 0**, **Lighting Channel 1**, **Lighting Channel 2**, and **Cast Shadows**.

Light and primitive components must share an appropriate lighting channel for channel-based interaction. Changing a light's channels alone may not affect a mesh still on another channel. Cast Shadows is a separate setting from the lighting channels.

## Example

To have a particular light affect only selected props, configure matching channels on that light and receiving props, while giving other props a different channel arrangement. Then verify actual lighting in the editor.

Channels are not a replacement for light visibility, mobility, materials, or other rendering constraints.

## Lighting: four separate toggles

Lighting Channel **0**, **1**, and **2** decide which enabled channels an eligible light or primitive belongs to. For a light to affect a receiving primitive through lighting channels, they generally need an overlapping enabled channel. **Cast Shadows** controls shadow casting for eligible components; shadow visibility still depends on light, material, and renderer settings.

A useful test: set a light and selected props to Channel 1, leave unrelated props on Channel 0, then inspect the lighting. Channel flags are not a replacement for normal light visibility or mobility rules.

## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
