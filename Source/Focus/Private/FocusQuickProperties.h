// Copyright Mippithedork 2026, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"

class AActor;
class UObject;

namespace FocusQuick
{
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
    int32 ColumnWidth();
    // Returns false if target has no instance-editable matching property.
    bool Read(UObject* Target, const FDef& Def, int32& OutValue);
    bool Write(UObject* Target, const FDef& Def, int32 Value);
    void GetTargets(AActor* Actor, const FDef& Def, TArray<UObject*>& Out);
}
