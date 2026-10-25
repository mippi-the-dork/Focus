// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "FocusState.h"
#include "FocusSettings.h"
#include "FocusQuickProperties.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SMenuAnchor.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SLeafWidget.h"
#include "Rendering/DrawElements.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h"
#include "Widgets/Layout/SScrollBox.h"
#include "FocusEditorMode.h"
#include "Modules/ModuleManager.h"
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "Editor/TransBuffer.h"
#include "Engine/Engine.h"
#include "Components/SceneComponent.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/Stack.h"
#include "CoreGlobals.h"
#include "UObject/Class.h"
#include "EditorModeManager.h"
#include "LevelEditor.h"
#include "LevelEditorActions.h"
#include "Selection.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/GenericCommands.h"
#include "SSubobjectEditor.h"
#include "Layout/Children.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Editor/GroupActor.h"
#include "Widgets/SOverlay.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "ScopedTransaction.h"
#include "SceneOutlinerModule.h"
#include "ISceneOutlinerColumn.h"
#include "ISceneOutliner.h"
#include "Widgets/Views/STreeView.h"
#include "ISceneOutlinerTreeItem.h"
#include "ActorTreeItem.h"
#include "FolderTreeItem.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "IDetailTreeNode.h"
#include "UObject/UnrealType.h"
#include "Containers/Ticker.h"
#include "SceneViewExtension.h"
#include "SceneView.h"
#include "SceneInterface.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Styling/AppStyle.h"
#include "Brushes/SlateImageBrush.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Brushes/SlateColorBrush.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/MetaData.h"
#include "UObject/Package.h"
#include "Misc/Change.h"
#include "Misc/ITransaction.h"
#include "Engine/Level.h"
#include "Math/NumericLimits.h"

namespace Focus
{
    // Widgets retain shared ownership, so their brush pointers stay valid until
    // those widgets are destroyed, including during editor shutdown.
    static TSharedPtr<FSlateVectorImageBrush> SoloOutline;
    static TSharedPtr<FSlateVectorImageBrush> SoloSolid;
    static TSharedPtr<FSlateVectorImageBrush> SoloMixed;
    static void LoadSoloIcons()
    {
        const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Focus"));
        if (!Plugin) return;
        const FString Resources = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources"));
        const FString OutlinePath = FPaths::Combine(Resources, TEXT("solo-outline.svg"));
        const FString SolidPath = FPaths::Combine(Resources, TEXT("solo-solid.svg"));
        const FString MixedPath = FPaths::Combine(Resources, TEXT("solo-mixed.svg"));
        if (!IFileManager::Get().FileExists(*OutlinePath) || !IFileManager::Get().FileExists(*SolidPath) || !IFileManager::Get().FileExists(*MixedPath))
        {
            UE_LOG(LogTemp, Warning, TEXT("Focus: Solo SVG files are missing from %s. Using text fallback."), *Resources);
            return;
        }
        SoloOutline = MakeShared<FSlateVectorImageBrush>(OutlinePath, FVector2D(16.f, 16.f), FLinearColor::White, ESlateBrushTileType::NoTile);
        SoloSolid = MakeShared<FSlateVectorImageBrush>(SolidPath, FVector2D(16.f, 16.f), FLinearColor::White, ESlateBrushTileType::NoTile);
        SoloMixed = MakeShared<FSlateVectorImageBrush>(MixedPath, FVector2D(16.f, 16.f), FLinearColor::White, ESlateBrushTileType::NoTile);
    }

    static TSharedPtr<FSlateVectorImageBrush> SelectionUnlocked;
    static TSharedPtr<FSlateVectorImageBrush> SelectionLocked;

    // Individually tinted, full-canvas vector silhouettes traced from the
    // three supplied miniwidget illustrations. Shared ownership keeps the
    // SVG brushes alive for the lifetime of every Outliner row that uses them.
    static TSharedPtr<FSlateVectorImageBrush> SimulationPieces[6];
    static TSharedPtr<FSlateVectorImageBrush> PhysicsEventPieces[3];
    static TSharedPtr<FSlateVectorImageBrush> NetworkPieces[3];

    static void LoadGeometricCategoryIcons()
    {
        const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Focus"));
        if (!Plugin) return;
        const FString Resources = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources"));
        auto Load = [&Resources](const TCHAR* Prefix, TSharedPtr<FSlateVectorImageBrush>* Pieces,
            int32 Count, const FVector2D& Size)
        {
            for (int32 Index = 0; Index < Count; ++Index)
            {
                const FString Filename = FPaths::Combine(Resources,
                    FString::Printf(TEXT("focus-%s-%d.svg"), Prefix, Index));
                if (!IFileManager::Get().FileExists(*Filename))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Focus: miniwidget SVG missing: %s"), *Filename);
                    continue;
                }
                Pieces[Index] = MakeShared<FSlateVectorImageBrush>(Filename, Size,
                    FLinearColor::White, ESlateBrushTileType::NoTile);
            }
        };
        Load(TEXT("simulation"), SimulationPieces, 6, FVector2D(18.f, 16.3f));
        Load(TEXT("physics"), PhysicsEventPieces, 3, FVector2D(16.f, 16.f));
        Load(TEXT("network"), NetworkPieces, 3, FVector2D(18.f, 15.6f));
    }
    static void LoadSelectionIcons()
    {
        const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Focus"));
        if (!Plugin) return;
        const FString Resources = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources"));
        const FString UnlockedPath = FPaths::Combine(Resources, TEXT("S-lock-unlocked.svg"));
        const FString LockedPath = FPaths::Combine(Resources, TEXT("S-lock-locked.svg"));
        if (!IFileManager::Get().FileExists(*UnlockedPath) || !IFileManager::Get().FileExists(*LockedPath))
        {
            UE_LOG(LogTemp, Warning, TEXT("Focus: Selection SVG files are missing from %s. Using padlock fallback."), *Resources);
            return;
        }
        SelectionUnlocked = MakeShared<FSlateVectorImageBrush>(UnlockedPath, FVector2D(16.f, 16.f), FLinearColor::White, ESlateBrushTileType::NoTile);
        SelectionLocked = MakeShared<FSlateVectorImageBrush>(LockedPath, FVector2D(16.f, 16.f), FLinearColor::White, ESlateBrushTileType::NoTile);
    }

    // Shared state is stored in the actor's actual package, including external
    // actor packages. No user config, actor tags or serialized plugin objects.
    static TMap<TWeakObjectPtr<AActor>, FFlags> States;
    static const FName SharedStateKey(TEXT("Focus.SharedState.V1"));

    bool IsEditorActor(const AActor* A)
    {
        return IsValid(A) && A->GetWorld() && A->GetWorld()->WorldType == EWorldType::Editor;
    }

    // Fixed-width versioned payload: selection, transform, edit, solo, hidden.
    // Strict parsing keeps unknown/corrupt data from silently becoming locks.
    static bool DecodeSharedState(const FString& Text, FFlags& Flags, bool& Hidden)
    {
        if (Text.Len() != 5) return false;
        for (int32 Index = 0; Index < 5; ++Index)
            if (Text[Index] != TEXT('0') && Text[Index] != TEXT('1')) return false;
        Flags.Selection = Text[0] == TEXT('1');
        Flags.Transform = Text[1] == TEXT('1');
        Flags.Edit = Text[2] == TEXT('1');
        Flags.Solo = Text[3] == TEXT('1');
        Hidden = Text[4] == TEXT('1');
        return true;
    }
    static FString EncodeSharedState(const FFlags& Flags, bool Hidden)
    {
        FString Text(TEXT("00000"));
        Text[0] = Flags.Selection ? TEXT('1') : TEXT('0');
        Text[1] = Flags.Transform ? TEXT('1') : TEXT('0');
        Text[2] = Flags.Edit ? TEXT('1') : TEXT('0');
        Text[3] = Flags.Solo ? TEXT('1') : TEXT('0');
        Text[4] = Hidden ? TEXT('1') : TEXT('0');
        return Text;
    }
    static void EnsureState(AActor* Actor)
    {
        if (!IsEditorActor(Actor) || States.Contains(Actor)) return;
        FFlags Flags;
        Flags.OriginalMovementLock = Actor->IsLockLocation();
        Flags.Transform = Flags.OriginalMovementLock;
        bool Hidden = Actor->IsTemporarilyHiddenInEditor(false);
        if (DecodeSharedState(Actor->GetPackage()->GetMetaData().GetValue(Actor, SharedStateKey), Flags, Hidden))
        {
            Actor->SetLockLocation(Flags.Transform || Flags.Edit);
            // Do not invoke AGroupActor's recursive override; each actor owns
            // its saved setting, including mixed folder and group states.
            Actor->AActor::SetIsTemporarilyHiddenInEditor(Hidden);
        }
        States.Add(Actor, Flags);
    }
    static void RestoreLoadedWorld(UWorld* World)
    {
        if (!World || World->WorldType != EWorldType::Editor) return;
        for (TActorIterator<AActor> It(World); It; ++It) EnsureState(*It);
    }
    bool Get(AActor* A, EControl C)
    {
        if (!IsEditorActor(A)) return false;
        EnsureState(A);
        if (C == EControl::EditorHidden) return A->IsTemporarilyHiddenInEditor(false);
        if (C == EControl::GameHidden) return A->IsHidden();
        const FFlags& F = States.FindChecked(A);
        switch (C) {
        case EControl::Selection: return F.Selection;
        case EControl::Transform: return F.Transform || F.Edit;
        case EControl::Edit: return F.Edit;
        case EControl::Solo: return F.Solo;
        default: return false;
        }
    }

    struct FSharedSnapshot
    {
        FFlags Flags;
        bool Hidden = false;
        bool HadMetadata = false;
        FString Metadata;
    };
    static FSharedSnapshot CaptureSharedState(AActor* Actor)
    {
        EnsureState(Actor);
        FSharedSnapshot Result;
        Result.Flags = States.FindChecked(Actor);
        Result.Hidden = Actor->IsTemporarilyHiddenInEditor(false);
        FMetaData& Meta = Actor->GetPackage()->GetMetaData();
        Result.HadMetadata = Meta.HasValue(Actor, SharedStateKey);
        Result.Metadata = Meta.GetValue(Actor, SharedStateKey);
        return Result;
    }
    static void ApplySharedState(AActor* Actor, const FSharedSnapshot& Snapshot)
    {
        if (!IsEditorActor(Actor)) return;
        UPackage* Package = Actor->GetPackage();
        FMetaData& Meta = Package->GetMetaData();
        if (Snapshot.HadMetadata) Meta.SetValue(Actor, SharedStateKey, *Snapshot.Metadata);
        else Meta.RemoveValue(Actor, SharedStateKey);
        States.Add(Actor, Snapshot.Flags);
        Actor->SetLockLocation(Snapshot.Flags.Transform || Snapshot.Flags.Edit);
        Actor->AActor::SetIsTemporarilyHiddenInEditor(Snapshot.Hidden);
        // Metadata has no automatic dirty tracking. Saving this package is what
        // makes the settings available after restart and to source control.
        Package->MarkPackageDirty();
    }
    class FSharedStateChange final : public FCommandChange
    {
        FSharedSnapshot Before, After;
        static void Restore(UObject* Object, const FSharedSnapshot& Snapshot)
        {
            ApplySharedState(Cast<AActor>(Object), Snapshot);
            if (GEditor) GEditor->RedrawLevelEditingViewports();
        }
    public:
        FSharedStateChange(FSharedSnapshot InBefore, FSharedSnapshot InAfter)
            : Before(MoveTemp(InBefore)), After(MoveTemp(InAfter)) {}
        virtual void Apply(UObject* Object) override { Restore(Object, After); }
        virtual void Revert(UObject* Object) override { Restore(Object, Before); }
        virtual FString ToString() const override { return TEXT("Focus shared actor settings"); }
    };
    void Set(AActor* A, EControl C, bool Value)
    {
        if (!IsEditorActor(A)) return;
        EnsureState(A);
        if (C == EControl::GameHidden)
        {
            if (!Get(A, EControl::Edit) && A->IsHidden() != Value)
            {
                A->Modify();
                A->SetActorHiddenInGame(Value);
            }
            return;
        }
        if (C == EControl::Transform && Get(A, EControl::Edit)) return;
        const FSharedSnapshot Before = CaptureSharedState(A);
        FSharedSnapshot After = Before;
        switch (C) {
        case EControl::EditorHidden: After.Hidden = Value; break;
        case EControl::Selection: After.Flags.Selection = Value; break;
        case EControl::Transform: After.Flags.Transform = Value; break;
        case EControl::Edit: After.Flags.Edit = Value; break;
        case EControl::Solo: After.Flags.Solo = Value; break;
        default: return;
        }
        if (EncodeSharedState(Before.Flags, Before.Hidden) == EncodeSharedState(After.Flags, After.Hidden)) return;
        After.HadMetadata = true;
        After.Metadata = EncodeSharedState(After.Flags, After.Hidden);
        // FMetaData is not a UObject in UE 5.8. Actor->Modify() alone cannot
        // capture it. Record explicit before/after metadata and live state.
        if (GUndo && !GIsTransacting)
            GUndo->StoreUndo(A, MakeUnique<FSharedStateChange>(Before, After));
        ApplySharedState(A, After);
    }
    bool HasSolo(UWorld* World) {
        for (const auto& P : States) if (P.Key.IsValid() && P.Key->GetWorld() == World && P.Value.Solo) return true;
        return false;
    }
    void ClearSession() {
        // Shared locks are intentional saved actor state. Clearing the cache
        // must not silently unlock actors or overwrite saved package data.
        States.Empty();
    }
    static const TCHAR* Tip(EControl C) {
        switch (C) {
        case EControl::EditorHidden: return TEXT("Hide in Editor\nToggle editor viewport visibility. Grey means visible; yellow means hidden. Does not change Hidden in Game.");
        case EControl::GameHidden: return TEXT("Hidden in Game\nToggle the actor's Hidden in Game property. Grey means off; red means on. Actors with Edit Lock are skipped.");
        case EControl::Solo: return TEXT("Solo\nIsolate soloed actors in editor viewports. Grey outline means off; white solid means on; the mixed icon means some actors are soloed. Clear all Solo states to restore the normal view. Editor-hidden actors remain hidden. Does not change Hidden in Game.");
        case EControl::Selection: return TEXT("Selection Lock\nPrevent viewport selection. Click the actor in the Outliner to select it. Grey means unlocked; white means locked; a dash means mixed. Does not prevent editing or change the padlock setting.");
        case EControl::Transform: return TEXT("Transform Lock\nProtect position, rotation and scale in standard editor controls. A white closed padlock means Transform Lock. Other properties remain editable. Blueprint-driven transform changes are allowed.");
        default: return TEXT("Edit Lock\nProtect actor and component values in the Level Editor Details panel, standard transforms, and supported actor/component editing commands. Search, category expansion and favorites remain available. Red means Edit Lock. Custom tools and scripts can bypass these protections.");
        }
    }

}

/** Transaction-scoped world-space protection for ordinary editor transforms.
 * No level scan or per-frame restoration. Snapshots exist only during movement.
 */
class FFocusTransformGuard
{
    struct FSnapshot
    {
        TWeakObjectPtr<AActor> Actor;
        TWeakObjectPtr<USceneComponent> Component;
        FTransform World;
        int32 Depth = 0;
        bool bActorRoot = false;
        bool bRecorded = false;
        USceneComponent* Resolve() const
        {
            AActor* Owner = Actor.Get();
            if (!Focus::IsEditorActor(Owner)) return nullptr;
            // Construction scripts can replace the actor's root during a gesture.
            return bActorRoot ? Owner->GetRootComponent() : Component.Get();
        }
    };
    TArray<FSnapshot> Protected;
    TArray<FSnapshot> Sources;
    TSet<TWeakObjectPtr<AActor>> AffectedActors;
    TWeakObjectPtr<UEditorEngine> Editor;
    TWeakObjectPtr<UTransBuffer> Transactions;
    FDelegateHandle BeginHandle, EndHandle, ComponentHandle, MovingHandle;
    FDelegateHandle PrePropertyHandle, PropertyHandle, TransactionHandle, MapHandle;
    bool bRestoring = false;
    bool bRejectMovement = false;
    bool bSortSnapshots = false;

