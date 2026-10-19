// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "FocusEditorMode.generated.h"

/** Editor selection/command protection only. Does not claim legacy viewport or widget input. */
UCLASS(Transient)
class UFocusEditorMode : public UEdMode
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
};
