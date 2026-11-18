# Applicability, Mixed States and Undo — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Why a quick control is unavailable.

## Verified behavior

Only instance-editable properties on eligible registered Actor/Component targets can be changed. Edit-locked Actors are skipped; writes use native transactions.

## Source eligibility checks

The source excludes template objects, missing or unregistered components, properties without CPF_Edit, instance-disabled, EditConst or EditCondition metadata, and properties denied by CanEditChange. A disabled boolean, a mixed value and an unavailable property are distinct states. Quick edits call Modify, PreEditChange and PostEditChangeProperty; physics/lighting/mobility/LOD changes use their dedicated engine setters where necessary.
## Native property catalog (source-derived)

| Category | Display label | Eligible scope | Property |
| --- | --- | --- | --- |
| Simulation | Simulate Physics | Primitive | `BodyInstance.bSimulatePhysics` |
| Simulation | Enable Gravity | Primitive | `BodyInstance.bEnableGravity` |
| Simulation | Ignore Radial Impulse | Primitive | `bIgnoreRadialImpulse` |
| Simulation | Ignore Radial Force | Primitive | `bIgnoreRadialForce` |
| Simulation | Apply Impulse on Damage | Primitive | `bApplyImpulseOnDamage` |
| Simulation | Async Physics Tick Enabled | Actor | `bAsyncPhysicsTickEnabled` |
| PhysicsEvents | Generate Hit Events | Primitive | `BodyInstance.bNotifyRigidBodyCollision` |
| PhysicsEvents | Generate Overlap Events | Primitive | `bGenerateOverlapEvents` |
| PhysicsEvents | Generate Wake Events | Primitive | `BodyInstance.bGenerateWakeEvents` |
| Lighting | Lighting Channel 0 | Lighting | `LightingChannels.bChannel0` |
| Lighting | Lighting Channel 1 | Lighting | `LightingChannels.bChannel1` |
| Lighting | Lighting Channel 2 | Lighting | `LightingChannels.bChannel2` |
| Lighting | Cast Shadows | Lighting | `CastShadow` |
| World | In Main World Only | Actor | `bIsMainWorldOnly` |
| World | Is Editor Only Actor | Actor | `bIsEditorOnlyActor` |
| World | Is Spatially Loaded | Actor | `bIsSpatiallyLoaded` |
| World | Include Actor in HLOD | Actor | `bEnableAutoLODGeneration` |
| Network | Net Load on Client | Actor | `bNetLoadOnClient` |
| Network | Replicate Physics to Autonomous Proxy | Primitive | `bReplicatePhysicsToAutonomousProxy` |
| Network | Static Mesh Replicate Movement | Actor | `bStaticMeshReplicateMovement` |
| Navigation | Can Ever Affect Navigation | ActorComponent | `bCanEverAffectNavigation` |
| Navigation | Dynamic Obstacle | Shape | `bDynamicObstacle` |
| Navigation | Fill Collision Underneath for Navmesh | Primitive | `bFillCollisionUnderneathForNavmesh` |
| Navigation | Rasterize as Filled Convex Volume | Primitive | `bRasterizeAsFilledConvexVolume` |
| Mobility | Mobility | Scene | `Mobility` |
| LOD | Forced LOD Model | Mesh | `ForcedLodModel` |
| LOD | Never Distance Cull | Primitive | `bNeverDistanceCull` |
| LOD | Allow Cull Distance Volume | Primitive | `bAllowCullDistanceVolume` |
## Native behavior and original reference

# Mixed States

Folders and selections can contain Actors with different Focus settings.

Focus represents this using a mixed state.

For example:

```text
Environment
├── Wall_A        Transform Locked
├── Wall_B        Transform Locked
└── Door          Unlocked
```

The protection control for the `Environment` Folder will show that its contents do not all share the same state.

This makes it easier to understand the actual state of a hierarchy without expanding and inspecting every Actor individually.

---

# Example Workflow

---

### Mixed States

Focus indicates when a hierarchy contains a mixture of enabled and disabled states.

### Undo / Redo

Focus control changes participate in Unreal Engine's normal Undo and Redo workflow.

### Persistent Settings

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
