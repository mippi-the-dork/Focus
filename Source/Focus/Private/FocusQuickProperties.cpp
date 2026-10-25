// Copyright Mippithedork 2026, Inc. All Rights Reserved.
#include "FocusQuickProperties.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/LightComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/BodyInstance.h"
#include "UObject/UnrealType.h"
#include "Misc/ConfigCacheIni.h"
#include "Framework/Application/SlateApplication.h"

namespace FocusQuick
{
    const TArray<FDef>& Definitions()
    {
        static const TArray<FDef> Defs = {
            {ECategory::Simulation, TEXT("Simulate Physics"), TEXT("P"), EScope::Primitive, EKind::Bool, TEXT("BodyInstance"), TEXT("bSimulatePhysics")},
            {ECategory::Simulation, TEXT("Enable Gravity"), TEXT("G"), EScope::Primitive, EKind::Bool, TEXT("BodyInstance"), TEXT("bEnableGravity")},
            {ECategory::Simulation, TEXT("Ignore Radial Impulse"), TEXT("I"), EScope::Primitive, EKind::Bool, TEXT("bIgnoreRadialImpulse")},
            {ECategory::Simulation, TEXT("Ignore Radial Force"), TEXT("F"), EScope::Primitive, EKind::Bool, TEXT("bIgnoreRadialForce")},
            {ECategory::Simulation, TEXT("Apply Impulse on Damage"), TEXT("D"), EScope::Primitive, EKind::Bool, TEXT("bApplyImpulseOnDamage")},
            {ECategory::Simulation, TEXT("Async Physics Tick Enabled"), TEXT("A"), EScope::Actor, EKind::Bool, TEXT("bAsyncPhysicsTickEnabled")},
            {ECategory::PhysicsEvents, TEXT("Generate Hit Events"), TEXT("H"), EScope::Primitive, EKind::Bool, TEXT("BodyInstance"), TEXT("bNotifyRigidBodyCollision")},
            {ECategory::PhysicsEvents, TEXT("Generate Overlap Events"), TEXT("O"), EScope::Primitive, EKind::Bool, TEXT("bGenerateOverlapEvents")},
            {ECategory::PhysicsEvents, TEXT("Generate Wake Events"), TEXT("W"), EScope::Primitive, EKind::Bool, TEXT("BodyInstance"), TEXT("bGenerateWakeEvents")},
            {ECategory::Lighting, TEXT("Lighting Channel 0"), TEXT("0"), EScope::Lighting, EKind::Bool, TEXT("LightingChannels"), TEXT("bChannel0")},
            {ECategory::Lighting, TEXT("Lighting Channel 1"), TEXT("1"), EScope::Lighting, EKind::Bool, TEXT("LightingChannels"), TEXT("bChannel1")},
            {ECategory::Lighting, TEXT("Lighting Channel 2"), TEXT("2"), EScope::Lighting, EKind::Bool, TEXT("LightingChannels"), TEXT("bChannel2")},
            {ECategory::Lighting, TEXT("Cast Shadows"), TEXT("S"), EScope::Lighting, EKind::Bool, TEXT("CastShadow")},
            {ECategory::World, TEXT("In Main World Only"), TEXT("M"), EScope::Actor, EKind::Bool, TEXT("bIsMainWorldOnly")},
            {ECategory::World, TEXT("Is Editor Only Actor"), TEXT("E"), EScope::Actor, EKind::Bool, TEXT("bIsEditorOnlyActor")},
            {ECategory::World, TEXT("Is Spatially Loaded"), TEXT("W"), EScope::Actor, EKind::Bool, TEXT("bIsSpatiallyLoaded")},
            {ECategory::World, TEXT("Include Actor in HLOD"), TEXT("L"), EScope::Actor, EKind::Bool, TEXT("bEnableAutoLODGeneration")},
            {ECategory::Network, TEXT("Net Load on Client"), TEXT("C"), EScope::Actor, EKind::Bool, TEXT("bNetLoadOnClient")},
            {ECategory::Network, TEXT("Replicate Physics to Autonomous Proxy"), TEXT("P"), EScope::Primitive, EKind::Bool, TEXT("bReplicatePhysicsToAutonomousProxy")},
            {ECategory::Network, TEXT("Static Mesh Replicate Movement"), TEXT("M"), EScope::Actor, EKind::Bool, TEXT("bStaticMeshReplicateMovement")},
            {ECategory::Navigation, TEXT("Can Ever Affect Navigation"), TEXT("N"), EScope::ActorComponent, EKind::Bool, TEXT("bCanEverAffectNavigation")},
            {ECategory::Navigation, TEXT("Dynamic Obstacle"), TEXT("D"), EScope::Shape, EKind::Bool, TEXT("bDynamicObstacle")},
            {ECategory::Navigation, TEXT("Fill Collision Underneath for Navmesh"), TEXT("F"), EScope::Primitive, EKind::Bool, TEXT("bFillCollisionUnderneathForNavmesh")},
            {ECategory::Navigation, TEXT("Rasterize as Filled Convex Volume"), TEXT("R"), EScope::Primitive, EKind::Bool, TEXT("bRasterizeAsFilledConvexVolume")},
            {ECategory::Mobility, TEXT("Mobility"), TEXT("M"), EScope::Scene, EKind::Mobility, TEXT("Mobility")},
            {ECategory::LOD, TEXT("Forced LOD Model"), TEXT("0"), EScope::Mesh, EKind::ForcedLOD, TEXT("ForcedLodModel")},
            {ECategory::LOD, TEXT("Never Distance Cull"), TEXT("N"), EScope::Primitive, EKind::Bool, TEXT("bNeverDistanceCull")},
            {ECategory::LOD, TEXT("Allow Cull Distance Volume"), TEXT("C"), EScope::Primitive, EKind::Bool, TEXT("bAllowCullDistanceVolume")},
        };
        return Defs;
    }
    const TCHAR* CategoryName(ECategory C)
    {
        static const TCHAR* Names[] = {TEXT("Visual"), TEXT("Interaction"), TEXT("Simulation"), TEXT("Physics Events"), TEXT("Lighting"), TEXT("World"), TEXT("Network"), TEXT("Navigation"), TEXT("Mobility"), TEXT("LOD")};
        return Names[static_cast<uint8>(C)];
    }
    static const TCHAR* ConfigSection = TEXT("Focus.CategoryVisibility");
    static TOptional<bool> CachedCategories[static_cast<int32>(ECategory::Count)];
    bool CategoryVisible(ECategory C)
    {
        const int32 Index = static_cast<int32>(C);
        if (CachedCategories[Index].IsSet()) return CachedCategories[Index].GetValue();
        bool bVisible = C == ECategory::Visual || C == ECategory::Interaction;
        if (GConfig) GConfig->GetBool(ConfigSection, CategoryName(C), bVisible, GEditorPerProjectIni);
        CachedCategories[Index] = bVisible;
        return bVisible;
    }
    void SetCategoryVisible(ECategory C, bool bVisible)
    {
        CachedCategories[static_cast<int32>(C)] = bVisible;
        if (GConfig)
        {
            GConfig->SetBool(ConfigSection, CategoryName(C), bVisible, GEditorPerProjectIni);
            GConfig->Flush(false, GEditorPerProjectIni);
        }
        // Column width, header menu checks and row visibility depend on this setting.
        if (FSlateApplication::IsInitialized()) FSlateApplication::Get().InvalidateAllWidgets(false);
    }
    float GapBefore(ECategory Category)
    {
        int32 Previous = static_cast<int32>(Category) - 1;
        while (Previous >= 0 && !CategoryVisible(static_cast<ECategory>(Previous)))
        {
            --Previous;
        }
        if (Previous < 0) return 0.0f;
        if (Category == ECategory::Interaction && Previous == static_cast<int32>(ECategory::Visual))
            return OriginalControlsGap;
        if (Category == ECategory::Lighting && Previous == static_cast<int32>(ECategory::Interaction))
            return InteractionToLightingGap;
        // Keep the art itself untouched. These narrower gutters balance the
        // six-piece Simulation icon and the three-piece Network icon with
        // surrounding category silhouettes.
        if (Category == ECategory::Simulation)
            return BeforeSimulationGap;
        if (Category == ECategory::Network)
            return BeforeNetworkGap;
        if (Previous == static_cast<int32>(ECategory::Network))
            return AfterNetworkGap;
        if (Previous == static_cast<int32>(ECategory::Simulation))
            return AfterSimulationGap;
        if (Category == ECategory::Navigation && Previous == static_cast<int32>(ECategory::World))
            return WorldToNavigationGap;
        if (Category == ECategory::Mobility && Previous == static_cast<int32>(ECategory::Navigation))
            return NavigationToMobilityGap;
        return CategoryGap;
    }

