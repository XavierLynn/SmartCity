// Fill out your copyright notice in the Description page of Project Settings.


#include "MESLevelManager.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Runtime\Engine\Classes\Engine\Texture.h"

FLevelChangeStartDynamicMultiDelegate AMESLevelManager::OnLevelChangeStartCallbacks;

void AMESLevelManager::HandlePath(TArray<FString>& LevelPaths)
{
	for (int i = 0; i < LevelPaths.Num(); i++)
	{
		FString symbol = TEXT("'");
		FString blueprintPath = LevelPaths[i];
		int FirstIndex = blueprintPath.Find(symbol);
		if (FirstIndex != -1)
		{
			int LastIndex = blueprintPath.FindLastCharByPredicate([](const TCHAR& c) {return c == '.'; });
			blueprintPath = blueprintPath.Left(blueprintPath.Len() - 1).Right(blueprintPath.Len() - FirstIndex - 2);
		}
		int FirstCode = blueprintPath.Find(TEXT("."));
		if (FirstCode != -1)
		{
			blueprintPath = blueprintPath.Left(FirstCode);
		}
		LevelPaths[i] = blueprintPath;
	}
}

AMESLevelManager* AMESLevelManager::Instance = nullptr;

// Sets default values
AMESLevelManager::AMESLevelManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AMESLevelManager::BeginPlay()
{
	Super::BeginPlay();

}

void AMESLevelManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Instance = nullptr;
	ShowedLevels.Empty();

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AMESLevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMESLevelManager::ShowLevel(UObject * WorldContextObject, TArray<FString> LevelPaths, FLevelShownDelegate Callback, bool isCanCache)
{
	if (AMESLevelManager::Instance != nullptr && AMESLevelManager::Instance->LevelCountToLoad > 0)
	{
		return;
	}
	HandlePath(LevelPaths);
	if (LevelPaths.Num() <= 0 && Callback.IsBound())
	{
		Callback.Execute(1.f);
		Callback.Unbind();
		return;
	}

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		Callback.Execute(0.f);
		if (Instance == nullptr)
		{
			Instance = World->SpawnActor<AMESLevelManager>();
		}
		Instance->CanCallBack = true;
		OnLevelChangeStartCallbacks.Broadcast();

		for (FString& LevelPath : LevelPaths)
		{
			LevelPath = LevelPath.Replace(TEXT("\\"), TEXT("/"));
		}

		Instance->LevelCountToLoad = LevelPaths.Num();
		Instance->LevelCountToHide = Instance->LastShowedLevel.Num();
		Instance->MaxTaskCont = Instance->LevelCountToLoad + Instance->LevelCountToHide;
		Instance->OnLevelShownCallback = Callback;
		Instance->WillShowedLevel = LevelPaths;
		//¼ì²â
		for (FString LastLevelPath : Instance->LastShowedLevel)
		{
			if (Instance->ShowedLevels.Contains(LastLevelPath))
			{
				if (Instance->CanCache && Instance->ShowedLevels[LastLevelPath]->IsLevelLoaded())
				{
					if (Instance->ShowedLevels[LastLevelPath]->ShouldBeVisible())
					{
						Instance->ShowedLevels[LastLevelPath]->SetShouldBeVisible(false);
					}
					else
					{
						--Instance->LevelCountToHide;
					}
				}
				else
				{
					if (Instance->ShowedLevels[LastLevelPath]->ShouldBeVisible())
					{
						Instance->ShowedLevels[LastLevelPath]->SetShouldBeLoaded(false);
					}
					else
					{
						--Instance->LevelCountToHide;
					}
				}
			}
			else
			{
				--Instance->LevelCountToHide;
			}
		}
		Instance->CanCache = isCanCache;
		if (Instance->LevelCountToHide == 0)
		{
			Instance->OnLevelHidened();
		}
	}
}


void AMESLevelManager::SetLevelVisible(UObject* WorldContextObject, TArray<FString> LevelPaths, bool IsVisble)
{
	HandlePath(LevelPaths);
	//Instance->LevelCountToHide = LevelPaths.Num() * 100;
	//Instance->LevelCountToLoad = LevelPaths.Num() * 100;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (Instance == nullptr)
		{
			Instance = World->SpawnActor<AMESLevelManager>();
		}
		Instance->CanCallBack = false;
		for (FString LevelPath : LevelPaths)
		{
			if (Instance->ShowedLevels.Contains(LevelPath))
			{
				if (Instance->ShowedLevels[LevelPath]->IsLevelLoaded())
				{
					Instance->ShowedLevels[LevelPath]->SetShouldBeVisible(IsVisble);
				}
			}
		}
	}
}

