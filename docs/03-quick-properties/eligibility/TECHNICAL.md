# Applicability, Mixed States and Undo — Technical

**Focus v2.1.0 · Source-verified implementation reference**

## Property validation

`FocusQuick::FindProperty` calls `FindFProperty` on the Actor/Component or nested `FStructProperty` as appropriate. It checks:
- valid, non-template instance;
- `CPF_Edit`;
- absence of `CPF_DisableEditOnInstance` and `CPF_EditConst`;
- absence of `EditCondition` metadata (handled conservatively);
- `UObject::CanEditChange`.

A property can therefore be unavailable even when a matching component exists, because Unreal does not allow editing that instance.

## Target collection

`GetTargets` walks registered, valid, non-template components and filters by `EScope::Primitive`, `Scene`, `Shape`, `ActorComponent`, `Mesh`, or `Lighting`. `EScope::Actor` directly returns the Actor.

## Read and mixed aggregation

`Read` handles `FBoolProperty`, `FIntProperty` for forced LOD, and `FByteProperty` for mobility. The Outliner row's quick state accumulates first eligible value, eligible target count, and mixed status. If no eligible value exists, the segment is unavailable and must not be toggled.

## Transaction and property update

`FFocusColumn::ApplyQuick` wraps the operation in `FScopedTransaction`, resolves the current Actor action scope, skips Edit-Locked Actors, and applies `FocusQuick::Write` to each eligible target. `Write` skips unchanged values, calls `Modify`, `PreEditChange`, dispatches the correct native setter or reflected edit, then `PostEditChangeProperty`.

This does not bypass native edit conditions, and it does not make every Actor capable of every property. Any restrictions applied by other Unreal systems remain relevant.

---

## Source files

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — definitions, property validation and setters
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — row state, mixed state and transactional editing

For user interaction, examples and interpretations, [see Design](DESIGN.md).

**Verification scope:** source-reviewed; no claim of a completed live editor QA matrix.
