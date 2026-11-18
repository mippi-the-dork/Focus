# Focus Editor Architecture — Technical

**Focus v2.1.0 · C++ source reference**

## Module startup

`FFocusModule::StartupModule` registers editor callbacks and the Actor Browser column. `TryActivate` waits for `GEditor`, the actual Level Editor instance and the registered Focus editor mode. Before activating its non-widget mode, it invokes the normal default mode when no other editor mode is active. This avoids suppressing Unreal's standard transform gizmo during startup.

## Integration layers

<!-- Media target: IMAGE | architecture-editor-layers | docs/assets/media/architecture-editor-layers.png -->
<figure class="doc-media" data-kind="image" data-media-id="architecture-editor-layers">
  <img src="../../assets/media-placeholder.png" alt="Architecture diagram linking the Focus module, Outliner column, editor guards and view extension" loading="lazy">
  <figcaption>The editor module coordinates presentation, input protection, state persistence and view filtering through Unreal’s native interfaces.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Create a technical architecture diagram with FFocusModule, FFocusColumn, FocusQuick, UFocusEditorMode, transform/command/interaction guards, FFocusViewExtension, UFocusSettings, and the Actor/package state. Use labeled arrows for registration, reading, writes, and viewport filtering.</span></figcaption>
</figure>


| Component | Role |
| --- | --- |
| `FFocusColumn` | Outliner row widgets, category menu, mixed and eligible state |
| `Focus` namespace | Saved and session Focus state, persistence, Undo/Redo |
| `FocusQuick` | Native property definitions, validation, reads and writes |
| `UFocusEditorMode` | Viewport selection filtering and selected cut/delete protection |
| `FFocusTransformGuard` | Transaction-scoped standard editor transform rollback |
| `FFocusCommandGuard` | Gating of supported native editor commands |
| `FFocusInteractionGuard` | Native Slate Outliner/component editor interactions |
| `FFocusViewExtension` | Solo via editor view's `ShowOnlyPrimitives` |
| `UFocusSettings` | Editor per-project preference values |

## Editor event subscriptions

The module listens for map opens, new Actors, added levels and object replacement to load saved state once objects are ready, including streamed Actor replacements after Blueprint recompilation. Details hooks preserve previous read-only and editing-enabled delegates. A core ticker reconciles transient editor state and discovers relevant views without scanning every widget every render frame.

## Unloading

`StopMode` unregisters callbacks/guards, resolves any in-progress temporary save overlays through `OnPackagePostSave`, removes the Focus default editor mode and destroys Focus mode when the level editor is still valid. Input processors and captured native editing hooks require safe teardown rather than leaked delegates.

## Trust boundary

This is entirely editor-side integration. Native editor systems and plugins not intercepted by the guards can still modify Actors. No engine-source patch or packaged runtime module is required.

---

## Browse relevant source

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusState.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusState.h)

For usage and workflows, [see Design](DESIGN.md).

**QA status:** Source-reviewed; no claim of exhaustive live-editor integration testing.
