// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// Private implementation included by FocusModule.cpp after its session-state definitions.
// Guards editor UI entry points only. Blueprint and native engine mutations are not intercepted.
#include "ActorPickerMode.h"
#include "InputCoreTypes.h"
#include "Textures/SlateIcon.h"
#include "Input/Events.h"
#include "Input/DragAndDrop.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Layout/WidgetPath.h"
#include "ISceneOutlinerMode.h"
#include "SceneOutlinerDragDrop.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/SWindow.h"

namespace FocusInteraction
{
    struct FRow
    {
        TWeakPtr<SWidget> Widget;
        TWeakPtr<ISceneOutlinerTreeItem> Item;
        TWeakPtr<SInlineEditableTextBlock> Label;
    };
    inline TArray<FRow> Rows;
    inline TArray<TWeakPtr<ISceneOutliner>> Outliners;
    struct FEditorView
    {
        TWeakPtr<SSubobjectEditor> Editor;
        TWeakPtr<IDetailsView> View;
    };
    inline TArray<FEditorView> EditorViews;

    inline void RegisterOutliner(ISceneOutliner& Outliner)
    {
        Outliners.AddUnique(StaticCastSharedRef<ISceneOutliner>(Outliner.AsShared()));
    }
    inline void RegisterRow(FSceneOutlinerTreeItemRef Item, const STableRow<FSceneOutlinerTreeItemPtr>& Row)
    {
        FRow Entry;
        Entry.Widget = ConstCastSharedRef<SWidget>(Row.AsShared());
        Entry.Item = Item;
        Rows.Add(MoveTemp(Entry));
    }
    inline bool EditorLocked(const TWeakPtr<SSubobjectEditor>& Weak)
    {
        const auto Editor = Weak.Pin();
        if (!Editor) return false;
        UObject* Object = Editor->GetObjectContext();
        AActor* Actor = Cast<AActor>(Object);
        if (auto* Component = Cast<UActorComponent>(Object)) Actor = Component->GetOwner();
        if (Actor) return Focus::Get(Actor, Focus::EControl::Edit);
        // UE can return a null object context for a multi-actor Details panel.
        // Use that panel's objects, not the global selection (which may differ).
        for (const FEditorView& Entry : EditorViews) if (Entry.Editor.Pin() == Editor)
            if (auto View = Entry.View.Pin()) for (const auto& WeakObject : View->GetSelectedObjects())
            {
                AActor* Owner = Cast<AActor>(WeakObject.Get());
                if (auto* Component = Cast<UActorComponent>(WeakObject.Get())) Owner = Component->GetOwner();
                if (Focus::Get(Owner, Focus::EControl::Edit)) return true;
            }
        return false;
    }
    inline bool InFolder(const AActor* Actor, const FFolder& Folder)
    {
        if (!Focus::IsEditorActor(Actor)) return false;
        const FFolder Current = Actor->GetFolder();
        return Current.GetRootObject() == Folder.GetRootObject() &&
            (Current.GetPath() == Folder.GetPath() || Current.GetPath().ToString().StartsWith(
                Folder.GetPath().ToString() + TEXT("/"), ESearchCase::IgnoreCase));
    }
    inline bool LabelLocked(const TWeakPtr<ISceneOutlinerTreeItem>& Weak)
    {
        const auto Item = Weak.Pin();
        if (!Item) return false;
        if (const auto* Actor = Item->CastTo<FActorTreeItem>())
            return Focus::Get(Actor->Actor.Get(), Focus::EControl::Edit);
        if (const auto* Folder = Item->CastTo<FFolderTreeItem>())
        {
            // Renaming a folder changes the folder paths of its contained actors.
            for (const auto& State : Focus::States)
                if (State.Value.Edit && InFolder(State.Key.Get(), Folder->GetFolder())) return true;
        }
        return false;
    }
    inline bool ActorStructureLocked(AActor* Root)
    {
        TArray<AActor*> Pending;
        TSet<AActor*> Seen;
        Pending.Add(Root);
        while (!Pending.IsEmpty())
        {
            AActor* Actor = Pending.Pop(EAllowShrinking::No);
            if (!Focus::IsEditorActor(Actor) || Seen.Contains(Actor)) continue;
            Seen.Add(Actor);
            if (Focus::Get(Actor, Focus::EControl::Edit)) return true;
            TArray<AActor*> Children;
            Actor->GetAttachedActors(Children);
            Pending.Append(Children);
            if (auto* Group = Cast<AGroupActor>(Actor))
            {
                for (const auto& Member : Group->GroupActors) Pending.Add(Member.Get());
                for (const auto& Subgroup : Group->SubGroups) Pending.Add(Subgroup.Get());
            }
        }
        // Moving a child out also changes its former parent's attachment hierarchy.
        return Focus::IsEditorActor(Root) && Focus::Get(Root->GetAttachParentActor(), Focus::EControl::Edit);
    }
    inline bool ItemStructureLocked(const FSceneOutlinerTreeItemPtr& Item)
    {
        if (!Item) return false;
        if (const auto* Actor = Item->CastTo<FActorTreeItem>()) return ActorStructureLocked(Actor->Actor.Get());
        if (const auto* Folder = Item->CastTo<FFolderTreeItem>())
        {
            UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
            if (World) for (TActorIterator<AActor> It(World); It; ++It)
                if (InFolder(*It, Folder->GetFolder()) && ActorStructureLocked(*It)) return true;
        }
        return false;
    }
    inline void NotifyBlocked()
    {
        FNotificationInfo Info(NSLOCTEXT("Focus", "InteractionBlocked",
            "Focus: turn off Edit Lock before changing this actor or its component hierarchy."));
        Info.ExpireDuration = 3.f;
        FSlateNotificationManager::Get().AddNotification(Info);
    }
}

