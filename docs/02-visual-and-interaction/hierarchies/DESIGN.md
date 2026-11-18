# Folders, Groups and Multi-Selection — Design

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

How control scope is resolved.

## Verified behavior

Bulk operations target currently loaded Actors, their attachment and group members; searching/collapsing Outliner rows does not restrict the action scope.

## Usage and workflows

# Working with Folders and Hierarchies

Focus controls are designed to work beyond individual Actors.

Applying a Focus control to a hierarchy can affect everything represented by that hierarchy.

|Target|Affected Scope|
|---|---|
|**Actor**|The Actor and attached descendants|
|**Folder**|Loaded Actors inside the Folder and nested Folders|
|**Group**|The Group, its members, and nested Groups|
|**Multiple Selection**|The combined scope of all selected rows|

Collapsed Outliner rows are still included.

Outliner search filters do not prevent matching Actors from being included in the operation.

This makes it possible to manage large sections of a level without expanding every hierarchy first.

---

# Working with Multiple Actors

If several rows are selected in the World Outliner, activating a Focus control from one of those selected rows applies that action across the combined selection.

This is useful for:

- Soloing several unrelated Actors
    
- Locking a group of finished environment pieces
    
- Hiding several folders
    
- Protecting a set of important Actors
    
- Changing several hierarchy states at once
    

For normal toggle controls:

- Clicking an **off** state enables it.
    
- Clicking a **mixed** state enables it across the complete scope.
    
- Clicking a **fully enabled** state disables it.
    

---

# Mixed States

Folders and selections can contain Actors with different Focus settings.

Focus represents this using a mixed state.

For example:

```text
Environment
├── Wall_A        Transform Locked
├── Wall_B        Transform Locked
└── Door          Unlocked
```

The protection control for the `Environment` Folder will show that its contents do not all share the same state.

This makes it easier to understand the actual state of a hierarchy without expanding and inspecting every Actor individually.

---

# Example Workflow

## Expected limits

Focus is an Editor-only workflow tool. Folder and selected-row changes affect eligible loaded Actors and are not automatically inherited by Actors added later. Supported native protections do not prevent arbitrary external tools or scripts.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
