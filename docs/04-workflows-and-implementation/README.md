# 04. Workflows and Implementation

Focus is meant to shorten repetitive Unreal Editor tasks, but the right workflow depends on whether you're protecting something, changing a native property, or diagnosing a particular editor interaction.

## Everyday scenarios

### Protect finished work

**Selection Lock** helps keep approved meshes from intercepting clicks. **Transform Lock** protects their placement while allowing unrelated property editing. **Edit Lock** covers a broader set of supported native editor operations. None is a permissions system for scripts or custom tools.

### Isolate a room without changing the whole level

Use **Solo** on the loaded Actors or folder you want to inspect. It filters rendered primitives in editor views, rather than applying Hide in Editor to everything else. Disable Solo when returning to the whole scene.

### Change a property across many Actors

<!-- Media target: IMAGE | workflows-hierarchy | docs/assets/media/workflows-hierarchy.png -->
<figure class="doc-media" data-kind="image" data-media-id="workflows-hierarchy">
  <img src="../assets/media-placeholder.png" alt="Outliner hierarchy showing a loaded folder, attachments and mixed selected Actors" loading="lazy">
  <figcaption>A Quick Action can apply across eligible loaded members of a folder, group, attachment hierarchy or selection.</figcaption>
</figure>


Select a folder, group, Actor hierarchy or multi-selection. Enable the corresponding Quick Action category, read the control's eligibility tooltip and apply. A mixed state tells you eligible targets disagree. Unloaded World Partition Actors aren't included in these operations.

### Investigate why an editor action isn't blocked

Focus has distinct paths for viewport selection, transforms, Details editing, component changes, and native commands. Their coverage is intentionally bounded; diagnosing a bypass begins with **how the change was initiated**, not just the lock's displayed icon.

## How Focus integrates with Unreal

Its editor module registers a World Outliner column, native interaction and command guards, a non-widget editor mode, a view extension for Solo and package-save callbacks. The underlying data is regular Actor/Component state plus Focus-specific Actor metadata. No engine-source modifications or runtime Focus Actors are required.

## Workflows and implementation topics

| Topic | What it explains | Design | Technical |
| --- | --- | --- | --- |
| **Practical Workflows** | Loaded-level authoring examples and batch operations | [Design](workflows/DESIGN.md) | [Technical](workflows/TECHNICAL.md) |
| **Focus Editor Architecture** | Why Focus has several editor integration paths | [Design](architecture/DESIGN.md) | [Technical](architecture/TECHNICAL.md) |
| **Source Code and Data Ownership** | Where behavior and stored state live | [Design](source-map/DESIGN.md) | [Technical](source-map/TECHNICAL.md) |

For the full **28 native property** reference, use the ten category pages in [Quick Actions](../02-quick-actions/README.md), not a separate duplicate catalog.
