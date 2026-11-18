# Complete Control Reference — Technical

**Focus v2.1.0 · C++ source reference**

## All 28 native definitions

The following controls come directly from `FocusQuick::Definitions()`; they are not new Focus-owned runtime properties.

| Group | Display label | Eligible target scope | Native property |
| --- | --- | --- | --- |
| Simulation | Simulate Physics | Primitive | `BodyInstance.bSimulatePhysics` |
| Simulation | Enable Gravity | Primitive | `BodyInstance.bEnableGravity` |
| Simulation | Ignore Radial Impulse | Primitive | `bIgnoreRadialImpulse` |
| Simulation | Ignore Radial Force | Primitive | `bIgnoreRadialForce` |
| Simulation | Apply Impulse on Damage | Primitive | `bApplyImpulseOnDamage` |
| Simulation | Async Physics Tick Enabled | Actor | `bAsyncPhysicsTickEnabled` |
| Physics Events | Generate Hit Events | Primitive | `BodyInstance.bNotifyRigidBodyCollision` |
| Physics Events | Generate Overlap Events | Primitive | `bGenerateOverlapEvents` |
| Physics Events | Generate Wake Events | Primitive | `BodyInstance.bGenerateWakeEvents` |
| Lighting | Lighting Channel 0 | Lighting | `LightingChannels.bChannel0` |
| Lighting | Lighting Channel 1 | Lighting | `LightingChannels.bChannel1` |
| Lighting | Lighting Channel 2 | Lighting | `LightingChannels.bChannel2` |
| Lighting | Cast Shadows | Lighting | `CastShadow` (light component naming may differ) |
| World | In Main World Only | Actor | `bIsMainWorldOnly` |
| World | Is Editor Only Actor | Actor | `bIsEditorOnlyActor` |
| World | Is Spatially Loaded | Actor | `bIsSpatiallyLoaded` |
| World | Include Actor in HLOD | Actor | `bEnableAutoLODGeneration` |
| Network | Net Load on Client | Actor | `bNetLoadOnClient` |
| Network | Replicate Physics to Autonomous Proxy | Primitive | `bReplicatePhysicsToAutonomousProxy` |
| Network | Static Mesh Replicate Movement | Actor | `bStaticMeshReplicateMovement` |
| Navigation | Can Ever Affect Navigation | Actor Component | `bCanEverAffectNavigation` |
| Navigation | Dynamic Obstacle | Shape | `bDynamicObstacle` |
| Navigation | Fill Collision Underneath for Navmesh | Primitive | `bFillCollisionUnderneathForNavmesh` |
| Navigation | Rasterize as Filled Convex Volume | Primitive | `bRasterizeAsFilledConvexVolume` |
| Mobility | Mobility | Scene | `Mobility` |
| LOD | Forced LOD Model | Static/Skinned Mesh | `ForcedLodModel` |
| LOD | Never Distance Cull | Primitive | `bNeverDistanceCull` |
| LOD | Allow Cull Distance Volume | Primitive | `bAllowCullDistanceVolume` |

## Quick-property API

`FocusQuick::Definitions` defines stable label/symbol/scope/kind/property mappings. `GetTargets` discovers valid eligible components; `FindProperty` validates editability and handles nested property structures; `Read` obtains current values and `Write` modifies via native setters or reflection with editor notifications. The Outliner column aggregates eligible values for mixed states.

## Distinction from Focus-specific state

The Visual/Interaction controls instead route through `Focus::EControl` and `Focus::Get/Set`. Their metadata and session-only behavior are not the same as the 28 quick-property definitions. No quick-property category toggling installs a new runtime component or authoring asset.

## Advanced caveats

Forced LOD 0 is automatic and positive values address mesh LODs starting at index zero. Eligibility depends on the specific Actor class, registered component and instance-editable property. Some values can be written but still require suitable engine systems to produce an observable effect.

---

## Browse relevant source

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusState.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusState.h)

For usage and workflows, [see Design](DESIGN.md).

**QA status:** Source-reviewed; no claim of exhaustive live-editor integration testing.