    static bool IsBlueprintExecuting()
    {
        // A native transform function called by a Blueprint can be the top frame.
        // Walk the tracking chain to find the Blueprint bytecode that called it.
        // Do not use bIsRunningConstructionScript as proof of script intent:
        // ExecuteConstruction also sets it during SCS creation and cache replay.
        for (const FFrame* Frame = FFrame::GetThreadLocalTopStackFrame();
            Frame; Frame = Frame->PreviousTrackingFrame)
        {
            if (Frame->Node && !Frame->Node->Script.IsEmpty()) return true;
        }
        return false;
    }
    static bool IsRebuilding(UWorld* World)
    {
        return GIsReconstructingBlueprintInstances || (World && World->bIsRunningConstructionScript);
    }
    bool IsScopeRebuilding() const
    {
        if (GIsReconstructingBlueprintInstances) return true;
        for (const auto& Entry : Sources)
            if (AActor* Actor = Entry.Actor.Get())
                if (IsRebuilding(Actor->GetWorld())) return true;
        return false;
    }
    void AcceptBlueprintTransform(USceneComponent* Component)
    {
        // This event is emitted after the component and its children have updated.
        // Accept only this component's notification, not unrelated protected actors.
        // Each affected descendant receives its own transform notification.
        if (!IsValid(Component) || !Focus::IsEditorActor(Component->GetOwner())) return;
        for (auto* List : { &Sources, &Protected }) for (auto& Entry : *List)
        {
            if (Entry.Resolve() != Component) continue;
            const FTransform Current = Component->GetComponentTransform();
            if (Current.ContainsNaN()) continue;
            Entry.World = Current;
            Entry.Component = Component;
            const int32 NewDepth = Depth(Component);
            bSortSnapshots |= Entry.Depth != NewDepth;
            Entry.Depth = NewDepth;
            // Leave the transaction's original snapshot intact. Updating this
            // movement baseline must not create a new undo transaction.
        }
    }
    static int32 Depth(USceneComponent* Component)
    {
        int32 Result = 0;
        TSet<USceneComponent*> Seen;
        for (auto* Parent = Component ? Component->GetAttachParent() : nullptr;
            Parent && !Seen.Contains(Parent); Parent = Parent->GetAttachParent())
        {
            Seen.Add(Parent);
            ++Result;
        }
        return Result;
    }
    static bool Locked(AActor* Actor)
    {
        return Focus::Get(Actor, Focus::EControl::Transform) || Focus::Get(Actor, Focus::EControl::Edit);
    }
    static bool IsTransformProperty(const FProperty* Property)
    {
        if (!Property) return false;
        const FName Name = Property->GetFName();
        return Name == TEXT("RelativeLocation") || Name == TEXT("RelativeRotation") || Name == TEXT("RelativeScale3D");
    }
    static void AddSnapshot(TArray<FSnapshot>& List, USceneComponent* Component)
    {
        if (!IsValid(Component) || !Focus::IsEditorActor(Component->GetOwner())) return;
        AActor* Owner = Component->GetOwner();
        const bool bRoot = Owner->GetRootComponent() == Component;
        for (const auto& Entry : List)
            if ((bRoot && Entry.bActorRoot && Entry.Actor.Get() == Owner) || Entry.Component.Get() == Component) return;
        FSnapshot Entry;
        Entry.Actor = Owner;
        Entry.Component = Component;
        Entry.World = Component->GetComponentTransform();
        Entry.Depth = Depth(Component);
        Entry.bActorRoot = bRoot;
        List.Add(Entry);
    }
    void RecordSnapshots()
    {
        if (Protected.IsEmpty() || !GEditor || !GEditor->IsTransactionActive() || GIsTransacting) return;
        // Record before inherited movement changes the child's relative transform.
        // Existing transaction ownership and undo annotations remain with Unreal.
        for (auto* List : { &Sources, &Protected }) for (auto& Entry : *List)
        {
            if (Entry.bRecorded) continue;
            if (USceneComponent* Component = Entry.Resolve())
            {
                Entry.bRecorded = true;
                Component->SetFlags(RF_Transactional);
                if (AActor* Actor = Entry.Actor.Get()) Actor->Modify();
                Component->Modify();
            }
        }
    }
    void Capture(UObject& Object)
    {
        if (bRestoring || GIsTransacting || Object.IsTemplate() || IsBlueprintExecuting()) return;
        AActor* Actor = Cast<AActor>(&Object);
        USceneComponent* Component = Cast<USceneComponent>(&Object);
        if (Component) Actor = Component->GetOwner();
        if (!Focus::IsEditorActor(Actor) || IsRebuilding(Actor->GetWorld())) return;
        if (!Component) Component = Actor->GetRootComponent();
        if (!Component) return;
        for (const auto& Entry : Sources)
            if (Entry.Resolve() == Component) { RecordSnapshots(); return; }

        AddSnapshot(Sources, Component);
        if (Locked(Actor)) AddSnapshot(Protected, Component);
        TArray<USceneComponent*> Pending;
        Pending.Add(Component);
        // Native grouping is separate from scene-component attachment.
        if (AGroupActor* Group = Cast<AGroupActor>(Actor))
        {
            TArray<AActor*> Members;
            Group->GetGroupActors(Members, true);
            for (AActor* Member : Members) if (Focus::IsEditorActor(Member))
            {
                AddSnapshot(Sources, Member->GetRootComponent());
                Pending.Add(Member->GetRootComponent());
            }
        }
        TSet<USceneComponent*> Seen;
        while (!Pending.IsEmpty())
        {
            USceneComponent* Current = Pending.Last();
            Pending.RemoveAt(Pending.Num() - 1, 1, EAllowShrinking::No);
            if (!IsValid(Current) || Seen.Contains(Current)) continue;
            Seen.Add(Current);
            AActor* Owner = Current->GetOwner();
            if (!Focus::IsEditorActor(Owner)) continue;
            if (Owner->GetRootComponent() == Current && Locked(Owner)) AddSnapshot(Protected, Current);
            for (USceneComponent* Child : Current->GetAttachChildren()) Pending.Add(Child);
        }
        auto ByDepth = [](const FSnapshot& A, const FSnapshot& B) { return A.Depth < B.Depth; };
        Sources.Sort(ByDepth);
        Protected.Sort(ByDepth);
        for (const auto& Entry : Sources) AffectedActors.Add(Entry.Actor);
        for (const auto& Entry : Protected) AffectedActors.Add(Entry.Actor);
        RecordSnapshots();
    }
    static void Apply(const FSnapshot& Entry)
    {
        if (USceneComponent* Component = Entry.Resolve())
        {
            // Details can write relative values directly after its first transform
            // notification. Refresh the cache before deciding no correction is needed.
            Component->UpdateComponentToWorld();
            if (!Component->GetComponentTransform().Equals(Entry.World, 0.0001))
                Component->SetWorldTransform(Entry.World, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }
    void Restore()
    {
        if (bRestoring || GIsTransacting || Protected.IsEmpty() || !GEditor || !GEditor->IsTransactionActive()) return;
        // Never write into a running Blueprint or a component rebuild. In the
        // latter case retain the baseline for the notification after reconstruction.
        if (IsBlueprintExecuting() || IsScopeRebuilding()) return;
        bRestoring = true;
        if (bSortSnapshots)
        {
            auto ByDepth = [](const FSnapshot& A, const FSnapshot& B) { return A.Depth < B.Depth; };
            Sources.Sort(ByDepth);
            Protected.Sort(ByDepth);
            bSortSnapshots = false;
        }
        if (bRejectMovement) for (const auto& Entry : Sources) Apply(Entry);
        for (const auto& Entry : Protected) if (Locked(Entry.Actor.Get())) Apply(Entry);
        bool bFailed = false;
        for (const auto& Entry : Protected) if (Locked(Entry.Actor.Get()))
            if (USceneComponent* Component = Entry.Resolve())
                bFailed |= !Component->GetComponentTransform().Equals(Entry.World, 0.001);
        if (bFailed && !bRejectMovement)
        {
            // A singular parent scale cannot represent every child's world transform.
            // Reject this gesture instead of silently changing a protected actor.
            bRejectMovement = true;
            for (const auto& Entry : Sources) Apply(Entry);
            for (const auto& Entry : Protected) if (Locked(Entry.Actor.Get())) Apply(Entry);
            FNotificationInfo Info(NSLOCTEXT("Focus", "UnsupportedParentTransform",
                "Focus stopped this transform because it could not preserve a locked actor. Check the parent's scale or unlock the actor."));
            Info.ExpireDuration = 6.f;
            FSlateNotificationManager::Get().AddNotification(Info);
        }
        bRestoring = false;
    }
    void Reset()
    {
        Protected.Reset();
        Sources.Reset();
        AffectedActors.Reset();
        bRejectMovement = false;
        bSortSnapshots = false;
    }
    void OnBegin(UObject& Object) { Capture(Object); }
    void OnEnd(UObject&)
    {
        Restore();
        if (!GEditor || !GEditor->IsTransactionActive()) Reset();
    }
    bool IsAffected(AActor* Actor) const
    {
        return Actor && AffectedActors.Contains(TWeakObjectPtr<AActor>(Actor));
    }
    void OnComponentChanged(USceneComponent* Component, ETeleportType)
    {
        if (bRestoring || GIsTransacting || !Component || !IsAffected(Component->GetOwner())) return;
        if (IsBlueprintExecuting())
        {
            AcceptBlueprintTransform(Component);
            return;
        }
        if (IsRebuilding(Component->GetWorld())) return;
        Restore();
    }
    void OnActorMoving(AActor* Actor) { if (IsAffected(Actor)) Restore(); }
    void OnPreProperty(UObject* Object, const FEditPropertyChain& Chain)
    {
        if (!Object) return;
        for (auto* Node = Chain.GetHead(); Node; Node = Node->GetNextNode())
            if (IsTransformProperty(Node->GetValue())) { Capture(*Object); break; }
    }
    void OnProperty(UObject* Object, FPropertyChangedEvent&)
    {
        AActor* Actor = Cast<AActor>(Object);
        if (auto* Component = Cast<UActorComponent>(Object)) Actor = Component->GetOwner();
        if (IsAffected(Actor)) Restore();
    }
    void OnTransaction(const FTransactionContext&, ETransactionStateEventType Event)
    {
        switch (Event)
        {
        case ETransactionStateEventType::TransactionStarted: RecordSnapshots(); break;
        case ETransactionStateEventType::PreTransactionFinalized: Restore(); break;
        case ETransactionStateEventType::TransactionFinalized:
        case ETransactionStateEventType::TransactionCanceled:
        case ETransactionStateEventType::UndoRedoStarted:
        case ETransactionStateEventType::UndoRedoFinalized: Reset(); break;
        default: break;
        }
    }
public:
    explicit FFocusTransformGuard(UEditorEngine& InEditor) : Editor(&InEditor)
    {
        Transactions = Cast<UTransBuffer>(InEditor.Trans);
        BeginHandle = InEditor.OnBeginObjectMovement().AddRaw(this, &FFocusTransformGuard::OnBegin);
        EndHandle = InEditor.OnEndObjectMovement().AddRaw(this, &FFocusTransformGuard::OnEnd);
        ComponentHandle = InEditor.OnComponentTransformChanged().AddRaw(this, &FFocusTransformGuard::OnComponentChanged);
        MovingHandle = InEditor.OnActorMoving().AddRaw(this, &FFocusTransformGuard::OnActorMoving);
        PrePropertyHandle = FCoreUObjectDelegates::OnPreObjectPropertyChanged.AddRaw(this, &FFocusTransformGuard::OnPreProperty);
        PropertyHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FFocusTransformGuard::OnProperty);
        if (UTransBuffer* Buffer = Transactions.Get())
            TransactionHandle = Buffer->OnTransactionStateChanged().AddRaw(this, &FFocusTransformGuard::OnTransaction);
        MapHandle = FEditorDelegates::OnMapOpened.AddLambda([this](const FString&, bool) { Reset(); });
    }
    ~FFocusTransformGuard()
    {
        FCoreUObjectDelegates::OnPreObjectPropertyChanged.Remove(PrePropertyHandle);
        FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(PropertyHandle);
        FEditorDelegates::OnMapOpened.Remove(MapHandle);
        if (UTransBuffer* Buffer = Transactions.Get()) Buffer->OnTransactionStateChanged().Remove(TransactionHandle);
        if (UEditorEngine* Owner = Editor.Get())
        {
            Owner->OnBeginObjectMovement().Remove(BeginHandle);
            Owner->OnEndObjectMovement().Remove(EndHandle);
            Owner->OnComponentTransformChanged().Remove(ComponentHandle);
            Owner->OnActorMoving().Remove(MovingHandle);
        }
    }
};

/** Per-view isolation; no temporary rewrites of actor visibility to implement Solo. */
class FFocusViewExtension : public FSceneViewExtensionBase
{
public:
    FFocusViewExtension(const FAutoRegister& R) : FSceneViewExtensionBase(R) {}
    virtual void SetupViewFamily(FSceneViewFamily&) override {}
    virtual void BeginRenderViewFamily(FSceneViewFamily&) override {}
    virtual void SetupView(FSceneViewFamily& Family, FSceneView& View) override
    {
        UWorld* World = Family.Scene ? Family.Scene->GetWorld() : nullptr;
        if (!World || World->WorldType != EWorldType::Editor || !Focus::HasSolo(World)) return;
        TSet<FPrimitiveComponentId> Allowed;
        for (const auto& P : Focus::States) if (P.Key.IsValid() && P.Key->GetWorld() == World && P.Value.Solo) {
            P.Key->ForEachComponent<UPrimitiveComponent>(false, [&Allowed](const UPrimitiveComponent* C) { Allowed.Add(C->GetPrimitiveSceneId()); });
        }
        // Preserve restrictions installed by another view extension.
        if (View.ShowOnlyPrimitives.IsSet()) {
            for (auto It = Allowed.CreateIterator(); It; ++It) if (!View.ShowOnlyPrimitives.GetValue().Contains(*It)) It.RemoveCurrent();
        }
        View.ShowOnlyPrimitives = MoveTemp(Allowed);
    }
};

#include "FocusInteractionGuard.h"

/** Resolve scope from live actors, never from filtered/materialized Outliner children. */
// SVG-matched property segment: the solid fill occupies the entire Slate widget.
// A regular SButton applies its own content and styling geometry, which squeezed
// 1.1.1's rectangles into thin, visually misleading marks. This lightweight
// widget owns the pointer interaction without a button's layout insets.
class SFocusPropertySegment final : public SLeafWidget
{
public:
    // Shape is purely presentational. Every state uses the same fixed geometry
    // and independent click/tooltip handling, including unavailable controls.
    enum class EShape : uint8 { Rectangle, Circle, PillLeft, PillMiddle, PillRight };

    SLATE_BEGIN_ARGS(SFocusPropertySegment) {}
        SLATE_ARGUMENT(TFunction<bool()>, Available)
        SLATE_ARGUMENT(TFunction<void()>, Toggle)
        SLATE_ARGUMENT(TFunction<FSlateColor()>, Tint)
        SLATE_ARGUMENT(TFunction<FText()>, Tooltip)
        SLATE_ARGUMENT(EShape, Shape)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args)
    {
        Available = Args._Available;
        Toggle = Args._Toggle;
        Tint = Args._Tint;
        Tooltip = Args._Tooltip;
        Shape = Args._Shape;
        SetToolTipText(TAttribute<FText>::CreateLambda([this] { return Tooltip(); }));
    }

    // The exact allotted rectangle is painted directly. Unlike an SImage,
    // changes in brush style, hover, enabled tint or state color cannot alter
    // the visible geometry. This is also independent of native button padding.
    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override
    {
        static const FSlateColorBrush SolidBrush(FLinearColor::White);
        // Rounded brushes are created once. Drawing a fixed brush in the allotted
        // rectangle ensures changes in state tint never alter hitbox or silhouette.
        // Corner order is Top Left, Top Right, Bottom Right, Bottom Left.
        static const FSlateRoundedBoxBrush CircleBrush(FLinearColor::White, 3.25f, FVector2D(6.5f, 6.5f));
        static const FSlateRoundedBoxBrush PillLeftBrush(FLinearColor::White,
            FVector4(7.5f, 0.f, 0.f, 7.5f), FVector2D(13.f, 15.f));
        static const FSlateRoundedBoxBrush PillMiddleBrush(FLinearColor::White,
            FVector4(0.f, 0.f, 0.f, 0.f), FVector2D(12.f, 15.f));
        static const FSlateRoundedBoxBrush PillRightBrush(FLinearColor::White,
            FVector4(0.f, 7.5f, 7.5f, 0.f), FVector2D(13.f, 15.f));
        const FSlateBrush* Brush = &SolidBrush;
        switch (Shape)
        {
        case EShape::Circle:     Brush = &CircleBrush; break;
        case EShape::PillLeft:   Brush = &PillLeftBrush; break;
        case EShape::PillMiddle: Brush = &PillMiddleBrush; break;
        case EShape::PillRight:  Brush = &PillRightBrush; break;
        default: break;
        }
        FSlateDrawElement::MakeBox(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(),
            Brush, ESlateDrawEffect::None, Tint().GetSpecifiedColor());
        return LayerId;
    }

    virtual FVector2D ComputeDesiredSize(float) const override
    {
        // The enclosing SBox specifies exact per-segment SVG dimensions.
        return FVector2D(1.0f, 1.0f);
    }

    virtual FReply OnMouseButtonDown(const FGeometry&, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton || !Available())
            return FReply::Unhandled();
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }

    virtual FReply OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton || !HasMouseCapture())
            return FReply::Unhandled();
        if (Available() && Geometry.IsUnderLocation(Event.GetScreenSpacePosition()))
            Toggle();
        return FReply::Handled().ReleaseMouseCapture();
    }

private:
    TFunction<bool()> Available;
    TFunction<void()> Toggle;
    TFunction<FSlateColor()> Tint;
    TFunction<FText()> Tooltip;
    EShape Shape = EShape::Rectangle;
};

