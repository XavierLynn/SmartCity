// Fill out your copyright notice in the Description page of Project Settings.


#include "DoTweenVector4.h"
#include "UpdateManager.h"


UDoTweenVector4* UDoTweenVector4::DoVector4(float durTime, FVector4 start, FVector4 end)
{
	UDoTweenVector4* result = NewObject<UDoTweenVector4>();
	result->Init(durTime);
	result->StartValue = start;
	result->EndValue = end;
	result->OnUpdate.Broadcast(start,result);
	return result;
}

void UDoTweenVector4::UpdateFunc(float deltaTime)
{
	Super::UpdateFunc(deltaTime);
	if (Finished)
	{
		OnUpdate.Broadcast(EndValue, this);
		OnComplete.Broadcast(EndValue, this);
		return;
	}
	FVector4 CurValue = StartValue + CurTime / TotalTime * (EndValue - StartValue);
	OnUpdate.Broadcast(CurValue,this);
}
