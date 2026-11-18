# Lighting and World — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Lighting: four separate toggles

Lighting Channel **0**, **1**, and **2** decide which enabled channels an eligible light or primitive belongs to. For a light to affect a receiving primitive through lighting channels, they generally need an overlapping enabled channel. **Cast Shadows** controls shadow casting for eligible components; shadow visibility still depends on light, material, and renderer settings.

A useful test: set a light and selected props to Channel 1, leave unrelated props on Channel 0, then inspect the lighting. Channel flags are not a replacement for normal light visibility or mobility rules.

## World: four Actor-level options

| Control | Purpose | When to use it carefully |
| --- | --- | --- |
| In Main World Only | Restricts supported Actors to their main-world context | World duplication and editor previews |
| Is Editor Only Actor | Marks an Actor as editor-only where supported | Do not leave gameplay references dependent on stripped Actors |
| Is Spatially Loaded | Controls World Partition spatial-loading eligibility | Relevant only in supported World Partition contexts |
| Include Actor in HLOD | Requests HLOD generation participation | Requires a suitable HLOD-enabled world |

## Workflow

Enable **Lighting** or **World** under the Focus header. Hover the miniwidget to identify each segment and read the current eligible-target count. Use the Lighting controls on Actors containing lights or primitives; World flags apply at the Actor level. Check the result in the appropriate editor rendering and world-loading context.

## Understand 'enabled'

An enabled native flag is not proof that a light contributes to the final render or an Actor will be included in HLOD. These are inputs to Unreal's systems, not a preview or diagnostics engine.

---

For editable C++ property targets, transaction details and native constraints, [see Technical](TECHNICAL.md).
