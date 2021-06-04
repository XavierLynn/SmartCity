// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraUtility.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine.h"

void UCameraUtility::SetPos(const UObject* WorldContextObject, FVector Location, FRotator Rotation)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				CameraController->SetPos(Location, Rotation);
				break;
			}
		}
	}
}

void UCameraUtility::MoveToPos(const UObject* WorldContextObject, FVector Location, FRotator Rotation, float Seconds, FCameraActionCompleteDynamicDelegate Callback)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				CameraController->MoveToPos(Location, Rotation, Seconds, Callback);
				break;
			}
		}
	}
}

void UCameraUtility::MoveToBound(const UObject * WorldContextObject, FBoxSphereBounds Bounds, float Seconds, FCameraActionCompleteDynamicDelegate Callback)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				CameraController->MoveToBound(Bounds, Seconds, Callback);
				break;
			}
		}
	}
}

void UCameraUtility::PlayAni(const UObject * WorldContextObject, FString AniName, FCameraActionCompleteDynamicDelegate Callback)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				CameraController->PlayAni(AniName, Callback);
				break;
			}
		}
	}
}

void UCameraUtility::PauseAni(const UObject * WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				CameraController->PauseAni();
				break;
			}
		}
	}
}

void UCameraUtility::ResetAni(const UObject * WorldContextObject, bool IsContinueToPlay/* = true*/)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				CameraController->ResetAni(IsContinueToPlay);
				break;
			}
		}
	}
}

bool UCameraUtility::IsPlayingAni(const UObject * WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				return CameraController->IsPlayingAni();
			}
		}
	}
	return false;
}

void UCameraUtility::UpdateRotateAroundMode(const UObject * WorldContextObject, EMESCameraControllerRotateAroundMode NewMode)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				CameraController->UpdateRotateAroundMode(NewMode);
				break;
			}
		}
	}
}

void UCameraUtility::UpdateRotateAroundPoint(const UObject * WorldContextObject, FVector SpecifyPoint)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				CameraController->UpdateRotateAroundPoint(SpecifyPoint);
				break;
			}
		}
	}
}

void UCameraUtility::JumpToAni(const UObject * WorldContextObject, FString AniName, float DurationPercentage, FCameraActionCompleteDynamicDelegate Callback, bool IsContinueToPlay/* = true*/)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				CameraController->JumpToAni(AniName, DurationPercentage, IsContinueToPlay, Callback);
				break;
			}
		}
	}
}

float UCameraUtility::GetAniPercentage(const UObject * WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (TActorIterator<AMES_CameraController> It(World, AMES_CameraController::StaticClass()); It; ++It)
		{
			AMES_CameraController* CameraController = *It;
			if (CameraController)
			{
				return CameraController->GetAniPercentage();
			}
		}
	}
	return 0.0f;
}
