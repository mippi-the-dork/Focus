# Visibility and Solo — Technical

> **Focus v2.1.0** · Unreal Engine 5.8.x · Win64 · Editor Only

Hide in Editor, Hidden in Game and Solo isolation.

This reference describes the shipped editor-facing behavior and native property boundaries documented in the Focus repository. It is not an assertion that the implementation has been independently audited line-by-line.

### Hide in Editor

Hide Actors from editor viewports without affecting their Hidden in Game state. Focus can save this visibility choice or keep Focus-initiated changes session-only, according to the Hide in Editor persistence setting.

### Hidden in Game

Control Unreal's existing Hidden in Game property directly from the World Outliner. Session-only Focus edits can be enabled without changing the persistence of native Details or Outliner actions.

### Solo

Isolate Actors, folders, or hierarchies while working in the editor, with optional session-only Solo changes.

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

### Solo

Solo primarily isolates rendered primitive components.

Some scene contributions, such as lighting or volumes, may continue affecting the viewport even when their Actor is outside the Solo set.

---

## Related information

- [Focus source and complete README](https://github.com/mippi-the-dork/Focus#readme)
- [Focus issue tracker](https://github.com/mippi-the-dork/Focus/issues)
- [Installation](../01/installation/DESIGN.md)


