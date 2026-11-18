# Simulation — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

| Control | Purpose | Important limitation |
| --- | --- | --- |
| Simulate Physics | Allow eligible primitives to move under physics | Needs compatible mobility and collision |
| Enable Gravity | Apply world gravity to a simulated primitive | Does not start physics simulation |
| Ignore Radial Impulse | Ignore instantaneous radial impulses | Does not also ignore sustained radial force |
| Ignore Radial Force | Ignore continuous radial force | Separate from impulse |
| Apply Impulse on Damage | Permit damage-triggered physical impulse | Does not enable damage events itself |
| Async Physics Tick Enabled | Allow Actor async physics tick | Not the same as ordinary Actor Tick |

## Practical example

To make a prop fall, start with an eligible Primitive Component that can simulate and has an appropriate collision setup. Enable **Simulate Physics** and **Enable Gravity**, then test in a supported simulation context.

A white enabled icon describes a property value, not proof that physics can currently run.

## Using these controls

Enable the **Simulation** and **Physics Events** categories from Focus's column-header menu. Hover each part of the compact symbol before clicking. Apply a choice to an Actor, an attachment hierarchy, or a loaded folder. Component-targeted controls apply to *each eligible component* within that Actor.

For a physics prop, first make its collision and mobility suitable for simulation, then enable Simulate Physics. Enable Gravity if it should fall. If you need Blueprint hit events, enable Generate Hit Events, confirm collision channels and responses, and test in Play.


## Why nothing happens

An enabled flag can be valid even when the rest of Unreal's setup makes it ineffective. Gravity without physics simulation does not make a static object fall. Hit events without suitable contacts will not fire. Unavailable segments mean Focus could not find an eligible editable target.

---

For editable C++ property targets, transaction details and native constraints, [see Technical](TECHNICAL.md).

## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
