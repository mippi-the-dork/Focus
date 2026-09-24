// Copyright Mippithedork 2026, Inc. All Rights Reserved.

using UnrealBuildTool;
public class Focus : ModuleRules
{
    public Focus(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "Slate", "SlateCore", "InputCore",
            "UnrealEd", "EditorFramework", "LevelEditor", "SceneOutliner",
            "PropertyEditor", "RenderCore", "Projects", "SubobjectEditor", "ActorPickerMode"
        });
    }
}
