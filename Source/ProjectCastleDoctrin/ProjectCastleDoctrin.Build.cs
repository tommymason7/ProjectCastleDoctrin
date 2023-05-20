// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectCastleDoctrin : ModuleRules
{
	public ProjectCastleDoctrin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" , "Niagara" });
	}
}
