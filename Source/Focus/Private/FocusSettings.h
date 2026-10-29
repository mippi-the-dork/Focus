// Copyright Mippithedork 2026, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "FocusSettings.generated.h"

/** Appearance shared by all Focus quick-property controls. */
UCLASS(config=EditorPerProjectUserSettings, defaultconfig, meta=(DisplayName="Focus"))
class UFocusSettings : public UDeveloperSettings
{
    GENERATED_BODY()
public:
    virtual FName GetContainerName() const override { return TEXT("Editor"); }
    virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    UPROPERTY(EditAnywhere, Config, Category="Control State Colors", meta=(ToolTip="Color for available, enabled controls. Default: white. Changes affect all Focus categories."))
    FLinearColor EnabledColor = FLinearColor::White;

    // Slate paint colors are LINEAR. Convert sRGB #616161 for an exact displayed grey.
    UPROPERTY(EditAnywhere, Config, Category="Control State Colors", meta=(ToolTip="Color for available, disabled controls. Default displayed sRGB: #616161. Different from Not Applicable."))
    FLinearColor DisabledColor = FLinearColor::FromSRGBColor(FColor(97, 97, 97, 255));

    UPROPERTY(EditAnywhere, Config, Category="Control State Colors", meta=(ToolTip="Color for Mixed values across selected or grouped eligible targets. Default: yellow."))
    FLinearColor MixedColor = FLinearColor::Yellow;

    UPROPERTY(EditAnywhere, Config, Category="Control State Colors", meta=(ToolTip="Color for unavailable or non-instance-editable properties. Default displayed sRGB: #242424. These controls cannot be changed from Focus."))
    FLinearColor NotApplicableColor = FLinearColor::FromSRGBColor(FColor(36, 36, 36, 255));

    // Update only previous shipped defaults; user-adjusted colors are preserved.
    UPROPERTY(Config)
    bool bMigratedSlateStateColorDefaults = false;

    // Internal one-time migration marker. This is not a user-facing preference.
    UPROPERTY(Config)
    bool bMigratedDisabledColorDefaultTo616161 = false;

    // Used by existing Focus-specific warning states; preserving the prior red.
    UPROPERTY(EditAnywhere, Config, Category="Control State Colors", meta=(ToolTip="Color for Focus warnings, including out-of-range forced LOD values, and strong protection indicators. Default: red."))
    FLinearColor AlertColor = FLinearColor::Red;

    // The two parent checkboxes are bulk controls. Child flags retain their own
    // values so that Visual and Interaction can be configured independently.
    UPROPERTY(EditAnywhere, Config, Category="Visual & Interaction Persistence", meta=(DisplayName="Visual (all)", ToolTip="Enable or disable persistence for every Visual control. New installations default to all enabled."))
    bool bPersistVisual = true;

    UPROPERTY(EditAnywhere, Config, Category="Visual & Interaction Persistence|Visual", meta=(DisplayName="Hide in Editor", ToolTip="When disabled, changes made with Focus are session-only and do not replace the saved actor visibility state."))
    bool bPersistEditorHidden = true;

    UPROPERTY(EditAnywhere, Config, Category="Visual & Interaction Persistence|Visual", meta=(DisplayName="Hidden in Game", ToolTip="When disabled, only Focus miniwidget edits are session-only and must not dirty the Actor package. Native Details or Outliner edits continue to save normally. External Actors and save workflows require testing."))
    bool bPersistGameHidden = true;

    UPROPERTY(EditAnywhere, Config, Category="Visual & Interaction Persistence|Visual", meta=(DisplayName="Solo", ToolTip="When disabled, Solo changes are temporary and are not written to the actor's Focus metadata."))
    bool bPersistSolo = true;

    UPROPERTY(EditAnywhere, Config, Category="Visual & Interaction Persistence", meta=(DisplayName="Interaction (all)", ToolTip="Enable or disable persistence for Selection Lock and the Transform / Edit protection levels."))
    bool bPersistInteraction = true;

    UPROPERTY(EditAnywhere, Config, Category="Visual & Interaction Persistence|Interaction", meta=(DisplayName="Selection Lock", ToolTip="When disabled, Focus Selection Lock changes last only for the editor session."))
    bool bPersistSelection = true;

    UPROPERTY(EditAnywhere, Config, Category="Visual & Interaction Persistence|Interaction", meta=(DisplayName="Protection Lock (Transform / Edit)", ToolTip="Transform Lock and Edit Lock are mutually exclusive levels of one persistence setting."))
    bool bPersistProtection = true;

    UPROPERTY(EditAnywhere, Config, Category="Session-Only State Colors", meta=(ToolTip="Blue color for enabled Visual or Interaction controls whose persistence is disabled. Default #2196F3."))
    FLinearColor SessionEnabledColor = FLinearColor::FromSRGBColor(FColor(33,150,243));

    UPROPERTY(EditAnywhere, Config, Category="Session-Only State Colors", meta=(ToolTip="Bright orange for mixed states when persistence is disabled. Default #FF9800."))
    FLinearColor SessionMixedColor = FLinearColor::FromSRGBColor(FColor(255,152,0));

    UPROPERTY(EditAnywhere, Config, Category="Session-Only State Colors", meta=(ToolTip="Pink for strong lock and alert states when persistence is disabled. Default #FF4DA6."))
    FLinearColor SessionAlertColor = FLinearColor::FromSRGBColor(FColor(255,77,166));

    UPROPERTY(EditAnywhere, Config, Category="Session-Only State Colors", meta=(ToolTip="Disabled control color for session-only editing. Default #616161; customizable independently."))
    FLinearColor SessionDisabledColor = FLinearColor::FromSRGBColor(FColor(97,97,97));

    UPROPERTY(EditAnywhere, Config, Category="Session-Only State Colors", meta=(ToolTip="Unavailable control color for session-only editing. Default #242424; customizable independently."))
    FLinearColor SessionNotApplicableColor = FLinearColor::FromSRGBColor(FColor(36,36,36));
};
