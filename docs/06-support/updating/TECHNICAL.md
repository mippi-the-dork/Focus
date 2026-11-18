# Updating Focus and Getting Support — Technical

**Focus v2.1.0 · Source-reviewed diagnostics and integration**

## Version and module compatibility

`Focus.uplugin` v2.1.0 uses `Version: 210`, `EngineVersion: 5.8.0`, the `Focus` `Editor` module with `LoadingPhase: PostEngineInit`, and a `Win64` platform allowlist. `Focus.Build.cs` references editor-facing modules such as `UnrealEd`, `SceneOutliner`, `LevelEditor`, `SubobjectEditor`, `PropertyEditor`, `Slate`, and `DeveloperSettings`.

A normal packaged target should not link this Editor module. Installing on another engine or platform is a separate support decision, not demonstrated by `VersionName` alone.

## Upgrade safeguards

When replacing a precompiled release, do not overlay old `Binaries`, `Intermediate`, `Source` or `Resources` blindly. Close the editor first, replace the complete directory, and restart. For source builds, ensure the engine's project-file generation and toolchain reflect the chosen UE version. The plugin doesn't promise migration of arbitrary manually modified internal C++ state.

## Settings and saved Actors

`UFocusSettings` uses Editor-per-project user settings. Saved Focus state uses Actor-package metadata `Focus.SharedState.V1`. Update/reinstall documentation should not instruct users to delete Actor packages to reset Focus: doing so would damage user content. The existing shared-state metadata remains part of normal level/Actor save workflows.

## Reporting actionable technical issues

Provide the exact engine/plugin version and native stack/log, package-save conditions, target Actor/component type, and whether the operation was Focus-native or initiated by another editor tool. To reproduce a lock issue, specify the input path (viewport mouse, Outliner rename, Details edit, component drag, script). To reproduce session persistence, note the last saved baseline and whether the package was clean before the action.

## Documentation maintenance

When a release changes `FocusQuick::Definitions`, persistence mask behavior, header controls, or native editor guards, update both the Design workflow and Technical ownership pages in the same documentation PR. This manual's source links point to the repository's main branch; historical version references should use versioned commits/tags if necessary.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Focus.uplugin](https://github.com/mippi-the-dork/Focus/blob/main/Focus.uplugin)

For normal user steps, [see Design](DESIGN.md).

**QA limit:** This documents the source behavior and test suggestions, not a full live UE 5.8 test report.