// Navigation's four visible circles are intentionally small. Hit-testing the
// whole diamond by cardinal direction gives every circle a usable region even
// in Unreal's stock 20-unit Outliner rows, with no invisible overlap into
// neighboring Focus categories or adjacent rows.
class SFocusNavigationDiamond final : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SFocusNavigationDiamond) {}
        SLATE_ARGUMENT(TFunction<bool(int32)>, Available)
        SLATE_ARGUMENT(TFunction<void(int32)>, Toggle)
        SLATE_ARGUMENT(TFunction<FLinearColor(int32)>, Tint)
        SLATE_ARGUMENT(TFunction<FText(int32)>, Tooltip)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args)
    {
        Available = Args._Available;
        Toggle = Args._Toggle;
        Tint = Args._Tint;
        Tooltip = Args._Tooltip;
        SetToolTipText(TAttribute<FText>::CreateLambda([this] { return Tooltip(HoveredChoice); }));
    }

    virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D(18.f, 18.f); }

    virtual int32 OnPaint(const FPaintArgs&, const FGeometry& Geometry,
        const FSlateRect&, FSlateWindowElementList& Elements, int32 Layer,
        const FWidgetStyle&, bool) const override
    {
        static const FSlateRoundedBoxBrush Dot(FLinearColor::White, 3.f, FVector2D(6.f, 6.f));
        // N, E, S, W in the exact same property order as FocusQuick::Definitions().
        static const FVector2D Positions[4] = {
            FVector2D(6.f, 0.f), FVector2D(12.f, 6.f),
            FVector2D(6.f, 12.f), FVector2D(0.f, 6.f)
        };
        for (int32 Choice = 0; Choice < 4; ++Choice)
        {
            FSlateDrawElement::MakeBox(Elements, Layer,
                Geometry.ToPaintGeometry(FVector2D(6.f, 6.f), FSlateLayoutTransform(Positions[Choice])),
                &Dot, ESlateDrawEffect::None, Tint(Choice));
        }
        return Layer;
    }

    virtual void OnMouseEnter(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        SLeafWidget::OnMouseEnter(Geometry, Event);
        HoveredChoice = ChoiceAt(Geometry, Event);
    }

    virtual FReply OnMouseMove(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        HoveredChoice = ChoiceAt(Geometry, Event);
        return HasMouseCapture() ? FReply::Handled() : FReply::Unhandled();
    }

    virtual FReply OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton) return FReply::Unhandled();
        PressedChoice = ChoiceAt(Geometry, Event);
        if (!Available(PressedChoice)) return FReply::Unhandled();
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }

    virtual FReply OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton || !HasMouseCapture())
            return FReply::Unhandled();
        const int32 Choice = ChoiceAt(Geometry, Event);
        if (Geometry.IsUnderLocation(Event.GetScreenSpacePosition()) && Choice == PressedChoice && Available(Choice))
            Toggle(Choice);
        return FReply::Handled().ReleaseMouseCapture();
    }

private:
    static int32 ChoiceAt(const FGeometry& Geometry, const FPointerEvent& Event)
    {
        const FVector2D Position = Geometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
        const FVector2D Offset = Position - FVector2D(9.f, 9.f);
        if (FMath::Abs(Offset.X) > FMath::Abs(Offset.Y)) return Offset.X >= 0.f ? 1 : 3;
        return Offset.Y >= 0.f ? 2 : 0;
    }
    TFunction<bool(int32)> Available;
    TFunction<void(int32)> Toggle;
    TFunction<FLinearColor(int32)> Tint;
    TFunction<FText(int32)> Tooltip;
    int32 HoveredChoice = 0;
    int32 PressedChoice = INDEX_NONE;
};

// Six-triangle Simulation, three-pill Physics Events, and the three-piece
// Network emblem use one light Slate widget per category, not one SButton per
// property. Each vector silhouette covers the whole, fixed canvas but only
// contains its own opaque shape. Native per-property state determines tint;
// geometry and hit regions do not change when a property becomes unavailable.
class SFocusRadialCategory final : public SLeafWidget
{
public:
    enum class EDesign : uint8 { Simulation, PhysicsEvents, Network };
    SLATE_BEGIN_ARGS(SFocusRadialCategory) {}
        SLATE_ARGUMENT(EDesign, Design)
        SLATE_ARGUMENT(TFunction<bool(int32)>, Available)
        SLATE_ARGUMENT(TFunction<void(int32)>, Toggle)
        SLATE_ARGUMENT(TFunction<FLinearColor(int32)>, Tint)
        SLATE_ARGUMENT(TFunction<FText(int32)>, Tooltip)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args)
    {
        Design = Args._Design;
        Available = Args._Available;
        Toggle = Args._Toggle;
        Tint = Args._Tint;
        Tooltip = Args._Tooltip;
        SetToolTipText(TAttribute<FText>::CreateLambda([this]
        {
            return Tooltip(HoveredChoice);
        }));
    }

    static FVector2D SizeFor(EDesign InDesign)
    {
        switch (InDesign)
        {
        case EDesign::Simulation: return FVector2D(18.f, 16.3f);
        case EDesign::PhysicsEvents: return FVector2D(16.f, 16.f);
        case EDesign::Network: return FVector2D(18.f, 15.6f);
        }
        return FVector2D(18.f, 18.f);
    }

    virtual FVector2D ComputeDesiredSize(float) const override { return SizeFor(Design); }

    virtual int32 OnPaint(const FPaintArgs&, const FGeometry& Geometry,
        const FSlateRect&, FSlateWindowElementList& Elements, int32 Layer,
        const FWidgetStyle&, bool) const override
    {
        const TSharedPtr<FSlateVectorImageBrush>* Pieces = nullptr;
        int32 Count = 0;
        switch (Design)
        {
        case EDesign::Simulation: Pieces = Focus::SimulationPieces; Count = 6; break;
        case EDesign::PhysicsEvents: Pieces = Focus::PhysicsEventPieces; Count = 3; break;
        case EDesign::Network: Pieces = Focus::NetworkPieces; Count = 3; break;
        }
        if (!Pieces) return Layer;
        for (int32 Index = 0; Index < Count; ++Index)
        {
            if (!Pieces[Index].IsValid()) continue;
            FSlateDrawElement::MakeBox(Elements, Layer, Geometry.ToPaintGeometry(),
                Pieces[Index].Get(), ESlateDrawEffect::None, Tint(Index));
        }
        return Layer;
    }

    virtual void OnMouseEnter(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        SLeafWidget::OnMouseEnter(Geometry, Event);
        HoveredChoice = ChoiceAt(Geometry, Event);
    }

    virtual FReply OnMouseMove(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        HoveredChoice = ChoiceAt(Geometry, Event);
        return HasMouseCapture() ? FReply::Handled() : FReply::Unhandled();
    }

    virtual FReply OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton) return FReply::Unhandled();
        PressedChoice = ChoiceAt(Geometry, Event);
        if (!Available(PressedChoice)) return FReply::Unhandled();
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }

    virtual FReply OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton || !HasMouseCapture())
            return FReply::Unhandled();
        const int32 ReleasedChoice = ChoiceAt(Geometry, Event);
        if (Geometry.IsUnderLocation(Event.GetScreenSpacePosition()) &&
            ReleasedChoice == PressedChoice && Available(ReleasedChoice))
        {
            Toggle(ReleasedChoice);
        }
        PressedChoice = INDEX_NONE;
        return FReply::Handled().ReleaseMouseCapture();
    }

private:
    int32 ChoiceAt(const FGeometry& Geometry, const FPointerEvent& Event) const
    {
        // Choose the nearest segment center, so invisible hit regions fill the
        // tiny spaces between shapes. No hit target extends beyond the widget.
        // The reference coordinates match the exact source artwork (3x PNG).
        const FVector2D Size = Geometry.GetLocalSize();
        const FVector2D Local = Geometry.AbsoluteToLocal(Event.GetScreenSpacePosition());
        const float X = Size.X > 0.f ? Local.X / Size.X : 0.f;
        const float Y = Size.Y > 0.f ? Local.Y / Size.Y : 0.f;
        if (Design == EDesign::PhysicsEvents)
            return FMath::Clamp(FMath::FloorToInt(Y * 3.f), 0, 2);

        static const FVector2D SimulationCenters[6] = {
            FVector2D(26.f, 7.f), FVector2D(42.f, 15.f),
            FVector2D(42.f, 32.f), FVector2D(26.f, 41.f),
            FVector2D(11.f, 32.f), FVector2D(11.f, 15.f)
        };
        static const FVector2D NetworkCenters[3] = {
            FVector2D(26.f, 15.f), FVector2D(12.f, 36.f),
            FVector2D(40.f, 36.f)
        };
        const FVector2D* Centers = Design == EDesign::Simulation ? SimulationCenters : NetworkCenters;
        const int32 Count = Design == EDesign::Simulation ? 6 : 3;
        const FVector2D Canvas = Design == EDesign::Simulation ? FVector2D(53.f, 48.f) : FVector2D(52.f, 45.f);
        const FVector2D Point(X * Canvas.X, Y * Canvas.Y);
        float SmallestDistance = TNumericLimits<float>::Max();
        int32 Closest = 0;
        for (int32 Index = 0; Index < Count; ++Index)
        {
            const float Distance = (Point - Centers[Index]).SizeSquared();
            if (Distance < SmallestDistance)
            {
                SmallestDistance = Distance;
                Closest = Index;
            }
        }
        return Closest;
    }

    EDesign Design = EDesign::Simulation;
    TFunction<bool(int32)> Available;
    TFunction<void(int32)> Toggle;
    TFunction<FLinearColor(int32)> Tint;
    TFunction<FText(int32)> Tooltip;
    int32 HoveredChoice = 0;
    int32 PressedChoice = INDEX_NONE;
};

// A vertical three-way selector uses the same visual footprint as other Focus
// category icons. It always fits in a 20-unit Outliner row, and is centered by
// the existing category-level VAlign when Stem increases the row height.
class SFocusMobilityColumn final : public SLeafWidget
{
public:
    // Match the paint geometry and hit-testing to the same three 6-unit bands.
    // The entire 14-unit width is interactive, including the visible gutters.
    static constexpr float Width = 14.f;
    static constexpr float Height = 18.f;
    static constexpr float BandHeight = Height / 3.f;

    SLATE_BEGIN_ARGS(SFocusMobilityColumn) {}
        SLATE_ARGUMENT(TFunction<int32()>, State)
        SLATE_ARGUMENT(TFunction<int32()>, Value)
        SLATE_ARGUMENT(TFunction<void(int32)>, Apply)
        SLATE_ARGUMENT(TFunction<FText(int32)>, Tooltip)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args)
    {
        State = Args._State;
        Value = Args._Value;
        Apply = Args._Apply;
        Tooltip = Args._Tooltip;
        SetToolTipText(TAttribute<FText>::CreateLambda([this] { return Tooltip(HoveredChoice); }));
    }

    virtual FVector2D ComputeDesiredSize(float) const override { return FVector2D(Width, Height); }

    virtual int32 OnPaint(const FPaintArgs&, const FGeometry& Geometry,
        const FSlateRect&, FSlateWindowElementList& Elements, int32 Layer,
        const FWidgetStyle&, bool) const override
    {
        static const FSlateRoundedBoxBrush Outline(FLinearColor::White, 5.5f, FVector2D(14.f, 18.f));
        static const FSlateRoundedBoxBrush Top(FLinearColor::White,
            FVector4(5.5f, 5.5f, 0.f, 0.f), FVector2D(14.f, 5.5f));
        static const FSlateColorBrush Middle(FLinearColor::White);
        static const FSlateRoundedBoxBrush Bottom(FLinearColor::White,
            FVector4(0.f, 0.f, 5.5f, 5.5f), FVector2D(14.f, 5.5f));
        static const FSlateRoundedBoxBrush Inner(FLinearColor::White, 4.5f, FVector2D(12.f, 16.f));
        static const FSlateColorBrush DividerBrush(FLinearColor::White);
        const UFocusSettings* Colors = GetDefault<UFocusSettings>();
        const int32 CurrentState = State();
        const int32 CurrentValue = Value();
        auto PaintPart = [&](const FSlateBrush* Brush, float X, float Y, float W, float H,
                             const FLinearColor& Color, int32 LayerOffset = 0)
        {
            FSlateDrawElement::MakeBox(Elements, Layer + LayerOffset,
                Geometry.ToPaintGeometry(FVector2D(W, H), FSlateLayoutTransform(FVector2D(X, Y))),
                Brush, ESlateDrawEffect::None, Color);
        };
        if (CurrentState == 2)
        {
            // Yellow is reserved for the outer outline. None of the three
            // exclusive mobility choices appears selected in a mixed hierarchy.
            PaintPart(&Outline, 0.f, 0.f, Width, Height, Colors->MixedColor);
            PaintPart(&Inner, 1.f, 1.f, Width - 2.f, Height - 2.f, Colors->DisabledColor, 1);
            // The yellow outline already communicates a mixed value. Keep the
            // internal dividers very dark so the widget preserves its 3-part
            // structure without adding visual noise at normal Outliner scale.
            // Inset the dividers by two units so they cannot cover the outline.
            static const FLinearColor MixedDividerColor =
                FLinearColor::FromSRGBColor(FColor(0x24, 0x24, 0x24));
            constexpr float DividerThickness = 2.f;
            constexpr float DividerInset = 2.f;
            PaintPart(&DividerBrush, DividerInset, BandHeight - DividerThickness * .5f,
                Width - DividerInset * 2.f, DividerThickness, MixedDividerColor, 2);
            PaintPart(&DividerBrush, DividerInset, BandHeight * 2.f - DividerThickness * .5f,
                Width - DividerInset * 2.f, DividerThickness, MixedDividerColor, 2);
            return Layer + 2;
        }
        const auto ChoiceColor = [&](int32 Choice) -> FLinearColor
        {
            if (CurrentState < 0) return Colors->NotApplicableColor;
            return Choice == CurrentValue ? Colors->EnabledColor : Colors->DisabledColor;
        };
        PaintPart(&Top, 0.f, 0.f, 14.f, 5.5f, ChoiceColor(0));
        PaintPart(&Middle, 0.f, 6.5f, 14.f, 5.f, ChoiceColor(1));
        PaintPart(&Bottom, 0.f, 12.5f, 14.f, 5.5f, ChoiceColor(2));
        return Layer;
    }

    virtual void OnMouseEnter(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        SLeafWidget::OnMouseEnter(Geometry, Event);
        HoveredChoice = ChoiceAt(Geometry, Event);
    }

    virtual FReply OnMouseMove(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        HoveredChoice = ChoiceAt(Geometry, Event);
        return HasMouseCapture() ? FReply::Handled() : FReply::Unhandled();
    }

    virtual FReply OnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton || State() < 0) return FReply::Unhandled();
        PressedChoice = ChoiceAt(Geometry, Event);
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }

    virtual FReply OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton || !HasMouseCapture())
            return FReply::Unhandled();
        const int32 Choice = ChoiceAt(Geometry, Event);
        // Only a press/release in the same six-unit band applies a change.
        // Dragging to another band or out of the widget never changes Mobility.
        if (Geometry.IsUnderLocation(Event.GetScreenSpacePosition()) && Choice == PressedChoice && State() >= 0)
            Apply(Choice);
        PressedChoice = INDEX_NONE;
        return FReply::Handled().ReleaseMouseCapture();
    }

private:
    static int32 ChoiceAt(const FGeometry& Geometry, const FPointerEvent& Event)
    {
        const float Y = Geometry.AbsoluteToLocal(Event.GetScreenSpacePosition()).Y;
        // 0..6 = Static, 6..12 = Stationary, 12..18 = Movable.
        // The gaps are deliberately included in the nearest band's hit area.
        return Y < BandHeight ? 0 : Y < BandHeight * 2.f ? 1 : 2;
    }
    TFunction<int32()> State;
    TFunction<int32()> Value;
    TFunction<void(int32)> Apply;
    TFunction<FText(int32)> Tooltip;
    int32 HoveredChoice = 0;
    int32 PressedChoice = INDEX_NONE;
};

// The supplied 15-17 unit SVG artwork was placed slightly larger than the
// standard 16-unit Focus icons in the Photoshop reference. Scale geometric
// blocks to match that optical weight while retaining independent hit regions.
// The LOD number is intentionally shorter than its width, to fit typical
// Scene Outliner row heights without clipping.
static constexpr float FocusLightingScale = 1.10f;
static constexpr float FocusWorldScale = 1.23f;
static constexpr float FocusLODWidthScale = 1.23f;
static constexpr float FocusLODHeightScale = 1.00f; // Keep LOD under 18 units for native Outliner rows.

