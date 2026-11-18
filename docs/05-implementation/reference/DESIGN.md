# Full Quick-Property Reference — Design

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

28 native quick-property definitions.

## Verified behavior

FocusQuickProperties.cpp defines 28 properties over eight categories, plus Visual and Interaction controls.

## Usage and workflows

### Native quick property editing

Optional controls operate on Unreal Engine's **native Actor and Component properties**, not copies stored by Focus. Each miniwidget segment shows the current aggregate state of eligible loaded Actors and components. A Mixed state is displayed only when applicable properties disagree, and unavailable or non-instance-editable properties appear dark gray without affecting adjacent controls.

The complete control reference appears below under **World Outliner Controls**, organized by category. Each control uses its corresponding native Unreal property where an eligible, instance-editable target exists.

**Hover any miniwidget segment** for an explanation of what the property controls, its current state, eligible targets, usage instructions, and relevant restrictions. The category checkbox tooltips in the header are shorter overviews that list the available controls; the miniwidget tooltips explain individual properties in more detail.

Controls are available only when Unreal exposes the backing property as editable on the relevant placed Actor or Component. Focus uses native property setters or editor change notifications and transactions where supported. It does not silently change prerequisites to make an inapplicable property editable. Actors protected by Focus Edit Lock are skipped, and folder or selection operations apply only to currently loaded, eligible targets. A single Actor can contain several eligible components, so inspect the tooltip before making a bulk change.

---

### World Outliner Controls

Focus adds its controls directly to the standard World Outliner.

No additional editor window is required.

### Hide in Editor

Hide Actors from editor viewports without affecting their Hidden in Game state. Focus can save this visibility choice or keep Focus-initiated changes session-only, according to the Hide in Editor persistence setting.

### Hidden in Game

Control Unreal's existing Hidden in Game property directly from the World Outliner. Session-only Focus edits can be enabled without changing the persistence of native Details or Outliner actions.

### Solo

Isolate Actors, folders, or hierarchies while working in the editor, with optional session-only Solo changes.

### Selection Lock

Prevent accidental viewport selection while keeping Actors selectable in the World Outliner. Focus-initiated Selection Lock changes can be saved or session-only.

### Transform Lock

Protect Actor location, rotation, and scale while leaving other properties editable. Transform Lock and Edit Lock share the Protection Lock persistence setting.

### Edit Lock

Protect supported Actor and Component editing while keeping their properties available for inspection. Edit Lock shares the Protection Lock persistence setting with Transform Lock.

### Simulation

Quickly manage physics behavior for eligible Actors and primitive components using six independently clickable segments in a radial ring:

- **Simulate Physics:** Enable or disable rigid-body physics simulation on eligible primitive components. Physics simulation also depends on compatible collision and mobility settings.
- **Enable Gravity:** Allow gravity to affect physics-simulated components. Enabling gravity does not automatically enable physics simulation.
- **Ignore Radial Impulse:** Prevent instantaneous radial impulses from affecting supported primitive components.
- **Ignore Radial Force:** Prevent ongoing radial forces from affecting supported primitive components, independently of radial impulses.
- **Apply Impulse on Damage:** Allow eligible primitive components to receive an impulse from supported damage events; this does not itself configure damage or start simulation.
- **Async Physics Tick Enabled:** Enable an Actor's asynchronous physics tick where supported; this is separate from its normal game-thread tick.

### Physics Events

Control physics event notifications with three stacked horizontal pills, from top to bottom:

- **Generate Hit Events:** Enable rigid-body collision hit notifications where the component's collision setup supports the relevant contacts.
- **Generate Overlap Events:** Enable overlap event generation, subject to the other object's collision responses and overlap settings.
- **Generate Wake Events:** Enable physics wake and sleep notifications for eligible primitive components taking part in physics simulation.

### Lighting

Adjust lighting interactions using three upper channel bars and a lower shadow bar:

