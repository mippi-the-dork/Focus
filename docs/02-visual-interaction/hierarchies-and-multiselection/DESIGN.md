# Folders, Hierarchies and Multi-selection — Design

> **Focus v2.1.0** · Unreal Engine 5.8.x · Win64 · Editor Only

Loaded Actor scope, mixed states and bulk application.

This page describes how to use the controls in the World Outliner, with the original feature description and workflow guidance retained.

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

Imagine you're working in a dense environment that contains finished architecture, temporary gameplay objects, lights, reference meshes, and the section you're currently editing.

You could:

1. **Selection Lock** large background geometry so it stops intercepting viewport clicks.
    
2. **Transform Lock** finished architecture so it cannot be nudged accidentally.
    
3. **Edit Lock** important Actors that should only be inspected.
    
4. **Solo** the Folder you're actively working on.
    
5. Clear Solo when the work is complete.
    

The level structure remains unchanged while the editor becomes much easier to navigate safely.

---

---

## Related information

- [Focus source and complete README](https://github.com/mippi-the-dork/Focus#readme)
- [Focus issue tracker](https://github.com/mippi-the-dork/Focus/issues)
- [Installation](../01/installation/DESIGN.md)


