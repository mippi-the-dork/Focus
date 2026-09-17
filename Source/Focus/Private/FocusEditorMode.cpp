#include "FocusEditorMode.h"
#include "FocusState.h"
#include "Editor.h"
#include "Selection.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "Layout/WidgetPath.h"
#include "Widgets/SWindow.h"
#include "Widgets/SWidget.h"

const FEditorModeID UFocusEditorMode::ModeId(TEXT("EM_FocusPrototype"));
UFocusEditorMode::UFocusEditorMode()
{
    Info = FEditorModeInfo(ModeId, NSLOCTEXT("Focus", "Mode", "FOCUS"), FSlateIcon(), false, 0);
}
bool UFocusEditorMode::IsCompatibleWith(FEditorModeID) const { return true; }

bool UFocusEditorMode::IsSelectionDisallowed(AActor* Actor, bool Selecting) const
{
    if (!Selecting || !Focus::IsEditorActor(Actor) || !Focus::Get(Actor, Focus::EControl::Selection)) return false;
    if (!FSlateApplication::IsInitialized()) return true;
    // Prototype routing: identify the Outliner under the pointer. This is deliberately
    // isolated here; keyboard selection and other selection entry points need testing.
    FSlateApplication& App = FSlateApplication::Get();
    const FWidgetPath Path = App.LocateWindowUnderMouse(App.GetCursorPos(), App.GetInteractiveTopLevelWindows());
    for (const FArrangedWidget& Widget : Path.Widgets.GetInternalArray())
    {
        if (Widget.Widget->GetType() == FName(TEXT("SSceneOutliner"))) return false;
    }
    return true;
}
static bool HasProtectedSelection()
{
    if (!GEditor) return false;
    for (FSelectionIterator It(*GEditor->GetSelectedActors()); It; ++It)
        if (AActor* Actor = Cast<AActor>(*It))
            if (Focus::Get(Actor, Focus::EControl::Edit)) return true;
    return false;
}
// Consume the whole command for mixed selections. Never silently delete only part.
bool UFocusEditorMode::ProcessEditDelete() { return HasProtectedSelection(); }
bool UFocusEditorMode::ProcessEditCut() { return HasProtectedSelection(); }

// FLevelEditorViewportClient offers standard widget input to editor modes before
// applying its actor transform. This does not cover component visualizers that
// consume input earlier or tools that write transforms through another path.
bool UFocusEditorMode::InputDelta(FEditorViewportClient*, FViewport*,
    FVector& Drag, FRotator& Rotation, FVector& Scale)
{
    if (!GEditor) return false;
    auto IsProtected = [](AActor* Actor) {
        return Focus::IsEditorActor(Actor) &&
            (Focus::Get(Actor, Focus::EControl::Transform) ||
                Focus::Get(Actor, Focus::EControl::Edit));
        };
    bool bHasProtected = false;
    bool bHasMovable = false;
    auto Inspect = [&](AActor* Actor) {
        if (!Focus::IsEditorActor(Actor)) return;
        if (IsProtected(Actor)) bHasProtected = true;
        else bHasMovable = true;
        };
    // Component selections take precedence over their implicitly selected actor.
    USelection* Components = GEditor->GetSelectedComponents();
    if (Components && Components->Num() > 0)
    {
        for (FSelectionIterator It(*Components); It; ++It)
            if (auto* Component = Cast<UActorComponent>(*It)) Inspect(Component->GetOwner());
    }
    else if (USelection* Actors = GEditor->GetSelectedActors())
    {
        for (FSelectionIterator It(*Actors); It; ++It) Inspect(Cast<AActor>(*It));
    }
    // Mixed selections may move their unlocked members. The transaction-scoped
    // module guard preserves protected roots, including attached descendants.
    if (!bHasProtected || bHasMovable) return false;
    // An entirely protected selection still consumes the gesture immediately.
    Drag = FVector::ZeroVector;
    Rotation = FRotator::ZeroRotator;
    Scale = FVector::ZeroVector;
    return true;
}