// Standalone editor for the LOD number. A click commits one increment after
// the double-click window; a double click cancels that action and opens text
// editing instead. No package edit or undo transaction is performed for the
// first click of a double click.
class SFocusLODNumber : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SFocusLODNumber) {}
        SLATE_ARGUMENT(TFunction<TOptional<int32>()>, Read)
        SLATE_ARGUMENT(TFunction<int32()>, IncrementBase)
        SLATE_ARGUMENT(TFunction<bool()>, Available)
        SLATE_ARGUMENT(TFunction<void(int32)>, Apply)
        SLATE_ARGUMENT(TFunction<FSlateColor()>, Color)
        SLATE_ARGUMENT(TFunction<FText()>, Tooltip)
    SLATE_END_ARGS()

    void Construct(const FArguments& Args)
    {
        Read = Args._Read;
        IncrementBase = Args._IncrementBase;
        Available = Args._Available;
        Apply = Args._Apply;
        Color = Args._Color;
        Tooltip = Args._Tooltip;
        // The regular number stays centered in the SVG-sized LOD icon. An
        // SEditableTextBox cannot display even a short number inside this tiny
        // footprint once Slate reserves its own padding. Use a centered popup
        // editor instead, without changing the normal Outliner column width.
        ChildSlot
        [SAssignNew(EditAnchor, SMenuAnchor)
            .Placement(MenuPlacement_Center)
            .UseApplicationMenuStack(true)
            .MenuContent(
                SNew(SBox).WidthOverride(64.0f)
                [SAssignNew(EditBox, SEditableTextBox)
                    .Font(FAppStyle::GetFontStyle(TEXT("NormalFont")))
                    .Justification(ETextJustify::Center)
                    .SelectAllTextWhenFocused(true)
                    .ClearKeyboardFocusOnCommit(true)
                    .OnTextCommitted_Lambda([this](const FText& Text, ETextCommit::Type Type)
                    {
                        if (!bEditing)
                            return;

                        bEditing = false;
                        bSuppressNextUp = false;
                        if (Type != ETextCommit::OnCleared)
                        {
                            const FString Entered = Text.ToString().TrimStartAndEnd();
                            if (!Entered.IsEmpty() && Entered.IsNumeric())
                                Apply(FMath::Clamp(FCString::Atoi(*Entered), 0, 255));
                        }
                        if (EditAnchor.IsValid() && EditAnchor->IsOpen())
                            EditAnchor->SetIsOpen(false, false);
                    })]
            )
            .OnMenuOpenChanged_Lambda([this](bool bIsOpen)
            {
                if (!bIsOpen)
                {
                    bEditing = false;
                    bSuppressNextUp = false;
                }
            })
            [SNew(SBox).WidthOverride(14.87f * FocusLODWidthScale).HeightOverride(14.47f * FocusLODHeightScale)
                [SNew(SOverlay)
                 + SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Fill)
                    .Padding(FMargin(0.6f, 0.0f, 0.6f, 1.3f))
                    [SNew(SScaleBox)
                     .Stretch(EStretch::ScaleToFit)
                     .StretchDirection(EStretchDirection::DownOnly)
                     .HAlign(HAlign_Center).VAlign(VAlign_Center)
                     [SNew(STextBlock)
                      .Text_Lambda([this]
                      {
                          const TOptional<int32> Value = Read();
                          return FText::FromString(Value.IsSet() ? FString::FromInt(Value.GetValue()) : TEXT("-"));
                      })
                      .Font_Lambda([]
                      {
                          FSlateFontInfo Font = FAppStyle::GetFontStyle(TEXT("NormalFont"));
                          Font.Size = 18;
                          return Font;
                      })
                      .ColorAndOpacity_Lambda([this] { return Color(); })]]]
            ]
        ];
        SetToolTipText(TAttribute<FText>::CreateLambda([this] {
            return FText::FromString(Tooltip().ToString() +
                TEXT("\nLeft-click: +1. Alt + left-click: -1. Mouse wheel: +/-1. Double-click: type a number."));
        }));
    }

    virtual FReply OnMouseButtonDown(const FGeometry&, const FPointerEvent& Event) override
    {
        // Only capture left-clicks. Right-click belongs to the native Outliner
        // context menu and must never be consumed by the LOD quick control.
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton || bEditing || !Available())
            return FReply::Unhandled();
        return FReply::Handled().CaptureMouse(SharedThis(this));
    }

    virtual FReply OnMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton || !HasMouseCapture())
            return FReply::Unhandled();
        FReply Reply = FReply::Handled().ReleaseMouseCapture();
        if (!bEditing && !bSuppressNextUp && Geometry.IsUnderLocation(Event.GetScreenSpacePosition()))
        {
            if (Event.IsAltDown())
            {
                // Alt + left-click decrements immediately, without scheduling
                // the ordinary delayed increment. This keeps right-click free
                // for Unreal's normal Scene Outliner context-menu behavior.
                CancelPendingIncrement();
                if (Available())
                    Apply(FMath::Clamp(IncrementBase() - 1, 0, 255));
                return Reply;
            }
            bPendingIncrement = true;
            if (PendingTimer.IsValid())
            {
                UnRegisterActiveTimer(PendingTimer.ToSharedRef());
                PendingTimer.Reset();
            }
            // Delay is necessary: Slate reports the first click of a double-click
            // as a normal release. Applying immediately would make a double-click
            // change the asset before the user can type a replacement value.
            PendingTimer = RegisterActiveTimer(0.50f,
                FWidgetActiveTimerDelegate::CreateSP(this, &SFocusLODNumber::CompleteSingleClick));
        }
        bSuppressNextUp = false;
        return Reply;
    }

    virtual FReply OnMouseButtonDoubleClick(const FGeometry&, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton || !Available())
            return FReply::Unhandled();
        if (Event.IsAltDown())
        {
            // Alt is exclusively the decrement modifier. A rapid pair of
            // Alt-clicks must not inadvertently enter the numeric editor.
            CancelPendingIncrement();
            return FReply::Handled().ReleaseMouseCapture();
        }
        CancelPendingIncrement();
        bSuppressNextUp = true;
        bEditing = true;
        EditBox->SetText(FText::AsNumber(IncrementBase()));
        EditAnchor->SetIsOpen(true, true);
        return FReply::Handled().ReleaseMouseCapture()
            .SetUserFocus(EditBox.ToSharedRef(), EFocusCause::SetDirectly);
    }

    virtual FReply OnMouseWheel(const FGeometry&, const FPointerEvent& Event) override
    {
        if (bEditing || !Available() || FMath::IsNearlyZero(Event.GetWheelDelta()))
            return FReply::Unhandled();
        CancelPendingIncrement();
        const int32 Step = Event.GetWheelDelta() > 0.0f ? 1 : -1;
        Apply(FMath::Clamp(IncrementBase() + Step, 0, 255));
        return FReply::Handled();
    }

private:
    void CancelPendingIncrement()
    {
        bPendingIncrement = false;
        if (PendingTimer.IsValid())
        {
            UnRegisterActiveTimer(PendingTimer.ToSharedRef());
            PendingTimer.Reset();
        }
    }

    EActiveTimerReturnType CompleteSingleClick(double, float)
    {
        PendingTimer.Reset();
        if (bPendingIncrement && !bEditing)
        {
            if (Available())
            {
                Apply(FMath::Clamp(IncrementBase() + 1, 0, 255));
            }
        }
        bPendingIncrement = false;
        return EActiveTimerReturnType::Stop;
    }

    TFunction<TOptional<int32>()> Read;
    TFunction<int32()> IncrementBase;
    TFunction<bool()> Available;
    TFunction<void(int32)> Apply;
    TFunction<FSlateColor()> Color;
    TFunction<FText()> Tooltip;
    TSharedPtr<SMenuAnchor> EditAnchor;
    TSharedPtr<SEditableTextBox> EditBox;
    TSharedPtr<FActiveTimerHandle> PendingTimer;
    bool bEditing = false;
    bool bPendingIncrement = false;
    bool bSuppressNextUp = false;
};

class FFocusColumn;
static TArray<TWeakPtr<FFocusColumn>> ActiveFocusColumns;
class FFocusColumn : public ISceneOutlinerColumn
{
    struct FRowState
    {
        TWeakPtr<ISceneOutlinerTreeItem> Item;
        int32 Total = 0;
        int32 Enabled[6] = {};
        TArray<int32> QuickTotal;
        TArray<int32> QuickFirst;
        TArray<bool> QuickMixed;
        TArray<bool> QuickInvalidLOD;
        TArray<int32> QuickMinAvailableLOD;
        int32 QuickState(int32 Index) const
        {
            if (!QuickTotal.IsValidIndex(Index) || QuickTotal[Index] == 0) return -1;
            if (QuickMixed[Index]) return 2;
            return QuickFirst[Index] ? 1 : 0;
        }
        int32 State(Focus::EControl C) const
        {
            const int32 Count = Enabled[static_cast<int32>(C)];
            return Count == 0 ? 0 : Count == Total ? 1 : 2;
        }
    };
    TArray<TWeakPtr<FRowState>> Rows;
    double NextRefresh = 0.0;

