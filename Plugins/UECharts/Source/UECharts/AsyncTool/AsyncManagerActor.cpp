// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncManagerActor.h"

AAsyncManagerActor* AAsyncManagerActor::Instance = nullptr;
int AAsyncManagerActor::KeyCount = 0;

// Sets default values
AAsyncManagerActor::AAsyncManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

int AAsyncManagerActor::RegisterUpdateCall(FTickHandle call)
{
	if (!GWorld->GetWorld())return 0;
	KeyCount++;
	if (Instance == nullptr)
	{
		Instance = GWorld->GetWorld()->SpawnActor<AAsyncManagerActor>(FVector::ZeroVector, FRotator::ZeroRotator);
	}
	Instance->EnterMap.Add(KeyCount, call);
	return KeyCount;
}

void AAsyncManagerActor::RemoveUpdateCall(int key)
{
	Instance->QuitKeys.Add(key);
}

// Called when the game starts or when spawned
void AAsyncManagerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAsyncManagerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Instance = nullptr;
}

// Called every frame
void AAsyncManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (TPair<int32, FTickHandle>& element : EnterMap)
	{
		CallMap.Add(element.Key, element.Value);
	}
	EnterMap.Empty();
	for (int i = 0; i < QuitKeys.Num(); i++)
	{
		CallMap.Remove(QuitKeys[i]);
	}
	QuitKeys.Empty();
	for (TPair<int32, FTickHandle>& element : CallMap)
	{
		element.Value.ExecuteIfBound(DeltaTime);
	}

}

