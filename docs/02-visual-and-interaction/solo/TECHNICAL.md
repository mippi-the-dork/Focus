# Solo and Viewport Isolation — Technical

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Per-view implementation

`FFocusViewExtension::SetupView` checks `Focus::HasSolo(World)` and applies only in `EWorldType::Editor`. It gathers primitive scene IDs from Soloed Actors and puts them into `View.ShowOnlyPrimitives`. If another extension already restricts `ShowOnlyPrimitives`, Focus intersects the sets rather than replacing prior restrictions.

## Data and lifecycle

The Solo bit is tracked in `Focus::FFlags` and is encoded in `Focus.SharedState.V1` metadata when persistent. The view extension does not rewrite Hidden in Editor or Hidden in Game on every other Actor.

## Performance and edge cases

The extension checks whether any Solo state is active before building an allowed primitive set. The filter is not an engine-wide rule for lights, volumes or every contribution to rendering. Solo only alters applicable editor views, not packaged gameplay. Clearing every Solo bit returns the view to standard scene visibility, subject to other editor settings.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)

For control workflows and examples, [read the Design page](DESIGN.md).

**Validation:** Implementation described from Focus 2.1.0 source. Project-specific live editor QA is separate from documentation review.
