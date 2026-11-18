# Network and Navigation — Technical

**Focus v2.1.0 · Source-verified implementation reference**

## Exact network property targets

| Label | Scope | Native property |
| --- | --- | --- |
| Net Load on Client | Actor | `bNetLoadOnClient` |
| Replicate Physics to Autonomous Proxy | Primitive | `bReplicatePhysicsToAutonomousProxy` |
| Static Mesh Replicate Movement | Actor | `bStaticMeshReplicateMovement` |

The last property is exposed through the Actor-level scope and is subject to that Actor's property availability; other classes do not gain a new replication system from Focus.

## Exact navigation property targets

| Label | Scope | Native property |
| --- | --- | --- |
| Can Ever Affect Navigation | Actor Component | `bCanEverAffectNavigation` |
| Dynamic Obstacle | Shape Component | `bDynamicObstacle` |
| Fill Collision Underneath for Navmesh | Primitive | `bFillCollisionUnderneathForNavmesh` |
| Rasterize as Filled Convex Volume | Primitive | `bRasterizeAsFilledConvexVolume` |

`FocusQuick::Write` invokes `UActorComponent::SetCanEverAffectNavigation` for that property, rather than only setting the reflected field. Remaining applicable flags go through validated property writes and normal edit-change notifications.

## Applicability

`FocusQuick::GetTargets` filters components by `EScope` and requires registered, non-template components. `FocusQuick::FindProperty` checks the reflected property, edit flags, `EditCondition`, and `CanEditChange`. An Actor may therefore have only some segments available. Mixed rows aggregate only eligible values.

## No replication/navmesh framework

This plugin does not create net drivers, replicate arbitrary Actors, call navigation movement solvers, or guarantee that a navmesh rebuild happens at a specific time. It changes native configuration and leaves runtime replication and nav generation to Unreal.

---

## Source files

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — definitions, property validation and setters
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — row state, mixed state and transactional editing

For user interaction, examples and interpretations, [see Design](DESIGN.md).

**Verification scope:** source-reviewed; no claim of a completed live editor QA matrix.
