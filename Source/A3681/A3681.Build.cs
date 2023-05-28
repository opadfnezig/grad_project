// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class A3681 : ModuleRules
{
	public A3681(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
        PrivateDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent"});
    }
}
