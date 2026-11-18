# Category Visibility and Column Layout — Technical

**Focus v2.1.0 · Source-reviewed implementation**

## Category visibility storage

`FocusQuick::CategoryVisible` caches per-category settings and reads `Focus.CategoryVisibility` from `GEditorPerProjectIni`. Defaults are true only for `ECategory::Visual` and `ECategory::Interaction`. `SetCategoryVisible` updates the cache and writes/flashes the configuration.

## Outliner presentation

`FFocusColumn::CategoryMenu` builds checkboxes from `ECategory::Count` and invalidates active Focus columns after changes. The reset action writes defaults for all ten categories, not just a transient visibility override.

`FocusQuick::ColumnWidth` counts only visible groups. `GapBefore` computes spacing using the preceding **visible** category, so hidden categories do not leave blank gutters. The Visual and Interaction category widths are deliberately compact; native property groups use their authored miniwidget geometry.

## Separation of concerns

Category visibility is not an Actor flag and does not invoke `Focus::Set`. Thus turning off a category cannot silently clear a saved lock, turn off simulation, or undo other quick-property edits. The source deliberately separates 'which controls are shown' from 'what state the Actors have'.

## Scope

The Focus column is registered on Actor Browser creation through the Scene Outliner module. A category setting does not guarantee a given row has a compatible editable target; unavailable state is evaluated separately.

---

## Relevant source files

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [FocusSettings.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.cpp)

For editor workflows and examples, [see Design](DESIGN.md).

**QA status:** Source verification completed; exhaustive real-editor package-save and source-control testing not claimed.
