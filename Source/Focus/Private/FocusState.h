// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
class AActor;
namespace Focus
{
    enum class EControl : uint8 { EditorHidden, GameHidden, Solo, Selection, Transform, Edit };
    struct FFlags
    {
        bool Selection = false;
        bool Transform = false;
        bool Edit = false;
        bool Solo = false;
        bool OriginalMovementLock = false;
    };
    bool IsEditorActor(const AActor* Actor);
    bool Get(AActor* Actor, EControl Control);
    void Set(AActor* Actor, EControl Control, bool Value);
    void ClearSession();
    bool HasSolo(UWorld* World);
}
