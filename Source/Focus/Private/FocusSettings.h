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

    // Neutral disabled control tint: #616161 (97/255 per channel).
    UPROPERTY(EditAnywhere, Config, Category="Control State Colors")
    FLinearColor DisabledColor = FLinearColor(97.f / 255.f, 97.f / 255.f, 97.f / 255.f, 1.f);

    UPROPERTY(EditAnywhere, Config, Category="Control State Colors")
    FLinearColor MixedColor = FLinearColor::Yellow;

    UPROPERTY(EditAnywhere, Config, Category="Control State Colors")
    FLinearColor NotApplicableColor = FLinearColor(.17f, .17f, .17f, 1.f);

    // Internal one-time migration marker. This is not a user-facing preference.
    UPROPERTY(Config)
    bool bMigratedDisabledColorDefaultTo616161 = false;

    // Used by existing Focus-specific warning states; preserving the prior red.
    UPROPERTY(EditAnywhere, Config, Category="Control State Colors")
    FLinearColor AlertColor = FLinearColor::Red;
};
