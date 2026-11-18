# Frequently Asked Questions — Technical

**Focus v2.1.0 · Source-reviewed diagnostics and integration**

## FAQ diagnosis by implementation owner

| Question family | Relevant implementation |
| --- | --- |
| Column missing | `FFocusModule` Actor Browser column registration and plugin loading |
| Category absent | `FocusQuick::CategoryVisible` and Editor-per-project config |
| Control unavailable | `FocusQuick::FindProperty`, `GetTargets`, `CanEditChange` |
| Unexpected mixed state | `FFocusColumn::RefreshRow` eligible value aggregation |
| Lock bypass | Editor mode, transform/command guards and native interaction guard |
| Unexpected Solo content | `FFocusViewExtension::SetupView` primitive visibility filtering |
| Save or checkout | `Focus::Temporary`, `PersistentMetadata`, package pre/post-save hooks |
| World Partition scope | Live loaded Actor traversal and Actor addition restoration |
| Unreal crashes on disable | Guard lifetime/delegate cleanup and in-flight save overlay restoration |

## Confirmation order

Inspect plugin/module load and platform first, then category visibility, target eligibility, Actor scope, lock state, and finally persistence. A Not Applicable segment is not a failure of the column. Similarly, Focus metadata persisting in an Actor package is not evidence that a native property edited through Details followed Focus's session-only configuration.

## Testing strategy

For any unexpected editor behavior, reproduce in a clean UE 5.8 project with one Actor of a known class and one compatible component. Repeat through a loaded folder and multi-selection only after the single-Actor case passes. Use normal editor transaction history and the Output Log to distinguish Focus control state from Unreal's underlying systems.

**Scope:** These are source-directed diagnostics. The site does not claim an automated compatibility result for every project configuration.

---

## Implementation references

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [FocusInteractionGuard.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Focus.uplugin](https://github.com/mippi-the-dork/Focus/blob/main/Focus.uplugin)

For normal user steps, [see Design](DESIGN.md).

**QA limit:** This documents the source behavior and test suggestions, not a full live UE 5.8 test report.
