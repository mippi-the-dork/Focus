# Mobility — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Native behavior

Mobility uses `EScope::Scene` and the reflected `Mobility` field on valid registered Scene Components. `FocusQuick::Write` calls `USceneComponent::SetMobility` with the selected `EComponentMobility` value.

`FFocusColumn::QuickControl` builds three choices in the order Static, Stationary, Movable. Eligible values aggregate into row On/Mixed state and use `FScopedTransaction`, component `Modify`, and property-change notifications. Edit-Locked Actors are skipped.

The engine may apply mobility and registration constraints beyond those represented by the compact segment.

## Mobility UI and notifications

Mobility offers three individual UI choices mapped to `EComponentMobility::Type`. `FocusQuick::Write` uses the native setter, `Modify()`, `PreEditChange`, and `PostEditChangeProperty`. Other components can reject edits because of native edit conditions.

## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
