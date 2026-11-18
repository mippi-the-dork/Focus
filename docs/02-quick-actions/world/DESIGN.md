# World — Design

**Focus v2.1.0 · Quick Actions · User guide**

## Available controls

<!-- Media target: IMAGE | world-controls | docs/assets/media/world-controls.png -->
<figure class="doc-media" data-kind="image" data-media-id="world-controls">
  <img src="../../assets/media-placeholder.png" alt="World Quick Actions showing main-world, editor-only, spatial-loading and HLOD flags" loading="lazy">
  <figcaption>The World category places four Actor-level configuration choices directly in the Outliner.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture the World category’s four controls on a loaded Actor row, with its tooltip identifying one of the Actor-level properties. Include enough of the Outliner to demonstrate where these choices sit in the Focus column.</span></figcaption>
</figure>


| Quick Action | What it changes | Important consequence |
| --- | --- | --- |
| **In Main World Only** | Actor main-world-only configuration | Only has meaning where Unreal respects this world-context flag |
| **Is Editor Only Actor** | Native editor-only Actor status | The Actor may be excluded from cooked content; check game references |
| **Is Spatially Loaded** | World Partition spatial-loading eligibility | Unrelated to whether the Actor happens to be loaded right now |
| **Include Actor in HLOD** | Actor HLOD-generation participation | Does not build HLODs on its own |

These are **Actor-level** options. They do not edit every component separately.

## Workflow: examine World Partition participation

<!-- Media target: IMAGE | world-partition | docs/assets/media/world-partition.png -->
<figure class="doc-media" data-kind="image" data-media-id="world-partition">
  <img src="../../assets/media-placeholder.png" alt="World Partition example showing an Actor’s spatial-loading setting" loading="lazy">
  <figcaption>Spatial loading is an Actor configuration decision whose result depends on the surrounding World Partition setup.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Show a World Partition-enabled level with a loaded Actor selected, its Focus Is Spatially Loaded control visible, and corresponding World Partition/Details context. Use labels to distinguish the stored setting from the Actor’s currently loaded state.</span></figcaption>
</figure>


1. Enable the **World** category in the Focus header menu.
2. Choose a loaded Actor in a World Partition level.
3. Read the **Is Spatially Loaded** tooltip and its current setting before toggling it.
4. If you intentionally change the value, save the relevant Actor package and inspect the level's data layers and streaming setup.
5. Verify the intended loaded/unloaded behavior in the project's World Partition workflow. Undo if the result is undesirable.

## Workflow: include an Actor in HLOD

Select a loaded Actor eligible for the native HLOD flag, toggle **Include Actor in HLOD**, then inspect the world's HLOD configuration and build/generation process. The property is an input to HLOD, not proof of generation.

## Risk and diagnosis

**Editor Only** can affect cooked gameplay content. Before enabling it, confirm nothing at runtime relies on that Actor. **Spatial loading** and **Main World Only** have no guaranteed visible effect in every level type. A missing or dark control means the native Actor property is not instance-editable for that target.

## Target scope, state and saving

These Quick Actions operate on **currently loaded, eligible Actors**, including compatible attached descendants, nested folders, groups and selected Outliner rows. Hover for the state and number of eligible targets. **Mixed** means values differ across those targets; **Not Applicable** means no editable target exists.

Native-property changes respect Edit Lock, support Unreal transactions/Undo and follow normal Actor/Component save behavior. They are not part of Focus's separate Visual/Interaction session-only persistence system. See [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For reflected Actor fields and target eligibility, [read Technical](TECHNICAL.md).
