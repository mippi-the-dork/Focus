// Copyright Mippithedork 2026, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

class AActor;
class UObject;

namespace FocusQuick
{
    // Category groups use consistent spacing. The original Visual and
    // Interaction controls remain a single compact cluster, with a smaller
    // gutter between Solo and Selection Lock.
    inline constexpr float CategoryGap = 10.0f;
    inline constexpr float OriginalControlsGap = 3.0f;
    inline constexpr float InteractionToLightingGap = 7.0f;
    inline constexpr float WorldToNavigationGap = 8.0f;
    inline constexpr float NavigationToMobilityGap = 8.0f;
    enum class ECategory : uint8 { Visual, Interaction, Simulation, PhysicsEvents, Lighting, World, Network, Navigation, Mobility, LOD, Count };
    enum class EKind : uint8 { Bool, Mobility, ForcedLOD };
    enum class EScope : uint8 { Actor, Primitive, Scene, Shape, ActorComponent, Mesh, Lighting };
    struct FDef
    {
        ECategory Category;
        const TCHAR* Label;
        const TCHAR* Symbol;
        EScope Scope;
        EKind Kind;
        const TCHAR* Property;
        const TCHAR* Inner = nullptr;
    };
    const TArray<FDef>& Definitions();
    const TCHAR* CategoryName(ECategory Category);
    bool CategoryVisible(ECategory Category);
    void SetCategoryVisible(ECategory Category, bool bVisible);
    float GapBefore(ECategory Category);
    int32 ColumnWidth();
    // Returns false if target has no instance-editable matching property.
    bool Read(UObject* Target, const FDef& Def, int32& OutValue);
    bool Write(UObject* Target, const FDef& Def, int32 Value);
    // Number of authored LODs, or -1 when the target is not a supported mesh.
    // Forced LOD Model is 1-based and 0 means automatic, so valid values are 0..Count.
    int32 GetAvailableLODCount(UObject* Target);
    void GetTargets(AActor* Actor, const FDef& Def, TArray<UObject*>& Out);
}
