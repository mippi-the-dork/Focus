# Physics Events — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Exact native property targets

| Focus control | Native property | Target |
| --- | --- | --- |
| Generate Hit Events | `BodyInstance.bNotifyRigidBodyCollision` | Primitive Component |
| Generate Overlap Events | `bGenerateOverlapEvents` | Primitive Component |
| Generate Wake Events | `BodyInstance.bGenerateWakeEvents` | Primitive Component |

`FocusQuick::Write` uses `SetNotifyRigidBodyCollision` and `SetGenerateOverlapEvents` where appropriate. Wake notifications use a validated native BodyInstance property write. For all three, component target eligibility is checked; runtime notifications depend on physics and collision behavior beyond Focus.

## Scope and notification conditions

All three controls require eligible, registered Primitive Components. A valid field write does not guarantee hit/overlap/wake events: the engine's body simulation, collision response and event routing still determine runtime delivery. The controls are persisted as native component properties and use Focus's standard quick-property transaction path.

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — property definition, eligibility and write implementation
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — state, Slate widget, transaction and Editor integrations

For the user workflow, [read Design](DESIGN.md).

**Verification:** Code reviewed; live Unreal Editor behavior must be confirmed using the [Verification Matrix](../../VERIFICATION_MATRIX.md).