- **Lighting Channel 0:** Include eligible lights and primitives in Channel 0 interactions.
- **Lighting Channel 1:** Include eligible lights and primitives in Channel 1 interactions.
- **Lighting Channel 2:** Include eligible lights and primitives in Channel 2 interactions.
- **Cast Shadows:** Enable or disable shadow casting on supported light or primitive components, subject to relevant rendering and material settings.

A light and a receiving primitive must have a compatible lighting channel for that channel to affect their interaction. Lighting channels do not override other lighting restrictions.

### World

Access four world-related Actor properties through a two-by-two grid:

- **In Main World Only:** Restrict a supported Actor to the main world where Unreal respects this setting.
- **Is Editor Only Actor:** Mark the Actor as editor-only so it is excluded from cooked gameplay content where supported. Check dependencies before enabling this on referenced Actors.
- **Is Spatially Loaded:** Control whether the Actor participates in World Partition spatial loading where applicable.
- **Include Actor in HLOD:** Control participation in automatic Hierarchical Level of Detail generation in supported worlds.

### Network

Manage three network-related properties through a triangular miniwidget:

- **Net Load on Client:** Control whether a placed Actor is loaded on clients as part of networked level loading. This does not enable Actor replication.
- **Replicate Physics to Autonomous Proxy:** Control physics-state replication to autonomous proxies on eligible primitive components. A suitable networking and physics setup is required.
- **Static Mesh Replicate Movement:** Control movement replication for supported Static Mesh Actors. It does not enable replication for unrelated Actor or component types.

### Navigation

Access four navigation settings through the four circular points of the navigation miniwidget:

- **Can Ever Affect Navigation (top):** Allow an eligible Actor component to contribute to navigation data.
- **Dynamic Obstacle (right):** Mark an eligible shape component as a dynamic navigation obstacle where supported.
- **Fill Collision Underneath for Navmesh (bottom):** Fill navigation data underneath eligible primitive collision according to the navmesh configuration.
- **Rasterize as Filled Convex Volume (left):** Use filled convex-volume rasterization for supported primitive collision when generating navigation data.

Changes to navigation settings may require appropriate collision geometry or navigation updates before their effects appear.

### Mobility

Set an eligible scene component's mobility with a single vertical pill divided into three sections:

- **Static (top):** Keep the component in the Static mobility mode.
- **Stationary (middle):** Choose Stationary mobility when supported by that component and its usage.
- **Movable (bottom):** Choose Movable mobility to allow dynamic movement where appropriate.

Click a section to assign that mobility to eligible components. The choices are mutually exclusive for each component. When the targets have different mobility values, the miniwidget shows a Mixed outline with neutral interior bands. Certain component types and lighting setups impose additional restrictions.

### LOD

Control mesh Level of Detail and distance culling from a forced-LOD number positioned above two horizontal bars:

- **Forced LOD Model (top number):** `0` uses automatic LOD. Positive values follow Unreal's one-based forced-LOD convention: `1` selects mesh LOD 0, `2` selects mesh LOD 1, and so on. The number supports **left-click to increment**, **Alt + left-click to decrement**, **mouse wheel** to adjust, and **double-click** to enter a value directly. The input range is 0 to 255, but each mesh may support fewer LODs. Invalid forced values use the Alert color (red by default) and are explained in the tooltip.
- **Never Distance Cull (first bar):** Prevent supported primitive components from being removed by distance-culling systems that honor this setting. This does not disable all visibility or streaming mechanisms.
- **Allow Cull Distance Volume (second bar):** Allow eligible primitive components to be influenced by Cull Distance Volumes in the level.

The two culling controls toggle independently. **Right-click** still opens the native Outliner context menu. Focus does not silently clamp an existing forced-LOD value to an available mesh LOD.

### Hierarchy Support

## Expected limits

Focus is an Editor-only workflow tool. Folder and selected-row changes affect eligible loaded Actors and are not automatically inherited by Actors added later. Supported native protections do not prevent arbitrary external tools or scripts.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
