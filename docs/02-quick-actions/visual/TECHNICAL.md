# Visual — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Storage boundaries

Editor-hidden and Solo have saved metadata flags when persistence is enabled. Hidden in Game uses its native Actor property. Per-control Focus session settings determine whether Focus-authored changes are committed or only retained for the current editor session.

## Native values

`Focus::Get(EditorHidden)` reads `AActor::IsTemporarilyHiddenInEditor(false)`. `Focus::Set(EditorHidden)` uses the base-Actor temporary editor-hidden path; the state can be represented in the Actor's `Focus.SharedState.V1` metadata.

`Focus::Get(GameHidden)` reads `AActor::IsHidden()`. `Focus::Set(GameHidden)` calls `SetActorHiddenInGame`; the persistent path uses `Modify()` and native Actor save semantics, whereas the session-only path uses a temporary baseline/undo command without a normal persistent edit.


## Persistence and save overlays

`SessionEditorHidden` and `SessionGameHidden` are different temporary-mask bits. For session-only values, `OnPackagePreSave` substitutes the baseline for serialization and `OnPackagePostSave` restores the editor-visible temporary value. A native edit to the same property takes priority over the old Focus baseline.


## Constraints

`Focus::Set(GameHidden)` checks Edit Lock and skips a protected Actor. The plugin does not change the actual editor/game visibility relationship defined by Unreal Engine. Verify World Partition saves and unusual Actor types in the target project.

---

## Per-view implementation

<!-- Media target: IMAGE | visual-solo-primitive-filter | docs/assets/media/visual-solo-primitive-filter.png -->
<figure class="doc-media" data-kind="image" data-media-id="visual-solo-primitive-filter">
  <img src="../../assets/media-placeholder.png" alt="Diagram of Solo filtering visible primitives in an Unreal Editor viewport" loading="lazy">
  <figcaption>Solo narrows the editor view to selected primitive scene IDs without rewriting the hidden state of unrelated Actors.</figcaption>
</figure>


`FFocusViewExtension::SetupView` checks `Focus::HasSolo(World)` and applies only in `EWorldType::Editor`. It gathers primitive scene IDs from Soloed Actors and puts them into `View.ShowOnlyPrimitives`. If another extension already restricts `ShowOnlyPrimitives`, Focus intersects the sets rather than replacing prior restrictions.


## Data and lifecycle

The Solo bit is tracked in `Focus::FFlags` and is encoded in `Focus.SharedState.V1` metadata when persistent. The view extension does not rewrite Hidden in Editor or Hidden in Game on every other Actor.


## Performance and edge cases

The extension checks whether any Solo state is active before building an allowed primitive set. The filter is not an engine-wide rule for lights, volumes or every contribution to rendering. Solo only alters applicable editor views, not packaged gameplay. Clearing every Solo bit returns the view to standard scene visibility, subject to other editor settings.

---

## Verification boundaries

`Focus::Set` changes Actor-specific state, not the 28 native quick-property definitions. Solo is applied by `FFocusViewExtension::SetupView` in editor worlds. Session-only Hidden in Game and editor-hidden values use temporary baselines and package-save overlays; see [Persistence Technical](../../03-settings-and-saving/persistence/TECHNICAL.md).

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — property definition, eligibility and write implementation
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — state, Slate widget, transaction and Editor integrations

For the user workflow, [read Design](DESIGN.md).

**Verification:** Code reviewed; live Unreal Editor behavior must be confirmed using the [Verification Matrix](../../VERIFICATION_MATRIX.md).
