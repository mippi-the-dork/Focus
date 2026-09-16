# Focus

Focus adds visibility, Solo and locking controls to Unreal Engine's World Outliner. Work with individual actors, multiple selections, folders and actor hierarchies without entering a separate editor mode.

**Target:** Unreal Engine 5.8.2 on Windows (64-bit).

Focus is an Editor-only plugin. It requires no engine modifications, Motion Design, Actor Locker, Surface or other third-party plugins.

## Installation

### Packaged plugin

1. Close Unreal Editor.
2. Extract the `Focus` folder into your project's `Plugins` folder. Create `Plugins` if it does not exist.
3. Confirm the descriptor is at `YourProject/Plugins/Focus/Focus.uplugin`.
4. Open the project and enable **Focus** under **Edit > Plugins**.
5. Restart if prompted. The standard World Outliner will have a **Focus** column.

Use a package built for your engine version and platform. If Unreal reports incompatible binaries, use a matching package or build from source.

### Source installation

1. Place the `Focus` folder in your C++ project's `Plugins` folder and close Unreal Editor.
2. Generate Visual Studio project files for the initial installation.
3. Open the solution and build your project's Editor target using **Development Editor / Win64**.
4. Open the project, enable **Focus** under **Edit > Plugins**, and restart if prompted.

A source build needs a compatible Unreal C++ toolchain. A Blueprint-only project can use a compatible packaged plugin. For source compilation, establish a C++ project build workflow first.

For subsequent edits to existing `.cpp` and `.h` files, close Unreal and build again. Project-file regeneration is not needed for those edits. Restart the editor after plugin changes rather than hot-unloading the module.

## Outliner controls

The Focus column contains two vertically stacked dots, a Solo icon, a selection icon and a padlock. It replaces the standard visibility eye column in the regular actor Outliner.

| Control | Appearance | Effect |
| --- | --- | --- |
| Top dot: Hide in Editor | Grey when off, yellow when on | Hides the affected actors in editor viewports. Does not change Hidden in Game. |
| Bottom dot: Hidden in Game | Grey when off, red when on | Changes the actual actor Hidden in Game property. Actors with Edit Lock are skipped. |
| Solo | Grey outline when off, white solid when on, white mixed icon for mixed states | Isolates soloed actors in editor viewports. Multiple actors can be soloed together. |
| Selection Lock | Grey cursor when unlocked, white cursor with a lock when locked | Prevents viewport selection. Click the actor's Outliner row to select it. |
| Protection padlock | Grey open, white closed or red closed | Cycles through Unlocked, Transform Lock and Edit Lock. |

Hover a control for its behavior and the affected row's state. A mixed indicator means only some actors represented by that row have the setting enabled. Grey visibility dots indicate these actor-level settings are off; other engine visibility systems can still hide an actor.

### Selection Lock

Selection Lock is independent of the protection padlock. An actor can be selection-locked and transform-locked or edit-locked at the same time.

Selection Lock alone does not prevent editing. Select the actor in the Outliner to access its Details panel and transform controls.

### Transform Lock and Edit Lock

Click the padlock to cycle:

**Unlocked > Transform Lock > Edit Lock > Unlocked**

- **Transform Lock:** Protects position, rotation and scale through standard editor transform controls. Other properties remain editable.
- **Edit Lock:** Includes Transform Lock and makes actor/component values read-only in the Level Editor Details panel, including Mobility and Materials. It also guards supported actor and component editing commands, such as deleting, cutting, renaming and changing attachments, plus supported component add/remove operations.
- **Unlocked:** Clears both Transform Lock and Edit Lock. Selection Lock is unchanged.

An Edit Lock still allows inspection: search, expand or collapse categories, and add or remove favorites. Surface's filters and Select Actor/Select Component navigation remain usable. Undo and Redo remain available while locks are on or off.

Hide in Editor, Solo and the lock controls remain usable under Edit Lock. Hidden in Game is protected because it changes an actor property.

Blueprint-driven transform changes are allowed. Focus is a guard against accidental manual edits, not a restriction on gameplay, class changes or every script and custom tool.

### Solo

