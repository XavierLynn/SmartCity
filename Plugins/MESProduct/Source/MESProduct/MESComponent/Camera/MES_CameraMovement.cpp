// Fill out your copyright notice in the Description page of Project Settings.


#include "MES_CameraMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"

void UMES_CameraMovement::ApplyControlInputToVelocity(float DeltaTime)
{
	/*const FVector ControlAcceleration = GetPendingInputVector().GetClampedToMaxSize(1.f);

	const float AnalogInputModifier = (ControlAcceleration.SizeSquared() > 0.f ? ControlAcceleration.Size() : 0.f);
	const float MaxPawnSpeed = GetMaxSpeed() * AnalogInputModifier;
	const bool bExceedingMaxSpeed = IsExceedingMaxSpeed(MaxPawnSpeed);

	if (AnalogInputModifier > 0.f && !bExceedingMaxSpeed)
	{
		// Apply change in velocity direction
		if (Velocity.SizeSquared() > 0.f)
		{
			// Change direction faster than only using acceleration, but never increase velocity magnitude.
			const float TimeScale = FMath::Clamp(DeltaTime * TurningBoost, 0.f, 1.f);
			Velocity = Velocity + (ControlAcceleration * Velocity.Size() - Velocity) * TimeScale;
		}
	}
	else
	{
		// Dampen velocity magnitude based on deceleration.
		if (Velocity.SizeSquared() > 0.f)
		{
			const FVector OldVelocity = Velocity;
			const float VelSize = FMath::Max(Velocity.Size() - FMath::Abs(Deceleration) * DeltaTime, 0.f);
			Velocity = Velocity.GetSafeNormal() * VelSize;

			// Don't allow braking to lower us below max speed if we started above it.
			if (bExceedingMaxSpeed && Velocity.SizeSquared() < FMath::Square(MaxPawnSpeed))
			{
				Velocity = OldVelocity.GetSafeNormal() * MaxPawnSpeed;
			}
		}
	}

	// Apply acceleration and clamp velocity magnitude.
	const float NewMaxSpeed = (IsExceedingMaxSpeed(MaxPawnSpeed)) ? Velocity.Size() : MaxPawnSpeed;
	Velocity += ControlAcceleration * FMath::Abs(Acceleration) * DeltaTime;
	Velocity = Velocity.GetClampedToMaxSize(NewMaxSpeed);*/

	float HeightScale = 1.f;
	if (PawnOwner && !PawnOwner->IsPendingKill() && MouseMoveStandardHeight > 1.f)
	{
		FVector PawnLocation = PawnOwner->GetActorLocation();
		float PawnHeight = PawnLocation.Z;
		HeightScale = PawnHeight / MouseMoveStandardHeight;
		HeightScale = HeightScale < 1.f ? 1.f : HeightScale;
	}

	FVector LastInputVector = GetLastInputVector();
	FVector InputVector = ConsumeInputVector();
	if (InputVector.IsNearlyZero() && LastInputVector.IsNearlyZero())
	{
		FVector DecelerationDelta = Velocity.GetSafeNormal() * FMath::Abs(Deceleration * HeightScale) * DeltaTime;
		Velocity = Velocity.SizeSquared() > DecelerationDelta.SizeSquared() ? Velocity - DecelerationDelta : FVector::ZeroVector;
	}
	else
	{
		Velocity += InputVector * FMath::Abs(Acceleration * HeightScale) * DeltaTime;
		Velocity = Velocity.GetClampedToMaxSize(MaxSpeed * MaxSpeedMultiplier * HeightScale);
	}

	
}