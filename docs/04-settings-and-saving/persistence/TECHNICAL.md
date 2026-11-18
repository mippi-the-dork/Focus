# Session-Only and Persistent Edits — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

What persists, what is temporary, and save behavior.

## Verified behavior

Shared state is serialized as Focus.SharedState.V1 metadata in each Actor's package, including External Actors. Pre-save and post-save hooks protect native temporary values.

## Exact storage behavior

The key `Focus.SharedState.V1` uses a strict five-character state payload in Actor-package metadata: Selection, Transform, Edit, Solo, Editor-hidden. Temporary baselines are kept in memory, while Hidden in Game and native movement lock are Actor properties. During a package save, Focus temporarily substitutes prior persistent values for session-only native edits, then restores the displayed temporary values post-save. This is relevant to One File Per Actor, Save All and autosaves.

**Limitation:** Interrupted saves, other plugins, dirty packages and native actor edits still need project-specific QA before claiming all checkouts are avoided.
## Native behavior and original reference

### Visual and Interaction persistence

Open **Edit > Editor Preferences > Plugins > Focus** and expand **Visual & Interaction Persistence** to choose whether changes made through Focus's Visual and Interaction micro-widgets are saved with the affected Actor or level. **All five persistence options are enabled by default.** The Visual and Interaction parent checkboxes switch all child options in their group; each child can also be configured independently.

| Category | Independently configurable controls |
|---|---|
| Visual | Hide in Editor, Hidden in Game, Solo |
| Interaction | Selection Lock, Protection Lock (Transform Lock and Edit Lock share one setting) |

- **Persistence On:** Focus changes use their normal saved behavior. Save the affected Actor or level package to keep those changes across reloads and editor sessions.
- **Persistence Off:** Changes made through the corresponding **Focus micro-widget** are session-only. The live editor value changes, but Focus retains the previous persistent value for later saves and reloads. Session-only Hidden in Game clicks are designed not to dirty an otherwise clean Actor package or request checkout on their own.
- **Off to On:** The current value stays as it is. Any outstanding temporary change becomes eligible for the next ordinary save. Switching persistence on does not automatically save the Actor or level.
- **Native editing:** Changes made through Unreal Engine's own Details panel, native Outliner commands, or other native editing paths retain Unreal's normal save and checkout behavior. Focus's persistence setting does not make those tools session-only. A later native edit to the same property supersedes that property's temporary Focus change.

The other eight categories, **Simulation, Physics Events, Lighting, World, Network, Navigation, Mobility, and LOD**, continue to edit native properties persistently. Their behavior and colors are not governed by Visual and Interaction persistence settings.

Use **Restore Temporary Changes** in the Focus header menu to discard outstanding session-only Focus edits across currently loaded Actors and return those controls to their pre-temporary values. This operation does not change persistence preferences, commit changes, or undo unrelated native edits. It applies to tracked temporary changes, not just the currently selected Outliner row.

Session-only editing of native properties such as Hidden in Game and movement locking requires additional save-workflow testing. See **Limitations** before relying on this behavior for production levels.

### Native quick property editing

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

---

### Session-Only Native Properties

Session-only **Hidden in Game** and native movement locking use package pre-save and post-save handling to avoid writing temporary values as persistent property changes. This needs in-editor validation across Save All, autosaves, External Actors / World Partition, Undo/Redo, and interrupted or failed saves. On a disposable level, confirm that Focus-only temporary edits do not request checkout on an otherwise clean Actor, that unrelated native edits save correctly, and that reloading restores the last persistent values.

An Actor already marked dirty for unrelated reasons can still appear in Unreal's save or checkout workflows. Focus's persistence options do not suppress native Details panel or Outliner edits, and they do not prevent other plugins or tools from dirtying a package.

---

# Troubleshooting

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
