# Troubleshooting Focus — Technical

**Focus v2.1.0 · Source-reviewed diagnostics and integration**

## Error categories and inspection path

### Module/column absent

Inspect the Editor-only `Focus.uplugin` module declaration and the `FFocusModule` activation prerequisites (Level Editor instance, AssetEditorSubsystem and registered editor mode). The Actor Browser column is registered through SceneOutliner `OnCreateActorBrowserColumns`, so unrelated Outliner picker contexts do not necessarily use it.

### Quick property unavailable

`FocusQuick::GetTargets` filters registered, non-template components by `EScope`; `FindProperty` validates native flags, `EditCondition` and `CanEditChange`. `QuickTooltip` reports eligible count and Not Applicable, Mixed or valid enabled state.

### Guard exceptions

`UFocusEditorMode::IsSelectionDisallowed` intentionally permits Outliner selection. `FFocusInteractionGuard` gates specific native Slate widgets and drag/drop paths. `FFocusCommandGuard` and `FFocusTransformGuard` cover additional native paths. A custom editor or script can bypass these without violating the documented supported path.

### Persistence failures

`Focus.SharedState.V1` is serialized with the Actor's package. `Focus::EnsureState` reads and strictly validates that five-character payload. `Temporary` baselines hold unsaved Focus changes. `PersistentMetadata` excludes temporary neighbors; `OnPackagePreSave` and `OnPackagePostSave` swap native temporary values around serialization. A direct native property edit can replace that property's temporary baseline.

Check whether the package is an OFPA external Actor package, already marked dirty, or altered by other editor actions. If the save was interrupted, the module's cleanup also attempts to resolve leftover overlays on shutdown.

### Rendering and lifetime

`FFocusViewExtension` only applies Solo restrictions in editor worlds when any Solo state is active. Missing Solo effect in a non-editor context is expected. Source settings and per-row cached state are invalidated through category changes and actor/property notifications.

## Suggested reproducible test matrix

Use an ordinary Static Mesh Actor, a Blueprint Actor with several components, a folder with mixed locks, and a loaded/unloaded World Partition pair. Test each separately, then with multi-selection. For persistence, run save, reload, Undo, redo and native edit-overrides in a throwaway map. Capture exact class/property information rather than reporting only 'Focus doesn't work'.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Focus.uplugin](https://github.com/mippi-the-dork/Focus/blob/main/Focus.uplugin)

For normal user steps, [see Design](DESIGN.md).

**QA limit:** This documents the source behavior and test suggestions, not a full live UE 5.8 test report.
