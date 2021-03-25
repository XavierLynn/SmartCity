/************************************************************************/
/* Author: YWT20                                                        */
/* Expected release year : 2020                                         */
/************************************************************************/

/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//                        (3-clause BSD License)
//
// Copyright (C) 2000-2020, Intel Corporation, all rights reserved.
// Copyright (C) 2009-2011, Willow Garage Inc., all rights reserved.
// Copyright (C) 2009-2016, NVIDIA Corporation, all rights reserved.
// Copyright (C) 2010-2013, Advanced Micro Devices, Inc., all rights reserved.
// Copyright (C) 2015-2016, OpenCV Foundation, all rights reserved.
// Copyright (C) 2015-2016, Itseez Inc., all rights reserved.
// Copyright (C) 2019-2020, Xperience AI, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modificatio
// are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * Neither the names of the copyright holders nor the names of the contributo
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" an
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclai
// In no event shall copyright holders or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//M*/

using System;
using System.IO;
using UnrealBuildTool;

public class MyOpenCV : ModuleRules
{
	public MyOpenCV(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64 ||
			Target.Platform == UnrealTargetPlatform.Win32)
		{
			string PlatformDir = Target.Platform.ToString();
			string IncPath = Path.Combine(ModuleDirectory, "include");
			PublicSystemIncludePaths.Add(IncPath);

			string LibPath = Path.Combine(ModuleDirectory, "lib", PlatformDir);
            string BinaryPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../Binaries/ThirdParty", PlatformDir));

			string LibName = "opencv_world440";

			if (Target.Configuration == UnrealTargetConfiguration.Debug &&
				Target.bDebugBuildsActuallyUseDebugCRT)
			{
					LibName += "d";
			}

			PublicAdditionalLibraries.Add(Path.Combine(LibPath, LibName + ".lib"));
			string DLLName = LibName + ".dll";
			PublicDelayLoadDLLs.Add(DLLName); 
            PublicDelayLoadDLLs.Add("opencv_videoio_ffmpeg440_64.dll"); 
            RuntimeDependencies.Add(Path.Combine(BinaryPath, DLLName));
            RuntimeDependencies.Add(Path.Combine(BinaryPath, "opencv_videoio_ffmpeg440_64.dll"));
            PublicDefinitions.Add("WITH_OPENCV=1");
			PublicDefinitions.Add("OPENCV_PLATFORM_PATH=Binaries/ThirdParty/" + PlatformDir);
			PublicDefinitions.Add("OPENCV_DLL_NAME=" + DLLName);
            PublicDefinitions.Add("OPENCV_DLL_NAME_FFMPEG=" + "opencv_videoio_ffmpeg440_64.dll");
        }
		else // unsupported platform
		{
            PublicDefinitions.Add("WITH_OPENCV=0");
		}
	}
}
