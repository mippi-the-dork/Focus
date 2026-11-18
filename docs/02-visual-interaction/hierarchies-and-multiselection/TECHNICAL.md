# Folders, Hierarchies and Multi-selection — Technical

> **Focus v2.1.0** · Unreal Engine 5.8.x · Win64 · Editor Only

Loaded Actor scope, mixed states and bulk application.

This reference describes the shipped editor-facing behavior and native property boundaries documented in the Focus repository. It is not an assertion that the implementation has been independently audited line-by-line.

### Hierarchy Support

Apply Focus controls to:

- Individual Actors
    
- Attached Actor hierarchies
    
- Folders
    
- Nested folders
    
- Groups
    
- Nested groups
    

### Multi-Selection Support

Apply Focus controls across several selected Actors or folders at once.

### Mixed States

Focus indicates when a hierarchy contains a mixture of enabled and disabled states.

### Undo / Redo

Focus control changes participate in Unreal Engine's normal Undo and Redo workflow.

---

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

---

## Related information

- [Focus source and complete README](https://github.com/mippi-the-dork/Focus#readme)
- [Focus issue tracker](https://github.com/mippi-the-dork/Focus/issues)
- [Installation](../01/installation/DESIGN.md)


