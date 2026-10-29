// Copyright Mippithedork 2026, Inc. All Rights Reserved.
#include "FocusSettings.h"
#include "FocusState.h"
#if WITH_EDITOR
#include "Framework/Application/SlateApplication.h"
void UFocusSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    const FName Property = PropertyChangedEvent.GetPropertyName();
    if (Property == GET_MEMBER_NAME_CHECKED(UFocusSettings, bPersistVisual))
    {
        bPersistEditorHidden = bPersistVisual;
        bPersistGameHidden = bPersistVisual;
        bPersistSolo = bPersistVisual;
    }
    else if (Property == GET_MEMBER_NAME_CHECKED(UFocusSettings, bPersistInteraction))
    {
        bPersistSelection = bPersistInteraction;
        bPersistProtection = bPersistInteraction;
    }
    else
    {
        bPersistVisual = bPersistEditorHidden && bPersistGameHidden && bPersistSolo;
        bPersistInteraction = bPersistSelection && bPersistProtection;
    }
    Super::PostEditChangeProperty(PropertyChangedEvent);
    Focus::OnPersistenceSettingsChanged();
    SaveConfig();
    // Only runs when the user edits settings, never during regular Outliner paint.
    if (FSlateApplication::IsInitialized()) FSlateApplication::Get().InvalidateAllWidgets(false);
}
#endif