/** All callbacks keep weak editor/actor references and a shared lifetime token.
 * No read-only attribute, Outliner mode or engine widget delegate is overwritten
 * without retaining its original value. Widget gates preserve the original content.
 */
class FFocusInteractionGuard : public IInputProcessor
{
    struct FLifetime { bool bActive = true; };
    TSharedRef<FLifetime> Lifetime = MakeShared<FLifetime>();
    struct FGate
    {
        TWeakPtr<SHorizontalBox> Parent;
        TWeakPtr<SWidget> Original;
        TWeakPtr<SBox> Box;
        TWeakPtr<SInlineEditableTextBlock> Inline;
        TFunction<bool()> Blocked;
        TArray<TWeakPtr<SComboButton>> Menus;
    };
    struct FRename
    {
        TWeakPtr<ISceneOutlinerTreeItem> Item;
        TWeakPtr<FSubobjectEditorTreeNode> Node;
        TWeakPtr<SWidget> Label;
        TFunction<void()> Restore;
    };
    TArray<FGate> Gates;
    TArray<FRename> Renames;
    TArray<TWeakPtr<SSubobjectEditor>> Editors;
    TWeakPtr<SSubobjectEditor> DragSourceEditor;
    bool bSynchronizing = false;
    TWeakPtr<FUICommandList> PickerList;
    FUIAction PriorPickerAction;
    FDelegateHandle PickerExecuteHandle;
    FDelegateHandle PickerCanExecuteHandle;

