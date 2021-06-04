// Fill out your copyright notice in the Description page of Project Settings.


#include "MES_FirstPersonCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMES_FirstPersonCharacter::AMES_FirstPersonCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMES_FirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMES_FirstPersonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMES_FirstPersonCharacter::FellOutOfWorld(const UDamageType & dmgType)
{
	if (!IsPendingKill() && IsValidLowLevelFast() && GetCharacterMovement())
	{
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom, 0);
		SetActorLocation(FVector::ZeroVector);
		SetActorEnableCollision(true);
	}
}

void AMES_FirstPersonCharacter::OutsideWorldBounds()
{
	if (!IsPendingKill() && IsValidLowLevelFast() && GetCharacterMovement())
	{
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom, 0);
		SetActorLocation(FVector::ZeroVector);
		SetActorEnableCollision(true);
	}
}

// Called to bind functionality to input
//void AMES_FirstPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}
//
