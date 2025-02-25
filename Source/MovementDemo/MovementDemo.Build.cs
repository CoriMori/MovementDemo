// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MovementDemo : ModuleRules
{
	public MovementDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
       {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "Slate",
            "SlateCore",
            "MotionWarping",
            "AnimGraphRuntime"
       });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities",
            "GameplayTasks",
            "GameplayTags",
            "EnhancedInput"
        });
    }
}
