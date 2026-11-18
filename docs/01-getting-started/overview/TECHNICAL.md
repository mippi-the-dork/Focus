# What Focus Does — Technical

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Plugin descriptor and startup

[Focus.uplugin](https://github.com/mippi-the-dork/Focus/blob/main/Focus.uplugin) declares the `Focus` module as `Editor`, loaded at `PostEngineInit`, with a `Win64` platform allowlist. `CanContainContent` is false.

## Integration surfaces

`FFocusModule` registers a column with `FSceneOutlinerModule::OnCreateActorBrowserColumns()`, initializes non-widget `UFocusEditorMode`, installs an input guard, registers Details view hooks and creates a view extension for Solo. Nothing about these editor interfaces implies a packaged-game dependency.

## Data ownership

The six Focus-specific Actor controls are exposed through `Focus::Get` and `Focus::Set` in `FocusState.h`; other quick property groups modify native Actor or Component properties. Saved Focus flags use Actor-package metadata. User-facing color and persistence preferences use `UFocusSettings`.

## Scope

The column is registered for the Actor Browser, not every Scene Outliner picker. `Focus::IsEditorActor` checks that the Actor belongs to an editor world. Custom editors and scripts may operate outside Focus guards.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)

For control workflows and examples, [read the Design page](DESIGN.md).

**Validation:** Implementation described from Focus 2.1.0 source. Project-specific live editor QA is separate from documentation review.
