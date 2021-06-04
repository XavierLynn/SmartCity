// Fill out your copyright notice in the Description page of Project Settings.


#include "MESGameInstance.h"
#include "Engine/World.h"
#include "Engine.h"

//UMESGameInstance * UMESGameInstance::GetMESGameInstance(const UObject* WorldContextObject)
//{
//	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
//	{
//		return World->GetGameInstance<UMESGameInstance>();
//	}
//
//	return nullptr;
//}

UMESGameInstance * UMESGameInstance::Get(const UWorld * World)
{
	if (World)
	{
		return World->GetGameInstance<UMESGameInstance>();
	}

	return nullptr;
}
