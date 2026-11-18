# Physics Events — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Exact native property targets

| Focus control | Native property | Target |
| --- | --- | --- |
| Generate Hit Events | `BodyInstance.bNotifyRigidBodyCollision` | Primitive Component |
| Generate Overlap Events | `bGenerateOverlapEvents` | Primitive Component |
| Generate Wake Events | `BodyInstance.bGenerateWakeEvents` | Primitive Component |

`FocusQuick::Write` uses `SetNotifyRigidBodyCollision` and `SetGenerateOverlapEvents` where appropriate. Wake notifications use a validated native BodyInstance property write. For all three, component target eligibility is checked; runtime notifications depend on physics and collision behavior beyond Focus.



## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
