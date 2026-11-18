# Simulation — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Exact native property targets

<!-- Media target: IMAGE | simulation-component-scopes | docs/assets/media/simulation-component-scopes.png -->
<figure class="doc-media" data-kind="image" data-media-id="simulation-component-scopes">
  <img src="../../assets/media-placeholder.png" alt="Diagram distinguishing Actor-scoped and Primitive Component-scoped simulation controls" loading="lazy">
  <figcaption>Async Physics Tick applies to Actors, while the remaining Simulation controls target compatible Primitive Components.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Create a labeled Actor/component hierarchy diagram showing Async Physics Tick on the Actor and the remaining five Simulation settings targeting registered Primitive Components. Include a non-primitive component to illustrate an ineligible target.</span></figcaption>
</figure>


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

## Scope, transactions and saved values

Five Simulation controls target eligible Primitive Components; Async Physics Tick targets the Actor. `FFocusColumn::ApplyQuick` skips Edit-Locked Actors and invokes `FocusQuick::Write` under an `FScopedTransaction`. These settings use Unreal's normal native save behavior, not Focus's Visual/Interaction session-only option.

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — property definition, eligibility and write implementation
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — state, Slate widget, transaction and Editor integrations

For the user workflow, [read Design](DESIGN.md).

**Verification:** Code reviewed; live Unreal Editor behavior must be confirmed using the [Verification Matrix](../../VERIFICATION_MATRIX.md).
