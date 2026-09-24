// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "Tools/LegacyEdModeInterfaces.h"
#include "FocusEditorMode.generated.h"

/** Experimental selection gate. No custom viewport or Motion Design dependency. */
UCLASS(Transient)
class UFocusEditorMode : public UEdMode, public ILegacyEdModeViewportInterface
{
    GENERATED_BODY()
public:
    static const FEditorModeID ModeId;
    UFocusEditorMode();
    virtual bool UsesToolkits() const override { return false; }
    virtual bool IsCompatibleWith(FEditorModeID OtherModeID) const override;
    virtual bool IsSelectionDisallowed(AActor* Actor, bool Selecting) const override;
    virtual bool ProcessEditDelete() override;
    virtual bool ProcessEditCut() override;
    // Consume standard gizmo deltas when a protected actor/component is selected.
    virtual bool InputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport,
        FVector& Drag, FRotator& Rotation, FVector& Scale) override;
};
