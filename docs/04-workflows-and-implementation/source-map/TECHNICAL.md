# Source Code and Data Ownership — Technical

**Focus v2.1.0 · C++ source reference**

## File ownership

| File | Main responsibility |
| --- | --- |
| `Focus.uplugin` | Module kind, loading phase, platform and plugin metadata |
| `Focus.Build.cs` | Editor module dependencies |
| `FocusModule.cpp` | Focus state, metadata transactions, save overlays, row widgets, native guards, Solo extension, module lifecycle |
| `FocusQuickProperties.cpp/.h` | 28 native definitions, scope validation, read/write and setters |
| `FocusSettings.h/.cpp` | Editor preferences, persistence choices and color handling |
| `FocusState.h` | Public internal state/control API |
| `FocusInteractionGuard.h` | Native Slate and component-editor event guarding |
| `FocusEditorMode.cpp/.h` | Editor mode and selection/cut/delete safeguards |

## Persistence responsibilities

<!-- Media target: IMAGE | source-map-data-ownership | docs/assets/media/source-map-data-ownership.png -->
<figure class="doc-media" data-kind="image" data-media-id="source-map-data-ownership">
  <img src="../../assets/media-placeholder.png" alt="Diagram of Focus user settings, Actor package metadata and native Actor properties" loading="lazy">
  <figcaption>Focus stores preferences separately from Actor state, while native quick properties remain part of the Actor and Component data they modify.</figcaption>
</figure>


`States` and `Temporary` are in-memory maps keyed by weak Actor references. `Focus.SharedState.V1` metadata is stored on the Actor's owning package. Native Hidden in Game and movement lock use native Actor properties. `OnPackagePreSave` and `OnPackagePostSave` are temporary serialization overlays; neither creates an independent user-authored Focus asset.

## Rendering and inputs

`FFocusColumn` and compact Slate widgets render the Outliner mini-controls. A registered `IInputProcessor` intercepts supported native interactions, while `FFocusViewExtension` filters Solo primitives per editor view.

## Integration and verification

This source map is organized by implementation ownership, not a compatibility promise for other plugins. Direct scripted changes or unusual editor modes may bypass the guarded path. Review Unreal version changes against the dependent editor APIs when maintaining Focus.

---

## Browse relevant source

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusState.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusState.h)

For usage and workflows, [see Design](DESIGN.md).

**QA status:** Source-reviewed; no claim of exhaustive live-editor integration testing.
