# Hide in Editor and Hidden in Game — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Different visibility states and their effects.

## Verified behavior

Editor Hide does not change Hidden in Game. The former is editor viewport visibility, the latter a native Actor property.

## Native behavior and original reference

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

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
