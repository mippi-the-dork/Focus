# Practical Workflows for Dense Levels — Technical

**Focus v2.1.0 · C++ source reference**

## How a row action resolves scope

<!-- Media target: IMAGE | workflows-scope-resolution | docs/assets/media/workflows-scope-resolution.png -->
<figure class="doc-media" data-kind="image" data-media-id="workflows-scope-resolution">
  <img src="../../assets/media-placeholder.png" alt="Diagram of row scope resolution across folders, attachments, groups and selected Actors" loading="lazy">
  <figcaption>Focus resolves its action targets from loaded Actor relationships rather than only from Outliner rows currently visible on screen.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Make a scope-resolution flowchart: clicked row / selected rows → loaded Actors in folders, groups, and attachment hierarchies → deduplicated eligible targets → resulting On, Off, Mixed, or Not Applicable state. Indicate that collapsed/filtered rows still count.</span></figcaption>
</figure>


`FFocusColumn::ActionActors` and `Gather` construct the relevant loaded Actor set from the activated Actor/folder/group row and the currently selected rows. This uses live Actor relationships, not Outliner search-result child widgets, so collapsed and filtered items still contribute.

## How state is aggregated

`RefreshRow` reads the current Focus flags for each Actor and the eligible values of each quick-property target, accumulating total/active/mixed counts. Outliner row state can be mixed even when the particular clicked Actor is not.

## Applying quick properties

`ApplyQuick` creates `FScopedTransaction`, skips Edit-Locked Actors, gets targets by `FocusQuick::EScope`, and writes native property values through `FocusQuick::Write`. Per-target setters and edit notifications make Undo/Redo and normal editor change propagation possible.

## Applying Focus-specific controls

`Focus::Set` is distinct from `FocusQuick::Write`. Visual/Interaction state uses the Actor's native hidden/movement fields and/or `Focus.SharedState.V1` metadata. `FSharedStateChange` stores explicit before/after snapshots for metadata Undo.

## What the workflow cannot guarantee

A folder edit cannot change an unloaded World Partition Actor, create inherited folder policies, or ensure a generated NavMesh/lighting/network change has already taken effect. The editor's native subsystems own those outcomes.

---

## Browse relevant source

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [FocusState.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusState.h)

For usage and workflows, [see Design](DESIGN.md).

**QA status:** Source-reviewed; no claim of exhaustive live-editor integration testing.
