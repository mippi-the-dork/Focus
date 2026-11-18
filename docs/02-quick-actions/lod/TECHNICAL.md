# LOD — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Native target mapping

| Focus control | Scope | Native property |
| --- | --- | --- |
| Forced LOD Model | Static/Skinned Mesh Component | `ForcedLodModel` or skinned-mesh equivalent |
| Never Distance Cull | Primitive Component | `bNeverDistanceCull` |
| Allow Cull Distance Volume | Primitive Component | `bAllowCullDistanceVolume` |

Forced LOD calls mesh-specific setters such as `SetForcedLodModel` or `SetForcedLOD`. The Outliner numeric entry accepts 0–255; 0 means automatic. Focus reports a common available LOD range for eligible meshes and marks an invalid requested value. It does not invent LOD levels or override other renderer/streaming systems.

## Numeric LOD behavior

The UI's `SNumericEntryBox<int32>` uses non-spinning entry with minimum 0 and maximum 255. `FocusQuick::GetAvailableLODCount` queries `UStaticMesh::GetNumLODs()` or `USkinnedMeshComponent::GetNumLODs()`. A multi-target row can compute a common eligible range and flag any mesh for which the chosen forced value is out of range.

The stored value follows Unreal's mapping: `0 = automatic`; positive `n` selects mesh LOD index `n-1`. A larger input is not necessarily blocked from entry merely because one target lacks that LOD; diagnostic color and range tooltip express that conflict.


## Boundaries

Force-LOD handling is not a custom LOD renderer. Cull flags are native properties and do not override streaming, occlusion, manual visibility, or unsupported culling paths.

---

## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
