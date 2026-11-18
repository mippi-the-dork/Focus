# Troubleshooting — Design

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

When controls, saves or protections do not behave as expected.

## Verified behavior

Check column/category visibility, supported registered components, Edit Lock, loaded World Partition Actors, and which system performed the edit.

## Usage and workflows

# Troubleshooting

## Focus Does Not Appear in the World Outliner

Check:

**Edit > Plugins**

Search for:

```text
Focus
```

Confirm that the plugin is enabled.

Restart Unreal Editor if the plugin was just enabled.

---

## A Focus Setting Did Not Persist

First check **Edit > Editor Preferences > Plugins > Focus > Visual & Interaction Persistence**. A disabled checkbox makes changes initiated through that Focus micro-widget session-only. To keep the current value, turn its persistence checkbox back on, then save the affected Actor or level package using **File > Save All** or your normal save workflow.

If the setting was already persistent, check that the relevant Actor or level package was saved, including External Actor packages where applicable. The eight native quick-property categories always use Unreal's normal saved-property behavior.

If you used **Restore Temporary Changes**, the outstanding session-only changes were discarded intentionally. Changes made through Unreal's native Details panel or native Outliner controls are not governed by Focus's session-only preferences.

---

## A Folder Did Not Affect an Actor

Check whether:

- The Actor is currently loaded.
    
- The Actor was added after the Focus state was applied.
    
- World Partition currently has the Actor unloaded.
    

Folder operations are bulk actions rather than persistent inheritance rules.

---

## Hidden in Game Cannot Be Changed

Check whether the Actor currently has **Edit Lock** enabled. Hidden in Game modifies an Actor property, so Edit Lock protects it from being changed through Focus.

If Hidden in Game changes in the current editor session but resets after reopening the level, check its **Visual & Interaction Persistence** checkbox. Session-only Focus edits are deliberately not saved. Edits made through the native Details panel retain Unreal's ordinary behavior.

If a session-only Focus click appears to request checkout, first check whether the Actor package was already dirty from a different edit. Test on a freshly saved Actor and verify that the change came from the Focus micro-widget rather than a native editor action. See **Session-Only Native Properties** under Limitations.

---

## A Selection-Locked Actor Can Still Be Selected

Selection Lock prevents **viewport selection**.

The Actor is intentionally still selectable from the World Outliner.

---

## A Script Changed a Locked Actor

Focus protects against supported manual editor interactions.

Scripts, custom tools, and direct engine operations can still modify Actors.

---

# Reporting Bugs

If you encounter a problem, please open an issue:

[https://github.com/mippi-the-dork/Focus/issues](https://github.com/mippi-the-dork/Focus/issues)

When reporting a bug, include:

- Focus version
    
- Unreal Engine version
    
- Windows version
    
- Whether Focus was installed from Fab, a GitHub Release, or source
    
- The Focus control involved
    
- Whether the target was an Actor, Folder, Group, or multi-selection
    
- Whether World Partition was involved
    
- Steps to reproduce the problem
    
- Screenshots or video when relevant
    
- Any relevant Unreal Editor log output
    

Clear reproduction steps make issues much easier to diagnose.

---

# Feature Requests

## Expected limits

Focus is an Editor-only workflow tool. Folder and selected-row changes affect eligible loaded Actors and are not automatically inherited by Actors added later. Supported native protections do not prevent arbitrary external tools or scripts.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
