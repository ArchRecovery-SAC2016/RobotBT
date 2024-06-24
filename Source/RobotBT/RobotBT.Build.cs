// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RobotBT : ModuleRules
{
	public RobotBT(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "EditorStyle", "Json" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" });
    }
}
