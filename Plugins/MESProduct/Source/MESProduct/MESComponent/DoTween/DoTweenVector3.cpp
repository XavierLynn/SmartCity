// Fill out your copyright notice in the Description page of Project Settings.


#include "DoTweenVector3.h"
#include "UpdateManager.h"

UDoTweenVector3* UDoTweenVector3::DoVector3(float durTime, FVector start, FVector end)
{
	UDoTweenVector3* result = NewObject<UDoTweenVector3>();
	result->Init(durTime);
	result->StartValue = start;
	result->EndValue = end;
	result->OnUpdate.Broadcast(start,result);
	return result;
}

void UDoTweenVector3::UpdateFunc(float deltaTime)
{
	Super::UpdateFunc(deltaTime);
	if (Finished)
	{
		OnUpdate.Broadcast(EndValue, this);
		OnComplete.Broadcast(EndValue, this);
		return;
	}
	FVector CurValue = StartValue + CurTime / TotalTime * (EndValue - StartValue);
	OnUpdate.Broadcast(CurValue,this);
}