    // Walk only the supplied widget subtree. Do not search all editor windows per frame.
    static void Walk(const TSharedRef<SWidget>& Root, TFunctionRef<bool(const TSharedRef<SWidget>&)> Visit)
    {
        TArray<TSharedRef<SWidget>> Pending;
        Pending.Add(Root);
        while (!Pending.IsEmpty())
        {
            const auto Widget = Pending.Pop(EAllowShrinking::No);
            if (!Visit(Widget)) continue;
            if (FChildren* Children = Widget->GetChildren())
                for (int32 I = 0; I < Children->Num(); ++I) Pending.Add(Children->GetChildAt(I));
        }
    }
    bool IsGated(const TSharedRef<SWidget>& Widget) const
    {
        for (const FGate& Gate : Gates) if (Gate.Original.Pin() == Widget) return true;
        return false;
    }
    bool InstallGate(const TSharedRef<SWidget>& Widget, TFunction<bool()> Blocked, bool bInline)
    {
        if (IsGated(Widget)) return false;
        const auto ParentWidget = Widget->GetParentWidget();
        // Confirmed parent layout in UE 5.8 ActorTreeItem and SSubobjectEditor.
        // If another extension uses a different layout, leave its widgets intact.
        if (!ParentWidget || ParentWidget->GetType() != FName(TEXT("SHorizontalBox"))) return false;
        const auto Parent = StaticCastSharedPtr<SHorizontalBox>(ParentWidget);
        FChildren* Children = Parent->GetChildren();
        for (int32 I = 0; I < Children->Num(); ++I)
        {
            if (Children->GetChildAt(I) != Widget) continue;
            FGate Gate;
            Gate.Parent = Parent;
            Gate.Original = Widget;
            Gate.Blocked = Blocked;
            if (bInline) Gate.Inline = StaticCastSharedRef<SInlineEditableTextBlock>(Widget);
            else Walk(Widget, [&Gate](const TSharedRef<SWidget>& Child) {
                if (Child->GetType() == FName(TEXT("SComboButton")))
                    Gate.Menus.Add(StaticCastSharedRef<SComboButton>(Child));
                return true;
            });
            const auto Life = Lifetime;
            const TWeakPtr<SWidget> WeakOriginal = Widget;
            // Detach before reparenting: a Slate widget must have only one parent.
            Parent->GetSlot(I).AttachWidget(SNullWidget::NullWidget);
            const auto Box = SNew(SBox)
                .Visibility_Lambda([WeakOriginal] {
                    const auto Original = WeakOriginal.Pin();
                    return Original ? Original->GetVisibility() : EVisibility::Collapsed;
                })
                .IsEnabled_Lambda([Life, Blocked] { return !Life->bActive || !Blocked(); })
                [Widget];
            Gate.Box = Box;
            Parent->GetSlot(I).AttachWidget(Box);
            Gates.Add(MoveTemp(Gate));
            return true;
        }
        return false;
    }
    void BindRename(const TSharedPtr<ISceneOutlinerTreeItem>& Item,
        const TSharedPtr<FSubobjectEditorTreeNode>& Node, const TSharedRef<SWidget>& Label, TFunction<bool()> Blocked)
    {
        // Retain each engine delegate in its own type so it can be restored exactly.
        const auto Life = Lifetime;
        FRename Record;
        Record.Item = Item;
        Record.Node = Node;
        Record.Label = Label;
        if (Item)
        {
            const auto Previous = Item->RenameRequestEvent;
            auto Guard = ISceneOutlinerTreeItem::FOnRenameRequest::CreateLambda([Life, Blocked, Previous] {
                if (!Life->bActive || !Blocked()) Previous.ExecuteIfBound();
            });
            const FDelegateHandle Installed = Guard.GetHandle();
            Item->RenameRequestEvent = Guard;
            // Exact original restoration is kept in a callback, below.
            Record.Restore = [Weak = TWeakPtr<ISceneOutlinerTreeItem>(Item), Previous, Handle = Installed] {
                if (auto Pinned = Weak.Pin()) if (Pinned->RenameRequestEvent.GetHandle() == Handle)
                    Pinned->RenameRequestEvent = Previous;
            };
        }
        else if (Node)
        {
            const auto Previous = Node->GetRenameRequestedDelegate();
            auto Guard = FSubobjectEditorTreeNode::FOnRenameRequested::CreateLambda([Life, Blocked, Previous] {
                if (!Life->bActive || !Blocked()) Previous.ExecuteIfBound();
            });
            const FDelegateHandle Installed = Guard.GetHandle();
            Node->SetRenameRequestedDelegate(Guard);
            Record.Restore = [Weak = TWeakPtr<FSubobjectEditorTreeNode>(Node), Previous, Handle = Installed] {
                if (auto Pinned = Weak.Pin()) if (Pinned->GetRenameRequestedDelegate().GetHandle() == Handle)
                    Pinned->SetRenameRequestedDelegate(Previous);
            };
        }
        Renames.Add(MoveTemp(Record));
    }

