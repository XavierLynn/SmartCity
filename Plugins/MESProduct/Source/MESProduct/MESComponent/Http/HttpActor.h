// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpNetComponent.h"
#include "HttpActor.generated.h"

UCLASS()
class MESPRODUCT_API AHttpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHttpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION(BlueprintCallable, Category = "MESUtil | Http")
		static void HttpRequestGet(FString Url, FHttpResponseDelegate Callback);
	UFUNCTION(BlueprintCallable, Category = "MESUtil | Http")
		static void HttpRequestPost(FString Url, FString ContentString, FHttpResponseDelegate Callback);

private:
	static AHttpActor* Instance;
	FHttpResponseDelegate OnHttpResponse;

	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
};
