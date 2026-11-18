# Interaction — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Protection cycle behavior

`FFocusColumn::ProtectionState` represents unlocked, transform-locked, edit-locked, or mixed protection levels. Clicking a mixed scope normalizes it to Transform Lock. The combined protection level never silently toggles Selection Lock.

## Selection Lock routing

`UFocusEditorMode::IsSelectionDisallowed` handles supported selection attempts. It explicitly permits selection initiated with a Scene Outliner under the pointer. Cut/delete processing also checks protected selections.


## Transform Lock routing

`FFocusTransformGuard` records transaction-scoped snapshots around standard editor changes and restores protected world transforms. It does not continuously rewrite Actors every frame. The transform guard deliberately allows applicable Blueprint-driven changes rather than treating authored runtime logic as an editor drag.


## Edit Lock routing

`FFocusModule::BindView` wraps a Details view's `FIsPropertyReadOnly` and `FIsPropertyEditingEnabled` delegates, preserving the original handlers. `FFocusInteractionGuard` wraps eligible Outliner renames, component add/rename paths and structural drag/drop interactions. `FFocusCommandGuard` handles supported editor commands.


## State model

`Focus::Get(Transform)` evaluates `Flags.Transform || Flags.Edit`. `Focus::Set(Transform)` refuses to change it while Edit Lock is active. `UFocusSettings::bPersistProtection` controls session persistence for both protection levels. The level selection is not a security mechanism for Python, custom editors or arbitrary C++.

---

## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
