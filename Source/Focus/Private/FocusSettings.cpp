// Copyright Mippithedork 2026, Inc. All Rights Reserved.
#include "FocusSettings.h"
#if WITH_EDITOR
#include "Framework/Application/SlateApplication.h"
void UFocusSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SaveConfig();
    // Only runs when the user edits settings, never during regular Outliner paint.
    if (FSlateApplication::IsInitialized()) FSlateApplication::Get().InvalidateAllWidgets(false);
}
#endif
