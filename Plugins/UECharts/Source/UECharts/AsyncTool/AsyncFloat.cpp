// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncFloat.h"
#include "AsyncManagerActor.h"

UAsyncFloat* UAsyncFloat::DoFloat(float durTime, float start, float end)
{
	UAsyncFloat* result = NewObject<UAsyncFloat>();
	result->Init(durTime);
	result->StartValue = start;
	result->EndValue = end;
	//result->OnUpdate.Broadcast(start, result);
	return result;
}



UAsyncFloat* UAsyncFloat::CallDoFloat(float durTime, float start, float end, const FHandleFloat_Single& OutOnComplete, const FHandleFloat_Single& OutOnUpdate)
{
	UAsyncFloat* result = DoFloat(durTime, start, end);
	result->OnComplete.Add(OutOnComplete);
	result->OnUpdate.Add(OutOnUpdate);
	return result;
}

void UAsyncFloat::UpdateFunc(float deltaTime)
{
	Super::UpdateFunc(deltaTime);
	if (Finished)
	{
		OnUpdate.Broadcast(EndValue, this);
		OnComplete.Broadcast(EndValue, this);
		return;
	}
	float CurValue = StartValue + CurTime / TotalTime * (EndValue - StartValue);
	OnUpdate.Broadcast(CurValue, this);
}
