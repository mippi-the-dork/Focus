# Solo and Viewport Isolation — Design

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Isolate without hiding the rest permanently

**Solo** makes the selected Actor or collection the focus of the editor viewport. Unlike selecting **Hide in Editor** on every other Actor, it does not require you to change all those Actors' visibility settings.

## Typical workflow

1. Solo a folder containing the area you are decorating.
2. Continue selecting and inspecting its loaded Actors.
3. Add another Solo scope if you need to compare several sections.
4. Turn off the Solo states when finished to restore the normal view.

You can Solo an Actor, attachment hierarchy, folder, group or multiple selection. Mixed state tells you that only part of the scope is Soloed.

## What Solo will and will not hide

Focus primarily isolates **rendered primitive components** in editor viewports. Lighting, volumes and other scene contributions can still influence what you see. An Actor explicitly hidden in the editor does not become visible simply because it is Soloed.

## Persistence

Solo can be persistent or session-only depending on Focus settings. It does not turn on **Hidden in Game**. When a persistent Solo state is saved, the related Actor package is part of the save workflow.

---

## Technical detail

[Read the Technical page](TECHNICAL.md) for engine integration, persistence and implementation boundaries.
