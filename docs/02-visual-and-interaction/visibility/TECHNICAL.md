# Hide in Editor and Hidden in Game — Technical

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Native values

`Focus::Get(EditorHidden)` reads `AActor::IsTemporarilyHiddenInEditor(false)`. `Focus::Set(EditorHidden)` uses the base-Actor temporary editor-hidden path; the state can be represented in the Actor's `Focus.SharedState.V1` metadata.

`Focus::Get(GameHidden)` reads `AActor::IsHidden()`. `Focus::Set(GameHidden)` calls `SetActorHiddenInGame`; the persistent path uses `Modify()` and native Actor save semantics, whereas the session-only path uses a temporary baseline/undo command without a normal persistent edit.

## Persistence and save overlays

`SessionEditorHidden` and `SessionGameHidden` are different temporary-mask bits. For session-only values, `OnPackagePreSave` substitutes the baseline for serialization and `OnPackagePostSave` restores the editor-visible temporary value. A native edit to the same property takes priority over the old Focus baseline.

## Constraints

`Focus::Set(GameHidden)` checks Edit Lock and skips a protected Actor. The plugin does not change the actual editor/game visibility relationship defined by Unreal Engine. Verify World Partition saves and unusual Actor types in the target project.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)

For control workflows and examples, [read the Design page](DESIGN.md).

**Validation:** Implementation described from Focus 2.1.0 source. Project-specific live editor QA is separate from documentation review.
