/************************************************************************/
/* Author: YWT20                                                        */
/* Expected release year : 2020                                         */
/************************************************************************/

#include "IMyOpenCVHelperModule.h"
#include "Modules/ModuleManager.h" 
#include "Interfaces/IPluginManager.h"
#include "HAL/PlatformProcess.h"

class FMyOpenCVHelperModule : public IMyOpenCVHelperModule
{
public:
	FMyOpenCVHelperModule();

public:
	//~ IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void* OpenCvDllHandle;
	void* OpenCvVideoDllHandle;
};

FMyOpenCVHelperModule::FMyOpenCVHelperModule()
	: OpenCvDllHandle(nullptr), OpenCvVideoDllHandle(nullptr)
{}

void FMyOpenCVHelperModule::StartupModule()
{
	const FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("WebCameraPlugin"))->GetBaseDir();

#if WITH_OPENCV
	const FString OpenCvBinPath = PluginDir / TEXT(PREPROCESSOR_TO_STRING(OPENCV_PLATFORM_PATH));
	const FString DLLPath = OpenCvBinPath / TEXT(PREPROCESSOR_TO_STRING(OPENCV_DLL_NAME));
	const FString DLL_VideoPath = OpenCvBinPath / TEXT(PREPROCESSOR_TO_STRING(OPENCV_DLL_NAME_FFMPEG));

	FPlatformProcess::PushDllDirectory(*OpenCvBinPath);
	OpenCvDllHandle = FPlatformProcess::GetDllHandle(*DLLPath);
	OpenCvVideoDllHandle = FPlatformProcess::GetDllHandle(*DLL_VideoPath);
	FPlatformProcess::PopDllDirectory(*OpenCvBinPath);
#endif
}

void FMyOpenCVHelperModule::ShutdownModule()
{
#if WITH_OPENCV
	if (OpenCvDllHandle)
	{
		FPlatformProcess::FreeDllHandle(OpenCvDllHandle);
		OpenCvDllHandle = nullptr;
	}
	if (OpenCvVideoDllHandle)
	{
		FPlatformProcess::FreeDllHandle(OpenCvVideoDllHandle);
		OpenCvVideoDllHandle = nullptr;
	}
#endif
}

IMPLEMENT_MODULE(FMyOpenCVHelperModule, MyOpenCVHelper);
