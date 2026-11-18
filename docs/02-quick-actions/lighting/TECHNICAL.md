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

## Eligibility and resulting render behavior

`FocusQuick::EScope::Lighting` resolves applicable light/primitive components; `FindProperty` checks native editability. A matching channel property is not proof that a light contributes to a given primitive: renderer settings, compatible channels on **both** participants and supported lighting features determine the result.

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — property definition, eligibility and write implementation
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — state, Slate widget, transaction and Editor integrations

For the user workflow, [read Design](DESIGN.md).

**Verification:** Code reviewed; live Unreal Editor behavior must be confirmed using the [Verification Matrix](../../VERIFICATION_MATRIX.md).
