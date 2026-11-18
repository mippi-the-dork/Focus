# Simulation — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Exact native property targets

| Focus control | Native property | Target |
| --- | --- | --- |
| Simulate Physics | `BodyInstance.bSimulatePhysics` | Primitive Component |
| Enable Gravity | `BodyInstance.bEnableGravity` | Primitive Component |
| Ignore Radial Impulse | `bIgnoreRadialImpulse` | Primitive Component |
| Ignore Radial Force | `bIgnoreRadialForce` | Primitive Component |
| Apply Impulse on Damage | `bApplyImpulseOnDamage` | Primitive Component |
| Async Physics Tick Enabled | `bAsyncPhysicsTickEnabled` | Actor |

## Native setter behavior

`FocusQuick::Write` routes physics simulation, gravity, collision notification, and overlap generation through `UPrimitiveComponent::SetSimulatePhysics`, `SetEnableGravity`, `SetNotifyRigidBodyCollision` and `SetGenerateOverlapEvents`. Other eligible booleans use reflected property writes after validating property editability.


## Eligible targets and transaction

`GetTargets` enumerates registered, non-template Primitive Components for primitive-scoped controls. The Actor-scoped async property directly targets the Actor. One Focus action gathers the loaded Actor scope, skips Actors under Edit Lock, and changes each eligible target inside an `FScopedTransaction`. `Write` calls `Modify`, `PreEditChange` and `PostEditChangeProperty` when writing.


## Engine constraints

A property being editable says nothing about collision viability, body creation, simulation support, collision responses, or tick prerequisites. Those outcomes are Unreal Engine configuration, not additional logic supplied by Focus. Verify event delivery and physics behavior in PIE or the appropriate simulation mode.

---

## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
