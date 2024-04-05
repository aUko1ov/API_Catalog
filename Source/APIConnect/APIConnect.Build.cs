// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class APIConnect : ModuleRules
{
	public APIConnect(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", 
			"InputCore", "EnhancedInput", "Http", 
			"Json", "JsonUtilities" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		
	}
}