    static UWorld* EditorWorld()
    {
        return GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
    }
    static TArray<TWeakObjectPtr<AActor>> LoadedActors()
    {
        TArray<TWeakObjectPtr<AActor>> Result;
        UWorld* World = EditorWorld();
        if (!World || World->WorldType != EWorldType::Editor) return Result;
        for (TActorIterator<AActor> It(World); It; ++It)
            if (Focus::IsEditorActor(*It)) Result.Add(*It);
        return Result;
    }
    static void Gather(const FSceneOutlinerTreeItemPtr& Item,
        const TArray<TWeakObjectPtr<AActor>>& Loaded, TSet<TWeakObjectPtr<AActor>>& Actors)
    {
        if (!Item) return;
        TArray<AActor*> Pending;
        if (const FActorTreeItem* ActorItem = Item->CastTo<FActorTreeItem>())
            Pending.Add(ActorItem->Actor.Get());
        else if (const FFolderTreeItem* FolderItem = Item->CastTo<FFolderTreeItem>())
        {
            const FFolder Folder = FolderItem->GetFolder();
            const FString Prefix = Folder.GetPath().ToString() + TEXT("/");
            for (const auto& Weak : Loaded) if (AActor* Actor = Weak.Get())
            {
                const FFolder ActorFolder = Actor->GetFolder();
                // Identical paths inside different level-instance roots are different folders.
                if (ActorFolder.GetRootObject() == Folder.GetRootObject() &&
                    (ActorFolder.GetPath() == Folder.GetPath() ||
                        ActorFolder.GetPath().ToString().StartsWith(Prefix, ESearchCase::IgnoreCase)))
                    Pending.Add(Actor);
            }
        }
        while (!Pending.IsEmpty())
        {
            AActor* Actor = Pending.Last();
            Pending.RemoveAt(Pending.Num() - 1, 1, EAllowShrinking::No);
            if (!Focus::IsEditorActor(Actor) || Actor->GetWorld() != EditorWorld()) continue;
            const TWeakObjectPtr<AActor> Key(Actor);
            if (Actors.Contains(Key)) continue;
            Actors.Add(Key);
            // One level at a time plus the visited set handles overlaps and prevents cycles.
            TArray<AActor*> Attached;
            Actor->GetAttachedActors(Attached);
            Pending.Append(Attached);
            if (AGroupActor* Group = Cast<AGroupActor>(Actor))
            {
                for (const auto& Member : Group->GroupActors) Pending.Add(Member.Get());
                for (const auto& SubGroup : Group->SubGroups) Pending.Add(SubGroup.Get());
            }
        }
    }
    static TSet<TWeakObjectPtr<AActor>> ActionActors(const TWeakPtr<ISceneOutlinerTreeItem>& WeakItem)
    {
        const auto Loaded = LoadedActors();
        TSet<TWeakObjectPtr<AActor>> Result;
        const auto Item = WeakItem.Pin();
        if (Item) if (auto Outliner = Item->WeakSceneOutliner.Pin())
        {
            const auto& Tree = Outliner->GetTree();
            if (Tree.IsItemSelected(Item.ToSharedRef()))
            {
                for (const auto& Selected : Tree.GetSelectedItems()) Gather(Selected, Loaded, Result);
                return Result;
            }
        }
        Gather(Item, Loaded, Result);
        return Result;
    }
    static void RefreshRow(FRowState& Row, const TArray<TWeakObjectPtr<AActor>>& Loaded)
    {
        TSet<TWeakObjectPtr<AActor>> Actors;
        Gather(Row.Item.Pin(), Loaded, Actors);
        Row.Total = Actors.Num();
        for (int32 Index = 0; Index < 6; ++Index)
        {
            Row.Enabled[Index] = 0;
            for (const auto& Actor : Actors)
                if (Focus::Get(Actor.Get(), static_cast<Focus::EControl>(Index))) ++Row.Enabled[Index];
        }
        const auto& Defs = FocusQuick::Definitions();
        // This is a full snapshot, not an incremental tally. SetNumZeroed()
        // only zero-initializes *new* elements: when the arrays already have
        // Defs.Num() elements, it preserves their previous counts/values.
        // That made a changed value (e.g. Forced LOD 0 -> 1) report Mixed
        // forever, and a formerly eligible property appear applicable after
        // switching to an actor without it. Init() resets every element.
        Row.QuickTotal.Init(0, Defs.Num());
        Row.QuickFirst.Init(0, Defs.Num());
        Row.QuickMixed.Init(false, Defs.Num());
        Row.QuickInvalidLOD.Init(false, Defs.Num());
        Row.QuickMinAvailableLOD.Init(INDEX_NONE, Defs.Num());
        for (const auto& WeakActor : Actors)
        {
            AActor* Actor = WeakActor.Get();
            if (!Actor) continue;
            TArray<UObject*> TargetsByScope[7];
            bool bFetched[7] = {};
            for (int32 Index = 0; Index < Defs.Num(); ++Index)
            {
                const auto& Def = Defs[Index];
                if (!FocusQuick::CategoryVisible(Def.Category)) continue;
                const int32 Scope = static_cast<int32>(Def.Scope);
                if (!bFetched[Scope])
                {
                    FocusQuick::GetTargets(Actor, Def, TargetsByScope[Scope]);
                    bFetched[Scope] = true;
                }
                for (UObject* Target : TargetsByScope[Scope])
                {
                    int32 Value = 0;
                    if (!FocusQuick::Read(Target, Def, Value)) continue;
                    if (Def.Kind == FocusQuick::EKind::ForcedLOD)
                    {
                        const int32 AvailableLODCount = FocusQuick::GetAvailableLODCount(Target);
                        if (AvailableLODCount >= 0)
                        {
                            if (Row.QuickMinAvailableLOD[Index] == INDEX_NONE)
                                Row.QuickMinAvailableLOD[Index] = AvailableLODCount;
                            else
                                Row.QuickMinAvailableLOD[Index] = FMath::Min(Row.QuickMinAvailableLOD[Index], AvailableLODCount);
                            Row.QuickInvalidLOD[Index] |= Value > AvailableLODCount;
                        }
                    }
                    if (Row.QuickTotal[Index] == 0) Row.QuickFirst[Index] = Value;
                    else if (Row.QuickFirst[Index] != Value) Row.QuickMixed[Index] = true;
                    ++Row.QuickTotal[Index];
                }
            }
        }
    }
    static FText Tooltip(const FRowState& Row, Focus::EControl C)
    {
        const FString StateText = Row.State(C) == 2 ? TEXT("Mixed") : Row.State(C) == 1 ? TEXT("On") : TEXT("Off");
        return FText::FromString(FString(Focus::Tip(C)) +
            TEXT("\n\nIncludes attached descendants, nested folders and native group members. If this row is selected, applies to all selected rows and their descendants.") +
            TEXT("\nClick a mixed or off state to enable it for the affected actors; click a fully on state to disable it.") +
            TEXT("\nLoaded actors only. Actors added later do not inherit these settings. Save All to retain changes.") +
            FString::Printf(TEXT("\nThis row: %s (%d of %d loaded actors on)."), *StateText,
                Row.Enabled[static_cast<int32>(C)], Row.Total));
    }
    static TSharedRef<SWidget> Control(const TSharedRef<FRowState>& Row, Focus::EControl C)
    {
        using namespace Focus;
        const bool Lock = C == EControl::Selection || C == EControl::Transform || C == EControl::Edit;
        TSharedRef<SWidget> Visual = SNew(STextBlock).Text_Lambda([Row, C] {
            const int32 S = Row->State(C);
            return FText::FromString(S == 2 ? TEXT("-") : C == EControl::Solo ? (S == 1 ? TEXT("[S]") : TEXT("(S)")) : TEXT("●"));
            }).ColorAndOpacity_Lambda([Row, C] {
                if (Row->State(C) == 0) return FSlateColor(GetDefault<UFocusSettings>()->DisabledColor);
                if (Row->State(C) == 2 || C == EControl::Solo) return FSlateColor(GetDefault<UFocusSettings>()->EnabledColor);
                return FSlateColor(C == EControl::GameHidden ? GetDefault<UFocusSettings>()->AlertColor : GetDefault<UFocusSettings>()->MixedColor);
                });
            if (C == EControl::EditorHidden || C == EControl::GameHidden)
            {
                // Geometry, rather than a font glyph, keeps the dot optically centered.
                static const FSlateRoundedBoxBrush DotBrush(FLinearColor::White, 3.f, FVector2D(6.f, 6.f));
                Visual = SNew(SBox).WidthOverride(6.f)
                    .HeightOverride_Lambda([Row, C] { return FOptionalSize(Row->State(C) == 2 ? 2.f : 6.f); })
                    [SNew(SImage).Image(&DotBrush).ColorAndOpacity_Lambda([Row, C] {
                    const int32 State = Row->State(C);
                    if (State == 0) return FSlateColor(GetDefault<UFocusSettings>()->DisabledColor);
                    if (State == 2) return FSlateColor(GetDefault<UFocusSettings>()->EnabledColor);
                    return FSlateColor(C == EControl::GameHidden ? GetDefault<UFocusSettings>()->AlertColor : GetDefault<UFocusSettings>()->MixedColor);
                        })];
            }
            if (C == EControl::Solo && SoloOutline.IsValid() && SoloSolid.IsValid() && SoloMixed.IsValid())
            {
                const auto Outline = SoloOutline;
                const auto Solid = SoloSolid;
                const auto Mixed = SoloMixed;
                Visual = SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
                    [SNew(SBox).WidthOverride(16.f).HeightOverride(16.f)
                    [SNew(SImage).Image_Lambda([Row, Outline, Solid, Mixed]() -> const FSlateBrush* {
                    const int32 State = Row->State(EControl::Solo);
                    return State == 2 ? Mixed.Get() : State == 1 ? Solid.Get() : Outline.Get();
                        }).ColorAndOpacity_Lambda([Row] {
                            return FSlateColor(Row->State(EControl::Solo) == 0 ?
                                GetDefault<UFocusSettings>()->DisabledColor : GetDefault<UFocusSettings>()->EnabledColor);
                            })]
                    ];
            }
            if (Lock) Visual = SNew(SOverlay)
                + SOverlay::Slot()[SNew(SImage).Image_Lambda([Row, C] {
                return FAppStyle::GetBrush(Row->State(C) == 0 ? TEXT("Icons.Unlock") : TEXT("Icons.Lock"));
                    }).ColorAndOpacity_Lambda([Row, C] {
                        if (Row->State(C) == 0) return FSlateColor(GetDefault<UFocusSettings>()->DisabledColor);
                        if (Row->State(C) == 2) return FSlateColor(GetDefault<UFocusSettings>()->EnabledColor);
                        return FSlateColor(C == EControl::Selection ? GetDefault<UFocusSettings>()->EnabledColor : C == EControl::Edit ? GetDefault<UFocusSettings>()->AlertColor : GetDefault<UFocusSettings>()->MixedColor);
                        })]
                + SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom)
                    [SNew(STextBlock).Text(FText::FromString(TEXT("-")))
                    .Visibility_Lambda([Row, C] { return Row->State(C) == 2 ? EVisibility::HitTestInvisible : EVisibility::Collapsed; })];
                if (C == EControl::Selection && SelectionUnlocked.IsValid() && SelectionLocked.IsValid())
                {
                    const auto Unlocked = SelectionUnlocked;
                    const auto Locked = SelectionLocked;
                    Visual = SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)
                        [SNew(SBox).WidthOverride(16.f).HeightOverride(16.f)
                        [SNew(SOverlay)
                        + SOverlay::Slot()[SNew(SImage).Image_Lambda([Row, Unlocked, Locked]() -> const FSlateBrush* {
                        return Row->State(EControl::Selection) == 0 ? Unlocked.Get() : Locked.Get();
                            }).ColorAndOpacity_Lambda([Row] {
                                return FSlateColor(Row->State(EControl::Selection) == 0 ? GetDefault<UFocusSettings>()->DisabledColor : Row->State(EControl::Selection) == 2 ? GetDefault<UFocusSettings>()->MixedColor : GetDefault<UFocusSettings>()->EnabledColor);
                                })]
                        + SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom)
                        [SNew(STextBlock).Text(FText::FromString(TEXT("-")))
                        .Visibility_Lambda([Row] {
                        return Row->State(EControl::Selection) == 2 ? EVisibility::HitTestInvisible : EVisibility::Collapsed;
                            })]
                        ]
                        ];
                }
                return SNew(SBox).WidthOverride((C == EControl::EditorHidden || C == EControl::GameHidden) ? 14.f : 20.f)
                    .HeightOverride((C == EControl::EditorHidden || C == EControl::GameHidden) ? 10.f : 20.f)
                    [SNew(SButton).ButtonStyle(FAppStyle::Get(), "SimpleButton").ContentPadding(0)
                    .HAlign(HAlign_Center).VAlign(VAlign_Center)
                    .ToolTipText_Lambda([Row, C] { return Tooltip(*Row, C); })
                    // Keep bulk actions available when only part of their scope is protected.
                    .IsEnabled_Lambda([Row] { return Row->Total > 0; })
                    .OnClicked_Lambda([Row, C] {
                    const auto Actors = ActionActors(Row->Item);
                    bool AllEnabled = true;
                    int32 Editable = 0;
                    int32 Skipped = 0;
                    for (const auto& Weak : Actors) if (AActor* Actor = Weak.Get())
                    {
                        AllEnabled &= Get(Actor, C);
                        if ((C == EControl::GameHidden || C == EControl::Transform) && Get(Actor, EControl::Edit)) ++Skipped;
                        else ++Editable;
                    }
                    const bool Value = !AllEnabled;
                    TUniquePtr<FScopedTransaction> Transaction;
                    if (Editable > 0)
                        Transaction = MakeUnique<FScopedTransaction>(NSLOCTEXT("Focus", "SharedControl", "Focus: Change actor settings"));
                    for (const auto& Weak : Actors) if (AActor* Actor = Weak.Get())
                    {
                        if ((C == EControl::GameHidden || C == EControl::Transform) && Get(Actor, EControl::Edit)) continue;
                        if (Get(Actor, C) != Value) Set(Actor, C, Value);
                    }
                    Transaction.Reset();
                    if (Skipped > 0)
                    {
                        FNotificationInfo Info(FText::Format(NSLOCTEXT("Focus", "SkippedLocked",
                            "Focus skipped {0} edit-locked actor(s). Unlock them to change this setting."), FText::AsNumber(Skipped)));
                        Info.ExpireDuration = 5.f;
                        FSlateNotificationManager::Get().AddNotification(Info);
                    }
                    RefreshRow(*Row, LoadedActors());
                    if (GEditor) GEditor->RedrawLevelEditingViewports();
                    return FReply::Handled();
                        })[Visual]];
    }
    static FLinearColor QuickColor(const FRowState& Row, int32 Index)
    {
        const auto* Settings = GetDefault<UFocusSettings>();
        const int32 State = Row.QuickState(Index);
        if (State < 0) return Settings->NotApplicableColor;
        if (State == 2) return Settings->MixedColor;
        if (FocusQuick::Definitions()[Index].Kind == FocusQuick::EKind::ForcedLOD
            && Row.QuickInvalidLOD.IsValidIndex(Index) && Row.QuickInvalidLOD[Index])
            return Settings->AlertColor;
        if (FocusQuick::Definitions()[Index].Kind == FocusQuick::EKind::Mobility) return Settings->EnabledColor;
        return State == 1 ? Settings->EnabledColor : Settings->DisabledColor;
    }
    static FText QuickLabel(const FRowState& Row, int32 Index)
    {
        const auto& Def = FocusQuick::Definitions()[Index];
        const int32 State = Row.QuickState(Index);
        if (Def.Kind == FocusQuick::EKind::ForcedLOD)
            return FText::FromString(State == 2 ? TEXT("-") : State < 0 ? TEXT("L") : FString::FromInt(Row.QuickFirst[Index]));
        if (Def.Kind == FocusQuick::EKind::Mobility)
        {
            if (State == 2) return FText::FromString(TEXT("-"));
            if (State < 0) return FText::FromString(TEXT("M"));
            const int32 Value = Row.QuickFirst[Index];
            return FText::FromString(Value == 0 ? TEXT("S") : Value == 1 ? TEXT("T") : TEXT("M"));
        }
        return FText::FromString(Def.Symbol);
    }
    static const TCHAR* QuickDescription(int32 Index)
    {
        // Match FocusQuick::Definitions() order. Keep explanations about the
        // native property separate from per-row state and click instructions.
        switch (Index)
        {
        case 0: return TEXT("Simulates rigid-body physics on eligible primitive components. Dynamic physics can move the component under forces and collisions. Collision and a compatible mobility setting may be required.");
        case 1: return TEXT("Allows world gravity to affect a physics-simulated primitive component. This does not start physics simulation by itself.");
        case 2: return TEXT("Prevents radial impulse events from applying an instantaneous velocity change to this primitive component.");
        case 3: return TEXT("Prevents continuous radial force effects from acting on this primitive component. Separate from radial impulses.");
        case 4: return TEXT("Allows damage events to impart an impulse to a primitive component when supported by its physics setup. This does not enable damage or physics simulation by itself.");
        case 5: return TEXT("Enables the Actor async physics tick where supported. Physics-tick scheduling is distinct from the regular game-thread tick.");
        case 6: return TEXT("Enables rigid-body collision hit notifications on eligible primitive components. Collision settings must also allow the relevant contacts.");
        case 7: return TEXT("Enables overlap event generation for a primitive component. Both objects and their collision responses must allow overlap events.");
        case 8: return TEXT("Enables physics wake/sleep notifications for supported primitive components. These events depend on the component participating in physics simulation.");
        case 9: return TEXT("Controls whether this primitive or light component uses Lighting Channel 0. Lighting channels filter interactions with lights set to the same channel.");
        case 10: return TEXT("Controls whether this primitive or light component uses Lighting Channel 1. A light and the receiving primitive must share a channel to interact.");
        case 11: return TEXT("Controls whether this primitive or light component uses Lighting Channel 2. Lighting channels do not replace the normal light visibility and mobility rules.");
        case 12: return TEXT("Controls whether the eligible primitive or light component casts shadows. The final result also depends on the light, material, and rendering settings.");
        case 13: return TEXT("Limits supported Actors to the main world instead of other world contexts where this property is respected.");
        case 14: return TEXT("Marks the Actor as editor-only, excluding it from cooked gameplay content where supported. Use with care for gameplay references.");
        case 15: return TEXT("Controls whether the Actor is eligible for World Partition spatial loading. Relevant only where World Partition applies.");
        case 16: return TEXT("Controls whether the Actor participates in automatic Hierarchical Level of Detail (HLOD) generation where the world supports HLOD.");
        case 17: return TEXT("Controls whether a placed Actor is loaded on clients as part of networked level loading. It does not enable Actor replication.");
        case 18: return TEXT("Controls whether eligible primitive components replicate their physics state to autonomous proxies. Requires a suitable replicated physics/network setup.");
        case 19: return TEXT("Controls the Static Mesh Actor movement-replication setting. This does not make unrelated Actor or component types replicate automatically.");
        case 20: return TEXT("Allows an Actor component to contribute to navigation data. Navigation rebuild and supported collision geometry determine the final result.");
        case 21: return TEXT("Marks an eligible shape component as a dynamic obstacle for navigation generation where supported.");
        case 22: return TEXT("Fills navigable space beneath eligible primitive collision for navigation generation, as supported by the navmesh configuration.");
        case 23: return TEXT("Uses filled convex-volume rasterization for eligible primitive collision when generating navigation data.");
        case 24: return TEXT("Sets how scene components may move: Static (not movable), Stationary (limited movement), or Movable. Other components and lighting features may restrict choices.");
        case 25: return TEXT("Overrides the mesh LOD selection. 0 uses automatic LOD; 1 forces mesh LOD 0; 2 forces mesh LOD 1, and so on. Available LODs depend on each mesh asset.");
        case 26: return TEXT("Prevents supported primitives from being removed by distance-culling systems that honor this setting. It does not disable every visibility or streaming mechanism.");
        case 27: return TEXT("Allows Cull Distance Volumes to influence culling for eligible primitive components. The level must contain a suitable Cull Distance Volume.");
        default: return TEXT("Changes a supported instance-editable Unreal Engine property.");
        }
    }

    static FText QuickTooltip(const FRowState& Row, int32 Index, int32 MobilityChoice = INDEX_NONE)
    {
        const auto& Def = FocusQuick::Definitions()[Index];
        const int32 State = Row.QuickState(Index);
        const int32 EligibleCount = Row.QuickTotal.IsValidIndex(Index) ? Row.QuickTotal[Index] : 0;

        FString Current;
        if (State < 0)
            Current = TEXT("Not Applicable (no eligible instance-editable target in this row)");
        else if (State == 2)
            Current = TEXT("Mixed (eligible targets have different values)");
        else if (Def.Kind == FocusQuick::EKind::Mobility)
            Current = Row.QuickFirst[Index] == 0 ? TEXT("Static") :
                Row.QuickFirst[Index] == 1 ? TEXT("Stationary") : TEXT("Movable");
        else if (Def.Kind == FocusQuick::EKind::ForcedLOD)
            Current = FString::Printf(TEXT("%d (%s)"), Row.QuickFirst[Index],
                Row.QuickFirst[Index] == 0 ? TEXT("Automatic") : TEXT("forced"));
        else
            Current = State == 1 ? TEXT("Enabled") : TEXT("Disabled");

        FString Tip = FString(Def.Label) + TEXT("\n") + QuickDescription(Index) +
            TEXT("\n\nCurrent: ") + Current +
            FString::Printf(TEXT("\nEligible targets in this row: %d"), EligibleCount);

        if (Def.Kind == FocusQuick::EKind::ForcedLOD)
        {
            if (State >= 0 && Row.QuickMinAvailableLOD.IsValidIndex(Index) &&
                Row.QuickMinAvailableLOD[Index] != INDEX_NONE)
            {
                Tip += FString::Printf(TEXT("\nCommon valid range: 0 to %d (0 = Automatic)."),
                    Row.QuickMinAvailableLOD[Index]);
            }
            if (State >= 0 && Row.QuickInvalidLOD.IsValidIndex(Index) && Row.QuickInvalidLOD[Index])
            {
                Tip += TEXT("\nOUT OF RANGE: at least one eligible mesh lacks the requested LOD. The value uses the Alert color.");
            }
            // SFocusLODNumber appends its own mouse/keyboard instructions.
        }
        else if (Def.Kind == FocusQuick::EKind::Mobility)
        {
            const TCHAR* Option = MobilityChoice == 0 ? TEXT("Static") :
                MobilityChoice == 1 ? TEXT("Stationary") : TEXT("Movable");
            Tip += FString(TEXT("\n\nHovered option: ")) + Option + TEXT(" (top / middle / bottom).");
            Tip += State < 0 ?
                TEXT("\nNo applicable component can be edited here.") :
                FString(TEXT("\nClick to set ")) + Option + TEXT(" on eligible scene components.");
        }
        else
        {
            Tip += State < 0 ? TEXT("\n\nUnavailable: this row has no matching editable Actor or Component property.") :
                State == 1 ? TEXT("\n\nClick to disable on eligible targets.") :
                TEXT("\n\nClick to enable on eligible targets, including any Mixed values.");
        }
        Tip += TEXT("\n\nAffects currently loaded Actors in this row, including its descendants and groups. If this row is selected, the action also covers the other selected rows. Component properties affect each eligible component.");
        Tip += TEXT("\nEdit-locked Actors are skipped. Changes support Undo/Redo. Unreal may impose additional property or collision requirements.");
        return FText::FromString(Tip);
    }
    static void ApplyQuick(const TSharedRef<FRowState>& Row, int32 Index, int32 NewValue)
    {
        const auto& Def = FocusQuick::Definitions()[Index];
        const auto Actors = ActionActors(Row->Item);
        const FScopedTransaction Transaction(NSLOCTEXT("Focus", "QuickControlChange", "Focus: Change actor property"));
        for (const auto& WeakActor : Actors)
        {
            AActor* Actor = WeakActor.Get();
            if (!Actor || Focus::Get(Actor, Focus::EControl::Edit)) continue;
            TArray<UObject*> Targets;
            FocusQuick::GetTargets(Actor, Def, Targets);
            for (UObject* Target : Targets) FocusQuick::Write(Target, Def, NewValue);
        }
        RefreshRow(*Row, LoadedActors());
        if (GEditor) GEditor->RedrawLevelEditingViewports();
    }
    static TSharedRef<SWidget> QuickControl(const TSharedRef<FRowState>& Row, int32 Index)
    {
        const auto& Def = FocusQuick::Definitions()[Index];
        if (Def.Kind == FocusQuick::EKind::ForcedLOD)
        {
            return SNew(SBox).WidthOverride(44.f).HeightOverride(20.f)
                [SNew(SNumericEntryBox<int32>)
                 .AllowSpin(false).MinValue(0).MaxValue(255)
                 .IsEnabled_Lambda([Row, Index] { return Row->QuickState(Index) >= 0; })
                 .Value_Lambda([Row, Index]() -> TOptional<int32> {
                     if (Row->QuickState(Index) < 0 || Row->QuickState(Index) == 2) return TOptional<int32>();
                     return Row->QuickFirst[Index];
                 })
                 .ToolTipText_Lambda([Row, Index] { return QuickTooltip(*Row, Index); })
                 .OnValueCommitted_Lambda([Row, Index](int32 NewValue, ETextCommit::Type) {
                     ApplyQuick(Row, Index, FMath::Clamp(NewValue, 0, 255));
                 })];
        }
        if (Def.Kind == FocusQuick::EKind::Mobility)
        {
            TSharedRef<SHorizontalBox> Choices = SNew(SHorizontalBox);
            for (int32 Value = 0; Value < 3; ++Value)
            {
                const int32 Choice = Value;
                const TCHAR* Caption = Value == 0 ? TEXT("S") : Value == 1 ? TEXT("T") : TEXT("M");
                Choices->AddSlot().AutoWidth()
                    [SNew(SBox).WidthOverride(20.f).HeightOverride(20.f)
                     [SNew(SButton).ButtonStyle(FAppStyle::Get(), "SimpleButton")
                      .ContentPadding(0).HAlign(HAlign_Center).VAlign(VAlign_Center)
                      .ToolTipText_Lambda([Row, Index, Choice] {
                          return QuickTooltip(*Row, Index, Choice);
                      })
                      .IsEnabled_Lambda([Row, Index] { return Row->QuickState(Index) >= 0; })
                      .OnClicked_Lambda([Row, Index, Choice] { ApplyQuick(Row, Index, Choice); return FReply::Handled(); })
                      [SNew(STextBlock).Text(FText::FromString(Caption))
                       .ColorAndOpacity_Lambda([Row, Index, Choice] {
                           const auto* Settings = GetDefault<UFocusSettings>();
                           const int32 State = Row->QuickState(Index);
                           return FSlateColor(State < 0 ? Settings->NotApplicableColor :
                               State == 2 ? Settings->MixedColor : Row->QuickFirst[Index] == Choice ?
                               Settings->EnabledColor : Settings->DisabledColor);
                       })]]];
            }
            return Choices;
        }
        return SNew(SBox).WidthOverride(20.f).HeightOverride(20.f)
            [SNew(SButton).ButtonStyle(FAppStyle::Get(), "SimpleButton").ContentPadding(0)
             .HAlign(HAlign_Center).VAlign(VAlign_Center)
             .ToolTipText_Lambda([Row, Index] { return QuickTooltip(*Row, Index); })
             .IsEnabled_Lambda([Row, Index] { return Row->QuickState(Index) >= 0; })
             .OnClicked_Lambda([Row, Index] {
                const auto& Def = FocusQuick::Definitions()[Index];
                if (Def.Kind == FocusQuick::EKind::Bool)
                    ApplyQuick(Row, Index, Row->QuickState(Index) == 1 ? 0 : 1);
                else if (Def.Kind == FocusQuick::EKind::Mobility)
                    ApplyQuick(Row, Index, Row->QuickState(Index) == 2 ? 0 : (Row->QuickFirst[Index] + 1) % 3);
                else
                    ApplyQuick(Row, Index, Row->QuickState(Index) == 2 ? 0 : (Row->QuickFirst[Index] + 1) % 9);
                return FReply::Handled();
             })
             [SNew(STextBlock).Text_Lambda([Row, Index] { return QuickLabel(*Row, Index); })
              .ColorAndOpacity_Lambda([Row, Index] { return FSlateColor(QuickColor(*Row, Index)); })]
            ];
    }
    // Individually interactive segments reproduce the SVG rectangles. Width and
    // height are explicit, and SFocusPropertySegment paints those allocations
    // directly. Neither per-state tint nor native button padding can resize them.
    static TSharedRef<SWidget> QuickSegment(const TSharedRef<FRowState>& Row,
        int32 Index, float Width, float Height,
        SFocusPropertySegment::EShape Shape = SFocusPropertySegment::EShape::Rectangle)
    {
        return SNew(SBox).WidthOverride(Width).HeightOverride(Height)
            [SNew(SFocusPropertySegment)
             .Shape(Shape)
             .Available([Row, Index] { return Row->QuickState(Index) >= 0; })
             .Toggle([Row, Index] { ApplyQuick(Row, Index, Row->QuickState(Index) == 1 ? 0 : 1); })
             .Tint([Row, Index] { return FSlateColor(QuickColor(*Row, Index)); })
             .Tooltip([Row, Index] { return QuickTooltip(*Row, Index); })];
    }

    static TSharedRef<SWidget> CompactCategory(const TSharedRef<FRowState>& Row,
        FocusQuick::ECategory Category, int32 First)
    {
        using namespace FocusQuick;
        if (Category == ECategory::Simulation || Category == ECategory::PhysicsEvents || Category == ECategory::Network)
        {
            const auto Design = Category == ECategory::Simulation ? SFocusRadialCategory::EDesign::Simulation :
                Category == ECategory::PhysicsEvents ? SFocusRadialCategory::EDesign::PhysicsEvents :
                SFocusRadialCategory::EDesign::Network;
            const FVector2D Size = SFocusRadialCategory::SizeFor(Design);
            return SNew(SBox).WidthOverride(Size.X).HeightOverride(Size.Y)
                [SNew(SFocusRadialCategory)
                 .Design(Design)
                 .Available([Row, First](int32 Choice) { return Row->QuickState(First + Choice) >= 0; })
                 .Toggle([Row, First](int32 Choice) {
                     const int32 Index = First + Choice;
                     ApplyQuick(Row, Index, Row->QuickState(Index) == 1 ? 0 : 1);
                 })
                 .Tint([Row, First](int32 Choice) { return QuickColor(*Row, First + Choice); })
                 .Tooltip([Row, First](int32 Choice) { return QuickTooltip(*Row, First + Choice); })];
        }
        if (Category == ECategory::Lighting)
        {
            // Lighting-Focus.svg geometry, optically scaled to match core Focus icons:
            // 3 x 4.24 by 8.48 channels, 2.12 gaps, and a 16.95 by 4.24
            // Cast Shadows bar 2.12 pixels below them.
            return SNew(SBox).WidthOverride(16.95f * FocusLightingScale).HeightOverride(14.83f * FocusLightingScale)
                [SNew(SVerticalBox)
                 + SVerticalBox::Slot().AutoHeight()
                    [SNew(SHorizontalBox)
                     + SHorizontalBox::Slot().AutoWidth()[QuickSegment(Row, First + 0, 4.24f * FocusLightingScale, 8.48f * FocusLightingScale)]
                     + SHorizontalBox::Slot().AutoWidth().Padding(FMargin(2.12f * FocusLightingScale, 0.f, 0.f, 0.f))
                        [QuickSegment(Row, First + 1, 4.24f * FocusLightingScale, 8.48f * FocusLightingScale)]
                     + SHorizontalBox::Slot().AutoWidth().Padding(FMargin(2.12f * FocusLightingScale, 0.f, 0.f, 0.f))
                        [QuickSegment(Row, First + 2, 4.24f * FocusLightingScale, 8.48f * FocusLightingScale)]]
                 + SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.12f * FocusLightingScale, 0.f, 0.f)
                    [QuickSegment(Row, First + 3, 16.95f * FocusLightingScale, 4.24f * FocusLightingScale)]];
        }
        if (Category == ECategory::World)
        {
            // World-Focus.svg: 2 x 2 grid of 6.37-square tiles with 2.13 px
            // gaps. Retain individual tooltips, toggles, and state tints.
            return SNew(SBox).WidthOverride(14.87f * FocusWorldScale).HeightOverride(14.87f * FocusWorldScale)
                [SNew(SVerticalBox)
                 + SVerticalBox::Slot().AutoHeight()
                    [SNew(SHorizontalBox)
                     + SHorizontalBox::Slot().AutoWidth()[QuickSegment(Row, First + 0, 6.37f * FocusWorldScale, 6.37f * FocusWorldScale)]
                     + SHorizontalBox::Slot().AutoWidth().Padding(FMargin(2.13f * FocusWorldScale, 0.f, 0.f, 0.f))
                        [QuickSegment(Row, First + 1, 6.37f * FocusWorldScale, 6.37f * FocusWorldScale)]]
                 + SVerticalBox::Slot().AutoHeight().Padding(0.f, 2.13f * FocusWorldScale, 0.f, 0.f)
                    [SNew(SHorizontalBox)
                     + SHorizontalBox::Slot().AutoWidth()[QuickSegment(Row, First + 2, 6.37f * FocusWorldScale, 6.37f * FocusWorldScale)]
                     + SHorizontalBox::Slot().AutoWidth().Padding(FMargin(2.13f * FocusWorldScale, 0.f, 0.f, 0.f))
                        [QuickSegment(Row, First + 3, 6.37f * FocusWorldScale, 6.37f * FocusWorldScale)]]];
        }
        if (Category == ECategory::Navigation)
        {
            // The original four-circle diamond, now 18 x 18 so it fits a 20-unit
            // stock Outliner row. Each quadrant is clickable, not just the dots.
            return SNew(SBox).WidthOverride(18.f).HeightOverride(18.f)
                [SNew(SFocusNavigationDiamond)
                 .Available([Row, First](int32 Choice) { return Row->QuickState(First + Choice) >= 0; })
                 .Toggle([Row, First](int32 Choice) {
                     const int32 Index = First + Choice;
                     ApplyQuick(Row, Index, Row->QuickState(Index) == 1 ? 0 : 1);
                 })
                 .Tint([Row, First](int32 Choice) { return QuickColor(*Row, First + Choice); })
                 .Tooltip([Row, First](int32 Choice) { return QuickTooltip(*Row, First + Choice); })];
        }
        if (Category == ECategory::Mobility)
        {
            // Top to bottom: Static, Stationary, Movable. The entire column is
            // one 14 x 18 segment selector, comparable to neighboring icons.
            return SNew(SBox).WidthOverride(SFocusMobilityColumn::Width).HeightOverride(SFocusMobilityColumn::Height)
                [SNew(SFocusMobilityColumn)
                 .State([Row, First] { return Row->QuickState(First); })
                 .Value([Row, First] { return Row->QuickFirst.IsValidIndex(First) ? Row->QuickFirst[First] : 0; })
                 .Apply([Row, First](int32 Choice) { ApplyQuick(Row, First, Choice); })
                 .Tooltip([Row, First](int32 Choice) { return QuickTooltip(*Row, First, Choice); })];
        }
        if (Category == ECategory::LOD)
        {
            // LOD-Focus.svg: large centered number, then two 6.37 x 3.18
            // culling bars. The number keeps single-click increment and
            // double-click edit behavior; the bars remain separate toggles.
            return SNew(SBox).WidthOverride(14.87f * FocusLODWidthScale).HeightOverride(17.66f * FocusLODHeightScale)
                [SNew(SVerticalBox)
                 + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center)
                    [SNew(SFocusLODNumber)
                     .Read([Row, First]() -> TOptional<int32> {
                         if (Row->QuickState(First) < 0 || Row->QuickState(First) == 2)
                             return TOptional<int32>();
                         return Row->QuickFirst[First];
                     })
                     .IncrementBase([Row, First]() { return Row->QuickFirst.IsValidIndex(First) ? Row->QuickFirst[First] : 0; })
                     .Available([Row, First]() { return Row->QuickState(First) >= 0; })
                     .Apply([Row, First](int32 Value) { ApplyQuick(Row, First, Value); })
                     .Color([Row, First]() { return FSlateColor(QuickColor(*Row, First)); })
                     .Tooltip([Row, First]() { return QuickTooltip(*Row, First); })]
                 + SVerticalBox::Slot().AutoHeight()
                    [SNew(SHorizontalBox)
                     + SHorizontalBox::Slot().AutoWidth()[QuickSegment(Row, First + 1, 6.37f * FocusLODWidthScale, 3.18f * FocusLODHeightScale)]
                     + SHorizontalBox::Slot().AutoWidth().Padding(FMargin(2.13f * FocusLODWidthScale, 0.f, 0.f, 0.f))
                        [QuickSegment(Row, First + 2, 6.37f * FocusLODWidthScale, 3.18f * FocusLODHeightScale)]]];
        }
        return SNullWidget::NullWidget;
    }

    // Every checkbox in the Focus header menu explains its purpose and shows
    // the exact controls that enabling that category exposes. Property names
    // for native quick options come directly from Definitions() so the menu
    // cannot silently drift out of sync with the miniwidgets.
    static FText CategoryMenuTooltip(FocusQuick::ECategory Category)
    {
        using FocusQuick::ECategory;
        const TCHAR* Purpose = TEXT("");
        switch (Category)
        {
        case ECategory::Visual:
            Purpose = TEXT("Manage Actor visibility and temporarily isolate parts of the scene.");
            break;
        case ECategory::Interaction:
            Purpose = TEXT("Protect Actors from viewport selection, transform changes, or other supported edits.");
            break;
        case ECategory::Simulation:
            Purpose = TEXT("Adjust physics simulation, gravity, force responses, and physics ticking.");
            break;
        case ECategory::PhysicsEvents:
            Purpose = TEXT("Control collision hit, overlap, and physics wake-event generation.");
            break;
        case ECategory::Lighting:
            Purpose = TEXT("Choose lighting channels and whether eligible components cast shadows.");
            break;
        case ECategory::World:
            Purpose = TEXT("Adjust Actor world restrictions, editor-only status, loading, and HLOD participation.");
            break;
        case ECategory::Network:
            Purpose = TEXT("Adjust client loading and supported replicated physics or movement options.");
            break;
        case ECategory::Navigation:
            Purpose = TEXT("Control how eligible components contribute to navigation generation.");
            break;
        case ECategory::Mobility:
            Purpose = TEXT("Choose the mobility setting for eligible Scene Components.");
            break;
        case ECategory::LOD:
            Purpose = TEXT("Override mesh LOD selection and control distance-culling behavior.");
            break;
        default:
            break;
        }

        FString Options;
        auto AppendOption = [&Options](const TCHAR* Label)
        {
            Options += FString::Printf(TEXT("\n  - %s"), Label);
        };
        if (Category == ECategory::Visual)
        {
            AppendOption(TEXT("Hide in Editor"));
            AppendOption(TEXT("Hidden in Game"));
            AppendOption(TEXT("Solo"));
        }
        else if (Category == ECategory::Interaction)
        {
            AppendOption(TEXT("Selection Lock"));
            AppendOption(TEXT("Unlocked (protection level)"));
            AppendOption(TEXT("Transform Lock"));
            AppendOption(TEXT("Edit Lock"));
        }
        else if (Category == ECategory::Mobility)
        {
            AppendOption(TEXT("Static"));
            AppendOption(TEXT("Stationary"));
            AppendOption(TEXT("Movable"));
        }
        else
        {
            for (const FocusQuick::FDef& Def : FocusQuick::Definitions())
            {
                if (Def.Category == Category)
                {
                    AppendOption(Def.Label);
                }
            }
        }

        const bool bVisibleByDefault = Category == ECategory::Visual || Category == ECategory::Interaction;
        return FText::FromString(FString::Printf(
            TEXT("%s\n\n%s\n\nControls:%s\n\nCheck to show this category in the World Outliner. "
                 "Changes take effect immediately and persist for this project.\nDefault: %s."),
            FocusQuick::CategoryName(Category), Purpose, *Options,
            bVisibleByDefault ? TEXT("Visible") : TEXT("Hidden")));
    }

    static TSharedRef<SWidget> CategoryMenu()
    {
        TSharedRef<SVerticalBox> Items = SNew(SVerticalBox);
        for (int32 Index = 0; Index < static_cast<int32>(FocusQuick::ECategory::Count); ++Index)
        {
            const auto Category = static_cast<FocusQuick::ECategory>(Index);
            Items->AddSlot().AutoHeight().Padding(3.f, 1.f)
                [SNew(SCheckBox)
                 .ToolTipText_Lambda([Category] { return CategoryMenuTooltip(Category); })
                 .IsChecked_Lambda([Category] {
                    return FocusQuick::CategoryVisible(Category) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                }).OnCheckStateChanged_Lambda([Category](ECheckBoxState NewState) {
                    FocusQuick::SetCategoryVisible(Category, NewState == ECheckBoxState::Checked);
                    for (auto& Weak : ActiveFocusColumns) if (auto Column = Weak.Pin()) Column->Invalidate();
                })
                [SNew(STextBlock).Text(FText::FromString(FocusQuick::CategoryName(Category)))]];
        }
        return SNew(SBox).WidthOverride(205.f).Padding(8.f)
            [SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight().Padding(3.f, 2.f)
            [SNew(STextBlock).Text(NSLOCTEXT("Focus", "CategoriesHeader", "Focus Categories"))]
            + SVerticalBox::Slot().AutoHeight()[Items]
            + SVerticalBox::Slot().AutoHeight().Padding(3.f, 5.f, 3.f, 2.f)
            [SNew(SButton).ButtonStyle(FAppStyle::Get(), "SimpleButton")
             .ToolTipText(NSLOCTEXT("Focus", "ResetCategoriesTip",
                 "Restore the default category layout: show Visual and Interaction, and hide the other eight categories.\nChanges are saved for this project and do not change Actor or Component properties."))
             .OnClicked_Lambda([] {
                 for (int32 Index = 0; Index < static_cast<int32>(FocusQuick::ECategory::Count); ++Index)
                 {
                     const auto C = static_cast<FocusQuick::ECategory>(Index);
                     FocusQuick::SetCategoryVisible(C, C == FocusQuick::ECategory::Visual || C == FocusQuick::ECategory::Interaction);
                 }
                 for (auto& Weak : ActiveFocusColumns) if (auto Column = Weak.Pin()) Column->Invalidate();
                 return FReply::Handled();
             })[SNew(STextBlock).Text(NSLOCTEXT("Focus", "ResetCategories", "Reset to Visual + Interaction"))]]
            ];
    }

    // Edit Lock includes Transform Lock; these are three protection levels,
    // independent of Selection Lock. 3 represents a mixed hierarchy/selection.
    static int32 ProtectionState(const FRowState& Row)
    {
        if (Row.Total == 0) return 0;
        const int32 EditCount = Row.Enabled[static_cast<int32>(Focus::EControl::Edit)];
        const int32 TransformCount = Row.Enabled[static_cast<int32>(Focus::EControl::Transform)];
        if (EditCount == Row.Total) return 2;
        if (EditCount > 0) return 3;
        if (TransformCount == Row.Total) return 1;
        return TransformCount > 0 ? 3 : 0;
    }
    static TSharedRef<SWidget> ProtectionControl(const TSharedRef<FRowState>& Row)
    {
        return SNew(SBox).WidthOverride(20.f).HeightOverride(20.f)
            [SNew(SButton).ButtonStyle(FAppStyle::Get(), "SimpleButton").ContentPadding(0)
            .HAlign(HAlign_Center).VAlign(VAlign_Center)
            .IsEnabled_Lambda([Row] { return Row->Total > 0; })
            .ToolTipText_Lambda([Row] {
            const int32 State = ProtectionState(*Row);
            const TCHAR* Current = State == 0 ? TEXT("Unlocked") : State == 1 ? TEXT("Transform locked") :
                State == 2 ? TEXT("Edit locked (includes transforms)") : TEXT("Mixed protection levels");
            return FText::FromString(FString(Current) +
                TEXT("\nClick to cycle: Unlocked > Transform Lock > Edit Lock > Unlocked.") +
                TEXT("\nA mixed scope becomes Transform Locked, including any currently edit-locked actors.") +
                TEXT("\nIncludes attached descendants, nested folders and native group members. When this row is selected, cycles the combined scope of all selected rows.") +
                TEXT("\nSelection Lock is independent and is not changed.") +
                TEXT("\nBlueprint-driven transform changes are allowed.") +
                TEXT("\nLoaded actors only; actors added later do not inherit these settings.") +
                TEXT("\nEdit Lock protects Details values, standard transforms and supported actor/component editing commands. Custom tools and scripts can bypass it.") +
                TEXT("\nSave All to retain changes with the level and actor files."));
                })
            .OnClicked_Lambda([Row] {
            using namespace Focus;
            const auto Actors = ActionActors(Row->Item);
            int32 Common = -1;
            bool bMixed = false;
            for (const auto& Weak : Actors) if (AActor* Actor = Weak.Get())
            {
                const int32 State = Get(Actor, EControl::Edit) ? 2 : Get(Actor, EControl::Transform) ? 1 : 0;
                if (Common < 0) Common = State;
                else if (Common != State) bMixed = true;
            }
            if (Common < 0) return FReply::Handled();
            const int32 Next = bMixed ? 1 : (Common + 1) % 3;
            const FScopedTransaction Transaction(NSLOCTEXT("Focus", "SharedProtection", "Focus: Change actor protection"));
            for (const auto& Weak : Actors) if (AActor* Actor = Weak.Get())
            {
                // Explicitly choosing a level replaces both protection flags.
                // Clear Edit first because it normally prevents Transform changes.
                Set(Actor, EControl::Edit, false);
                Set(Actor, EControl::Transform, Next >= 1);
                if (Next == 2) Set(Actor, EControl::Edit, true);
            }
            RefreshRow(*Row, LoadedActors());
            if (GEditor) GEditor->RedrawLevelEditingViewports();
            return FReply::Handled();
                })
            [SNew(SBox).WidthOverride(16.f).HeightOverride(16.f)
            [SNew(SOverlay)
            + SOverlay::Slot()[SNew(SImage).Image_Lambda([Row] {
            return FAppStyle::GetBrush(ProtectionState(*Row) == 0 ? TEXT("Icons.Unlock") : TEXT("Icons.Lock"));
                }).ColorAndOpacity_Lambda([Row] {
                    switch (ProtectionState(*Row))
                    {
                    case 0: return FSlateColor(GetDefault<UFocusSettings>()->DisabledColor);
                    case 1: return FSlateColor(GetDefault<UFocusSettings>()->EnabledColor);
                    case 2: return FSlateColor(GetDefault<UFocusSettings>()->AlertColor);
                    default: return FSlateColor(GetDefault<UFocusSettings>()->MixedColor);
                    }
                    })]
            + SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom)
                [SNew(STextBlock).Text(FText::FromString(TEXT("-")))
                .Visibility_Lambda([Row] { return ProtectionState(*Row) == 3 ? EVisibility::HitTestInvisible : EVisibility::Collapsed; })]
            ]]
            ];
    }
