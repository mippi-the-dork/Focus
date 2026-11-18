# Selection, Transform and Edit Locks — Design

> **Focus v2.1.0** · Unreal Engine 5.8.x · Win64 · Editor Only

Three separate protection behaviors and lock states.

This page describes how to use the controls in the World Outliner, with the original feature description and workflow guidance retained.

## Selection Lock

Selection Lock prevents an Actor from being selected through the viewport.

When enabled:

- Clicking the Actor in the viewport will not select it.
    
- The Actor can still be selected from the World Outliner.
    
- Its properties can still be inspected.
    
- Selection Lock does not prevent editing by itself.
    

The **Selection Lock persistence** setting determines whether changes made through Focus are saved or session-only. With persistence off, active locks use the session-only blue palette; a Mixed state uses orange. Selection Lock does not prevent native property editing by itself.

This is useful for Actors such as:

- Large background meshes
    
- Blocking geometry
    
- Lighting Actors
    
- Reference geometry
    
- Level infrastructure
    
- Anything that frequently sits in front of what you actually want to select
    

---

## Transform Lock

Transform Lock protects an Actor's:

- Location
    
- Rotation
    
- Scale
    

Other properties remain editable.

Focus uses native Actor movement locking and a movement/transaction guard rather than replacing Unreal's viewport transform gizmo. The same Focus controls are available while using Unreal Engine's legacy or Interactive Tools Framework (ITF) gizmo system.

This is useful when an Actor is in the correct place but still needs to remain otherwise editable.

**Transform Lock and Edit Lock share one Protection Lock persistence option.** When it is off, changing the protection level through Focus remains session-only. Using Unreal's native Actor movement-lock command directly follows Unreal's normal saved behavior.

---

## Edit Lock

Edit Lock provides stronger protection.

It includes Transform Lock and protects supported Actor and Component editing operations.

This includes common workflows such as:

- Editing values in the Details panel
    
- Changing transforms
    
- Changing mobility
    
- Changing materials
    
- Renaming
    
- Deleting
    
- Cutting
    
- Changing attachments
    
- Adding Components
    
- Removing Components
    

The Actor can still be selected from the World Outliner and inspected.

You can continue to:

- Browse properties
    
- Search the Details panel
    
- Expand and collapse categories
    
- Inspect values
    
- Change Focus states
    

The purpose is to prevent accidental editing while still allowing developers to understand the Actor.

The **Protection Lock** persistence setting applies to both Transform Lock and Edit Lock. An active Edit Lock uses the **Alert** color: red when persistent or pink when session-only. Turning persistence back on keeps the current protection level, which can then be saved normally.

---

## Protection Lock States

The protection control cycles through:

```text
Unlocked → Transform Lock → Edit Lock → Unlocked
```

The visual state of the padlock indicates the current protection level. Both locked levels use the same **Protection Lock (Transform / Edit)** persistence checkbox, and the tooltip reports whether Focus changes are persistent or session-only.

![Transform Lock and Edit Lock in Focus](Doc/Images/Focus-Locking.gif)

---

## Related information

- [Focus source and complete README](https://github.com/mippi-the-dork/Focus#readme)
- [Focus issue tracker](https://github.com/mippi-the-dork/Focus/issues)
- [Installation](../01/installation/DESIGN.md)


