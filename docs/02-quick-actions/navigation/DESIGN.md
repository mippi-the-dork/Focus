# Navigation — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

| Control | Meaning |
| --- | --- |
| Can Ever Affect Navigation | Allow eligible Actor Components to contribute navigation data |
| Dynamic Obstacle | Mark eligible Shape Components as navigation obstacles |
| Fill Collision Underneath for Navmesh | Fill area beneath compatible primitive collision |
| Rasterize as Filled Convex Volume | Treat appropriate primitive collision as a filled convex volume |

## Example

If decorative collision is contaminating navigation, disable its navigation contribution through Focus and rebuild/inspect the navmesh. For a volume intended as a dynamic obstacle, check Dynamic Obstacle and the actual navigation settings.

Toggling a property does not guarantee an immediate or successful navigation rebuild.

## Navigation controls

| Control | Intended effect |
| --- | --- |
| Can Ever Affect Navigation | Eligible Actor Component may contribute navigation data |
| Dynamic Obstacle | Eligible Shape Component contributes as a dynamic obstacle where supported |
| Fill Collision Underneath for Navmesh | Fill navigable space beneath compatible primitive collision |
| Rasterize as Filled Convex Volume | Treat compatible primitive geometry as a filled convex region for navigation rasterization |

These affect inputs to navigation generation. They do not guarantee an immediate navmesh rebuild or a valid navigation path.


## A practical scene example

You have decorative collision that should not modify walking routes. Disable its navigation contribution. For a collision volume intended to carve a dynamic area, review Dynamic Obstacle. Verify the generated NavMesh visually in Unreal rather than assuming the toggle alone accomplished the intended navigation result.

## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
