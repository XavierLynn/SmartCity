// Fill out your copyright notice in the Description page of Project Settings.


#include "DoTweenBase.h"
#include "UpdateManager.h"

void UDoTweenBase::StopUpdate()
{
	SetReadyToDestroy();
}

void UDoTweenBase::Init(float durTime)
{
	FTickHandle updateCall = FTickHandle();
	updateCall.BindUObject(this, &UDoTweenBase::UpdateFunc);
	TotalTime = durTime;
	UpdateKey = AUpdateManager::RegisterUpdateCall(updateCall);
}

void UDoTweenBase::SetReadyToDestroy()
{
	if (Finished)return;
	Finished = true;
	AUpdateManager::RemoveUpdateCall(UpdateKey);
	Super::SetReadyToDestroy();
}

void UDoTweenBase::UpdateFunc(float deltaTime)
{
	if (Finished)return;
	CurTime += deltaTime;
	if (CurTime >= TotalTime)//Finish
	{
		SetReadyToDestroy();	
	}
}