public:
    void Invalidate() { NextRefresh = 0.0; }
    // Stable internal identifier; the displayed column name is "Focus".
    static FName GetID() { return TEXT("Focus.PrototypeControls"); }
    virtual FName GetColumnID() override { return GetID(); }
    virtual void Tick(double InCurrentTime, float) override
    {
        if (InCurrentTime < NextRefresh) return;
        NextRefresh = InCurrentTime + .5;
        Rows.RemoveAll([](const TWeakPtr<FRowState>& Row) { return !Row.IsValid(); });
        if (Rows.IsEmpty()) return;
        // One world enumeration per refresh; Slate attributes only read aggregate counts.
        const auto Loaded = LoadedActors();
        for (const auto& Weak : Rows) if (const auto Row = Weak.Pin()) RefreshRow(*Row, Loaded);
    }
    virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override
    {
        return SHeaderRow::Column(GetID())
            .ManualWidth(TAttribute<float>::CreateLambda([] { return static_cast<float>(FocusQuick::ColumnWidth()); }))
            .HeaderContent()
            [SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(2.f, 0.f)
                [SNew(STextBlock).Text(NSLOCTEXT("Focus", "Header", "Focus"))]
            + SHorizontalBox::Slot().FillWidth(1.f)
                [SNullWidget::NullWidget]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).HAlign(HAlign_Right)
                [SNew(SComboButton).ButtonStyle(FAppStyle::Get(), "SimpleButton")
                 .HasDownArrow(false).ContentPadding(FMargin(3.f, 0.f))
                 .ToolTipText(NSLOCTEXT("Focus", "SettingsTip", "Show or hide Focus control categories in this World Outliner.\nVisual and Interaction are shown by default; eight optional groups start hidden.\nYour choices are saved for this project.\nChange state colors in Editor Preferences > Plugins > Focus."))
                 .OnGetMenuContent_Lambda([] { return CategoryMenu(); })
                 .ButtonContent()[SNew(STextBlock)
                     .Text(FText::FromString(TEXT("...")))
                     .Font(FAppStyle::GetFontStyle(TEXT("SmallFont")))]]
            ];
    }

    static void AddCategoryGroup(const TSharedRef<SHorizontalBox>& Content,
        FocusQuick::ECategory Category, const TSharedRef<SWidget>& Body)
    {
        // A spacer belongs to the CATEGORY, not an individual property. Because
        // both widgets are children of the same collapsed parent, hiding a
        // category removes its entire width, including its spacer. Checking
        // earlier visible categories also avoids a leading gap if Visual and
        // Interaction are disabled in the header menu.
        Content->AddSlot().AutoWidth().VAlign(VAlign_Center)
            [SNew(SBox)
             .Visibility_Lambda([Category] {
                 return FocusQuick::CategoryVisible(Category) ? EVisibility::Visible : EVisibility::Collapsed;
             })
             [SNew(SHorizontalBox)
              + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                  [SNew(SBox)
                   .WidthOverride_Lambda([Category]() -> FOptionalSize {
                       return FOptionalSize(FocusQuick::GapBefore(Category));
                   })
                   .Visibility_Lambda([Category] {
                       return FocusQuick::GapBefore(Category) > 0.0f ? EVisibility::Visible : EVisibility::Collapsed;
                   })]
              + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[Body]]];
    }

    virtual const TSharedRef<SWidget> ConstructRowWidget(FSceneOutlinerTreeItemRef Item, const STableRow<FSceneOutlinerTreeItemPtr>& TableRow) override
    {
        using namespace Focus;
        if (!Item->IsA<FActorTreeItem>() && !Item->IsA<FFolderTreeItem>()) return SNullWidget::NullWidget;
        FocusInteraction::RegisterRow(Item, TableRow);
        const auto Row = MakeShared<FRowState>();
        Row->Item = Item;
        Rows.Add(Row);
        // The next column tick fills counts, avoiding one world scan per generated row.
        NextRefresh = 0.0;
        TSharedRef<SHorizontalBox> Content = SNew(SHorizontalBox);
        AddCategoryGroup(Content, FocusQuick::ECategory::Visual,
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
                [SNew(SVerticalBox)
                 + SVerticalBox::Slot().AutoHeight()[Control(Row, EControl::EditorHidden)]
                 + SVerticalBox::Slot().AutoHeight()[Control(Row, EControl::GameHidden)]]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[Control(Row, EControl::Solo)]);

        AddCategoryGroup(Content, FocusQuick::ECategory::Interaction,
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[Control(Row, EControl::Selection)]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[ProtectionControl(Row)]);

        const auto& Defs = FocusQuick::Definitions();
        for (int32 Index = 0; Index < Defs.Num();)
        {
            const auto Category = Defs[Index].Category;
            const int32 First = Index;
            while (Index < Defs.Num() && Defs[Index].Category == Category)
            {
                ++Index;
            }

            if (Category == FocusQuick::ECategory::Simulation ||
                Category == FocusQuick::ECategory::PhysicsEvents ||
                Category == FocusQuick::ECategory::Network ||
                Category == FocusQuick::ECategory::Lighting ||
                Category == FocusQuick::ECategory::World ||
                Category == FocusQuick::ECategory::Navigation ||
                Category == FocusQuick::ECategory::Mobility ||
                Category == FocusQuick::ECategory::LOD)
            {
                AddCategoryGroup(Content, Category, CompactCategory(Row, Category, First));
            }
            else
            {
                TSharedRef<SHorizontalBox> CategoryControls = SNew(SHorizontalBox);
                for (int32 ControlIndex = First; ControlIndex < Index; ++ControlIndex)
                {
                    CategoryControls->AddSlot().AutoWidth().VAlign(VAlign_Center)
                        [QuickControl(Row, ControlIndex)];
                }
                AddCategoryGroup(Content, Category, CategoryControls);
            }
        }
        return SNew(SBox).Padding(FMargin(2.f, 0.f)).VAlign(VAlign_Center)[Content];
    }
};

