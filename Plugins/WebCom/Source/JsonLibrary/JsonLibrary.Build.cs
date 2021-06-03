// Copyright 2020 Tracer Interactive, LLC. All Rights Reserved.

using UnrealBuildTool;

//namespace UnrealBuildTool.Rules
//{
//	public class JsonLibrary : ModuleRules
//	{
//		public JsonLibrary(ReadOnlyTargetRules Target) : base(Target)
//		{
//			PublicDependencyModuleNames.AddRange(
//				new string[]
//				{
//					"Core",
//					"CoreUObject",
//					"Engine",
//					"Json"
//				}
//			);
//		}
//	}
//}
public class JsonLibrary : ModuleRules
{
	public JsonLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Json"
			 }
);
    }
}