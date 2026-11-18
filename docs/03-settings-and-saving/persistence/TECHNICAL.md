# Persistent and Session-Only Editing — Technical

**Focus v2.1.0 · Source-reviewed implementation**

## Three distinct persistence layers

1. **Editor user config:** `UFocusSettings` and category visibility live in EditorPerProject user settings and do not encode Actor state.
2. **Saved Focus metadata:** `Focus.SharedState.V1` is attached to the Actor's actual package, including External Actor packages. Its five-bit string encodes Selection, Transform, Edit, Solo and Hide in Editor.
3. **Native Actor properties:** Hidden in Game and movement location-lock have their own native storage and serialization behavior.

The state cache is held in `TMap<TWeakObjectPtr<AActor>, FFlags> States`, with session edits tracked in `Temporary` baselines.

## Control persistence mapping

`Focus::PersistenceIndex` maps EditorHidden, GameHidden, Solo, Selection, and **shared Protection** to five mask positions. `UFocusSettings::bPersistProtection` covers both Transform and Edit Lock. `Focus::IsPersistent` uses the most recently read settings for Visual and Interaction controls; quick-property groups fall through to native persistence.

## Serializing only intended changes

<!-- Media target: IMAGE | settings-package-overlay | docs/assets/media/settings-package-overlay.png -->
<figure class="doc-media" data-kind="image" data-media-id="settings-package-overlay">
  <img src="../../assets/media-placeholder.png" alt="Diagram of Focus temporarily restoring persistent Actor values during package serialization" loading="lazy">
  <figcaption>During a package save, Focus substitutes recorded persistent values for active session-only native changes before restoring the live editor state.</figcaption>
</figure>


`PersistentMetadata` computes saved Focus metadata from the current shared state, substituting pre-temporary values for controls whose session-only baselines remain active. This prevents saving an unrelated persistent Focus control from unintentionally persisting temporary neighbors.

`OnPackagePreSave` collects temporary values for Actors in the package and temporarily substitutes the prior persistent Hidden in Game, editor-hidden and native movement-lock values. `OnPackagePostSave` returns the live temporary values to the editor. The module subscribes to `UPackage::PreSavePackageWithContextEvent` and `PackageSavedWithContextEvent`.

## Native edits take precedence

`OnNativePropertyEdited` and reconciliation code distinguish genuine Unreal modifications from Focus's session-only setters. An explicit native edit of the same underlying property can invalidate that property's temporary baseline; editing a different property should not.

## Actor metadata and source control

`Focus::EnsureState` loads valid metadata from the Actor package; strict decoding rejects malformed five-character payloads. Metadata changes call `MarkPackageDirty` when the saved value actually changes. In OFPA this is generally the Actor's external package, otherwise the containing level package.

**Evidence limit:** These are the implemented safeguards, not a universal guarantee that every Editor save/transaction or third-party tool avoids a checkout. Test interrupted saves, autosave, Undo/Redo, dirty packages and Blueprint recompilation in the target project.

---

## Relevant source files

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [FocusSettings.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.cpp)

For editor workflows and examples, [see Design](DESIGN.md).

**QA status:** Source verification completed; exhaustive real-editor package-save and source-control testing not claimed.
