# Simulation and Physics Events — Technical

**Focus v2.1.0 · Source-verified implementation reference**

## Definitions and target scopes

`FocusQuick::Definitions()` maps six Simulation and three Physics Events controls. All but **Async Physics Tick Enabled** use `EScope::Primitive`; async tick is `EScope::Actor`.

| Control | Native property |
| --- | --- |
| Simulate Physics | `BodyInstance.bSimulatePhysics` |
| Enable Gravity | `BodyInstance.bEnableGravity` |
| Ignore Radial Impulse | `bIgnoreRadialImpulse` |
| Ignore Radial Force | `bIgnoreRadialForce` |
| Apply Impulse on Damage | `bApplyImpulseOnDamage` |
| Async Physics Tick Enabled | `AActor::bAsyncPhysicsTickEnabled` |
| Generate Hit Events | `BodyInstance.bNotifyRigidBodyCollision` |
| Generate Overlap Events | `bGenerateOverlapEvents` |
| Generate Wake Events | `BodyInstance.bGenerateWakeEvents` |

## Native setter behavior

`FocusQuick::Write` routes physics simulation, gravity, collision notification, and overlap generation through `UPrimitiveComponent::SetSimulatePhysics`, `SetEnableGravity`, `SetNotifyRigidBodyCollision` and `SetGenerateOverlapEvents`. Other eligible booleans use reflected property writes after validating property editability.

## Eligible targets and transaction

`GetTargets` enumerates registered, non-template Primitive Components for primitive-scoped controls. The Actor-scoped async property directly targets the Actor. One Focus action gathers the loaded Actor scope, skips Actors under Edit Lock, and changes each eligible target inside an `FScopedTransaction`. `Write` calls `Modify`, `PreEditChange` and `PostEditChangeProperty` when writing.

## Engine constraints

A property being editable says nothing about collision viability, body creation, simulation support, collision responses, or tick prerequisites. Those outcomes are Unreal Engine configuration, not additional logic supplied by Focus. Verify event delivery and physics behavior in PIE or the appropriate simulation mode.

---

## Source files

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — definitions, property validation and setters
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — row state, mixed state and transactional editing

For user interaction, examples and interpretations, [see Design](DESIGN.md).

**Verification scope:** source-reviewed; no claim of a completed live editor QA matrix.