    int32 ColumnWidth()
    {
        // Match the category-level spacing in FFocusColumn::AddCategoryGroup.
        // The first visible group has no leading gutter; hidden categories
        // contribute no width. Keep enough room for native Slate row padding.
        int32 Width = 8;
        for (int32 Id = 0; Id < static_cast<int32>(ECategory::Count); ++Id)
        {
            const ECategory Category = static_cast<ECategory>(Id);
            if (!CategoryVisible(Category)) continue;
            Width += FMath::CeilToInt(GapBefore(Category));
            if (Category == ECategory::Visual) { Width += 34; continue; }
            if (Category == ECategory::Interaction) { Width += 40; continue; }
            if (Category == ECategory::Simulation) { Width += 18; continue; }
            if (Category == ECategory::PhysicsEvents) { Width += 16; continue; }
            if (Category == ECategory::Network) { Width += 18; continue; }
            if (Category == ECategory::Lighting) { Width += 19; continue; }
            if (Category == ECategory::World) { Width += 19; continue; }
            if (Category == ECategory::Navigation) { Width += 18; continue; }
            if (Category == ECategory::Mobility) { Width += 14; continue; }
            if (Category == ECategory::LOD) { Width += 19; continue; }
            for (const FDef& Def : Definitions())
            {
                if (Def.Category == Category)
                {
                    Width += Def.Kind == EKind::Mobility ? 60 : 20;
                }
            }
        }
        return FMath::Max(75, Width);
    }
    static bool CanEdit(UObject* Obj, FProperty* Property)
    {
        return Obj && Property && !Obj->IsTemplate() &&
            Property->HasAnyPropertyFlags(CPF_Edit) &&
            !Property->HasAnyPropertyFlags(CPF_DisableEditOnInstance | CPF_EditConst) &&
            !Property->HasMetaData(TEXT("EditCondition")) && Obj->CanEditChange(Property);
    }
    static FProperty* FindProperty(UObject* Obj, const FDef& Def, void*& Container, FProperty*& Outer)
    {
        Container = Obj;
        Outer = Obj ? FindFProperty<FProperty>(Obj->GetClass(), Def.Property) : nullptr;
        if (!Outer && Cast<ULightComponent>(Obj) && FString(Def.Property) == TEXT("CastShadow"))
        {
            // Light components name the corresponding Details property differently.
            Outer = FindFProperty<FProperty>(Obj->GetClass(), TEXT("CastShadows"));
        }
        if (!CanEdit(Obj, Outer)) return nullptr;
        if (Def.Inner)
        {
            auto* StructProp = CastField<FStructProperty>(Outer);
            if (!StructProp) return nullptr;
            Container = StructProp->ContainerPtrToValuePtr<void>(Obj);
            FProperty* Inner = FindFProperty<FProperty>(StructProp->Struct, Def.Inner);
            return Inner && Inner->HasAnyPropertyFlags(CPF_Edit) &&
                !Inner->HasAnyPropertyFlags(CPF_EditConst | CPF_DisableEditOnInstance) &&
                !Inner->HasMetaData(TEXT("EditCondition")) && Obj->CanEditChange(Inner)
                ? Inner : nullptr;
        }
        return Outer;
    }
    bool Read(UObject* Target, const FDef& Def, int32& OutValue)
    {
        void* Container = nullptr;
        FProperty* Outer = nullptr;
        FProperty* Leaf = FindProperty(Target, Def, Container, Outer);
        if (!Leaf) return false;
        if (Def.Kind == EKind::Bool)
        {
            if (auto* B = CastField<FBoolProperty>(Leaf))
            {
                OutValue = B->GetPropertyValue_InContainer(Container) ? 1 : 0;
                return true;
            }
            return false;
        }
        if (Def.Kind == EKind::ForcedLOD)
        {
            if (auto* I = CastField<FIntProperty>(Leaf))
            {
                OutValue = I->GetPropertyValue_InContainer(Container);
                return true;
            }
            return false;
        }
        if (Def.Kind == EKind::Mobility)
        {
            if (auto* B = CastField<FByteProperty>(Leaf))
            {
                OutValue = B->GetPropertyValue_InContainer(Container);
                return true;
            }
        }
        return false;
    }
    int32 GetAvailableLODCount(UObject* Target)
    {
        if (const UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Target))
        {
            const UStaticMesh* Asset = StaticMeshComponent->GetStaticMesh();
            return Asset ? Asset->GetNumLODs() : 0;
        }
        if (const USkinnedMeshComponent* SkinnedMeshComponent = Cast<USkinnedMeshComponent>(Target))
        {
            return SkinnedMeshComponent->GetNumLODs();
        }
        return -1;
    }

    bool Write(UObject* Target, const FDef& Def, int32 Value)
    {
        int32 Previous = 0;
        if (!Read(Target, Def, Previous) || Previous == Value) return false;
        void* Container = nullptr;
        FProperty* Outer = nullptr;
        FProperty* Leaf = FindProperty(Target, Def, Container, Outer);
        if (!Leaf || !Outer) return false;
        Target->Modify();
        Target->PreEditChange(Outer);
        if (Def.Kind == EKind::Mobility)
        {
            if (auto* Comp = Cast<USceneComponent>(Target)) Comp->SetMobility(static_cast<EComponentMobility::Type>(Value));
        }
        else if (Def.Kind == EKind::ForcedLOD)
        {
            if (auto* StaticMesh = Cast<UStaticMeshComponent>(Target)) StaticMesh->SetForcedLodModel(Value);
            else if (auto* Skinned = Cast<USkinnedMeshComponent>(Target)) Skinned->SetForcedLOD(Value);
        }
        else if (Def.Kind == EKind::Bool)
        {
            if (FString(Def.Property) == TEXT("bCanEverAffectNavigation"))
            {
                if (auto* Component = Cast<UActorComponent>(Target)) Component->SetCanEverAffectNavigation(Value != 0);
            }
            else if (auto* Comp = Cast<UPrimitiveComponent>(Target))
            {
                const FString PropertyName = Def.Inner ? FString(Def.Inner) : FString(Def.Property);
                if (PropertyName == TEXT("bSimulatePhysics")) Comp->SetSimulatePhysics(Value != 0);
                else if (PropertyName == TEXT("bEnableGravity")) Comp->SetEnableGravity(Value != 0);
                else if (PropertyName == TEXT("bNotifyRigidBodyCollision")) Comp->SetNotifyRigidBodyCollision(Value != 0);
                else if (PropertyName == TEXT("bGenerateOverlapEvents")) Comp->SetGenerateOverlapEvents(Value != 0);
                else if (PropertyName == TEXT("CastShadow")) Comp->SetCastShadow(Value != 0);
                else if (FString(Def.Property) == TEXT("LightingChannels"))
                {
                    // Read other channels first; only replace the chosen channel.
                    const FDef& D0 = Definitions()[9];
                    const FDef& D1 = Definitions()[10];
                    const FDef& D2 = Definitions()[11];
                    int32 A = 0, B = 0, C = 0;
                    Read(Target, D0, A); Read(Target, D1, B); Read(Target, D2, C);
                    if (FString(Def.Inner) == TEXT("bChannel0")) A = Value;
                    if (FString(Def.Inner) == TEXT("bChannel1")) B = Value;
                    if (FString(Def.Inner) == TEXT("bChannel2")) C = Value;
                    Comp->SetLightingChannels(A != 0, B != 0, C != 0);
                }
                else if (auto* Bool = CastField<FBoolProperty>(Leaf)) Bool->SetPropertyValue_InContainer(Container, Value != 0);
            }
            else if (auto* Light = Cast<ULightComponent>(Target))
            {
                if (FString(Def.Property) == TEXT("LightingChannels"))
                {
                    int32 A = 0, B = 0, C = 0;
                    Read(Target, Definitions()[9], A);
                    Read(Target, Definitions()[10], B);
                    Read(Target, Definitions()[11], C);
                    if (FString(Def.Inner) == TEXT("bChannel0")) A = Value;
                    if (FString(Def.Inner) == TEXT("bChannel1")) B = Value;
                    if (FString(Def.Inner) == TEXT("bChannel2")) C = Value;
                    Light->SetLightingChannels(A != 0, B != 0, C != 0);
                }
                else if (auto* Bool = CastField<FBoolProperty>(Leaf))
                {
                    Bool->SetPropertyValue_InContainer(Container, Value != 0);
                }
            }
            else if (auto* Bool = CastField<FBoolProperty>(Leaf)) Bool->SetPropertyValue_InContainer(Container, Value != 0);
        }
        FPropertyChangedEvent Changed(Outer, EPropertyChangeType::ValueSet);
        Target->PostEditChangeProperty(Changed);
        return true;
    }
    void GetTargets(AActor* Actor, const FDef& Def, TArray<UObject*>& Out)
    {
        if (!IsValid(Actor)) return;
        if (Def.Scope == EScope::Actor) { Out.Add(Actor); return; }
        TArray<UActorComponent*> Components;
        Actor->GetComponents(Components);
        for (UActorComponent* Comp : Components)
        {
            if (!IsValid(Comp) || !Comp->IsRegistered() || Comp->IsTemplate()) continue;
            switch (Def.Scope)
            {
            case EScope::Primitive: if (Cast<UPrimitiveComponent>(Comp)) Out.Add(Comp); break;
            case EScope::Scene: if (Cast<USceneComponent>(Comp)) Out.Add(Comp); break;
            case EScope::Shape: if (Cast<UShapeComponent>(Comp)) Out.Add(Comp); break;
            case EScope::ActorComponent: Out.Add(Comp); break;
            case EScope::Mesh: if (Cast<UStaticMeshComponent>(Comp) || Cast<USkinnedMeshComponent>(Comp)) Out.Add(Comp); break;
            case EScope::Lighting: if (Cast<UPrimitiveComponent>(Comp) || Cast<ULightComponent>(Comp)) Out.Add(Comp); break;
            default: break;
            }
        }
    }
}
