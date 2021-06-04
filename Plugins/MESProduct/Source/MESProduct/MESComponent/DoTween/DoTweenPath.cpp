// Fill out your copyright notice in the Description page of Project Settings.


#include "DoTweenPath.h"
#include "UpdateManager.h"

void UDoTweenPath::UpdateFunc(float deltaTime)
{
	if (!bLoop)
	{
		Super::UpdateFunc(deltaTime);
	}
	else
	{
		CurTime += deltaTime;
	}
	FVector Dir;
	FVector CurPos;
	float Dis;
	if (PathValue.IsValidIndex(Count) && PathValue.IsValidIndex(Count-1))
	{
		Dis = FVector::Distance(PathValue[Count], PathValue[Count - 1]);
		Dir = (PathValue[Count] - PathValue[Count - 1]) / Dis;
		CurPos = LastPos + (PathTotalLenth /TotalTime) * Dir*deltaTime;
		OnUpdate.Broadcast(CurPos, this);
		LastPos = CurPos;
		float CurrPathLenth = PathTotalLenth / TotalTime * CurTime;

		if (PathLenths.IsValidIndex(Count-1)&& CurrPathLenth >= PathLenths[Count-1])
		{
			if (Count == PathValue.Num() - 1 )
			{
				Count = 1;
				LastPos = StartValue;
				if (bLoop)
				{
					CurTime = 0.f;
				}
				else
				{
					StopUpdate();
				}
				
				return;			
			}
			else
			{
				Count++;
			}
		}
	}
	if (Finished)
	{
		OnUpdate.Broadcast(CurPos, this);
		OnComplete.Broadcast(CurPos, this);
		return;
	}
}

UDoTweenPath* UDoTweenPath::CallDoPath(float durTime, FVector start, TArray<FVector> Path, bool Loop, const FHandleFPath_Single& OutOnComplete, const FHandleFPath_Single& OutOnUpdate)
{
	UDoTweenPath* result = DoPath(durTime, start, Path, Loop);
	result->OnComplete.Add(OutOnComplete);
	result->OnUpdate.Add(OutOnUpdate);
	return result;
}

UDoTweenPath* UDoTweenPath::DoPath(float durTime, FVector start, TArray<FVector> Path, bool Loop)
{
	UDoTweenPath* result = NewObject<UDoTweenPath>();
	result->Init(durTime);
	result->StartValue = start;
	result->PathTotalLenth = 0;
	result->Count = 1;
	result->PathValue = Path;
	result->bLoop = Loop;
	result->LastPos = start;
	for (int32 i=1;i<Path.Num();i++)
	{
		result->PathTotalLenth += FVector::Distance(Path[i], Path[i - 1]);
		result->PathLenths.Add(result->PathTotalLenth);
	}
	result->OnUpdate.Broadcast(start, result);
	return result;
}