/** Guards standard Level Editor command bindings before their normal handlers run.
 * Also guards the discovered actor component tree's standard command bindings.
 * Direct engine APIs, inline text commits, Add Component buttons and Outliner
 * drag/drop have separate entry points and are not covered by this class.
 */
class FFocusCommandGuard
{
    struct FBinding
    {
        TWeakPtr<FUICommandList> List;
        TSharedPtr<const FUICommandInfo> Command;
        FUIAction Previous;
        FDelegateHandle InstalledExecute;
        FDelegateHandle InstalledCanExecute;
    };
    struct FLifetime { bool bActive = true; };
    TSharedRef<FLifetime> Lifetime = MakeShared<FLifetime>();
    TArray<TWeakPtr<SWidget>> InspectedHosts;
    TArray<FBinding> Bindings;

    static bool IsBlueprintExecuting()
    {
        for (const FFrame* Frame = FFrame::GetThreadLocalTopStackFrame(); Frame; Frame = Frame->PreviousTrackingFrame)
            if (Frame->Node && !Frame->Node->Script.IsEmpty()) return true;
        return false;
    }
    static bool IsBlocked(bool bIncludeDescendants)
    {
        if (!GEditor || GIsTransacting || IsBlueprintExecuting()) return false;
        // Component commands may be routed through the Level Editor's command list.
        // This covers that route only; SSubobjectEditor owns an additional command list.
        if (USelection* Components = GEditor->GetSelectedComponents())
        {
            for (FSelectionIterator It(*Components); It; ++It)
                if (auto* Component = Cast<UActorComponent>(*It))
                    if (Focus::Get(Component->GetOwner(), Focus::EControl::Edit)) return true;
        }
        TArray<AActor*> Pending;
        if (USelection* Actors = GEditor->GetSelectedActors())
            for (FSelectionIterator It(*Actors); It; ++It)
                if (AActor* Actor = Cast<AActor>(*It)) Pending.Add(Actor);
        TSet<AActor*> Seen;
        while (!Pending.IsEmpty())
        {
            AActor* Actor = Pending.Last();
            Pending.RemoveAt(Pending.Num() - 1, 1, EAllowShrinking::No);
            if (!Focus::IsEditorActor(Actor) || Seen.Contains(Actor)) continue;
            Seen.Add(Actor);
            if (Focus::Get(Actor, Focus::EControl::Edit)) return true;
            if (!bIncludeDescendants) continue;
            // Deleting or reparenting an unlocked parent can detach or otherwise
            // restructure its protected children. Reject the whole command.
            TArray<AActor*> Children;
            Actor->GetAttachedActors(Children);
            Pending.Append(Children);
            if (AGroupActor* Group = Cast<AGroupActor>(Actor))
            {
                for (const auto& Member : Group->GroupActors) Pending.Add(Member.Get());
                for (const auto& ChildGroup : Group->SubGroups) Pending.Add(ChildGroup.Get());
            }
        }
        return false;
    }
    void Bind(const TSharedPtr<FUICommandList>& List, const TSharedPtr<FUICommandInfo>& Command,
        TFunction<bool()> Blocked)
    {
        if (!List || !Command) return;
        const FUIAction* Existing = List->GetActionForCommand(Command);
        if (!Existing) return;
        FBinding Binding;
        Binding.List = List;
        Binding.Command = Command;
        Binding.Previous = *Existing;
        const FUIAction Prior = Binding.Previous;
        const auto Life = Lifetime;
        FUIAction Guarded = Prior; // Preserve check state, visibility and repeat settings.
        Guarded.CanExecuteAction = FCanExecuteAction::CreateLambda([Prior, Life, Blocked] {
            if (Life->bActive && Blocked()) return false;
            return !Prior.CanExecuteAction.IsBound() || Prior.CanExecuteAction.Execute();
            });
        Guarded.ExecuteAction = FExecuteAction::CreateLambda([Prior, Life, Blocked] {
            // Recheck at execution too: a menu may have been opened before locking.
            if (Life->bActive && Blocked())
            {
                FNotificationInfo Info(NSLOCTEXT("Focus", "StructuralEditBlocked",
                    "Focus: unlock the edit-locked actor before changing this selection or hierarchy."));
                Info.ExpireDuration = 4.f;
                FSlateNotificationManager::Get().AddNotification(Info);
                return;
            }
            if (!Prior.CanExecuteAction.IsBound() || Prior.CanExecuteAction.Execute())
                Prior.ExecuteAction.ExecuteIfBound();
            });
        Binding.InstalledExecute = Guarded.ExecuteAction.GetHandle();
        Binding.InstalledCanExecute = Guarded.CanExecuteAction.GetHandle();
        List->UnmapAction(Command);
        List->MapAction(Command, Guarded);
        Bindings.Add(MoveTemp(Binding));
    }
public:
    explicit FFocusCommandGuard(FLevelEditorModule& Module)
    {
        const auto List = Module.GetGlobalLevelEditorActions();
        Bind(List, FGenericCommands::Get().Delete, [] { return IsBlocked(true); });
        Bind(List, FGenericCommands::Get().Cut, [] { return IsBlocked(true); });
        Bind(List, FGenericCommands::Get().Rename, [] { return IsBlocked(false); });
        Bind(List, FLevelEditorCommands::Get().DetachFromParent, [] { return IsBlocked(true); });
        Bind(List, FLevelEditorCommands::Get().AttachSelectedActors, [] { return IsBlocked(true); });
        // FFocusInteractionGuard handles the interactive picker separately and
        // validates its destination again when the user commits the attachment.
    }
    void DiscoverComponentCommands(const TSharedPtr<IDetailsView>& View)
    {
        // Only inspect the standard actor Details host, not Blueprint class editors.
        TSharedPtr<SWidget> Host = View;
        while (Host && Host->GetType() != FName(TEXT("SActorDetails"))) Host = Host->GetParentWidget();
        if (!Host) return;
        InspectedHosts.RemoveAll([](const TWeakPtr<SWidget>& Item) { return !Item.IsValid(); });
        for (const auto& Weak : InspectedHosts) if (Weak.Pin() == Host) return;
        TArray<TSharedRef<SWidget>> Pending;
        Pending.Add(Host.ToSharedRef());
        while (!Pending.IsEmpty())
        {
            const TSharedRef<SWidget> Widget = Pending.Last();
            Pending.RemoveAt(Pending.Num() - 1, 1, EAllowShrinking::No);
            if (Widget->GetType() == FName(TEXT("SSubobjectInstanceEditor")))
            {
                const auto Editor = StaticCastSharedRef<SSubobjectEditor>(Widget);
                const auto List = Editor->GetCommandList();
                if (!List) return; // Retry after construction completes.
                const TWeakPtr<SSubobjectEditor> WeakEditor = Editor;
                auto Blocked = [WeakEditor] {
                    if (GIsTransacting || IsBlueprintExecuting()) return false;
                    const auto Pinned = WeakEditor.Pin();
                    if (!Pinned) return true;
                    UObject* Context = Pinned->GetObjectContext();
                    AActor* Actor = Cast<AActor>(Context);
                    if (auto* Component = Cast<UActorComponent>(Context)) Actor = Component->GetOwner();
                    return Focus::Get(Actor, Focus::EControl::Edit);
                    };
                Bind(List, FGenericCommands::Get().Delete, Blocked);
                Bind(List, FGenericCommands::Get().Cut, Blocked);
                Bind(List, FGenericCommands::Get().Rename, Blocked);
                Bind(List, FGenericCommands::Get().Duplicate, Blocked);
                Bind(List, FGenericCommands::Get().Paste, Blocked);
                InspectedHosts.Add(Host);
                return;
            }
            if (FChildren* Children = Widget->GetChildren())
                for (int32 Index = 0; Index < Children->Num(); ++Index) Pending.Add(Children->GetChildAt(Index));
        }
    }
    ~FFocusCommandGuard()
    {
        Lifetime->bActive = false;
        for (const FBinding& Binding : Bindings) if (const auto List = Binding.List.Pin())
        {
            const FUIAction* Current = List->GetActionForCommand(Binding.Command);
            // Do not overwrite a newer binding installed by another extension.
            if (Current && Current->ExecuteAction.GetHandle() == Binding.InstalledExecute &&
                Current->CanExecuteAction.GetHandle() == Binding.InstalledCanExecute)
            {
                List->UnmapAction(Binding.Command);
                List->MapAction(Binding.Command, Binding.Previous);
            }
        }
    }
};