Solo affects editor-world viewports for the current world. It does not change Hidden in Game. Clear every Solo state to restore the normal view.

Actors explicitly hidden with the top dot stay hidden while soloed. Solo filters rendered primitive components; it does not guarantee that lighting, volumes or other non-primitive scene contributions are isolated. It does not automatically include supporting lights or volumes.

## Multiple selections, folders and hierarchies

A control on a selected row applies to the combined scope of all selected rows. A control on an unselected row applies to that row's scope.

| Row | Actors affected |
| --- | --- |
| Actor | The actor and its attached descendants. |
| Folder | Loaded actors in the folder and nested folders, plus their attached descendants and resolved group members. |
| Native group actor | The group actor, its members and nested groups, plus attached descendants. |

Overlapping scopes affect each actor once. Collapsed rows and Outliner search filters do not exclude actors from an action. These rules apply to visibility, Hidden in Game, Solo and both lock controls.

For dots, Solo and Selection Lock, clicking an off or mixed state enables the setting across the affected actors. Clicking a fully enabled state disables it. Hidden in Game skips edit-locked actors and reports the skipped count.

For the padlock, clicking a mixed state sets the affected actors to **Transform Lock**, including actors that were previously edit-locked. Further clicks follow the normal cycle.

Folder and hierarchy controls apply settings to the actors currently in their scope. They do not create an ongoing inheritance rule. Actors added, attached or moved into a folder later do not automatically inherit its settings. Unloaded World Partition actors are not changed by a bulk action; load them before applying it.

## Saving settings

Use **Save All** after changing Focus settings.

Selection Lock, Transform Lock, Edit Lock, Solo and Hide in Editor are stored as Focus metadata in the affected actor's package. For externally packaged actors, this means the external actor file. Otherwise, it is normally the containing level package. Hidden in Game uses Unreal's existing saved actor property.

Saved settings survive level reloads and editor restarts, including settings you turn off. Focus control changes support Undo and Redo. Save again after Undo or Redo if you want that resulting state retained.

Solo and Hide in Editor are shared saved settings too. Clear any temporary isolation or hiding before saving if you do not want it restored later or shared with teammates.

### Sharing through source control

1. Enable Focus for everyone who needs its selection and edit protections.
2. Change the settings and use **Save All**, completing your normal checkout/save workflow when required.
3. Submit the affected level and external actor files, along with the plugin and project configuration if they are not already shared.
4. Teammates sync those files and reopen or reload the level to read the saved settings.

Focus does not submit files or manage checkouts. Its locks reduce accidental edits; they are not source-control locks or user permissions. Another user can intentionally unlock an actor.

Restart and level-reload persistence have been tested during development. Transfer between separate source-control checkouts remains unverified.

## Scope and limitations

- Focus targets placed actors in Editor worlds. PIE, simulation worlds, runtime gameplay and Blueprint class editors are outside its editing scope. The native Hidden in Game property still affects gameplay normally.
- Selection Lock's Outliner exception currently uses the pointer's location. Click the Outliner row to select a locked actor; keyboard-only selection with the pointer elsewhere may be restricted. It does not implement click-through selection of actors behind a locked actor.
- Edit Lock guards supported Level Editor controls. Custom Details widgets, specialized editor modes, third-party tools and scripts may bypass those controls. It is not a universal engine-level prohibition on modifying an actor.
- With mixed actor selections, shared Details editing can become read-only when any selected actor has Edit Lock. Protected structural commands can reject the whole operation rather than partially modify the selection.
- Focus can coexist with Surface and does not require it. Other tools that replace the same editor delegates, selection rules or viewport filters need compatibility testing.
- Focus must be enabled for its selection and edit protections. Native saved properties, including Hidden in Game and the movement-lock flag, may remain set when Focus is disabled. Clear unwanted settings and save before disabling the plugin.

## Roadmap

- Automatic inheritance for actors added to folders or hierarchies after settings are applied.
- Performance investigation and optimization for large scenes and interactive editing.

## Attribution

The selection-gating approach references Actor Locker by Gradess Games. Keep the included `THIRD-PARTY-NOTICES.txt` with distributions.
