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

    UPROPERTY(EditAnywhere, Config, Category="Control State Colors")
    FLinearColor EnabledColor = FLinearColor::White;

    // Slate paint colors are LINEAR. Convert sRGB #616161 for an exact displayed grey.
    UPROPERTY(EditAnywhere, Config, Category="Control State Colors")
    FLinearColor DisabledColor = FLinearColor::FromSRGBColor(FColor(97, 97, 97, 255));

    UPROPERTY(EditAnywhere, Config, Category="Control State Colors")
    FLinearColor MixedColor = FLinearColor::Yellow;

    UPROPERTY(EditAnywhere, Config, Category="Control State Colors", meta=(ToolTip="Color for properties unavailable on this actor/component. Default displayed sRGB: #242424"))
    FLinearColor NotApplicableColor = FLinearColor::FromSRGBColor(FColor(36, 36, 36, 255));

    // Update only previous shipped defaults; user-adjusted colors are preserved.
    UPROPERTY(Config)
    bool bMigratedSlateStateColorDefaults = false;

    // Internal one-time migration marker. This is not a user-facing preference.
    UPROPERTY(Config)
    bool bMigratedDisabledColorDefaultTo616161 = false;

    // Used by existing Focus-specific warning states; preserving the prior red.
    UPROPERTY(EditAnywhere, Config, Category="Control State Colors")
    FLinearColor AlertColor = FLinearColor::Red;
};
