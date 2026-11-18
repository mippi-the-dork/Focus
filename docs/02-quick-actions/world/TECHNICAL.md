# World — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Native property mapping

<!-- Media target: IMAGE | world-native-flags | docs/assets/media/world-native-flags.png -->
<figure class="doc-media" data-kind="image" data-media-id="world-native-flags">
  <img src="../../assets/media-placeholder.png" alt="Diagram showing World Quick Actions flowing to Actor properties and engine subsystems" loading="lazy">
  <figcaption>Focus updates native Actor flags; World Partition, HLOD generation and cooking interpret those flags independently.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Create a diagram of the four World category Actor properties branching to the systems that interpret them: world context, cook/editor-only filtering, World Partition streaming, and HLOD generation. Clearly show Focus as property authoring, not the subsystem executor.</span></figcaption>
</figure>


| Focus control | Actor property |
| --- | --- |
| In Main World Only | `bIsMainWorldOnly` |
| Is Editor Only Actor | `bIsEditorOnlyActor` |
| Is Spatially Loaded | `bIsSpatiallyLoaded` |
| Include Actor in HLOD | `bEnableAutoLODGeneration` |

All four use `EScope::Actor` and validated instance-editable reflected properties. The World Partition loader, HLOD builder and cooker own the downstream result; Focus does not replace those systems.

## World flags are reflected Actor edits

World options use `EScope::Actor`, so a folder/multi-selection applies them to the eligible loaded Actors individually. `FindProperty` requires instance-editable properties; engine-imposed `EditCondition` restrictions, `EditConst`, and `CanEditChange` can render a control unavailable.

## Scope, cooking and World Partition

Each control is a reflected **Actor** property, not a component setting. Native `CanEditChange` and edit conditions may make a field unavailable. Focus neither cooks Actors nor drives streaming/HLOD generation: saved flags are inputs to their respective engine subsystems.

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — property definition, eligibility and write implementation
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — state, Slate widget, transaction and Editor integrations

For the user workflow, [read Design](DESIGN.md).

**Verification:** Code reviewed; live Unreal Editor behavior must be confirmed using the [Verification Matrix](../../VERIFICATION_MATRIX.md).
