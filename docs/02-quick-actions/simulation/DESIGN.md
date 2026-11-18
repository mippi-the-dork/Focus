# Simulation — Design

**Focus v2.1.0 · Quick Actions · User guide**

## Available controls

<!-- Media target: IMAGE | simulation-controls | docs/assets/media/simulation-controls.png -->
<figure class="doc-media" data-kind="image" data-media-id="simulation-controls">
  <img src="../../assets/media-placeholder.png" alt="Simulation Quick Actions micro-widget showing its six controls" loading="lazy">
  <figcaption>The Simulation category exposes physics, gravity, impulse and asynchronous ticking settings in one compact group.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture the Simulation category enabled in the Focus column, preferably with an eligible physics mesh selected. Frame all six Simulation micro-controls together and hover one to show its actual tooltip and eligible-target information.</span></figcaption>
</figure>


| Quick Action | Meaning | Needed for a visible result |
| --- | --- | --- |
| **Simulate Physics** | Enable rigid-body physics on eligible primitives | Compatible primitive/collision/mobility |
| **Enable Gravity** | Allow world gravity on a simulated body | Simulate Physics must also be appropriate |
| **Ignore Radial Impulse** | Ignore a one-time radial impulse | Something must actually apply an impulse |
| **Ignore Radial Force** | Ignore continuous radial force | A radial force source/effect |
| **Apply Impulse on Damage** | Allow a supported damage impulse | Damage and compatible physics setup |
| **Async Physics Tick Enabled** | Enable Actor async physics ticking where supported | A compatible Actor/physics tick implementation |

These controls edit Unreal's native values, not a custom simulation system.

## Workflow: make a prop respond to gravity

<!-- Media target: GIF | simulation-prop-physics | docs/assets/media/simulation-prop-physics.gif -->
<figure class="doc-media" data-kind="gif" data-media-id="simulation-prop-physics">
  <img src="../../assets/media-placeholder.png" alt="Animation of a mesh being configured for physics and gravity using Focus" loading="lazy">
  <figcaption>An eligible movable mesh can be configured to simulate physics and respond to world gravity through separate controls.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record a movable physics-enabled mesh in a simple viewport: show Simulate Physics and Enable Gravity toggled through Focus, then Play/Simulate so the mesh visibly falls onto a floor. End by returning to the editor with the controls visible.</span></figcaption>
</figure>


1. Enable the **Simulation** category through the Focus column-header menu.
2. Use a Static Mesh Actor with a compatible Primitive Component, collision and mobility.
3. Enable **Simulate Physics** and **Enable Gravity** for the eligible component(s).
4. Test under simulation/Play. If the prop remains stationary, inspect collision, body setup and mobility in Unreal's Details panel.
5. Undo the Focus change or restore the original values if the prop should remain static.

## Targets and unavailable controls

Most Simulation actions require **eligible registered Primitive Components**. **Async Physics Tick Enabled** is Actor-level, so it can be applicable even when no primitive body exists. A row with an Actor but no compatible editable component may show some controls as **Not Applicable**.

## Common questions

**Gravity is enabled, but nothing falls.** Gravity does not enable physics simulation by itself.

**Why can I change Ignore Radial Force but still receive an impulse?** Force and impulse are separate properties.

**Why does an enabled icon not prove the simulation works?** The icon reflects the native setting; actual runtime body creation and simulation depend on Unreal configuration.

## Scope, state and saving

A row can represent one Actor or a loaded folder, attachment hierarchy, group or selection. Focus resolves eligible targets across that scope, even if Outliner children are collapsed or hidden by a search filter. **Mixed** means applicable targets disagree; **Not Applicable** means no editable matching property or control target exists. New Actors and unloaded World Partition Actors do not inherit previous bulk actions.

For the six Visual/Interaction controls, review **Editor Preferences → Plugins → Focus** for session-only persistence. The other eight Quick Actions categories change native Unreal properties using ordinary save and Undo/Redo behavior. See the [Quick Actions overview](../README.md) and [Saving guide](../../03-settings-and-saving/README.md).

For the precise BodyInstance properties and native setters, [read Technical](TECHNICAL.md).
