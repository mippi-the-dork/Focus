# Focus Editor Architecture — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## What happens when Focus is enabled?

Focus integrates into the normal Unreal Editor. There is no separate editor application, special Actor class to place, or new level format. The controls appear in the World Outliner as a compact column.

When you change a Visual or Interaction control, Focus updates the appropriate editor-side Actor state. When you change a native quick property, Focus modifies the eligible native Actor/Component property. The source of truth remains the existing Unreal level and Actor packages.

## Why some behavior feels different

Some features require different engine integration paths:

- Selection Lock preserves intentional selection through the Outliner.
- Transform Lock protects normal editor transforms.
- Edit Lock disables supported editing paths but keeps Details browsing functional.
- Solo filters rendered primitives in an editor view without hiding every other Actor's data.
- Session-only settings prevent temporary Visual/Interaction choices from becoming normal saved changes.

These are deliberate distinctions, not arbitrary inconsistencies.

## Startup and shutdown

Focus appears after Unreal's editor systems are ready. If the Focus column doesn't appear immediately after enabling the plugin, restart the editor and confirm its enabled state and column visibility. Disabling the plugin should not leave behind an additional Actor component or runtime behavior.

## What users do not need to manage

There are no separate Focus data assets or runtime Actors. The plugin's preferences are in the editor settings; saved per-Actor controls live with their usual Actor or external-Actor packages.

---

For implementation, native data and engine-integration details, [see Technical](TECHNICAL.md).
