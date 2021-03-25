// Copyright 2020 byTangs, All Rights Reserved.

using UnrealBuildTool;

public class TextureRuntimeImporter : ModuleRules
{
	public TextureRuntimeImporter(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"ImageWrapper",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

		if (Target.Platform == UnrealTargetPlatform.Linux ||
		    Target.Platform == UnrealTargetPlatform.Mac ||
		    Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicDefinitions.Add("WITH_FREEIMAGE_LIB=1");
			PrivateDependencyModuleNames.Add("FreeImage");
		}	
		else
		{
			PublicDefinitions.Add("WITH_FREEIMAGE_LIB=0");
		}
		
		
	}
}
