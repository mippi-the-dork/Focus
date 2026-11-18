# What Focus Does — Design

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## The problem it solves

<!-- Media target: IMAGE | overview-outliner | docs/assets/media/overview-outliner.png -->
<figure class="doc-media" data-kind="image" data-media-id="overview-outliner">
  <img src="../../assets/media-placeholder.png" alt="Focus controls displayed in the Unreal Engine World Outliner" loading="lazy">
  <figcaption>Focus adds compact controls alongside Actors, keeping visibility and protection decisions within the Outliner.</figcaption>
</figure>


Large Unreal levels become slow to work in when routine questions require opening a Details panel, finding a component, and navigating several property categories. Focus brings frequently changed Actor and Component controls into one compact World Outliner column.

## What you can accomplish

- **See what matters:** Hide in Editor or Solo parts of the level without deleting or moving content.
- **Protect finished work:** Prevent accidental viewport picking, transformations, or supported editing operations.
- **Change native properties:** Use the optional physics, lighting, world, networking, navigation, mobility, and LOD groups.
- **Work on collections:** Apply a change to attached Actor hierarchies, folders, groups or several selected rows.

## A typical use

Imagine dressing a scene with finished architecture and temporary gameplay props. First Selection Lock the background so it does not intercept picking. Transform Lock the finished structures. Solo the work area for visual clarity. Later turn Solo off and restore the full environment.

## Important distinctions

A lock is a workflow safeguard, **not** a project permissions system. Focus operates in the Unreal Editor, not in a packaged game. The settings on a folder are a bulk operation on loaded Actors, not a policy that newly added Actors inherit.

---

## Technical detail

[Read the Technical page](TECHNICAL.md) for engine integration, persistence and implementation boundaries.
