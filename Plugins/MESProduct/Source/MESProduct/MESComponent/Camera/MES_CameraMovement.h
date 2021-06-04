// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "MES_CameraMovement.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMES_CameraMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()
	
protected:

	virtual void ApplyControlInputToVelocity(float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MouseMoveStandardHeight = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxSpeedMultiplier = 1.0f;
};