void AMESLevelManager::BindOnLevelChangeStart(UObject * WorldContextObject, FLevelChangeStartDynamicDelegate Callback)
{
	if (!OnLevelChangeStartCallbacks.Contains(Callback))
	{
		OnLevelChangeStartCallbacks.Add(Callback);
	}
}

void AMESLevelManager::UnbindOnLevelChangeStart(UObject * WorldContextObject, FLevelChangeStartDynamicDelegate Callback)
{
	if (OnLevelChangeStartCallbacks.Contains(Callback))
	{
		OnLevelChangeStartCallbacks.Remove(Callback);
	}
}

void AMESLevelManager::RealShow()
{
	for (FString LevelPath : WillShowedLevel)
	{
		if (ShowedLevels.Contains(LevelPath))
		{
			if (Instance->ShowedLevels[LevelPath] != nullptr && !Instance->ShowedLevels[LevelPath]->ShouldBeVisible())
			{
				if (!Instance->ShowedLevels[LevelPath]->ShouldBeLoaded())
				{
					Instance->ShowedLevels[LevelPath]->SetShouldBeLoaded(true);
				}
				else
				{
					Instance->ShowedLevels[LevelPath]->SetShouldBeVisible(true);
				}
#if !WITH_EDITOR
				WillShowedLevel.Remove(LevelPath);
				return;
#endif
			}
			else
			{
				Instance->LevelCountToLoad--;
			}
		}
		else
		{
			bool LoadResult;
			ULevelStreamingDynamic* StreamingLevel = ULevelStreamingDynamic::LoadLevelInstance(this, LevelPath, FVector::ZeroVector, FRotator::ZeroRotator, LoadResult);
			if (LoadResult && StreamingLevel != nullptr)
			{
				Instance->ShowedLevels.Add(LevelPath, StreamingLevel);
				FScriptDelegate LoadedCallback;
				LoadedCallback.BindUFunction(Instance, "OnLevelShowned");
				//StreamingLevel->OnLevelLoaded.Add(LoadedCallback);
				StreamingLevel->OnLevelShown.Add(LoadedCallback);
				FScriptDelegate HideCallBack;
				HideCallBack.BindUFunction(Instance, "OnLevelHidened");
				//StreamingLevel->OnLevelUnloaded.Add(HideCallBack);
				StreamingLevel->OnLevelHidden.Add(HideCallBack);
#if !WITH_EDITOR
				WillShowedLevel.Remove(LevelPath);
				return;
#endif
			}
			else
			{
				Instance->LevelCountToLoad--;
			}
		}
	}
	if (LevelCountToLoad == 0)
	{
		OnLevelShowned();
	}
}

void AMESLevelManager::OnLevelShowned()
{
	if (!CanCallBack)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("on level showned"));
	--LevelCountToLoad;
	if (LevelCountToLoad <= 0 && OnLevelShownCallback.IsBound())
	{
		LevelCountToLoad = 0;
		Instance->OnLevelShownCallback.Execute((Instance->MaxTaskCont - LevelCountToHide - LevelCountToLoad) / Instance->MaxTaskCont);
		OnLevelShownCallback.Unbind();
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}
	else
	{
		if (OnLevelShownCallback.IsBound())
		{
			Instance->OnLevelShownCallback.Execute((Instance->MaxTaskCont - LevelCountToHide - LevelCountToLoad) / Instance->MaxTaskCont);
		}
#if !WITH_EDITOR
		RealShow();
#endif
	}
}

void AMESLevelManager::OnLevelLoaded()
{
	if (!CanCallBack)
	{
		return;
	}
	--LevelCountToLoad;
	UE_LOG(LogTemp, Warning, TEXT("on level loaded, remain count: %d"), LevelCountToLoad);
	if (OnLevelLoadedCallback.IsBound())
	{
		OnLevelLoadedCallback.Execute(LevelCountToLoad);
	}
	if (LevelCountToLoad <= 0)
	{
		OnLevelLoadedCallback.Unbind();
	}
}

void AMESLevelManager::OnLevelHidened()
{
	if (!CanCallBack)
	{
		return;
	}
	--LevelCountToHide;

	if (LevelCountToHide <= 0)
	{
		LastShowedLevel = WillShowedLevel;
		LevelCountToHide = 0;
		if (OnLevelShownCallback.IsBound())
		{
			Instance->OnLevelShownCallback.Execute((Instance->MaxTaskCont - LevelCountToHide - LevelCountToLoad) / Instance->MaxTaskCont);
		}
		//HideFinish To LoadShow
		if (WillShowedLevel.Num() != 0)
		{
			Instance->RealShow();
		}
	}
	else
	{
		if (OnLevelShownCallback.IsBound())
		{
			Instance->OnLevelShownCallback.Execute((Instance->MaxTaskCont - LevelCountToHide - LevelCountToLoad) / Instance->MaxTaskCont);
		}
	}
}
