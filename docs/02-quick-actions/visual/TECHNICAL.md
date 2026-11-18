# Visual — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Storage boundaries

Editor-hidden and Solo have saved metadata flags when persistence is enabled. Hidden in Game uses its native Actor property. Per-control Focus session settings determine whether Focus-authored changes are committed or only retained for the current editor session.

## Native values

`Focus::Get(EditorHidden)` reads `AActor::IsTemporarilyHiddenInEditor(false)`. `Focus::Set(EditorHidden)` uses the base-Actor temporary editor-hidden path; the state can be represented in the Actor's `Focus.SharedState.V1` metadata.

`Focus::Get(GameHidden)` reads `AActor::IsHidden()`. `Focus::Set(GameHidden)` calls `SetActorHiddenInGame`; the persistent path uses `Modify()` and native Actor save semantics, whereas the session-only path uses a temporary baseline/undo command without a normal persistent edit.


## Persistence and save overlays

`SessionEditorHidden` and `SessionGameHidden` are different temporary-mask bits. For session-only values, `OnPackagePreSave` substitutes the baseline for serialization and `OnPackagePostSave` restores the editor-visible temporary value. A native edit to the same property takes priority over the old Focus baseline.


## Constraints

`Focus::Set(GameHidden)` checks Edit Lock and skips a protected Actor. The plugin does not change the actual editor/game visibility relationship defined by Unreal Engine. Verify World Partition saves and unusual Actor types in the target project.

---

## Per-view implementation

`FFocusViewExtension::SetupView` checks `Focus::HasSolo(World)` and applies only in `EWorldType::Editor`. It gathers primitive scene IDs from Soloed Actors and puts them into `View.ShowOnlyPrimitives`. If another extension already restricts `ShowOnlyPrimitives`, Focus intersects the sets rather than replacing prior restrictions.


## Data and lifecycle

The Solo bit is tracked in `Focus::FFlags` and is encoded in `Focus.SharedState.V1` metadata when persistent. The view extension does not rewrite Hidden in Editor or Hidden in Game on every other Actor.


## Performance and edge cases

The extension checks whether any Solo state is active before building an allowed primitive set. The filter is not an engine-wide rule for lights, volumes or every contribution to rendering. Solo only alters applicable editor views, not packaged gameplay. Clearing every Solo bit returns the view to standard scene visibility, subject to other editor settings.

---

## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
