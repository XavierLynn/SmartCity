// Fill out your copyright notice in the Description page of Project Settings.


#include "DoTweenVector2.h"
#include "UpdateManager.h"

UDoTweenVector2* UDoTweenVector2::DoVector2(float durTime, FVector2D start, FVector2D end)
{
	UDoTweenVector2* result = NewObject<UDoTweenVector2>();
	result->Init(durTime);
	result->StartValue = start;
	result->EndValue = end;
	result->OnUpdate.Broadcast(start, result);
	return result;
}

void UDoTweenVector2::UpdateFunc(float deltaTime)
{
	Super::UpdateFunc(deltaTime);
	if (Finished)
	{
		OnUpdate.Broadcast(EndValue, this);
		OnComplete.Broadcast(EndValue, this);
		return;
	}
	FVector2D CurValue = StartValue + CurTime / TotalTime * (EndValue - StartValue);
	OnUpdate.Broadcast(CurValue, this);
}
