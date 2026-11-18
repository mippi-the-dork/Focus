# Mobility — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Native behavior

Mobility uses `EScope::Scene` and the reflected `Mobility` field on valid registered Scene Components. `FocusQuick::Write` calls `USceneComponent::SetMobility` with the selected `EComponentMobility` value.

`FFocusColumn::QuickControl` builds three choices in the order Static, Stationary, Movable. Eligible values aggregate into row On/Mixed state and use `FScopedTransaction`, component `Modify`, and property-change notifications. Edit-Locked Actors are skipped.

The engine may apply mobility and registration constraints beyond those represented by the compact segment.

## Mobility UI and notifications

Mobility offers three individual UI choices mapped to `EComponentMobility::Type`. `FocusQuick::Write` uses the native setter, `Modify()`, `PreEditChange`, and `PostEditChangeProperty`. Other components can reject edits because of native edit conditions.

## Native setter and conditions

`EScope::Scene` resolves registered, non-template Scene Components. `FocusQuick::Write` dispatches `USceneComponent::SetMobility` using an `EComponentMobility` value and appropriate transaction/change notifications. A mixed row can include eligible components with different mobility; engine restrictions remain in force.

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)

For user interaction, [read Design](DESIGN.md). The [Verification Matrix](../../VERIFICATION_MATRIX.md) contains unperformed UE and website checks.
