# Installation and Compatibility — Technical

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Declared compatibility

`Focus.uplugin` declares version `2.1.0` (`Version: 210`), `EngineVersion: 5.8.0`, and an Editor module with `PlatformAllowList: ["Win64"]`. This describes UE 5.8.x/Win64 targeting; it is not proof of other platform or engine-version compatibility.

## Build integration

`Focus.Build.cs` contains the `UnrealEd`, `EditorFramework`, `LevelEditor`, `SceneOutliner`, `SubobjectEditor`, `PropertyEditor`, `RenderCore`, `DeveloperSettings`, and related editor dependencies. Build with a C++-capable Editor target. This module must not be referenced by a packaged runtime target.

## Startup and editor readiness

`FFocusModule::TryActivate` waits for the actual Level Editor instance and the scriptable editor mode registry before activating. It calls `ActivateDefaultMode` first when needed so Focus does not suppress native transform widgets during early startup. The column is added through the Actor Browser extension point, not a forced edit to engine files.

## Failed appearance diagnostics

Separate module loading, column registration, category visibility, and row eligibility. The Focus column can exist even when individual property categories are turned off. Plugin startup or native UI issues require checking Unreal's Output Log and module/build compatibility.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)

For control workflows and examples, [read the Design page](DESIGN.md).

**Validation:** Implementation described from Focus 2.1.0 source. Project-specific live editor QA is separate from documentation review.