class FFocusModule : public IModuleInterface
{
    struct FBinding {
        TWeakPtr<IDetailsView> View;
        FIsPropertyReadOnly Previous;
        FDelegateHandle Installed;
        FIsPropertyEditingEnabled PreviousEditing;
        FDelegateHandle InstalledEditing;
    };
    TUniquePtr<FFocusTransformGuard> TransformGuard;
    TUniquePtr<FFocusCommandGuard> CommandGuard;
    TSharedPtr<FFocusInteractionGuard> InteractionGuard;
    TArray<FBinding> Bindings;
    FTSTicker::FDelegateHandle Ticker;
    FDelegateHandle Columns;
    FDelegateHandle MapOpened;
    FDelegateHandle ActorAdded, LevelAdded, ObjectsReplaced, NativePropertyChanged;
    TSet<TWeakObjectPtr<AActor>> PendingRestore;
    bool bRestoreWorld = true;
    TSharedPtr<FFocusViewExtension, ESPMode::ThreadSafe> ViewExtension;
    bool bModeActivated = false;
    bool bStopping = false;
    FDelegateHandle PreExit;
    FDelegateHandle RowExtensions;
    TArray<TWeakPtr<IDetailsView>> DiscoveredViews;

    // A loaded LevelEditor module is not sufficient: its actual editor instance
    // and the asset subsystem's scriptable-mode registry must both exist.
    void TryActivate()
    {
        if (bStopping || bModeActivated || !GEditor || IsRunningCommandlet()) return;
        auto* LevelEditor = FModuleManager::GetModulePtr<FLevelEditorModule>("LevelEditor");
        if (!LevelEditor || !LevelEditor->GetFirstLevelEditor().IsValid()) return;
        if (!CommandGuard) CommandGuard = MakeUnique<FFocusCommandGuard>(*LevelEditor);
        if (!InteractionGuard && FSlateApplication::IsInitialized())
        {
            InteractionGuard = MakeShared<FFocusInteractionGuard>(*LevelEditor);
            FSlateApplication::Get().RegisterInputPreProcessor(InteractionGuard);
        }
        auto* Assets = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        if (!Assets) return;
        FEditorModeInfo Info;
        if (!Assets->FindEditorModeInfo(UFocusEditorMode::ModeId, Info)) return;

        if (!TransformGuard && Cast<UTransBuffer>(GEditor->Trans))
            TransformGuard = MakeUnique<FFocusTransformGuard>(*GEditor);

        auto& Modes = GLevelEditorModeTools();
        // FOCUS must never become the only mode during initial editor startup.
        // Unreal normally starts its default modes only when no modes are active.
        // Installing our non-widget mode first suppresses that fallback and loses
        // the standard transform widget and its interactive editing behavior.
        bool bHasOtherMode = false;
        for (const FEditorModeInfo& RegisteredMode : Assets->GetEditorModeInfoOrderedByPriority())
        {
            if (RegisteredMode.ID != UFocusEditorMode::ModeId && Modes.IsModeActive(RegisteredMode.ID))
            {
                bHasOtherMode = true;
                break;
            }
        }
        if (!bHasOtherMode) Modes.ActivateDefaultMode();
        Modes.ActivateMode(UFocusEditorMode::ModeId);
        if (Modes.IsModeActive(UFocusEditorMode::ModeId))
        {
            // Only add a default after successful activation. A nonexistent default
            // can cause assertions when Unreal restores default modes.
            Modes.AddDefaultMode(UFocusEditorMode::ModeId);
            bModeActivated = true;
        }
    }

    void StopInteractionGuard()
    {
        if (!InteractionGuard) return;
        if (FSlateApplication::IsInitialized()) FSlateApplication::Get().UnregisterInputPreProcessor(InteractionGuard);
        InteractionGuard->Shutdown();
        InteractionGuard.Reset();
    }
    void StopMode()
    {
        bStopping = true;
        StopInteractionGuard();
        CommandGuard.Reset();
        TransformGuard.Reset();
        if (!bModeActivated) return;
        // Never recreate/access the global mode manager after its editor is gone.
        auto* LevelEditor = FModuleManager::GetModulePtr<FLevelEditorModule>("LevelEditor");
        if (GEditor && LevelEditor && LevelEditor->GetFirstLevelEditor().IsValid())
        {
            auto& Modes = GLevelEditorModeTools();
            Modes.RemoveDefaultMode(UFocusEditorMode::ModeId);
            Modes.DestroyMode(UFocusEditorMode::ModeId);
        }
        bModeActivated = false;
    }
    void BindView(const TSharedPtr<IDetailsView>& View)
    {
        if (!View) return;
        if (CommandGuard) CommandGuard->DiscoverComponentCommands(View);
        if (InteractionGuard) InteractionGuard->Discover(View);
        for (const auto& B : Bindings) if (B.View.Pin() == View) return;
        FBinding B;
        B.View = View;
        B.Previous = View->GetIsPropertyReadOnlyDelegate();
        const auto Prior = B.Previous;
        const TWeakPtr<IDetailsView> WeakView = View;
        auto Guard = FIsPropertyReadOnly::CreateLambda([Prior, WeakView](const FPropertyAndParent& P) {
            if (Prior.IsBound() && Prior.Execute(P)) return true;
            auto IsTransform = [](const FProperty* Property) {
                if (!Property) return false;
                const FName Name = Property->GetFName();
                return Name == TEXT("RelativeLocation") || Name == TEXT("RelativeRotation") || Name == TEXT("RelativeScale3D");
                };
            bool Transform = IsTransform(&P.Property);
            for (const FProperty* Parent : P.ParentProperties) Transform |= IsTransform(Parent);
            auto IsProtected = [Transform](UObject* Object) {
                AActor* Actor = Cast<AActor>(Object);
                if (auto* Component = Cast<UActorComponent>(Object)) Actor = Component->GetOwner();
                return Focus::Get(Actor, Focus::EControl::Edit) ||
                    (Transform && Focus::Get(Actor, Focus::EControl::Transform));
                };
            // Use actual property owners, including external component properties.
            for (const auto& Object : P.Objects) if (IsProtected(Object.Get())) return true;
            // Some property providers omit owner objects. Only then use view selection.
            if (P.Objects.IsEmpty()) if (auto Details = WeakView.Pin())
                for (const auto& Object : Details->GetSelectedObjects()) if (IsProtected(Object.Get())) return true;
            return false;
            });
        B.Installed = Guard.GetHandle();
        View->SetIsPropertyReadOnlyDelegate(Guard);
        // Custom-builder headers (including Mobility) bypass the property and
        // custom-row read-only delegates. They do inherit the category's parent
        // enabled state, supplied by IDetailsView::IsPropertyEditingEnabled().
        // Use that shared editing gate for Edit Lock. Do not disable the SDetailsView
        // widget itself: its search, category tree and other navigation stay intact.
        B.PreviousEditing = View->GetIsPropertyEditingEnabledDelegate();
        const auto PriorEditing = B.PreviousEditing;
        auto EditingGuard = FIsPropertyEditingEnabled::CreateLambda(
            [PriorEditing, WeakView]() {
                if (PriorEditing.IsBound() && !PriorEditing.Execute()) return false;
                if (auto Details = WeakView.Pin())
                {
                    for (const auto& Object : Details->GetSelectedObjects())
                    {
                        AActor* Actor = Cast<AActor>(Object.Get());
                        if (auto* Component = Cast<UActorComponent>(Object.Get())) Actor = Component->GetOwner();
                        if (Focus::Get(Actor, Focus::EControl::Edit)) return false;
                    }
                }
                return true;
            });
        B.InstalledEditing = EditingGuard.GetHandle();
        View->SetIsPropertyEditingEnabledDelegate(EditingGuard);
        Bindings.Add(B);
    }

    bool Poll(float)
    {
        if (bStopping) return false;
        TryActivate();
        if (GEditor && bRestoreWorld)
        {
            UWorld* World = GEditor->GetEditorWorldContext().World();
            if (World && World->WorldType == EWorldType::Editor)
            {
                Focus::RestoreLoadedWorld(World);
                bRestoreWorld = false;
            }
        }
        // Actor-added notifications can precede construction completion. Restore
        // on the next editor tick, including actors streamed in after map open.
        auto PendingActors = MoveTemp(PendingRestore);
        PendingRestore.Reset();
        for (const auto& Weak : PendingActors) if (Focus::IsEditorActor(Weak.Get()))
        {
            Focus::States.Remove(Weak);
            Focus::EnsureState(Weak.Get());
        }
        // Prune unloaded/deleted actors without scanning the world every tick.
        for (auto It = Focus::States.CreateIterator(); It; ++It)
            if (!It.Key().IsValid()) It.RemoveCurrent();
        // Row generation only queues weak views; install delegates outside the layout pass.
        TArray<TWeakPtr<IDetailsView>> Pending = MoveTemp(DiscoveredViews);
        DiscoveredViews.Reset();
        for (const auto& Weak : Pending) BindView(Weak.Pin());
        Bindings.RemoveAll([](const FBinding& B) { return !B.View.IsValid(); });
        auto* Module = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor");
        if (Module) for (int32 Index = 1; Index <= 4; ++Index)
            BindView(Module->FindDetailView(FName(*FString::Printf(TEXT("LevelEditorSelectionDetails%d"), Index))));
        if (InteractionGuard) InteractionGuard->Synchronize();
        return true;
    }

public:
    virtual void StartupModule() override {
        // Earlier Focus releases used #737373 as their disabled color default.
        // EditorPerProjectUserSettings can contain that serialized value, which
        // otherwise takes precedence over the new C++ default. Migrate only the
        // exact former default once; preserve all customized colors.
        if (UFocusSettings* Settings = GetMutableDefault<UFocusSettings>())
        {
            if (!Settings->bMigratedDisabledColorDefaultTo616161)
            {
                const FLinearColor PreviousDefault(.45f, .45f, .45f, 1.f);
                if (Settings->DisabledColor.Equals(PreviousDefault, KINDA_SMALL_NUMBER))
                {
                    Settings->DisabledColor = FLinearColor::FromSRGBColor(FColor(97, 97, 97, 255));
                }
                Settings->bMigratedDisabledColorDefaultTo616161 = true;
                Settings->SaveConfig();
            }
            // Former defaults used linear values as if they were sRGB, causing
            // Slate to display greys much brighter than their documented hex.
            // Migrate only those exact shipped defaults, leaving custom colors.
            if (!Settings->bMigratedSlateStateColorDefaults)
            {
                const FLinearColor PreviousDisabled(97.f / 255.f, 97.f / 255.f, 97.f / 255.f, 1.f);
                const FLinearColor OlderDisabled(.45f, .45f, .45f, 1.f);
                const FLinearColor PreviousUnavailable(.17f, .17f, .17f, 1.f);
                if (Settings->DisabledColor.Equals(PreviousDisabled, KINDA_SMALL_NUMBER) ||
                    Settings->DisabledColor.Equals(OlderDisabled, KINDA_SMALL_NUMBER))
                {
                    Settings->DisabledColor = FLinearColor::FromSRGBColor(FColor(97, 97, 97, 255));
                }
                if (Settings->NotApplicableColor.Equals(PreviousUnavailable, KINDA_SMALL_NUMBER))
                {
                    Settings->NotApplicableColor = FLinearColor::FromSRGBColor(FColor(36, 36, 36, 255));
                }
                Settings->bMigratedSlateStateColorDefaults = true;
                Settings->SaveConfig();
            }
        }
        Focus::LoadSoloIcons();
        Focus::LoadSelectionIcons();
        Focus::LoadGeometricCategoryIcons();
        NativePropertyChanged = FCoreUObjectDelegates::OnObjectPropertyChanged.AddLambda([](UObject* Object, FPropertyChangedEvent&) {
            if (!Cast<AActor>(Object) && !Cast<UActorComponent>(Object)) return;
            ActiveFocusColumns.RemoveAll([](const TWeakPtr<FFocusColumn>& Column) { return !Column.IsValid(); });
            for (auto& Weak : ActiveFocusColumns) if (auto Column = Weak.Pin()) Column->Invalidate();
        });
        auto& Module = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>("SceneOutliner");
        Columns = Module.OnCreateActorBrowserColumns().AddLambda([](FSceneOutlinerInitializationOptions& Options, UWorld*) {
            Options.ColumnMap.Add(FFocusColumn::GetID(), FSceneOutlinerColumnInfo(ESceneOutlinerColumnVisibility::Visible, 9,
                FCreateSceneOutlinerColumn::CreateLambda([](ISceneOutliner& Outliner) {
                    FocusInteraction::RegisterOutliner(Outliner);
                    TSharedRef<FFocusColumn> Column = MakeShared<FFocusColumn>();
                    ActiveFocusColumns.Add(Column);
                    return Column;
                }),
                true, TOptional<float>(), NSLOCTEXT("Focus", "ColumnLabel", "Focus")));
            // Scoped to actor-browser construction. Stock registration remains intact.
            Options.ColumnMap.Remove(FSceneOutlinerBuiltInColumnTypes::Gutter());
            });
        MapOpened = FEditorDelegates::OnMapOpened.AddLambda([this](const FString&, bool) {
            Focus::ClearSession();
            PendingRestore.Reset();
            bRestoreWorld = true;
            });
        if (GEngine) ActorAdded = GEngine->OnLevelActorAdded().AddLambda([this](AActor* Actor) {
            if (!bStopping && Focus::IsEditorActor(Actor)) PendingRestore.Add(Actor);
            });
        LevelAdded = FWorldDelegates::LevelAddedToWorld.AddLambda([this](ULevel* Level, UWorld* World) {
            if (bStopping || !Level || !World || World->WorldType != EWorldType::Editor) return;
            for (AActor* Actor : Level->Actors) if (IsValid(Actor)) PendingRestore.Add(Actor);
            });
        ObjectsReplaced = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
            [this](const TMap<UObject*, UObject*>& Replacements) {
                if (bStopping) return;
                for (const auto& Pair : Replacements)
                {
                    AActor* OldActor = Cast<AActor>(Pair.Key);
                    AActor* NewActor = Cast<AActor>(Pair.Value);
                    if (!OldActor || !Focus::IsEditorActor(NewActor)) continue;
                    // Blueprint recompilation may replace an actor object. Copy
                    // only Focus's own metadata, never unrelated package data.
                    const FString Saved = OldActor->GetPackage()->GetMetaData().GetValue(OldActor, Focus::SharedStateKey);
                    Focus::FFlags Flags;
                    bool Hidden = false;
                    if (Focus::DecodeSharedState(Saved, Flags, Hidden))
                    {
                        FMetaData& Meta = NewActor->GetPackage()->GetMetaData();
                        if (Meta.GetValue(NewActor, Focus::SharedStateKey) != Saved)
                        {
                            Meta.SetValue(NewActor, Focus::SharedStateKey, *Saved);
                            NewActor->GetPackage()->MarkPackageDirty();
                        }
                    }
                    Focus::States.Remove(OldActor);
                    Focus::States.Remove(NewActor);
                    PendingRestore.Add(NewActor);
                }
            });
        PreExit = FEditorDelegates::OnEditorPreExit.AddRaw(this, &FFocusModule::StopMode);
        auto& Properties = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
        RowExtensions = Properties.GetGlobalRowExtensionDelegate().AddLambda(
            [this](const FOnGenerateGlobalRowExtensionArgs& Args, TArray<FPropertyRowExtensionButton>&) {
                if (bStopping) return;
                if (auto Node = Args.OwnerTreeNode.Pin()) if (auto View = Node->GetNodeDetailsViewSharedPtr())
                    DiscoveredViews.AddUnique(TWeakPtr<IDetailsView>(View));
            });
        ViewExtension = FSceneViewExtensions::NewExtension<FFocusViewExtension>();
        Ticker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FFocusModule::Poll), .25f);
    }
    virtual void ShutdownModule() override {
        bStopping = true;
        StopInteractionGuard();
        CommandGuard.Reset();
        TransformGuard.Reset();
        FTSTicker::GetCoreTicker().RemoveTicker(Ticker);
        if (auto* Properties = FModuleManager::GetModulePtr<FPropertyEditorModule>("PropertyEditor"))
            Properties->GetGlobalRowExtensionDelegate().Remove(RowExtensions);
        DiscoveredViews.Empty();
        FEditorDelegates::OnMapOpened.Remove(MapOpened);
        if (GEngine) GEngine->OnLevelActorAdded().Remove(ActorAdded);
        FWorldDelegates::LevelAddedToWorld.Remove(LevelAdded);
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(ObjectsReplaced);
        FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(NativePropertyChanged);
        ActiveFocusColumns.Empty();
        PendingRestore.Reset();
        if (auto* Module = FModuleManager::GetModulePtr<FSceneOutlinerModule>("SceneOutliner")) Module->OnCreateActorBrowserColumns().Remove(Columns);
        for (const auto& B : Bindings) if (auto V = B.View.Pin())
        {
            if (V->GetIsPropertyReadOnlyDelegate().GetHandle() == B.Installed)
                V->SetIsPropertyReadOnlyDelegate(B.Previous);
            if (V->GetIsPropertyEditingEnabledDelegate().GetHandle() == B.InstalledEditing)
                V->SetIsPropertyEditingEnabledDelegate(B.PreviousEditing);
        }
        Bindings.Empty();
        FEditorDelegates::OnEditorPreExit.Remove(PreExit);
        StopMode();
        Focus::ClearSession(); ViewExtension.Reset();
        Focus::SoloOutline.Reset();
        Focus::SoloSolid.Reset();
        Focus::SoloMixed.Reset();
        Focus::SelectionUnlocked.Reset();
        Focus::SelectionLocked.Reset();
    }
    virtual bool SupportsDynamicReloading() override { return false; }
};
IMPLEMENT_MODULE(FFocusModule, Focus)