    void GuardOutlinerRow(FocusInteraction::FRow& Row)
    {
        if (Row.Label.IsValid()) return;
        const auto Widget = Row.Widget.Pin();
        const auto Item = Row.Item.Pin();
        if (!Widget || !Item) return;
        const auto WeakItem = Row.Item;
        auto Blocked = [WeakItem] { return FocusInteraction::LabelLocked(WeakItem); };
        Walk(Widget.ToSharedRef(), [&](const TSharedRef<SWidget>& Child) {
            if (Child->GetType() != FName(TEXT("SInlineEditableTextBlock"))) return true;
            if (InstallGate(Child, Blocked, true)) BindRename(Item, nullptr, Child, Blocked);
            Row.Label = StaticCastSharedRef<SInlineEditableTextBlock>(Child);
            return false;
        });
    }
    void GuardComponentEditor(const TSharedPtr<SSubobjectEditor>& Editor)
    {
        if (!Editor) return;
        const TWeakPtr<SSubobjectEditor> Weak = Editor;
        auto Blocked = [Weak] { return FocusInteraction::EditorLocked(Weak); };
        if (auto Buttons = Editor->GetToolButtonsBox()) Walk(Buttons.ToSharedRef(), [&](const TSharedRef<SWidget>& Widget) {
            if (Widget->GetType() != FName(TEXT("SComponentClassCombo"))) return true;
            InstallGate(Widget, Blocked, false);
            return false;
        });
        if (auto Tree = Editor->GetDragDropTree()) Walk(Tree.ToSharedRef(), [&](const TSharedRef<SWidget>& Widget) {
            if (Widget->GetType() != FName(TEXT("SSubobject_RowWidget"))) return true;
            const auto Row = StaticCastSharedRef<SSubobject_RowWidget>(Widget);
            const auto Node = Row->GetSubobjectPtr();
            Walk(Widget, [&](const TSharedRef<SWidget>& Child) {
                if (Child->GetType() != FName(TEXT("SInlineEditableTextBlock"))) return true;
                if (InstallGate(Child, Blocked, true)) BindRename(nullptr, Node, Child, Blocked);
                return false;
            });
            return false;
        });
    }
    void CancelLockedEditors()
    {
        for (const FGate& Gate : Gates)
        {
            if (!Gate.Blocked()) continue;
            // ExitEditingMode switches back to the label before restoring focus;
            // it does not commit a new label. Retain the engine's original validators.
            if (auto Inline = Gate.Inline.Pin()) if (Inline->IsInEditMode()) Inline->ExitEditingMode();
            for (const auto& WeakMenu : Gate.Menus) if (auto Menu = WeakMenu.Pin())
                if (Menu->IsOpen()) Menu->SetIsOpen(false, false);
        }
    }
    static TSharedPtr<SSubobjectEditor> EditorInPath(const FWidgetPath& Path)
    {
        for (int32 I = Path.Widgets.Num() - 1; I >= 0; --I)
            if (Path.Widgets[I].Widget->GetType() == FName(TEXT("SSubobjectInstanceEditor")))
                return StaticCastSharedRef<SSubobjectEditor>(Path.Widgets[I].Widget);
        return nullptr;
    }
    static bool PathContains(const FWidgetPath& Path, const TSharedPtr<SWidget>& Widget)
    {
        if (!Widget) return false;
        for (int32 I = 0; I < Path.Widgets.Num(); ++I) if (Path.Widgets[I].Widget == Widget) return true;
        return false;
    }
    // A path can reveal an Outliner whose Focus column is currently hidden.
    // Use the table's public item lookup, never a private Outliner row field.
    void ObservePath(const FWidgetPath& Path)
    {
        TSharedPtr<ISceneOutliner> Outliner;
        for (int32 I = 0; I < Path.Widgets.Num(); ++I)
        {
            const auto Widget = Path.Widgets[I].Widget;
            if (Widget->GetType() == FName(TEXT("SSceneOutliner")))
            {
                Outliner = StaticCastSharedRef<ISceneOutliner>(Widget);
                // Restrict to Outliners already registered by Focus's actor-browser column.
                // Actor/reference picker Outliners must not have their editing behavior changed.
                bool bKnown = false;
                for (const auto& Weak : FocusInteraction::Outliners) bKnown |= Weak.Pin() == Outliner;
                if (!bKnown) Outliner.Reset();
            }
            if (Outliner && Widget->GetType() == FName(TEXT("SSceneOutlinerTreeRow")))
            {
                const auto Row = StaticCastSharedRef<STableRow<FSceneOutlinerTreeItemPtr>>(Widget);
                const auto* Item = Outliner->GetTree().ItemFromWidget(&Row.Get());
                if (Item && *Item)
                {
                    bool bKnown = false;
                    for (const auto& Entry : FocusInteraction::Rows) bKnown |= Entry.Widget.Pin() == Widget;
                    if (!bKnown) FocusInteraction::RegisterRow((*Item).ToSharedRef(), Row.Get());
                }
            }
        }
        if (auto Editor = EditorInPath(Path)) Editors.AddUnique(Editor);
    }
    bool DropBlocked(const FWidgetPath& Path, const TSharedPtr<FDragDropOperation>& Operation) const
    {
        if (!Operation) return false;
        if (const auto Editor = EditorInPath(Path))
            return FocusInteraction::EditorLocked(Editor) || FocusInteraction::EditorLocked(DragSourceEditor);
        for (const auto& Weak : FocusInteraction::Outliners)
        {
            const auto Outliner = Weak.Pin();
            if (!Outliner || !PathContains(Path, Outliner)) continue;
            if (FocusInteraction::EditorLocked(DragSourceEditor)) return true;
            // Protect the destination actor, including actors in other Outliner tabs.
            for (const auto& Row : FocusInteraction::Rows) if (PathContains(Path, Row.Widget.Pin()))
                if (const auto Item = Row.Item.Pin()) if (const auto* Actor = Item->CastTo<FActorTreeItem>())
                    if (Focus::Get(Actor->Actor.Get(), Focus::EControl::Edit)) return true;
            FSceneOutlinerDragDropPayload Payload(*Operation);
            if (Outliner->GetMode() && Outliner->GetMode()->ParseDragDrop(Payload, *Operation))
                for (const auto& Item : Payload.DraggedItems)
                    if (FocusInteraction::ItemStructureLocked(Item.Pin())) return true;
        }
        return false;
    }
    static TArray<TWeakObjectPtr<AActor>> SelectedActors()
    {
        TArray<TWeakObjectPtr<AActor>> Result;
        if (GEditor) if (auto* Selection = GEditor->GetSelectedActors())
            for (FSelectionIterator It(*Selection); It; ++It)
                if (auto* Actor = Cast<AActor>(*It)) if (Focus::IsEditorActor(Actor)) Result.AddUnique(Actor);
        return Result;
    }
    static bool CanAttach(const TArray<TWeakObjectPtr<AActor>>& Actors, AActor* Parent)
    {
        if (!GEditor || !Focus::IsEditorActor(Parent) || Focus::Get(Parent, Focus::EControl::Edit) || Actors.IsEmpty()) return false;
        const auto Current = SelectedActors();
        if (Current.Num() != Actors.Num()) return false;
        for (const auto& Weak : Actors)
        {
            AActor* Actor = Weak.Get();
            if (!Focus::IsEditorActor(Actor) || !Current.Contains(Weak) ||
                FocusInteraction::ActorStructureLocked(Actor) || !GEditor->CanParentActors(Parent, Actor)) return false;
        }
        return FLevelEditorActionCallbacks::IsAttachableActor(Parent);
    }
    void BindPicker(FLevelEditorModule& Module)
    {
        const auto List = Module.GetGlobalLevelEditorActions();
        const auto Command = FLevelEditorCommands::Get().AttachActorIteractive;
        const FUIAction* Existing = List->GetActionForCommand(Command);
        if (!Existing) return;
        PickerList = List;
        PriorPickerAction = *Existing;
        const auto Prior = PriorPickerAction;
        const auto Life = Lifetime;
        FUIAction Guarded = Prior;
        Guarded.CanExecuteAction = FCanExecuteAction::CreateLambda([Life, Prior] {
            if (Prior.CanExecuteAction.IsBound() && !Prior.CanExecuteAction.Execute()) return false;
            if (!Life->bActive) return true;
            const auto Actors = SelectedActors();
            if (Actors.IsEmpty()) return false;
            for (const auto& Actor : Actors) if (FocusInteraction::ActorStructureLocked(Actor.Get())) return false;
            return true;
        });
        Guarded.ExecuteAction = FExecuteAction::CreateLambda([Life, Prior] {
            if (!Life->bActive) { Prior.ExecuteAction.ExecuteIfBound(); return; }
            if (Prior.CanExecuteAction.IsBound() && !Prior.CanExecuteAction.Execute()) return;
            const auto Actors = SelectedActors();
            if (Actors.IsEmpty()) return;
            for (const auto& Actor : Actors) if (FocusInteraction::ActorStructureLocked(Actor.Get()))
                { FocusInteraction::NotifyBlocked(); return; }
            auto& Picker = FModuleManager::LoadModuleChecked<FActorPickerModeModule>(TEXT("ActorPickerMode"));
            Picker.BeginActorPickingMode(FOnGetAllowedClasses(),
                FOnShouldFilterActor::CreateLambda([Life, Actors](const AActor* Parent) {
                    return Life->bActive && CanAttach(Actors, const_cast<AActor*>(Parent));
                }),
                FOnActorSelected::CreateLambda([Life, Actors](AActor* Parent) {
                    if (!Life->bActive) return;
                    if (!CanAttach(Actors, Parent)) { FocusInteraction::NotifyBlocked(); return; }
                    const TWeakObjectPtr<AActor> WeakParent = Parent;
                    auto Commit = [Life, Actors, WeakParent](FName Socket) {
                        if (!Life->bActive) return;
                        AActor* Target = WeakParent.Get();
                        if (!CanAttach(Actors, Target)) { FocusInteraction::NotifyBlocked(); return; }
                        if (!Socket.IsNone() && (!Target->GetRootComponent() || !Target->GetRootComponent()->DoesSocketExist(Socket)))
                            { FocusInteraction::NotifyBlocked(); return; }
                        // Native handler retains Undo/Redo, attachment rules and editor notifications.
                        FLevelEditorActionCallbacks::AttachToSocketSelection(Socket, Target);
                    };
                    USceneComponent* Root = Parent->GetRootComponent();
                    if (!Root || !Root->HasAnySockets()) { Commit(NAME_None); return; }
                    // Recheck at socket selection, not just before opening the socket menu.
                    FMenuBuilder Menu(true, nullptr);
                    Menu.AddMenuEntry(NSLOCTEXT("Focus", "AttachRoot", "Actor root (no socket)"),
                        FText::GetEmpty(), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([Commit] { Commit(NAME_None); })));
                    for (const FName Socket : Root->GetAllSocketNames())
                        Menu.AddMenuEntry(FText::FromName(Socket), FText::GetEmpty(), FSlateIcon(),
                            FUIAction(FExecuteAction::CreateLambda([Commit, Socket] { Commit(Socket); })));
                    if (auto Window = FSlateApplication::Get().GetActiveTopLevelWindow())
                        FSlateApplication::Get().PushMenu(Window.ToSharedRef(), FWidgetPath(), Menu.MakeWidget(),
                            FSlateApplication::Get().GetCursorPos(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
                }));
        });
        PickerExecuteHandle = Guarded.ExecuteAction.GetHandle();
        PickerCanExecuteHandle = Guarded.CanExecuteAction.GetHandle();
        List->UnmapAction(Command);
        List->MapAction(Command, Guarded);
    }
public:
    explicit FFocusInteractionGuard(FLevelEditorModule& Module) { BindPicker(Module); }
    void Discover(const TSharedPtr<IDetailsView>& View)
    {
        for (const auto& Entry : FocusInteraction::EditorViews)
            if (Entry.View.Pin() == View && Entry.Editor.IsValid()) return;
        TSharedPtr<SWidget> Host = View;
        while (Host && Host->GetType() != FName(TEXT("SActorDetails"))) Host = Host->GetParentWidget();
        if (!Host) return;
        // Use actual panel context, including pinned Details panels.
        Walk(Host.ToSharedRef(), [this, View](const TSharedRef<SWidget>& Widget) {
            if (Widget->GetType() != FName(TEXT("SSubobjectInstanceEditor"))) return true;
            const auto Editor = StaticCastSharedRef<SSubobjectEditor>(Widget);
            Editors.AddUnique(Editor);
            bool bKnown = false;
            for (auto& Entry : FocusInteraction::EditorViews) if (Entry.Editor.Pin() == Editor)
                { Entry.View = View; bKnown = true; break; }
            if (!bKnown)
            {
                FocusInteraction::FEditorView Entry;
                Entry.Editor = Editor;
                Entry.View = View;
                FocusInteraction::EditorViews.Add(MoveTemp(Entry));
            }
            return false;
        });
    }
    void Synchronize()
    {
        if (!Lifetime->bActive || bSynchronizing) return;
        TGuardValue<bool> Guard(bSynchronizing, true);
        FocusInteraction::Rows.RemoveAll([](const FocusInteraction::FRow& Row) { return !Row.Widget.IsValid() || !Row.Item.IsValid(); });
        FocusInteraction::Outliners.RemoveAll([](const auto& Weak) { return !Weak.IsValid(); });
        Editors.RemoveAll([](const auto& Weak) { return !Weak.IsValid(); });
        FocusInteraction::EditorViews.RemoveAll([](const auto& Entry) { return !Entry.Editor.IsValid() || !Entry.View.IsValid(); });
        Gates.RemoveAll([](const FGate& Gate) { return !Gate.Box.IsValid(); });
        Renames.RemoveAll([](FRename& Rename) {
            if (Rename.Label.IsValid()) return false;
            if (Rename.Restore) Rename.Restore();
            return true;
        });
        for (auto& Row : FocusInteraction::Rows) GuardOutlinerRow(Row);
        for (const auto& Weak : Editors) GuardComponentEditor(Weak.Pin());
        CancelLockedEditors();
    }
    virtual void Tick(float, FSlateApplication&, TSharedRef<ICursor>) override
    {
        // Only cached, live widgets are checked here. Structural discovery runs at
        // the module's existing interval and immediately before relevant user input.
        if (Lifetime->bActive) CancelLockedEditors();
    }
    virtual bool HandleKeyDownEvent(FSlateApplication& App, const FKeyEvent& Event) override
    {
        if (auto Focused = App.GetUserFocusedWidget(Event.GetUserIndex()))
        {
            FWidgetPath Path;
            if (App.GeneratePathToWidgetUnchecked(Focused.ToSharedRef(), Path)) ObservePath(Path);
        }
        if (Event.GetKey() == EKeys::F2)
        {
            // Keyboard-only selection works even while the Focus column is hidden.
            for (const auto& Weak : FocusInteraction::Outliners) if (auto Outliner = Weak.Pin())
                for (const auto& Item : Outliner->GetTree().GetSelectedItems())
                    if (auto Row = Outliner->GetTree().WidgetFromItem(Item))
                    {
                        const auto Widget = Row->AsWidget();
                        bool bKnown = false;
                        for (const auto& Entry : FocusInteraction::Rows) bKnown |= Entry.Widget.Pin() == Widget;
                        if (!bKnown)
                        {
                            FocusInteraction::FRow Entry;
                            Entry.Widget = Widget;
                            Entry.Item = Item;
                            FocusInteraction::Rows.Add(MoveTemp(Entry));
                        }
                    }
            Synchronize();
        }
        else CancelLockedEditors();
        return false; // Preserve commands, search, navigation and text entry elsewhere.
    }
    virtual bool HandleMouseButtonDownEvent(FSlateApplication& App, const FPointerEvent& Event) override
    {
        const auto Path = App.LocateWindowUnderMouse(Event.GetScreenSpacePosition(), App.GetInteractiveTopLevelWindows(), false, Event.GetUserIndex());
        ObservePath(Path);
        Synchronize();
        if (Event.GetEffectingButton() == EKeys::LeftMouseButton) DragSourceEditor = EditorInPath(Path);
        return false;
    }
    virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& App, const FPointerEvent& Event) override
    {
        const auto Path = App.LocateWindowUnderMouse(Event.GetScreenSpacePosition(), App.GetInteractiveTopLevelWindows(), false, Event.GetUserIndex());
        ObservePath(Path);
        Synchronize();
        return false;
    }
    virtual bool HandleMouseButtonUpEvent(FSlateApplication& App, const FPointerEvent& Event) override
    {
        if (Event.GetEffectingButton() != EKeys::LeftMouseButton) return false;
        const auto Operation = App.GetDragDroppingContent();
        if (Operation)
        {
            const auto Path = App.LocateWindowUnderMouse(Event.GetScreenSpacePosition(), App.GetInteractiveTopLevelWindows(), false, Event.GetUserIndex());
            ObservePath(Path);
            Synchronize();
            if (DropBlocked(Path, Operation))
            {
                App.CancelDragDrop();
                FocusInteraction::NotifyBlocked();
            }
        }
        DragSourceEditor.Reset();
        // Let Slate finish processing mouse release and clear its capture state.
        // A cancelled operation is already gone, so no drop handler can apply it.
        return false;
    }
    void Shutdown()
    {
        if (!Lifetime->bActive) return;
        Lifetime->bActive = false;
        for (auto& Rename : Renames) if (Rename.Restore) Rename.Restore();
        Renames.Empty();
        for (const FGate& Gate : Gates)
        {
            const auto Parent = Gate.Parent.Pin();
            const auto Box = Gate.Box.Pin();
            const auto Original = Gate.Original.Pin();
            if (!Parent || !Box || !Original) continue;
            FChildren* Children = Parent->GetChildren();
            for (int32 I = 0; I < Children->Num(); ++I) if (Children->GetChildAt(I) == Box)
            {
                Box->SetContent(SNullWidget::NullWidget);
                Parent->GetSlot(I).AttachWidget(Original.ToSharedRef());
                break;
            }
        }
        Gates.Empty();
        if (auto List = PickerList.Pin())
        {
            const auto Command = FLevelEditorCommands::Get().AttachActorIteractive;
            if (const FUIAction* Current = List->GetActionForCommand(Command))
                if (Current->ExecuteAction.GetHandle() == PickerExecuteHandle && Current->CanExecuteAction.GetHandle() == PickerCanExecuteHandle)
                {
                    List->UnmapAction(Command);
                    List->MapAction(Command, PriorPickerAction);
                }
        }
        FocusInteraction::Rows.Empty();
        FocusInteraction::Outliners.Empty();
        FocusInteraction::EditorViews.Empty();
        Editors.Empty();
    }
    virtual ~FFocusInteractionGuard() override { Shutdown(); }
    virtual const TCHAR* GetDebugName() const override { return TEXT("Focus Edit Lock"); }
};
