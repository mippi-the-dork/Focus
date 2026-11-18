# Selection, Transform and Edit Locks — Technical

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Selection Lock routing

`UFocusEditorMode::IsSelectionDisallowed` handles supported selection attempts. It explicitly permits selection initiated with a Scene Outliner under the pointer. Cut/delete processing also checks protected selections.

## Transform Lock routing

`FFocusTransformGuard` records transaction-scoped snapshots around standard editor changes and restores protected world transforms. It does not continuously rewrite Actors every frame. The transform guard deliberately allows applicable Blueprint-driven changes rather than treating authored runtime logic as an editor drag.

## Edit Lock routing

`FFocusModule::BindView` wraps a Details view's `FIsPropertyReadOnly` and `FIsPropertyEditingEnabled` delegates, preserving the original handlers. `FFocusInteractionGuard` wraps eligible Outliner renames, component add/rename paths and structural drag/drop interactions. `FFocusCommandGuard` handles supported editor commands.

## State model

`Focus::Get(Transform)` evaluates `Flags.Transform || Flags.Edit`. `Focus::Set(Transform)` refuses to change it while Edit Lock is active. `UFocusSettings::bPersistProtection` controls session persistence for both protection levels. The level selection is not a security mechanism for Python, custom editors or arbitrary C++.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)

For control workflows and examples, [read the Design page](DESIGN.md).

**Validation:** Implementation described from Focus 2.1.0 source. Project-specific live editor QA is separate from documentation review.
