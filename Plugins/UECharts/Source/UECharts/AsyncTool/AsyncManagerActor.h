// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AsyncManagerActor.generated.h"


DECLARE_DELEGATE_OneParam(FTickHandle, float);

UCLASS()
class UECHARTS_API AAsyncManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsyncManagerActor();

public:

	static int RegisterUpdateCall(FTickHandle call);


	static void RemoveUpdateCall(int key);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	TMap<int, FTickHandle> CallMap;
	TMap<int, FTickHandle> EnterMap;
	TArray<int> QuitKeys;
	static int KeyCount;
	static AAsyncManagerActor* Instance;
};
