# Network — Design

**Focus v2.1.0 · Quick Actions · User guide**

## What this category controls

Network exposes **Net Load on Client**, **Replicate Physics to Autonomous Proxy**, and **Static Mesh Replicate Movement**.

These are targeted native settings, not a one-click replication setup. If the Actor's replication, ownership, or component physics are not otherwise configured correctly, toggling one field will not solve network synchronization.

## Example

For a networked Static Mesh Actor, configure its general Actor replication normally, then use Focus for the specific movement or physics replication option if supported. Validate in multi-client PIE.

## Network controls

Focus exposes three narrow native networking options rather than a complete replication editor.

- **Net Load on Client:** whether a placed Actor loads on clients as part of networked level loading. It does not activate replication.
- **Replicate Physics to Autonomous Proxy:** permits a compatible Primitive Component physics state to replicate to autonomous proxies when the gameplay setup supports it.
- **Static Mesh Replicate Movement:** controls the corresponding Static Mesh Actor movement-replication setting. It does not apply to every arbitrary Actor type.

For a networked test, first configure the Actor's broader replication, ownership, movement and physics settings, then use Focus for the specific property. Verify behavior in multi-instance PIE; a lit segment cannot prove a packet was sent.

## Shared Quick Action behavior

Quick Actions apply to **currently loaded eligible Actors**, including applicable attached descendants, nested folders, native groups and selected rows. If the row is part of a multi-selection, the action can cover the other selected rows. A Mixed control means different eligible targets disagree, while Not Applicable means no editable compatible target exists. An Edit-Locked Actor is skipped by native quick-property writes.

For the full eligibility and saved/temporary editing rules, see [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For the Unreal implementation and exact property mapping, [read Technical](TECHNICAL.md).
