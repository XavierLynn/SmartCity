// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DigitalFrameNetWorkBlueprintLibrary.h"
#include "ProceduralMeshComponent.h"
#include "PlateMapActor.generated.h"



//DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateReturnSiteValue, TMap<FString, FString>)

USTRUCT(BlueprintType)
struct FSiteData
{
	GENERATED_USTRUCT_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlateMap")
	TMap<FString, FString> RetrunJsonValue;

};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelegateReturnSiteValue, FSiteData, JsonValue);

UCLASS()
class PLATEMAP_API APlateMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlateMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;




public:

	
	FHttpResponseDelegate HttpResponseDelegate;
	FHttpResponseDelegate SiteHttpResponseDelegate;


	UPROPERTY(BlueprintAssignable)
	
	FDelegateReturnSiteValue DelegateReturnSiteValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlateMap")
		FString ActorID;

	UFUNCTION(BlueprintCallable, Category = "PlateMap")
		void SetActorID(const FString& ID);

	UFUNCTION(BlueprintCallable, Category = "PlateMap")
		FString GetActorID();
	UFUNCTION()
	void GetResult(const FString& Result);

	void AnalysisHttpData(TSharedPtr<FJsonObject> JsonMeg);


	void AnalysisSiteHttpData(TSharedPtr<FJsonObject> JsonMeg);


	UFUNCTION()
		void GetConditionData(const FString& Result);

	UFUNCTION(BlueprintCallable,Category="PlateMap")
	void GetRespoent();

	UFUNCTION(BlueprintCallable, Category = "PlateMapCondition")
		void GetSiteCondition();
public:

	/*板块图坐标*/
	TArray<FVector> PlateCoordinates;


protected:

	class EMGISManager* GISManager;
	/*GIS中心坐标*/
	const FVector2D GISCenter = FVector2D(104.40788600613644, 31.12224719781043);

	TArray<FVector> Verties;

	TArray<int32> Triangles;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "ProceduralMesh")
	UMaterial* MeshMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProceduralMesh")
	UProceduralMeshComponent* PMC;

	void GenerateMesh();
	void ClearMeshData();


};
