# Restore Temporary Changes, Undo and Reload — Technical

**Focus v2.1.0 · Source-reviewed implementation**

## RestoreTemporaryChanges implementation

`Focus::RestoreTemporaryChanges` creates an `FScopedTransaction` and iterates the `Temporary` baseline map. For each editor Actor with outstanding mask bits, it restores editor-hidden, Solo, Selection and Protection state from prior metadata/baselines, and Hidden in Game from its previous native value. Temporary Hidden in Game undo uses `FTemporaryGameHiddenChange`.

At completion it clears the temporary map, redraws editor viewports, and returns the count of restored Actors for the notification in `FFocusColumn::CategoryMenu`.

## Undo of shared Focus metadata

Because `FMetaData` is not itself a UObject with ordinary transaction tracking, Focus uses `FSharedStateChange`, holding before/after snapshots. `ApplySharedState` restores the exact metadata state and live lock/hidden properties. This is distinct from native `Modify` on ordinary Actor or Component properties.

## Promoting temporary values

`Focus::OnPersistenceSettingsChanged` compares the previous five persistence booleans with updated settings. On an OFF → ON transition, it clears relevant temporary mask bits while retaining the live value and marking the appropriate package dirty for future saving.

## Reload and streamed Actors

`RestoreLoadedWorld` initializes saved metadata on loaded editor-world Actors. The module listens for maps opening, level additions and Actor additions and defers restoration until editor objects are ready. It also handles replacement Actors after Blueprint recompilation without copying unrelated metadata.

## Interrupted saves and shutdown

The module's shutdown calls `OnPackagePostSave` for any outstanding package overlays, avoiding leaving saved-only values in the working editor session. This is a recovery path, not proof that all editor crash and save-interruption cases are impossible.

---

## Relevant source files

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [FocusSettings.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.cpp)

For editor workflows and examples, [see Design](DESIGN.md).

**QA status:** Source verification completed; exhaustive real-editor package-save and source-control testing not claimed.
