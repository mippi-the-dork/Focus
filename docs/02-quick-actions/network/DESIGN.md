# Network — Design

**Focus v2.1.0 · Quick Actions · User guide**

## Available controls

| Quick Action | Meaning | Doesn't replace |
| --- | --- | --- |
| **Net Load on Client** | Whether a placed Actor participates in loading on clients | Actor replication configuration |
| **Replicate Physics to Autonomous Proxy** | Physics-state replication setting on eligible Primitive Components | Suitable replication roles, ownership and physics setup |
| **Static Mesh Replicate Movement** | Movement-replication flag on compatible Static Mesh Actors | Replication on unrelated Actor classes |

## Workflow: check a networked Static Mesh Actor

1. Configure the Actor's normal replication, authority and ownership in Unreal; Focus does not set all of these.
2. Enable **Network** in the Outliner Focus header.
3. Hover the desired micro-widget segment to confirm its native purpose and eligibility.
4. Toggle **Static Mesh Replicate Movement** only if its eligible Actor instance exposes that property.
5. Run a multi-client PIE test and verify movement on client instances. A highlighted button alone does not prove the replicated state arrived.

## Understand unavailable versus ineffective

An Actor without the relevant instance-editable property may show **Not Applicable**. A compatible property that is enabled can still be ineffective if the network driver, ownership, or physics configuration doesn't support the scenario.

**Replicate Physics to Autonomous Proxy** is Primitive Component scoped, unlike the Actor-scoped Net Load on Client. A mixed folder might have both eligible and ineligible components.

## Target scope, state and saving

These Quick Actions operate on **currently loaded, eligible Actors**, including compatible attached descendants, nested folders, groups and selected Outliner rows. Hover for the state and number of eligible targets. **Mixed** means values differ across those targets; **Not Applicable** means no editable target exists.

Native-property changes respect Edit Lock, support Unreal transactions/Undo and follow normal Actor/Component save behavior. They are not part of Focus's separate Visual/Interaction session-only persistence system. See [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For native properties and the exact Actor/Primitive eligibility rules, [read Technical](TECHNICAL.md).
