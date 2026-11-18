# Mobility, LOD and Distance Culling — Technical

**Focus v2.1.0 · Source-verified implementation reference**

## Target mapping and setters

| Control | Scope | Native property | Native write |
| --- | --- | --- | --- |
| Mobility | Scene Component | `Mobility` | `USceneComponent::SetMobility` |
| Forced LOD Model | Static/Skinned Mesh Component | `ForcedLodModel` | `SetForcedLodModel` / `SetForcedLOD` |
| Never Distance Cull | Primitive | `bNeverDistanceCull` | Reflected write |
| Allow Cull Distance Volume | Primitive | `bAllowCullDistanceVolume` | Reflected write |

## Numeric LOD behavior

The UI's `SNumericEntryBox<int32>` uses non-spinning entry with minimum 0 and maximum 255. `FocusQuick::GetAvailableLODCount` queries `UStaticMesh::GetNumLODs()` or `USkinnedMeshComponent::GetNumLODs()`. A multi-target row can compute a common eligible range and flag any mesh for which the chosen forced value is out of range.

The stored value follows Unreal's mapping: `0 = automatic`; positive `n` selects mesh LOD index `n-1`. A larger input is not necessarily blocked from entry merely because one target lacks that LOD; diagnostic color and range tooltip express that conflict.

## Mobility UI and notifications

Mobility offers three individual UI choices mapped to `EComponentMobility::Type`. `FocusQuick::Write` uses the native setter, `Modify()`, `PreEditChange`, and `PostEditChangeProperty`. Other components can reject edits because of native edit conditions.

## Boundaries

Force-LOD handling is not a custom LOD renderer. Cull flags are native properties and do not override streaming, occlusion, manual visibility, or unsupported culling paths.

---

## Source files

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — definitions, property validation and setters
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — row state, mixed state and transactional editing

For user interaction, examples and interpretations, [see Design](DESIGN.md).

**Verification scope:** source-reviewed; no claim of a completed live editor QA matrix.
