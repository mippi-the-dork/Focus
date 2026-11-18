# Category Visibility and Column Layout — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Start with the controls you need

Focus has ten groups in a fixed order: **Visual**, **Interaction**, **Simulation**, **Physics Events**, **Lighting**, **World**, **Network**, **Navigation**, **Mobility**, and **LOD**.

Only Visual and Interaction are enabled by default. The other eight are optional because showing every control makes the Outliner column wider.

## Customize your Outliner

<!-- Media target: IMAGE | settings-category-visibility | docs/assets/media/settings-category-visibility.png -->
<figure class="doc-media" data-kind="image" data-media-id="settings-category-visibility">
  <img src="../../assets/media-placeholder.png" alt="Focus category visibility menu with several Quick Actions groups enabled" loading="lazy">
  <figcaption>The header menu lets each editor user choose which Quick Actions categories occupy space in their Outliner.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture the Focus column-header menu with several optional categories checked and others unchecked. Show the resized column containing only the enabled category groups, ideally with the Reset to Visual + Interaction command visible.</span></figcaption>
</figure>


1. Open the World Outliner and locate the Focus header.
2. Open its category menu.
3. Check the groups you frequently use. Uncheck those that add clutter.
4. Inspect the row icons. They appear or disappear immediately and the Focus column adjusts its width.
5. Use **Reset to Visual + Interaction** to restore the compact default.

The menu explains the purpose and available controls in each category. Enabling a category **does not change any Actor or Component property**. It merely makes its quick controls available in the column.

## Example layouts

For environment art, start with Visual, Interaction, Lighting and LOD. For a gameplay blockout, add Simulation, Physics Events and Navigation. Technical designers may also want World and Network. These are workflow suggestions, not separate Focus presets.

## Does this travel with my level?

Category visibility is an **editor-per-project user preference**. It does not add Actors, change levels, or affect what another teammate sees in a packaged build.

---

For exact native settings, serialization and recovery, [see Technical](TECHNICAL.md).
