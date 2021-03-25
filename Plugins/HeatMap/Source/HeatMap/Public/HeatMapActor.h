// Copyright x.Lin, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "HeatMapActor.generated.h"


UCLASS()
class HEATMAP_API AHeatMapActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHeatMapActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HeatMap")
		USceneComponent* Scene;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HeatMap")
		UStaticMeshComponent* TCMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HeatMap")
		UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HeatMap")
		UMaterialInstanceDynamic* HeatMapMID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HeatMap")
		UMaterialInstanceDynamic* TCRenderTargetMID;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "HeatMap")
		UMaterialInstance* HeatMapMI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "HeatMap")
		UMaterialInstance* TCRenderTargetMI;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeatMap")
		int32 Resolution;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeatMap")
		FVector2D MinPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeatMap")
		FVector2D MaxPos;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeatMap")
		float Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HeatMap")
		float Radius;

public:
	UFUNCTION(BlueprintCallable, Category = "HeatMap")
		void AddPointToRenderTarget(FVector2D InPos,bool bIsNormalized = true);

	UFUNCTION(BlueprintCallable, Category = "HeatMap")
		void AddPointsToRenderTarget(TArray<FVector2D> InPosArray, bool bIsNormalized = true);

	UFUNCTION(BlueprintCallable, Category = "HeatMap")
		void RandomMap(int32 Num = 1000);
};
