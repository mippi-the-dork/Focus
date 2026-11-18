# World — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Native property mapping

| Focus control | Actor property |
| --- | --- |
| In Main World Only | `bIsMainWorldOnly` |
| Is Editor Only Actor | `bIsEditorOnlyActor` |
| Is Spatially Loaded | `bIsSpatiallyLoaded` |
| Include Actor in HLOD | `bEnableAutoLODGeneration` |

All four use `EScope::Actor` and validated instance-editable reflected properties. The World Partition loader, HLOD builder and cooker own the downstream result; Focus does not replace those systems.

## World flags are reflected Actor edits

World options use `EScope::Actor`, so a folder/multi-selection applies them to the eligible loaded Actors individually. `FindProperty` requires instance-editable properties; engine-imposed `EditCondition` restrictions, `EditConst`, and `CanEditChange` can render a control unavailable.

## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
