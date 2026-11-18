# Category Visibility — Design

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Configure which column groups are displayed.

## Verified behavior

Visual and Interaction are visible by default; eight other groups are optional. Visibility is stored in per-project Editor user config.

## Category state storage

Visibility flags are read and written under `Focus.CategoryVisibility` in `GEditorPerProjectIni`; the column width is calculated from enabled category widgets, and hidden groups do not reserve gaps. Header reset restores Visual + Interaction only, without changing level state.
## Usage and workflows

### Configurable categories

Focus adds a column to Unreal Engine's World Outliner. Open the **three-dots (`...`) menu at the far right of the Focus column header** to choose which categories appear. Each category has a checkbox, and hovering it shows the category's purpose and a complete list of controls it contains. Changes take effect immediately and persist in your editor-per-project preferences without altering the level.

**Visual and Interaction are visible by default.** The other eight categories, **Simulation, Physics Events, Lighting, World, Network, Navigation, Mobility, and LOD**, are hidden by default. The menu's **Reset to Visual + Interaction** button restores the default set. **Restore Temporary Changes** separately discards outstanding session-only Visual and Interaction edits without changing category visibility or persistence preferences. The category order is fixed.

Each category uses a distinct miniwidget:

| Category | Layout |
|---|---|
| Visual | Visibility controls and Solo icon |
| Interaction | Selection Lock and a three-level protection selector |
| Simulation | Six individually clickable triangles around a central opening, clockwise from the top |
| Physics Events | Three evenly stacked horizontal pills |
| Lighting | Three upright channel bars above one full-width shadow bar |
| World | Four equally sized squares in a two-by-two grid |
| Network | Three angular pieces arranged in a radial triangle |
| Navigation | Four circular controls arranged north, east, south, and west |
| Mobility | Three stacked sections of one vertical pill |
| LOD | A centered forced-LOD number above two culling bars |

Miniwidgets have separate hover and click targets for each option. Their geometry and spacing remain consistent as property states change. They fit Unreal's standard 20-unit Outliner rows and remain vertically centered at larger row heights, including those configured with Stem. The Focus column resizes to the visible categories, without leaving gaps for hidden ones.

Category spacing is intentionally compact around Simulation and Network: 4 units before Simulation, 7 after Simulation, 6 before Network, and 4 after Network. The original Visual-to-Interaction gap remains 3 units; all other existing spacing rules are preserved.

### State colors

## Expected limits

Focus is an Editor-only workflow tool. Folder and selected-row changes affect eligible loaded Actors and are not automatically inherited by Actors added later. Supported native protections do not prevent arbitrary external tools or scripts.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
