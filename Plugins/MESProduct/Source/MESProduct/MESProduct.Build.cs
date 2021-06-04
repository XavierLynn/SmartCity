// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class MESProduct : ModuleRules
{
	public MESProduct(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableUndefinedIdentifierWarnings = false;
        //PrecompileForTargets = PrecompileTargetsType.Any;

        //bUsePrecompiled = true;
        //bPrecompile = true;  
        /*  RabbitMq  */
        string RabbitMqIncludePath = Path.Combine(ModuleDirectory, "../ThirdParty/RabbitMq/include");
        PublicSystemIncludePaths.Add(RabbitMqIncludePath);

        string RabbitMqLibPath = Path.Combine(ModuleDirectory, "../ThirdParty/RabbitMq/lib/librabbitmq.4.lib");
        //PublicLibraryPaths.Add(RabbitMqLibPath);
        PublicAdditionalLibraries.AddRange(
            new string[]
            {
                RabbitMqLibPath
                //"libssl.lib",
                //"libcrypto.lib"
            }
            );
        //RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(LibPath, "rabbitmq.4.dll")));

        /* OpenExcel */
        string OpenExcelIncludePath = Path.Combine(ModuleDirectory, "../ThirdParty/OpenExcel/include");
        PublicSystemIncludePaths.Add(OpenExcelIncludePath);

        string OpenExcelLibPath = Path.Combine(ModuleDirectory, "../ThirdParty/OpenExcel/lib/OpenXLSX.lib");
        //PublicLibraryPaths.Add(OpenExcelLibPath);
        PublicAdditionalLibraries.AddRange(
            new string[]
            {
                OpenExcelLibPath
            }
            );



        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
            }
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
                "MESProduct/MESComponent/RabbitMq/Public",
                "MESProduct/MESComponent/OpenExcel/Public"
            }
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Projects",
                "Engine",
                "InputCore",
                "Http",
                "SlateCore",
                "Slate",
                "UMG",
                "LevelSequence",
                "MovieScene",
                "MovieSceneTracks",
				"CinematicCamera",
                "JsonUtilities",
                "Json",
                "ImageWrapper",
                "RenderCore",
                "RHI",
                "SQLiteCore",
                "SQLiteSupport",
                "MediaAssets",
                "WebBrowserWidget",
                "WebBrowser",
                "DesktopPlatform"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
