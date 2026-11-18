# Physics Events — Design

**Focus v2.1.0 · Quick Actions · User guide**

## Available controls

<!-- Media target: IMAGE | physics-events-controls | docs/assets/media/physics-events-controls.png -->
<figure class="doc-media" data-kind="image" data-media-id="physics-events-controls">
  <img src="../../assets/media-placeholder.png" alt="Physics Events micro-widget showing hit, overlap and wake event controls" loading="lazy">
  <figcaption>The Physics Events category exposes the three native notification settings for eligible primitive components.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture a compatible Primitive Component’s Actor row with the Physics Events category enabled and all three hit, overlap, and wake controls visible. Hover one control so its label and scope can be read.</span></figcaption>
</figure>


| Quick Action | What you configure | Additional Unreal prerequisites |
| --- | --- | --- |
| **Generate Hit Events** | Rigid-body collision hit notifications | Compatible collision and contact behavior |
| **Generate Overlap Events** | Overlap event generation on a primitive | Both objects' collision/overlap settings |
| **Generate Wake Events** | Physics wake/sleep notifications | A body supporting physics simulation |

## Workflow: investigate a missing overlap

<!-- Media target: GIF | physics-events-overlap | docs/assets/media/physics-events-overlap.gif -->
<figure class="doc-media" data-kind="gif" data-media-id="physics-events-overlap">
  <img src="../../assets/media-placeholder.png" alt="Animation showing overlap generation being enabled and tested in Play" loading="lazy">
  <figcaption>Overlap notifications require a compatible collision setup as well as the Generate Overlap Events setting.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record two simple collision primitives: first show the Generate Overlap Events control and collision response setup, then enter Play and move one object through the other with an obvious on-screen Blueprint/debug message confirming the overlap callback.</span></figcaption>
</figure>


1. Enable the **Physics Events** category.
2. Select an Actor with a registered, editable Primitive Component; activate **Generate Overlap Events**.
3. Inspect **both** participants' collision responses in Details. An overlap event needs a compatible interaction, not merely an enabled checkbox.
4. In PIE, cause the objects to overlap and observe the Blueprint/C++ event handler.
5. If no event arrives, check collision profile, component registration, binding and overlap setup before changing unrelated Focus flags.

## Workflow: hit versus overlap

**Generate Hit Events** concerns contact/hit notifications. **Generate Overlap Events** concerns overlap notifications. They are not interchangeable, and enabling one does not create event logic for the other.

## Why a segment may be unavailable

These actions target compatible, registered **Primitive Components**, not all Actor types. An Actor without a suitable editable component shows **Not Applicable**, not Off. An Edit-Locked Actor is excluded from native quick-property writes.

## Scope, state and saving

A row can represent one Actor or a loaded folder, attachment hierarchy, group or selection. Focus resolves eligible targets across that scope, even if Outliner children are collapsed or hidden by a search filter. **Mixed** means applicable targets disagree; **Not Applicable** means no editable matching property or control target exists. New Actors and unloaded World Partition Actors do not inherit previous bulk actions.

For the six Visual/Interaction controls, review **Editor Preferences → Plugins → Focus** for session-only persistence. The other eight Quick Actions categories change native Unreal properties using ordinary save and Undo/Redo behavior. See the [Quick Actions overview](../README.md) and [Saving guide](../../03-settings-and-saving/README.md).

For native notification fields and setters, [read Technical](TECHNICAL.md).
