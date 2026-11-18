# Persistent and Session-Only Editing — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## The most important choice in Focus

You can use some Focus controls for **temporary editor organization** or for **changes you intend to save**. That distinction matters when working in shared levels, with Perforce, or with One File Per Actor.

Open **Editor Preferences → Plugins → Focus → Visual & Interaction Persistence**. There are two convenient parent groups, **Visual (all)** and **Interaction (all)**, plus separate settings for the underlying controls.

| Control | Can be session-only through Focus? |
| --- | --- |
| Hide in Editor | Yes |
| Hidden in Game | Yes |
| Solo | Yes |
| Selection Lock | Yes |
| Transform Lock / Edit Lock | Yes, as one Protection persistence choice |
| Simulation, Physics Events, Lighting, World, Network, Navigation, Mobility, LOD | No; these edit native properties normally |

All Visual and Interaction persistence options default **On** for new configurations.

## Persistent example

You Transform Lock a finished building and leave that control's persistence enabled. Save the affected Actor or level packages. Reopening the level can restore the saved Focus lock state.

## Session-only example

<!-- Media target: GIF | settings-persistence-workflow | docs/assets/media/settings-persistence-workflow.gif -->
<figure class="doc-media" data-kind="gif" data-media-id="settings-persistence-workflow">
  <img src="../../assets/media-placeholder.png" alt="Animation changing Solo persistence and temporarily isolating Actors" loading="lazy">
  <figcaption>With Solo configured as session-only, the editing view can be isolated without replacing the previously saved Solo state.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record opening Focus preferences, disabling Solo persistence, then Soloing a group of Actors and saving a disposable level. Show that the temporary editor view is active, then reopen/reload to demonstrate the previously saved Solo state returns.</span></figcaption>
</figure>


You temporarily Solo a room or hide distractions while working. Disable persistence for the corresponding Focus control first. Changes through that Focus control remain active in the current editor session without replacing the last saved value. To discard outstanding temporary edits, choose **Restore Temporary Changes** from Focus's header menu.

## Two common misconceptions

**Turning persistence off is not Undo.** It does not instantly reverse a value already changed. **Turning it back on** keeps the current value and makes it eligible for the next save.

**The setting does not change Unreal's native Details panel.** If you edit Hidden in Game or movement lock through Unreal's own Details/Outliner controls, those native changes follow normal save behavior.

## Perforce and OFPA caution

Temporary Focus changes are designed not to commit their native property values to package saves. That does not guarantee an Actor never requests checkout: an unrelated property change can dirty its package. Validate the save workflow on a disposable level in your project, especially Save All, autosave, external Actor packages, interruption and Undo.

---

For exact native settings, serialization and recovery, [see Technical](TECHNICAL.md).
