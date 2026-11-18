# Your First Five Minutes — Design

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Try Focus safely

<!-- Media target: GIF | first-use-controls | docs/assets/media/first-use-controls.gif -->
<figure class="doc-media" data-kind="gif" data-media-id="first-use-controls">
  <img src="../../assets/media-placeholder.png" alt="Animation showing Focus controls being enabled and used for the first time" loading="lazy">
  <figcaption>The initial workflow reveals the Focus column, adjusts its visible categories and applies an action to a test Actor.</figcaption>
</figure>


Use a disposable test level or a few simple Actors first.

1. Open **Window → Outliner** and locate the **Focus** column.
2. Open the Focus column-header menu. Keep **Visual** and **Interaction** visible initially; the other eight categories are hidden by default.
3. Click **Hide in Editor** on a test Actor. Confirm it disappears in the editor viewport; click again to show it.
4. Enable **Selection Lock**. Try selecting the Actor in the viewport, then select it intentionally through the Outliner.
5. Switch between **Transform Lock** and **Edit Lock**. Compare what you can adjust in Details.
6. Solo the Actor or its folder. Clear Solo to return to the normal view.
7. Select multiple Outliner rows and apply one control. Inspect how the combined selection changes.

## Read the controls

Enabled, disabled, mixed and unavailable are different states. Hover a control for its exact meaning and number of eligible loaded Actors. If a property icon is too dark or cannot be clicked, the Actor might not expose an editable compatible property.

## Before using it in production

Open **Editor Preferences → Plugins → Focus** and review Visual and Interaction persistence. Test Save, reload and Undo on a disposable level. Do not assume that session-only Focus changes alter the saved Actor state.

---

## Technical detail

[Read the Technical page](TECHNICAL.md) for engine integration, persistence and implementation boundaries.
