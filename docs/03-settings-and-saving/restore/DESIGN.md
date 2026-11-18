# Restore Temporary Changes, Undo and Reload — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Restore Temporary Changes

<!-- Media target: GIF | settings-restore-temporary | docs/assets/media/settings-restore-temporary.gif -->
<figure class="doc-media" data-kind="gif" data-media-id="settings-restore-temporary">
  <img src="../../assets/media-placeholder.png" alt="Animation restoring temporarily hidden, Soloed and protected Actors" loading="lazy">
  <figcaption>Restore Temporary Changes reverts tracked session-only Focus edits while leaving unrelated persistent changes intact.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record a disposable level where Hide in Editor, Solo, and a lock are changed with persistence off; open the Focus column-header menu and run Restore Temporary Changes. Show the Actors returning to their prior states while an unrelated saved change remains.</span></figcaption>
</figure>


The Focus header menu has **Restore Temporary Changes**. It discards outstanding session-only edits made through Focus's Visual and Interaction controls and restores the previous recorded values. It does **not** reset saved persistent changes, change your persistence preferences, or undo unrelated native Details edits.

Use it when you have temporarily hidden, Soloed, or locked a collection of Actors and want to return to the state from before those temporary edits.

## Example: temporary scene isolation

1. Turn off Solo persistence in Editor Preferences.
2. Solo the area you are currently authoring.
3. Work normally while the other visible primitives are filtered out.
4. Open the Focus header menu and choose **Restore Temporary Changes**.
5. Check that temporary Solo changes have been discarded.

## Undo/Redo

Focus's authored controls support Unreal Undo/Redo. After changing a whole folder, Undo is the quickest way to reverse an accidental bulk operation. Do not confuse Undo (which reverses the last transaction) with Restore Temporary Changes (which clears pending session-only changes across affected Actors).

## After closing and reopening

Session-only changes are intentionally not written as the saved value. Persistent Focus states reappear from saved Actor packages when the editor reloads the level. For an Actor streamed in later under World Partition, its saved state is restored when that Actor is loaded.

## Switching persistence while a value is active

Disabling persistence does not restore an earlier state by itself. Enabling persistence for a temporarily changed control promotes its **current** value for subsequent saving. Choose Restore Temporary Changes instead if you want to discard it.

---

For exact native settings, serialization and recovery, [see Technical](TECHNICAL.md).
