#include "FocusState.h"
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
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"
#include "Styling/AppStyle.h"
#include "Brushes/SlateImageBrush.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/MetaData.h"
#include "UObject/Package.h"
#include "Misc/Change.h"
#include "Misc/ITransaction.h"
#include "Engine/Level.h"

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
class FFocusColumn : public ISceneOutlinerColumn
{
    struct FRowState
    {
        TWeakPtr<ISceneOutlinerTreeItem> Item;
        int32 Total = 0;
        int32 Enabled[6] = {};
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
                if (Row->State(C) == 0) return FSlateColor(FLinearColor(.45f, .45f, .45f));
                if (Row->State(C) == 2 || C == EControl::Solo) return FSlateColor(FLinearColor::White);
                return FSlateColor(C == EControl::GameHidden ? FLinearColor::Red : FLinearColor::Yellow);
                });
            if (C == EControl::EditorHidden || C == EControl::GameHidden)
            {
                // Geometry, rather than a font glyph, keeps the dot optically centered.
                static const FSlateRoundedBoxBrush DotBrush(FLinearColor::White, 3.f, FVector2D(6.f, 6.f));
                Visual = SNew(SBox).WidthOverride(6.f)
                    .HeightOverride_Lambda([Row, C] { return FOptionalSize(Row->State(C) == 2 ? 2.f : 6.f); })
                    [SNew(SImage).Image(&DotBrush).ColorAndOpacity_Lambda([Row, C] {
                    const int32 State = Row->State(C);
                    if (State == 0) return FSlateColor(FLinearColor(.45f, .45f, .45f));
                    if (State == 2) return FSlateColor(FLinearColor::White);
                    return FSlateColor(C == EControl::GameHidden ? FLinearColor::Red : FLinearColor::Yellow);
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
                                FLinearColor(.45f, .45f, .45f) : FLinearColor::White);
                            })]
                    ];
            }
            if (Lock) Visual = SNew(SOverlay)
                + SOverlay::Slot()[SNew(SImage).Image_Lambda([Row, C] {
                return FAppStyle::GetBrush(Row->State(C) == 0 ? TEXT("Icons.Unlock") : TEXT("Icons.Lock"));
                    }).ColorAndOpacity_Lambda([Row, C] {
                        if (Row->State(C) == 0) return FSlateColor(FLinearColor(.45f, .45f, .45f));
                        if (Row->State(C) == 2) return FSlateColor(FLinearColor::White);
                        return FSlateColor(C == EControl::Selection ? FLinearColor::White : C == EControl::Edit ? FLinearColor::Red : FLinearColor::Yellow);
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
                                return FSlateColor(Row->State(EControl::Selection) == 0 ? FLinearColor(.45f, .45f, .45f) : FLinearColor::White);
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
                    case 0: return FSlateColor(FLinearColor(.45f, .45f, .45f));
                    case 1: return FSlateColor(FLinearColor::White);
                    case 2: return FSlateColor(FLinearColor::Red);
                    default: return FSlateColor(FLinearColor::White);
                    }
                    })]
            + SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom)
                [SNew(STextBlock).Text(FText::FromString(TEXT("-")))
                .Visibility_Lambda([Row] { return ProtectionState(*Row) == 3 ? EVisibility::HitTestInvisible : EVisibility::Collapsed; })]
            ]]
            ];
    }
public:
    // Stable internal identifier; the displayed column name is "Focus".
    static FName GetID() { return TEXT("Focus.PrototypeControls"); }
    virtual FName GetColumnID() override { return GetID(); }
    virtual void Tick(double InCurrentTime, float) override
    {
        if (InCurrentTime < NextRefresh) return;
        NextRefresh = InCurrentTime + .25;
        Rows.RemoveAll([](const TWeakPtr<FRowState>& Row) { return !Row.IsValid(); });
        if (Rows.IsEmpty()) return;
        // One world enumeration per refresh; Slate attributes only read aggregate counts.
        const auto Loaded = LoadedActors();
        for (const auto& Weak : Rows) if (const auto Row = Weak.Pin()) RefreshRow(*Row, Loaded);
    }
    virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override
    {
        return SHeaderRow::Column(GetID()).DefaultLabel(NSLOCTEXT("Focus", "Header", "Focus")).FixedWidth(78.f);
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
        // 14px dot column + three 20px buttons + 2px on each outside edge.
        return SNew(SBox).Padding(FMargin(2.f, 0.f)).VAlign(VAlign_Center)
            [SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[SNew(SVerticalBox)
            + SVerticalBox::Slot().AutoHeight()[Control(Row, EControl::EditorHidden)]
            + SVerticalBox::Slot().AutoHeight()[Control(Row, EControl::GameHidden)]]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[Control(Row, EControl::Solo)]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[Control(Row, EControl::Selection)]
            + SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)[ProtectionControl(Row)]
            ];
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
    FDelegateHandle ActorAdded, LevelAdded, ObjectsReplaced;
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
        Focus::LoadSoloIcons();
        Focus::LoadSelectionIcons();
        auto& Module = FModuleManager::LoadModuleChecked<FSceneOutlinerModule>("SceneOutliner");
        Columns = Module.OnCreateActorBrowserColumns().AddLambda([](FSceneOutlinerInitializationOptions& Options, UWorld*) {
            Options.ColumnMap.Add(FFocusColumn::GetID(), FSceneOutlinerColumnInfo(ESceneOutlinerColumnVisibility::Visible, 9,
                FCreateSceneOutlinerColumn::CreateLambda([](ISceneOutliner& Outliner) { FocusInteraction::RegisterOutliner(Outliner); return MakeShared<FFocusColumn>(); }),
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
