# Mobility — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

Mobility offers **S** (Static), **T** (Stationary) and **M** (Movable) for eligible Scene Components.

- **Static:** Nonmoving authored geometry.
- **Stationary:** Restricted movement appropriate to applicable Unreal systems.
- **Movable:** Content that can move as allowed by its component and engine setup.

## Usage

Enable Mobility, hover the three options, and choose the one you intend. For several Actors, click the appropriate value on a selected row to apply it to the current eligible selection. Different Scene Components inside the same Actor can be eligible.

Mobility choices interact with physics simulation, light settings and construction. Changing Mobility alone does not enable Simulate Physics.

## Mobility: Static, Stationary or Movable

Focus presents **S**, **T**, and **M** for eligible Scene Components. Hover each choice to confirm the selection. Static is for nonmoving content, Stationary allows certain restricted changes, and Movable supports dynamic movement where Unreal allows it. The correct choice depends on the component, physics and lighting setup.

To change a group, select the appropriate Actor/folder and click the desired mobility option. This affects eligible Scene Components, not just an Actor's root in all cases.

## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
