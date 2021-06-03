// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FJsonUEModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual void PreUnloadCallback() override;
};

//JSONEASY_API DECLARE_LOG_CATEGORY_EXTERN(LogJsonEasy, Log, All);
