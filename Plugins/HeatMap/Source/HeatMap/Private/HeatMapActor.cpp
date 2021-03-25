// Copyright x.Lin, Inc. All Rights Reserved.


#include "HeatMapActor.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Components/SceneComponent.h"

// Sets default values
AHeatMapActor::AHeatMapActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Resolution = 1024;
	Strength = 0.1f;
	Radius = 0.1f;
	MinPos = FVector2D(0.0f, 0.0f);
	MaxPos = FVector2D(Resolution, Resolution);

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = Scene;

	TCMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TCMesh"));
	TCMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneStaticMesh(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (PlaneStaticMesh.Succeeded())
	{
		TCMesh->SetStaticMesh(PlaneStaticMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_HeatMap(TEXT("MaterialInstanceConstant'/HeatMap/HeatMap/Materials/MI_HeatMap.MI_HeatMap'"));
	if (MI_HeatMap.Succeeded())
	{
	
		HeatMapMI = MI_HeatMap.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MI_TCRenderTarget(TEXT("MaterialInstanceConstant'/HeatMap/HeatMap/Materials/MI_RenderTarget.MI_RenderTarget'"));
	if (MI_TCRenderTarget.Succeeded())
	{
		TCRenderTargetMI = MI_TCRenderTarget.Object;
	}

	TCMesh->SetMaterial(0, HeatMapMI);
}

// Called when the game starts or when spawned
void AHeatMapActor::BeginPlay()
{
	Super::BeginPlay();

	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(GetWorld(), Resolution, Resolution);

	if (HeatMapMID)
	{
		HeatMapMID->SetTextureParameterValue(TEXT("RenderTarget"), RenderTarget);
		TCMesh->SetMaterial(0, HeatMapMID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HeatMapMID is null"));
	}
	if (!TCRenderTargetMID)
	{
		UE_LOG(LogTemp, Warning, TEXT("TCRenderTargetMID is null"));
	}
}

// Called every frame
void AHeatMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHeatMapActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	HeatMapMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), HeatMapMI);
	TCRenderTargetMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), TCRenderTargetMI);
}

void AHeatMapActor::AddPointToRenderTarget(FVector2D InPos, bool bIsNormalized)
{
	FVector2D tempPos = FVector2D(InPos);

	if (bIsNormalized)
	{
		tempPos = (tempPos - MinPos) / (MaxPos - MinPos);
	}
	float tempheaght = 0.0;
	tempheaght = UKismetMathLibrary::RandomFloat();
	TCRenderTargetMID->SetVectorParameterValue(TEXT("Pos"), FVector(tempPos, tempheaght));
	TCRenderTargetMID->SetScalarParameterValue(TEXT("Strength"), Strength);
	TCRenderTargetMID->SetScalarParameterValue(TEXT("Radius"), Radius);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(GetWorld(), RenderTarget, TCRenderTargetMID);
}

void AHeatMapActor::AddPointsToRenderTarget(TArray<FVector2D> InPosArray, bool bIsNormalized)
{
	for (auto pos : InPosArray)
	{
		AddPointToRenderTarget(pos, bIsNormalized);
	}
}

void AHeatMapActor::RandomMap(int32 Num /*= 1000*/)
{
	float tempPosX = 0.0;
	float tempPosY = 0.0;

	for (int32 i = 0; i < Num; i++)
	{
		tempPosX = UKismetMathLibrary::RandomFloat();
		tempPosY = UKismetMathLibrary::RandomFloat();
	
		AddPointToRenderTarget(FVector2D(tempPosX, tempPosY), false);
	}
}
