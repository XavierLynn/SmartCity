// Copyright x.Lin, Inc. All Rights Reserved.

#include "HeatMap.h"

#define LOCTEXT_NAMESPACE "FHeatMapModule"

void FHeatMapModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FHeatMapModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHeatMapModule, HeatMap)