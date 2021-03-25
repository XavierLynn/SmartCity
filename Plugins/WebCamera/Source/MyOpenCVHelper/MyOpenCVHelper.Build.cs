/************************************************************************/
/* Author: YWT20                                                        */
/* Expected release year : 2020                                         */
/************************************************************************/

using UnrealBuildTool;

public class MyOpenCVHelper: ModuleRules
{
	public MyOpenCVHelper(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
				"MyOpenCVHelper/Private"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[] {
            }
        );

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
                "Projects",
                "MyOpenCV"
            }
        );
	}
}
