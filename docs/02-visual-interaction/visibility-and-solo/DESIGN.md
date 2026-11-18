# Visibility and Solo — Design

> **Focus v2.1.0** · Unreal Engine 5.8.x · Win64 · Editor Only

Hide in Editor, Hidden in Game and Solo isolation.

This page describes how to use the controls in the World Outliner, with the original feature description and workflow guidance retained.

## Hide in Editor

The upper visibility control determines whether an Actor is hidden in editor viewports.

When enabled:

- The Actor is hidden while editing.
    
- Hidden in Game is not changed.
    
- The Actor can still remain part of the level.
    

Use this when something needs to be temporarily removed from view while working.

When **Hide in Editor persistence is on**, the hidden state can be saved with the Actor. When it is **off**, changes made by the Focus dot remain session-only. A fully hidden state uses **white** or **blue** depending on persistence; a Mixed row uses **yellow** or **orange**. Gray means visible. Hidden in Game is unaffected.

---

## Hidden in Game

The lower visibility control modifies Unreal's existing **Hidden in Game** property.

When enabled, the Actor is hidden during gameplay according to Unreal's normal runtime behavior.

This is separate from Hide in Editor.

With **Hidden in Game persistence off**, clicking Focus's dot changes the property for the current session without intentionally marking an otherwise clean Actor package dirty or prompting checkout. The active dot is **red** when persistent or **pink** when session-only. Mixed uses **yellow** or **orange**; gray means off.

If you change Hidden in Game through the **Details panel** or a native Outliner command instead, Unreal's ordinary edit and save behavior still applies. Focus's persistence preference does not override native editing.

That separation allows an Actor to be:

- Visible in the editor but hidden in game
    
- Hidden in the editor but visible in game
    
- Visible in both
    
- Hidden in both
    

---

## Solo

Solo isolates part of the level inside the editor viewport.

You can Solo:

- An Actor
    
- An Actor hierarchy
    
- A Folder
    
- Multiple Actors
    
- Multiple Folders
    
- Groups
    

Multiple Solo targets can be active simultaneously.

When Solo is cleared, normal editor visibility returns.

Solo does not change Hidden in Game.

Use the **Solo persistence** checkbox to choose between saving Focus's Solo choices and keeping them temporary for the editor session. A session-only active Solo icon uses **blue** rather than white; Mixed uses **orange** rather than yellow. **Restore Temporary Changes** discards tracked temporary Solo changes along with other temporary Focus edits.

![Using Solo with Focus](Doc/Images/Focus-Solo.gif)

### Solo and Existing Visibility

Actors already hidden using **Hide in Editor** remain hidden while Solo is active.

Solo should be treated as a temporary viewport isolation tool rather than a replacement for normal level visibility.

Lights, volumes, and other scene systems may continue influencing the viewport even when their Actor is outside the current Solo set.

---

---

## Related information

- [Focus source and complete README](https://github.com/mippi-the-dork/Focus#readme)
- [Focus issue tracker](https://github.com/mippi-the-dork/Focus/issues)
- [Installation](../01/installation/DESIGN.md)


