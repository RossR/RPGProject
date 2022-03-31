// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPGProject : ModuleRules
{
	public RPGProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "CableComponent", "Slate", "SlateCore", "AIModule" });

		PrivateDependencyModuleNames.AddRange(new string[] { "CableComponent" });
	}
}
