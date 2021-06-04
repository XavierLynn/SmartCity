// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuManager.h"

UMenuManager* UMenuManager::mInstance = nullptr;

void UMenuManager::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	mInstance = this;
}

void UMenuManager::ChangeMenuTo(int MenuID, FString MenuName)
{
	MenuEvent.Broadcast(MenuID, MenuName);
}

UMenuManager* UMenuManager::Instance()
{
	return mInstance;
}