# Practical Workflows — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Scene isolation and level organization.

## Verified behavior

Edit Lock and hierarchy scope apply to each eligible loaded Actor. New Actors added later do not inherit previous folder operations.

## Native behavior and original reference

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

---

# Example Workflow

Imagine you're working in a dense environment that contains finished architecture, temporary gameplay objects, lights, reference meshes, and the section you're currently editing.

You could:

1. **Selection Lock** large background geometry so it stops intercepting viewport clicks.
    
2. **Transform Lock** finished architecture so it cannot be nudged accidentally.
    
3. **Edit Lock** important Actors that should only be inspected.
    
4. **Solo** the Folder you're actively working on.
    
5. Clear Solo when the work is complete.
    

The level structure remains unchanged while the editor becomes much easier to navigate safely.

---

# Saving Focus Settings

Focus separates **editor interface preferences**, **persistent Actor settings**, and **session-only Visual and Interaction edits**:

- **Interface preferences:** Category visibility, Visual and Interaction persistence checkboxes, and both state-color palettes are saved as editor-per-project user preferences. They do not modify Actors or levels.
- **Persistent Focus edits:** With a Visual or Interaction persistence checkbox enabled, changes made through its Focus control can be stored with the affected Actor or level package when you save.
- **Session-only Focus edits:** With that checkbox disabled, changes made through the corresponding Focus control remain active in the editor but are not intended to replace the last persistent value on disk. This affects only Focus-initiated Visual and Interaction edits; native Details and Outliner edits retain normal Unreal saving behavior.
- **Other quick-property categories:** Simulation, Physics Events, Lighting, World, Network, Navigation, Mobility, and LOD change native properties with Unreal's regular persistent behavior.

The persistent Visual and Interaction states include **Hide in Editor, Hidden in Game, Solo, Selection Lock, Transform Lock, and Edit Lock**. Hidden in Game and native movement locking use Actor properties; Focus's other Visual and Interaction states use saved Actor-package metadata.

For projects using External Actors / One File Per Actor, that metadata is stored in the corresponding Actor package. Otherwise, it is typically stored with the level package. To keep persistent changes across level reloads, editor sessions, or source-control syncs, save the affected packages using Unreal's normal workflow.

**Switching persistence off does not immediately reset a property.** Switching it back on does not restore an older value either. The current value is kept and becomes eligible for the next normal save. To discard outstanding session-only changes instead, use **Restore Temporary Changes** from the Focus header menu.

> [!WARNING]  
> **Session-only native property editing requires save-workflow validation.**
>
> Hidden in Game and movement locking are native Unreal properties. Focus isolates temporary values during package saves, but Save All, autosaves, World Partition / External Actors, Undo/Redo, and interrupted saves should be tested in your project before relying on the behavior for production levels. A previously dirty Actor may still appear in the save list due to unrelated changes.

---

# Installation

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
