# Lighting — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Native property mapping

| Focus control | Native target |
| --- | --- |
| Channel 0/1/2 | `LightingChannels.bChannel0/bChannel1/bChannel2` |
| Cast Shadows | `CastShadow` on eligible primitives, or equivalent light-component shadow property |

Lighting channels are applied as a combined three-channel state through `SetLightingChannels`, preserving the other two flags when one is changed.

## Lighting channels are a combined update

`FocusQuick::Write` reads all three current channel booleans before writing the chosen channel, then calls `SetLightingChannels(A,B,C)` on eligible primitives or lights. This prevents toggling one miniwidget segment from unintentionally clearing the others. `FindProperty` handles the light-component shadow-property naming variation.

## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
