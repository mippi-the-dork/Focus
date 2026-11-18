# Your First Five Minutes — Technical

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Column assembly

`FFocusModule` adds `FFocusColumn::GetID()` to Actor Browser column options. The column builds its visible groups from `FocusQuick::CategoryVisible` and calculates width using `FocusQuick::ColumnWidth`. Category changes invalidate visible column widgets.

## Row state and toggling

<!-- Media target: IMAGE | first-use-row-state | docs/assets/media/first-use-row-state.png -->
<figure class="doc-media" data-kind="image" data-media-id="first-use-row-state">
  <img src="../../assets/media-placeholder.png" alt="Diagram showing Focus evaluating a selected Outliner row and its eligible Actors" loading="lazy">
  <figcaption>A row’s quick-action state is derived from the eligible loaded Actors in its resolved scope.</figcaption>
</figure>


`FFocusColumn::RefreshRow` aggregates eligible loaded Actors. Mixed controls depend on counts of enabled vs total eligible Actors, not simply the clicked parent row. Clicking a mixed or disabled control enables the entire affected scope; clicking fully enabled disables it.

## Editor guards

The native `UFocusEditorMode` selection path allows intentional Outliner selection despite Selection Lock. The component/property guards are installed separately, not through an all-purpose read-only mode.

## Diagnostic order

If nothing appears, verify module/column registration; if just one control is absent, verify category visibility; if one segment is unavailable, verify target eligibility and native property editability. These represent different failures.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)

For control workflows and examples, [read the Design page](DESIGN.md).

**Validation:** Implementation described from Focus 2.1.0 source. Project-specific live editor QA is separate from documentation review.
