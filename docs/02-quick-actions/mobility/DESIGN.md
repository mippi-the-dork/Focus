# Mobility — Design

**Focus v2.1.0 · Quick Actions · User guide**

## Choose a mobility mode

<!-- Media target: IMAGE | mobility-options | docs/assets/media/mobility-options.png -->
<figure class="doc-media" data-kind="image" data-media-id="mobility-options">
  <img src="../../assets/media-placeholder.png" alt="Mobility micro-widget showing its Static, Stationary and Movable choices" loading="lazy">
  <figcaption>The three vertically arranged Mobility choices map directly to Unreal’s native Scene Component mobility values.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture the full three-segment Mobility control in the Outliner with Static (S), Stationary (T), and Movable (M) readable. Show at least one selected option, and include a tooltip identifying the native mobility value.</span></figcaption>
</figure>


The compact Mobility widget contains **three vertically arranged choices**, top to bottom:

| Choice | Native mode | Typical use |
| --- | --- | --- |
| **Top — S** | Static | Nonmoving content |
| **Middle — T** | Stationary | Engine-supported limited movement/use cases |
| **Bottom — M** | Movable | Objects that need to move |

Hover the segment to confirm the label, then click the option you want. This is a **three-choice selector**, not a button you repeatedly cycle.

## Workflow: prepare a mesh to simulate physics

<!-- Media target: GIF | mobility-bulk | docs/assets/media/mobility-bulk.gif -->
<figure class="doc-media" data-kind="gif" data-media-id="mobility-bulk">
  <img src="../../assets/media-placeholder.png" alt="Animation applying Movable to eligible Scene Components with Focus" loading="lazy">
  <figcaption>Changing Mobility across a selected scope prepares eligible components for workflows that require movement.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record selecting several loaded Scene Component Actors in the Outliner, clicking the Movable segment, then inspecting one or two affected components in Details. Include an Undo step that visibly restores their previous mobility values.</span></figcaption>
</figure>


1. Enable **Mobility** in Focus's Outliner header menu.
2. On an Actor with a suitable Scene Component, choose **Movable** using the bottom segment.
3. Enable **Simulation** separately, and configure collision and Simulate Physics on appropriate Primitive Components.
4. Test in PIE/simulation, and confirm which component is moving. Changing Mobility alone will not activate physics simulation.
5. If your Actor has multiple eligible Scene Components, verify the resulting state of each component rather than assuming only the root was changed.

## Mixed and unavailable states

If a folder or Actor contains several eligible components with different mobility, the combined result can be **Mixed**. Clicking one option applies it to the eligible target set. A non-component Actor without an editable Scene Component can show **Not Applicable**.

## Constraints

Unreal may restrict changing component mobility based on component setup and other systems. A configured value is not a guarantee that lighting or physics will work as expected; inspect those features separately.

## Target scope, state and saving

These Quick Actions operate on **currently loaded, eligible Actors**, including compatible attached descendants, nested folders, groups and selected Outliner rows. Hover for the state and number of eligible targets. **Mixed** means values differ across those targets; **Not Applicable** means no editable target exists.

Native-property changes respect Edit Lock, support Unreal transactions/Undo and follow normal Actor/Component save behavior. They are not part of Focus's separate Visual/Interaction session-only persistence system. See [Quick Actions overview](../README.md) and [Settings and Saving](../../03-settings-and-saving/README.md).

For `USceneComponent::SetMobility`, native target discovery and update notifications, [read Technical](TECHNICAL.md).
