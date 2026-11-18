# Your First Five Minutes — Technical

> **Focus v2.1.0** · Unreal Engine 5.8.x · Win64 · Editor Only

Enable categories and perform everyday Outliner operations.

This reference describes the shipped editor-facing behavior and native property boundaries documented in the Focus repository. It is not an assertion that the implementation has been independently audited line-by-line.

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

---

### Persistent Settings

Visual and Interaction states can be saved with the affected Actor or level packages, or kept session-only when the corresponding Focus persistence option is disabled.

### Editor Only

Focus is an editor utility and does not add runtime systems to packaged builds.

---

![Focus World Outliner controls](Doc/Images/Focus-Controls.png)

---

---

## Related information

- [Focus source and complete README](https://github.com/mippi-the-dork/Focus#readme)
- [Focus issue tracker](https://github.com/mippi-the-dork/Focus/issues)
- [Installation](../01/installation/DESIGN.md)


