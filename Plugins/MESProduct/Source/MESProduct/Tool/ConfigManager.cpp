// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigManager.h"
#include "MESProduct/UI/Launcher/LauncherDataAsset.h"
#include <string>

UConfigManager::UConfigManager()
{
	LauncherConfig = LoadObject<ULauncherDataAsset>(this, TEXT("LauncherDataAsset'/MESProduct/Main/Config/MainConfig.MainConfig'"));
}

UConfigManager* UConfigManager::Get()
{
	static UConfigManager* Instance = nullptr;
	if (!Instance)
	{
		Instance = NewObject<UConfigManager>();
		Instance->AddToRoot();
		UE_LOG(LogTemp, Warning, TEXT("create config manager"));
	}
	return Instance;
}

UConfigManager * UConfigManager::GetConfigManager()
{
	return UConfigManager::Get();
}
