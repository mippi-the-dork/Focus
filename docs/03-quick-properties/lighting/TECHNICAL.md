# Lighting and World — Technical

**Focus v2.1.0 · Source-verified implementation reference**

## Native field mapping

| Category | Control | Scope | Property |
| --- | --- | --- | --- |
| Lighting | Channel 0/1/2 | Lighting components | `LightingChannels.bChannel0/1/2` |
| Lighting | Cast Shadows | Eligible primitives/lights | `CastShadow` (or light-component `CastShadows`) |
| World | In Main World Only | Actor | `bIsMainWorldOnly` |
| World | Is Editor Only Actor | Actor | `bIsEditorOnlyActor` |
| World | Is Spatially Loaded | Actor | `bIsSpatiallyLoaded` |
| World | Include Actor in HLOD | Actor | `bEnableAutoLODGeneration` |

## Lighting channels are a combined update

`FocusQuick::Write` reads all three current channel booleans before writing the chosen channel, then calls `SetLightingChannels(A,B,C)` on eligible primitives or lights. This prevents toggling one miniwidget segment from unintentionally clearing the others. `FindProperty` handles the light-component shadow-property naming variation.

## World flags are reflected Actor edits

World options use `EScope::Actor`, so a folder/multi-selection applies them to the eligible loaded Actors individually. `FindProperty` requires instance-editable properties; engine-imposed `EditCondition` restrictions, `EditConst`, and `CanEditChange` can render a control unavailable.

## No special runtime ownership

Focus only edits the native properties and propagates normal Unreal property notifications and transactions. It does not override the World Partition scheduler, HLOD builder, light transport, cooking behavior, or renderer's lighting-channel rules. Those systems determine the actual result.

---

## Source files

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — definitions, property validation and setters
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — row state, mixed state and transactional editing

For user interaction, examples and interpretations, [see Design](DESIGN.md).

**Verification scope:** source-reviewed; no claim of a completed live editor QA matrix.
