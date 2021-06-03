// Copyright Epic Games, Inc. All Rights Reserved.

#include "JsonUE.h"
//#include "jsonue/jsonue.h"
#define LOCTEXT_NAMESPACE "FJsonUEModule"

void FJsonUEModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// 这里是使用方法 样例
}

void FJsonUEModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("FJsonUEModule::ShutdownModule"));
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}
void FJsonUEModule::PreUnloadCallback() {
	UE_LOG(LogTemp, Warning, TEXT("FJsonUEModule::PreUnloadCallback"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FJsonUEModule, JsonUE)