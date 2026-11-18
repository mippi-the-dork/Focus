# First Use — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

How to enable and use Focus.

## Verified behavior

Category menu lives in the World Outliner column header.

## Native behavior and original reference

# Using Focus

Focus works directly in the **World Outliner**. The standard layout keeps **Visual** and **Interaction** enabled; the eight optional quick-property categories are hidden until you choose to show them.

### Choose the controls you need

1. Open Unreal Engine's **World Outliner** and locate the Focus column.
2. Open the **`...` menu** at the far right of the Focus column header.
3. Hover over a category checkbox to read its purpose and see which individual controls it contains. Check the categories relevant to your work, such as **Simulation**, **Lighting**, **Network**, or **LOD**. The miniwidgets appear immediately in each applicable Outliner row.
4. To return to the initial layout, choose **Reset to Visual + Interaction** in the same menu. Category visibility is saved in your editor-per-project user preferences; it does not change level data.

### Read and change properties

1. Locate an Actor, folder, group, or supported hierarchy in the Outliner. You can also select multiple rows before using a Focus control.
2. Hover over the **individual miniwidget segment** you want to change. Its tooltip explains the native property, current value, eligible target count, and any known restrictions.
3. Check the segment's state. Persistent controls use white for Enabled, yellow for Mixed, red for Alert, gray for Disabled, and dark gray for Not Applicable by default. Visual and Interaction controls configured as session-only instead use blue, orange, and pink for Enabled, Mixed, and Alert. All colors can be customized; Hidden in Game uses Alert red or pink when active.
4. Click an applicable segment to toggle that property on eligible loaded targets. A Mixed toggle is enabled across eligible targets when clicked. Edit-locked Actors are skipped. Changes participate in Unreal's normal Undo/Redo workflow where supported.
5. For **Mobility**, click the Static, Stationary, or Movable part of the vertical pill to choose that value. For **LOD**, click or scroll the upper number to adjust Forced LOD Model, use Alt + click to decrement, or double-click to type a value. Use the two lower bars to toggle culling properties separately.

Not every Actor has every property. For example, a physics setting may apply to an Actor's primitive components, a navigation setting may apply to a shape component, and a network setting may be limited to supported Actor classes. Unavailable controls remain visible but dark, rather than changing unrelated properties. Bulk operations affect currently loaded, eligible targets, not automatically all possible Actors in a world.

### Customize appearance

Open **Edit > Editor Preferences > Plugins > Focus** and expand **Control State Colors** to customize the persistent palette, or **Session-Only State Colors** to customize the alternate blue, orange, pink, and gray palette. The two palettes are independent. Color changes affect the appearance of the controls, not their underlying values.

### Choose persistent or session-only controls

1. In **Edit > Editor Preferences > Plugins > Focus**, open **Visual & Interaction Persistence**.
2. Keep a checkbox enabled to save changes made through its Focus control normally, or disable it for session-only editing. The **Visual (all)** and **Interaction (all)** parent checkboxes change their respective child options together.
3. Use the Visual and Interaction controls as usual. Active session-only states use blue instead of white, Mixed uses orange instead of yellow, and Alert uses pink instead of red. The disabled and unavailable defaults remain gray and dark gray.
4. To **keep a temporary configuration**, turn persistence back on. The current state remains active and becomes eligible for your next ordinary save.
5. To **discard outstanding temporary edits**, open the Focus header **`...` menu** and select **Restore Temporary Changes**. This restores tracked changes on currently loaded Actors without changing your persistence settings.

These preferences apply only to edits initiated by Focus's Visual and Interaction controls. Editing a native property through Unreal's Details panel or a native Outliner action still follows Unreal's normal saving and checkout behavior. A session-only Hidden in Game click should not independently mark a clean Actor for saving; saving an Actor for another reason must not silently commit the temporary Focus value.

### Example workflows

- **Working with physics:** Show Simulation and Physics Events, then use the six simulation segments and three event pills to inspect or change eligible component behavior without repeatedly opening Details panels.
- **Preparing scene lighting:** Show Lighting to inspect channel assignments and shadow casting across selected lights or primitives.
- **Reviewing streaming and replication:** Show World and Network to inspect supported Actor loading, HLOD, client loading, and replication properties.
- **Tuning traversal and visibility:** Show Navigation to inspect navmesh-affecting settings, or LOD to adjust the forced LOD and distance-culling properties on eligible meshes and components.
- **Temporary level inspection:** Turn off persistence for Solo or Hide in Editor, isolate or hide Actors with Focus while investigating a scene, and then use **Restore Temporary Changes** to return to the previous saved arrangement.
- **Keeping a temporary setup:** Temporarily adjust Visual or Interaction states through Focus, then turn their persistence options back on when you decide to keep the current configuration. Save the affected packages normally.

Save Actor or level packages normally when you want **persistent** Actor properties or Focus states to be retained. **Session-only** Focus changes are excluded from that saved configuration unless you re-enable persistence and save. Header category visibility, persistence preferences, and both color palettes are saved separately as editor user settings.

The detailed guides that follow cover the original Visual and Interaction controls first, then all eight optional property categories in the order they appear in the Focus header menu.

---

## Hide in Editor

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
