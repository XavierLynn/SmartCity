/* Copyright (C) 2019 Hugo ATTAL - All Rights Reserved
 * This plugin is downloadable from the UE4 Marketplace
 */

using UnrealBuildTool;

public class ElectronicNodes : ModuleRules
{
	public ElectronicNodes(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {}
			);
				
		PrivateIncludePaths.AddRange(
			new string[] {}
			);
			
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
			);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"GraphEditor",
				"MaterialEditor",
				"BlueprintGraph"
			}
			);
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{}
			);
	}
}
