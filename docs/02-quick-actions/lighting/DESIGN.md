# Lighting — Design

**Focus v2.1.0 · Quick Actions · User guide**

## Available controls

<!-- Media target: IMAGE | lighting-controls | docs/assets/media/lighting-controls.png -->
<figure class="doc-media" data-kind="image" data-media-id="lighting-controls">
  <img src="../../assets/media-placeholder.png" alt="Lighting Quick Actions category displaying channels and shadow settings" loading="lazy">
  <figcaption>Lighting Channel 0, 1 and 2 can be adjusted independently of the Cast Shadows setting.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture a World Outliner containing an eligible light and mesh with the Lighting category enabled. Show the three lighting-channel segments and Cast Shadows together, preferably with differing channel states that make the independent choices obvious.</span></figcaption>
</figure>


| Quick Action | Effect |
| --- | --- |
| **Lighting Channel 0** | Enable/disable channel 0 for eligible lights or primitives |
| **Lighting Channel 1** | Enable/disable channel 1 |
| **Lighting Channel 2** | Enable/disable channel 2 |
| **Cast Shadows** | Control the native shadow-casting choice on eligible light/primitive components |

Lighting channels and Cast Shadows are independent. Changing a channel preserves the other channel settings.

## Workflow: light only selected props

<!-- Media target: IMAGE | lighting-channel-example | docs/assets/media/lighting-channel-example.png -->
<figure class="doc-media" data-kind="image" data-media-id="lighting-channel-example">
  <img src="../../assets/media-placeholder.png" alt="Comparison showing a light and selected meshes sharing a lighting channel" loading="lazy">
  <figcaption>Matching channel selections allow compatible lights and primitives to interact without changing the other channel flags.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Create two side-by-side viewport captures with the same light and props: one with matching channel assignments so selected props receive the light, and one with mismatched assignments showing the different result. Add readable callouts for the light and mesh channel settings.</span></figcaption>
</figure>


1. Enable **Lighting** in the Focus header menu.
2. Select a compatible light Actor and enable **Channel 1** on its light component.
3. Select the intended receiving mesh Actor(s), enabling the corresponding **Channel 1** on eligible primitives.
4. Arrange other lights/objects so their channels don't unintentionally overlap.
5. Verify the final lighting under the project's render settings and mobility constraints.

## Common misconceptions

**Why is my light still affecting other objects?** Check channel combinations on both the light and receiving primitives, not just the selected light.

**Why are shadows missing even though Cast Shadows is on?** The final shadow result also depends on light, renderer, materials and other settings.

**Why can't I toggle Lighting on this Actor?** The category requires eligible editable lighting/primitive components. A generic Actor with none can show **Not Applicable**.

## Scope, state and saving

A row can represent one Actor or a loaded folder, attachment hierarchy, group or selection. Focus resolves eligible targets across that scope, even if Outliner children are collapsed or hidden by a search filter. **Mixed** means applicable targets disagree; **Not Applicable** means no editable matching property or control target exists. New Actors and unloaded World Partition Actors do not inherit previous bulk actions.

For the six Visual/Interaction controls, review **Editor Preferences → Plugins → Focus** for session-only persistence. The other eight Quick Actions categories change native Unreal properties using ordinary save and Undo/Redo behavior. See the [Quick Actions overview](../README.md) and [Saving guide](../../03-settings-and-saving/README.md).

For native LightingChannels and shadow setter behavior, [read Technical](TECHNICAL.md).
