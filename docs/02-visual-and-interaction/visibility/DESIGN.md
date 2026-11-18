# Hide in Editor and Hidden in Game — Design

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Two independent visibility settings

**Hide in Editor** controls whether an Actor appears in editor viewports. It is useful for reducing visual noise without changing the Actor's gameplay visibility choice.

**Hidden in Game** changes the Actor's native Hidden in Game setting. Use it when you intentionally want that runtime property configured. Hiding an Actor in the editor does **not** automatically set Hidden in Game, and the reverse is also true.

## Use them

Select an Actor or folder in the Outliner and toggle the appropriate Focus visual segment. Hover first when unsure which is which. A mixed folder means some eligible Actors are hidden while others are not. Toggle mixed to apply one consistent state to the loaded scope.

## Example

You can hide decorative geometry in the editor while leaving it set to appear in game. Conversely, you can see a debug Actor while editing but mark it Hidden in Game.

## Saving matters

These two Focus controls have separate persistence checkboxes. Turning persistence off makes **Focus-initiated** changes temporary for the editor session. Native Details panel changes still save normally; use **Restore Temporary Changes** to discard eligible temporary Focus changes. Edit-Locked Actors are not changed by the Focus Hidden in Game toggle.

---

## Technical detail

[Read the Technical page](TECHNICAL.md) for engine integration, persistence and implementation boundaries.
