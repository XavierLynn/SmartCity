// Fill out your copyright notice in the Description page of Project Settings.


#include "MES_CameraController.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "MES_CameraMovement.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "LevelSequence/Public/LevelSequencePlayer.h"
#include "LevelSequence/Public/LevelSequenceActor.h"
#include "LevelSequence/Public/LevelSequence.h"
#include "MovieSceneTrack.h"
#include "Tracks/MovieSceneFloatTrack.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Sections/MovieSceneFloatSection.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "CinematicCamera/Public/CineCameraActor.h"
#include "EngineUtils.h"

// Sets default values
AMES_CameraController::AMES_CameraController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(CapsuleComponent);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 0.f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	MovementComponent = CreateDefaultSubobject<UMES_CameraMovement>(TEXT("MovementComponent"));
	if (MovementComponent)
	{
		MovementComponent->UpdatedComponent = GetRootComponent();
	}
}

// Called when the game starts or when spawned
void AMES_CameraController::BeginPlay()
{
	Super::BeginPlay();
	IsMoveKeyDown = false;
	IsRotateSelfKeyDown = false;
	IsRotateAroundKeyDown = false;
	IsRotateAroundExtraKeyDown = false;
	IsPrepareFocus = false;
	BlendTimeToGo = 0.f;
	BlendTimeCache = 0.f;
	IsFreezing = false;
	IsPlayingAnimation = false;

	//SpawnLevelSequenceActor();
}

void AMES_CameraController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LevelSequenceActor && !LevelSequenceActor->IsPendingKill() && LevelSequenceActor->IsValidLowLevelFast())
	{
		LevelSequenceActor->Destroy();
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AMES_CameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetController() || !GetController()->IsValidLowLevelFast())
	{
		return;
	}

	UpdateRotateAround();

	if (BlendTimeToGo > 0.f)
	{
		BlendTimeToGo -= DeltaTime;
		if (BlendTimeToGo > 0.f)
		{
			float LerpWeight = (BlendTimeCache - BlendTimeToGo) / BlendTimeCache;
			FVector ViewLocation;
			FRotator ViewRotation;

			switch (BlendFunction)
			{
			case VTBlend_Linear:
				ViewLocation = FMath::Lerp(FocusStartPosition, FocusPosition, LerpWeight);
				ViewRotation = FMath::Lerp(FocusStartRotation, FocusRotation, LerpWeight);
				break;
			case VTBlend_Cubic:
				ViewLocation = FMath::CubicInterp(FocusStartPosition, FVector::ZeroVector, FocusPosition, FVector::ZeroVector, LerpWeight);
				ViewRotation = FMath::CubicInterp(FocusStartRotation, FRotator::ZeroRotator, FocusRotation, FRotator::ZeroRotator, LerpWeight);
				break;
			case VTBlend_EaseIn:
				ViewLocation = FMath::Lerp(FocusStartPosition, FocusPosition, FMath::Pow(LerpWeight, BlendExp));
				ViewRotation = FMath::Lerp(FocusStartRotation, FocusRotation, FMath::Pow(LerpWeight, BlendExp));
				break;
			case VTBlend_EaseOut:
				ViewLocation = FMath::Lerp(FocusStartPosition, FocusPosition, FMath::Pow(LerpWeight, 1.f / BlendExp));
				ViewRotation = FMath::Lerp(FocusStartRotation, FocusRotation, FMath::Pow(LerpWeight, 1.f / BlendExp));
				break;
			case VTBlend_EaseInOut:
				ViewLocation = FMath::InterpEaseInOut(FocusStartPosition, FocusPosition, LerpWeight, BlendExp);
				ViewRotation = FMath::InterpEaseInOut(FocusStartRotation, FocusRotation, LerpWeight, BlendExp);
				break;
			default:
				break;
			}

			UpdateLocation(ViewLocation);
			UpdateRotation(ViewRotation);
		}
		else
		{
			BlendTimeToGo = 0;
			UpdateLocation(FocusPosition);
			UpdateRotation(FocusRotation);
			InvokeCallback(CameraMoveType);
		}
	}

	/*if (IsPlayingAni())
	{
		UpdateRotation(GetActorRotation());
	}*/

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController &&
		(IsMoveKeyDown || IsRotateSelfKeyDown || IsRotateAroundKeyDown || IsRotateAroundExtraKeyDown) &&
		ShouldFixMousePosWhenOnScreenEdge())
	{
		int SizeX, SizeY;
		PlayerController->GetViewportSize(SizeX, SizeY);

		FVector2D MousePos;
		GetMousePos_Implementation(MousePos);

		float MouseX = MousePos.X;
		float MouseY = MousePos.Y;
		bool ShouldFixMousePosition = false;

		if (MouseX <= 2.f)
		{
			MouseX = SizeX - 2.1f;
			ShouldFixMousePosition = true;
		}
		else if(MouseX >= SizeX - 2.f)
		{
			MouseX = 2.1f;
			ShouldFixMousePosition = true;
		}
		
		if (MouseY <= 2.f)
		{
			MouseY = SizeY - 2.1f;
			ShouldFixMousePosition = true;
		}
		else if (MouseY >= SizeY - 2.f)
		{
			MouseY = 2.1f;
			ShouldFixMousePosition = true;
		}

		if (ShouldFixMousePosition)
		{
			PlayerController->SetMouseLocation(MouseX, MouseY);
		}
	}
}

//// Called to bind functionality to input
//void AMES_CameraController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//	PlayerInputComponent->BindAxis("MoveForward", this, &AMES_CameraController::MoveForward);
//	PlayerInputComponent->BindAxis("MoveRight", this, &AMES_CameraController::MoveRight);
//	PlayerInputComponent->BindAxis("MoveScale", this, &AMES_CameraController::MoveScale);
//	PlayerInputComponent->BindAxis("MouseX", this, &AMES_CameraController::MouseX);
//	PlayerInputComponent->BindAxis("MouseY", this, &AMES_CameraController::MouseY);
//
//	PlayerInputComponent->BindAction("MoveKey", EInputEvent::IE_Pressed, this, &AMES_CameraController::MoveKeyDown);
//	PlayerInputComponent->BindAction("MoveKey", EInputEvent::IE_Released, this, &AMES_CameraController::MoveKeyUp);
//
//	PlayerInputComponent->BindAction("RotateSelfKey", EInputEvent::IE_Pressed, this, &AMES_CameraController::RotateSelfKeyDown);
//	PlayerInputComponent->BindAction("RotateSelfKey", EInputEvent::IE_Released, this, &AMES_CameraController::RotateSelfKeyUp);
//
//	PlayerInputComponent->BindAction("RotateAroundKey", EInputEvent::IE_Pressed, this, &AMES_CameraController::RotateAroundKeyDown);
//	PlayerInputComponent->BindAction("RotateAroundKey", EInputEvent::IE_Released, this, &AMES_CameraController::RotateAroundKeyUp);
//	PlayerInputComponent->BindAction("RotateAroundExtraKey", EInputEvent::IE_Pressed, this, &AMES_CameraController::RotateAroundExtraKeyDown);
//	PlayerInputComponent->BindAction("RotateAroundExtraKey", EInputEvent::IE_Released, this, &AMES_CameraController::RotateAroundExtraKeyUp);
//
//	PlayerInputComponent->BindAction("FocusTo", EInputEvent::IE_Pressed, this, &AMES_CameraController::FocusDown);
//	PlayerInputComponent->BindAction("FocusTo", EInputEvent::IE_Released, this, &AMES_CameraController::FocusUp);
//}

void AMES_CameraController::MoveToBound(AActor * FocusTarget, float FocusTime)
{
	if (!FocusTarget)
	{
		return;
	}

	USceneComponent* SceneComponent = FocusTarget->FindComponentByClass<USceneComponent>();
	if (SceneComponent)
	{
		FCameraActionCompleteDynamicDelegate Callback;
		Callback.BindUFunction(this, "TestDynamicCallback");
		MoveToBound(SceneComponent->Bounds, FocusTime, Callback);
	}
}

void AMES_CameraController::CalcCamera(float DeltaTime, FMinimalViewInfo & OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	/*if (BlendTimeToGo > 0.f)
	{
		FMinimalViewInfo DesiredPOV = OutResult;
		DesiredPOV.Location = FocusPosition;
		DesiredPOV.Rotation = FocusRotation;

		BlendTimeToGo -= DeltaTime;
		if (BlendTimeToGo > 0.f)
		{
			float LerpWeight = (BlendTime - BlendTimeToGo) / BlendTime;

			float BlendPct = 0.f;
			switch (BlendFunction)
			{
			case VTBlend_Linear:
				BlendPct = FMath::Lerp(0.f, 1.f, LerpWeight);
				break;
			case VTBlend_Cubic:
				BlendPct = FMath::CubicInterp(0.f, 0.f, 1.f, 0.f, LerpWeight);
				break;
			case VTBlend_EaseIn:
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(LerpWeight, BlendExp));
				break;
			case VTBlend_EaseOut:
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(LerpWeight, 1.f / BlendExp));
				break;
			case VTBlend_EaseInOut:
				BlendPct = FMath::InterpEaseInOut(0.f, 1.f, LerpWeight, BlendExp);
				break;
			default:
				break;
			}

			OutResult.BlendViewInfo(DesiredPOV, BlendPct);
		}
		else
		{
			BlendTimeToGo = 0;
			UpdateLocation(FocusPosition);
			UpdateRotation(FocusRotation);
		}
	}*/
}

void AMES_CameraController::SetPos(FVector Location, FRotator Rotation)
{
	CheckAniStateOnMove();

	UpdateLocation(Location);
	UpdateRotation(Rotation);
}

void AMES_CameraController::MoveToPos(FVector Location, FRotator Rotation, float Seconds, FCameraActionCompleteDynamicDelegate Callback)
{
	CheckAniStateOnMove();

	FocusPosition = Location;
	FocusRotation = Rotation;
	BlendTimeToGo = Seconds;
	BlendTimeCache = Seconds;
	Controller->GetPlayerViewPoint(FocusStartPosition, FocusStartRotation);

	/*if (!MoveToPosDynamicCallbacks.Contains(Callback))
	{
		MoveToPosDynamicCallbacks.Add(Callback);
	}*/
	MoveToPosDynamicCallback.Unbind();
	MoveToPosDynamicCallback = Callback;
	ChangeCameraMoveType(ECameraMoveType::MoveToPos);
}

void AMES_CameraController::MoveToBound(FBoxSphereBounds Bounds, float Seconds, FCameraActionCompleteDynamicDelegate Callback)
{
	CheckAniStateOnMove();

	const FBox BoundingBox = Bounds.GetBox();

	const FVector Position = BoundingBox.GetCenter();
	float Radius = FMath::Max(BoundingBox.GetExtent().Size(), 10.f);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
	{
		return;
	}
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer || !LocalPlayer->ViewportClient)
	{
		return;
	}

	FViewport* Viewport = LocalPlayer->ViewportClient->Viewport;
	if (!Viewport)
	{
		return;
	}

	float AspectToUse = 1.777777f;
	FIntPoint ViewportSize = Viewport->GetSizeXY();
	if (ViewportSize.X > 0 && ViewportSize.Y > 0)
	{
		AspectToUse = Viewport->GetDesiredAspectRatio();
	}

	if (AspectToUse > 1.0f)
	{
		Radius *= AspectToUse;
	}

	const float ViewFOV = FollowCamera->FieldOfView;
	const float HalfFOVRadians = FMath::DegreesToRadians(ViewFOV / 2.0f);
	const float DistanceFromSphere = Radius / FMath::Tan(HalfFOVRadians);
	FVector CameraOffsetVector = PlayerController->PlayerCameraManager->GetActorForwardVector() * DistanceFromSphere * -1;

	FocusPosition = Position + CameraOffsetVector;
	FocusRotation = (Position - FocusPosition).Rotation();
	BlendTimeToGo = Seconds;
	BlendTimeCache = Seconds;
	Controller->GetPlayerViewPoint(FocusStartPosition, FocusStartRotation);

	/*if (!MoveToBoundDynamicCallbacks.Contains(Callback))
	{
		MoveToBoundDynamicCallbacks.Add(Callback);
	}*/
	MoveToBoundDynamicCallback.Unbind();
	MoveToBoundDynamicCallback = Callback;
	ChangeCameraMoveType(ECameraMoveType::MoveToBound);
}

bool AMES_CameraController::IsMovingToPos()
{
	return BlendTimeToGo > 0.f;
}

void AMES_CameraController::PlayAni(FString AniName, FCameraActionCompleteDynamicDelegate Callback)
{
	CheckMoveStateOnAni();

	if (LevelSequenceActor && LevelSequenceActor->GetSequencePlayer() && LevelSequenceActor->GetSequencePlayer()->IsPaused() && LevelSequenceActor->GetSequence() && LevelSequenceActor->GetSequence()->GetName().Equals(AniName))
	{
		LevelSequenceActor->GetSequencePlayer()->Play();
		
		PlayAniDynamicCallback.Unbind();
		PlayAniDynamicCallback = Callback;
		OnPlayAni();
		return;
	}

	InitializeSequence(AniName, Callback);
}

void AMES_CameraController::PauseAni()
{
	if (LevelSequenceActor && LevelSequenceActor->GetSequencePlayer() && LevelSequenceActor->GetSequencePlayer()->IsPlaying())
	{
		LevelSequenceActor->GetSequencePlayer()->Pause();
		OnPauseAni();
	}
}

void AMES_CameraController::ResetAni(bool IsContinueToPlay)
{
	CheckMoveStateOnAni();

	if (LevelSequenceActor && LevelSequenceActor->GetSequencePlayer())
	{
		LevelSequenceActor->GetSequencePlayer()->PlayToFrame(LevelSequenceActor->GetSequencePlayer()->GetStartTime().Time);
		//LevelSequenceActor->GetSequencePlayer()->Stop();

		if (IsContinueToPlay)
		{
			LevelSequenceActor->GetSequencePlayer()->Play();
			OnPlayAni();
		}
		else
		{
			LevelSequenceActor->GetSequencePlayer()->Pause();
			OnPauseAni();
		}
	}
}

bool AMES_CameraController::IsPlayingAni()
{
	/*if (LevelSequenceActor && LevelSequenceActor->GetSequencePlayer())
	{
		return LevelSequenceActor->GetSequencePlayer()->IsPlaying();
	}*/

	return IsPlayingAnimation;
}

void AMES_CameraController::JumpToAni(FString AniName, float DurationPercentage, bool IsContinueToPlay, FCameraActionCompleteDynamicDelegate Callback)
{
	CheckMoveStateOnAni();

	if (LevelSequenceActor && LevelSequenceActor->GetSequencePlayer() && LevelSequenceActor->GetSequence() && LevelSequenceActor->GetSequence()->GetName().Equals(AniName))
	{
		LevelSequenceActor->GetSequencePlayer()->PlayToFrame(LevelSequenceActor->GetSequencePlayer()->GetDuration().Time * DurationPercentage);

		PlayAniDynamicCallback.Unbind();
		PlayAniDynamicCallback = Callback;
	}
	else
	{
		InitializeSequence(AniName, Callback);
		LevelSequenceActor->GetSequencePlayer()->PlayToFrame(LevelSequenceActor->GetSequencePlayer()->GetDuration().Time * DurationPercentage);
	}

	if (LevelSequenceActor->GetSequencePlayer())
	{
		if (IsContinueToPlay)
		{
			LevelSequenceActor->GetSequencePlayer()->Play();
			OnPlayAni();
		}
		else
		{
			LevelSequenceActor->GetSequencePlayer()->Pause();
			OnPauseAni();
		}
	}
}

float AMES_CameraController::GetAniPercentage()
{
	float Result = 0.0f;
	if (LevelSequenceActor && LevelSequenceActor->GetSequencePlayer())
	{
		float TotalDuration = LevelSequenceActor->GetSequencePlayer()->GetDuration().AsSeconds();
		float CurrentPos = LevelSequenceActor->GetSequencePlayer()->GetCurrentTime().AsSeconds();
		Result = TotalDuration == 0.f ? 0.f : CurrentPos / TotalDuration;
	}

	return Result;
}

void AMES_CameraController::FreezeCamera()
{
	IsFreezing = true;
}

void AMES_CameraController::UnfreezeCamera()
{
	IsFreezing = false;
}

void AMES_CameraController::UpdateRotateAroundMode(EMESCameraControllerRotateAroundMode NewMode)
{
	RotateAroundMode = NewMode;
}

void AMES_CameraController::UpdateRotateAroundPoint(FVector SpecifyPoint)
{
	RotateAroundPos = SpecifyPoint;
	UpdateRotateAroundMode(EMESCameraControllerRotateAroundMode::SpecifyPoint);
}

void AMES_CameraController::GetMousePos_Implementation(FVector2D & MousePos)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		float MouseX, MouseY;
		PlayerController->GetMousePosition(MouseX, MouseY);
		MousePos.X = MouseX;
		MousePos.Y = MouseY;
	}
}

void AMES_CameraController::TestDynamicCallback()
{
	UE_LOG(LogTemp, Log, TEXT("TestDynamicCallback invoke"));
}

template<typename UserClass> void MoveToPos(FVector WorldPos, float Seconds, UserClass* Object,
	typename FCameraActionCompleteDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr Func)
{
	UE_LOG(LogTemp, Log, TEXT("Move To Pos"));
}

template<typename UserClass> void MoveToBound(struct FBoxSphereBounds Bound, float Seconds, UserClass* Object,
	typename FCameraActionCompleteDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr Func)
{
	UE_LOG(LogTemp, Log, TEXT("Move To Bound"));
}

template<typename UserClass> void PlayAni(FString AniName, UserClass* Object,
	typename FCameraActionCompleteDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr Func)
{
	UE_LOG(LogTemp, Log, TEXT("Play Ani: %s"), *AniName);
}

void AMES_CameraController::MoveForward(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f) && CouldMoveAndRotate())
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(Rotation.Pitch, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
		OnInputMoveOrRotate(true);
	}
}

void AMES_CameraController::MoveRight(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f) && CouldMoveAndRotate())
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(Rotation.Pitch, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, AxisValue);
		OnInputMoveOrRotate(true);
	}
}

void AMES_CameraController::MoveUp(float AxisValue)
{
	if ((Controller != NULL) && (AxisValue != 0.0f) && CouldMoveAndRotate())
	{
		// find out which way is up
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(Rotation.Pitch, Rotation.Yaw, 0);

		// get up vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
		AddMovementInput(Direction, AxisValue);
		OnInputMoveOrRotate(true);
	}
}

void AMES_CameraController::MoveScale(float AxisValue)
{
	MoveForward(AxisValue * MoveScaleMultiplier);
}

void AMES_CameraController::MouseX(float AxisValue)
{
	// rotate around
	if (IsRotateAroundKeyDown && IsRotateAroundExtraKeyDown && FMath::Abs(AxisValue) > MoveZoomFactor && CouldMoveAndRotate())
	{
		MouseInputAxis.X += AxisValue;
		IsPrepareFocus = false;
		return;
	}

	if (IsMoveKeyDown)
	{
		MoveRight(AxisValue * -1.f);
		if (IsPrepareFocus && FMath::Abs(AxisValue) > MoveZoomFactor)
		{
			IsPrepareFocus = false;
		}
		return;
	}

	if (IsRotateSelfKeyDown && CouldMoveAndRotate())
	{
		AddControllerYawInput(AxisValue);
		OnInputMoveOrRotate(false);
	}
}

void AMES_CameraController::MouseY(float AxisValue)
{
	// rotate around
	if (IsRotateAroundKeyDown && IsRotateAroundExtraKeyDown && FMath::Abs(AxisValue) > MoveZoomFactor && CouldMoveAndRotate())
	{
		MouseInputAxis.Y += AxisValue;
		IsPrepareFocus = false;
		return;
	}

	if (IsMoveKeyDown)
	{
		MoveUp(AxisValue * -1.f);
		if (IsPrepareFocus && FMath::Abs(AxisValue) > MoveZoomFactor)
		{
			IsPrepareFocus = false;
		}
		return;
	}

	if (IsRotateSelfKeyDown && CouldMoveAndRotate() && (!IsRotateAroundKeyDown || !IsRotateAroundExtraKeyDown))
	{
		AddControllerPitchInput(AxisValue * -1);
		OnInputMoveOrRotate(false);
	}
}

void AMES_CameraController::MoveKeyDown()
{
	IsMoveKeyDown = true;
}

void AMES_CameraController::MoveKeyUp()
{
	IsMoveKeyDown = false;
}

void AMES_CameraController::RotateSelfKeyDown()
{
	IsRotateSelfKeyDown = true;
}

void AMES_CameraController::RotateSelfKeyUp()
{
	IsRotateSelfKeyDown = false;
}

void AMES_CameraController::RotateAroundKeyDown()
{
	IsRotateAroundKeyDown = true;
	if (IsRotateAroundExtraKeyDown)
	{
		CheckRotateAroundMode();
	}
}

void AMES_CameraController::RotateAroundKeyUp()
{
	IsRotateAroundKeyDown = false;
}

void AMES_CameraController::RotateAroundExtraKeyDown()
{
	IsRotateAroundExtraKeyDown = true;
	if (IsRotateAroundKeyDown)
	{
		CheckRotateAroundMode();
	}
}

void AMES_CameraController::RotateAroundExtraKeyUp()
{
	IsRotateAroundExtraKeyDown = false;
}

void AMES_CameraController::FocusDown()
{
	IsPrepareFocus = true;
}

void AMES_CameraController::FocusUp()
{
	if (IsPrepareFocus)
	{
		MoveToBound(RayTraceToMouse(), FocusToTime);
		IsPrepareFocus = false;
	}
}

bool AMES_CameraController::IsWithEditor()
{
#if WITH_EDITOR
	return true;
#endif

	return false;
}

void AMES_CameraController::UpdateRotateAround()
{
	if (IsRotateAroundKeyDown && IsRotateAroundExtraKeyDown &&
		(FMath::Abs(MouseInputAxis.X) > MoveZoomFactor || FMath::Abs(MouseInputAxis.Y) > MoveZoomFactor) &&
		CouldMoveAndRotate())
	{
		FVector CameraLocation = GetActorLocation()/*UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation()*/;
		FVector BetweenVector = CameraLocation - RotateAroundPos;

		float AngleX = RotateAroundYawMultiplier * MouseInputAxis.X;
		float AngleY = RotateAroundPitchMultiplier * MouseInputAxis.Y;
		FVector RightVector = FVector::CrossProduct(FVector::UpVector, BetweenVector).GetSafeNormal();

		//UE_LOG(LogTemp, Warning, TEXT("angle x: %f, angle y: %f"), AngleX, AngleY);

		/*FQuat QuatX(FVector::UpVector, AngleX * PI / 180.f);
		FQuat QuatY(RightVector, AngleY * PI / 180.f);

		FVector NewBetweenVectorX = QuatX.RotateVector(BetweenVector);
		FVector NewBetweenVectorY = QuatY.RotateVector(BetweenVector);*/

		FVector NewBetweenVectorX = BetweenVector.RotateAngleAxis(AngleX, FVector::UpVector);
		FVector NewBetweenVectorY = BetweenVector.RotateAngleAxis(AngleY, RightVector);
		
		FVector BetweenProjection = FVector(BetweenVector.X, BetweenVector.Y, 0.f);
		float BetweenAngle = FMath::Acos(FVector::DotProduct(BetweenVector, BetweenProjection) / BetweenVector.Size() / BetweenProjection.Size()) * 180.f / PI;

		/*FVector NewBetweenProjection = FVector(NewBetweenVectorY.X, NewBetweenVectorY.Y, 0.f);
		float NewBetweenAngle = FMath::Acos(FVector::DotProduct(NewBetweenVectorY, NewBetweenProjection) / NewBetweenVectorY.Size() / NewBetweenProjection.Size()) * 180.f / PI;*/

		/*FQuat BetweenQuat = FQuat::FindBetween(BetweenVector.GetAbs(), FVector(1.f, 1.f, 0.f));
		float BetweenAngle = BetweenQuat.GetAngle() * 180.f / PI;*/

		float HeightSign = FMath::Sign(BetweenVector.Z);
		BetweenAngle *= HeightSign;


		/*FQuat NewBetweenQuat = FQuat::FindBetween(NewBetweenVectorY.GetAbs(), FVector(1.f, 1.f, 0.f));
		float NewBetweenAngle = NewBetweenQuat.GetAngle() * 180.f / PI;
		float NewHeightSign = FMath::Sign(NewBetweenVectorY.Z);
		NewBetweenAngle *= NewHeightSign;*/
		float NewBetweenAngle = BetweenAngle - AngleY;

		//UE_LOG(LogTemp, Warning, TEXT("between angle: %f, new between angle: %f"), BetweenAngle, NewBetweenAngle);

		bool IsIncreaseAngle = BetweenAngle < NewBetweenAngle;
		bool IsOutOfAngle = false;
		if (NewBetweenAngle > RotateAroundPitchAngleMax ||
			BetweenAngle > RotateAroundPitchAngleMax ||
			NewBetweenAngle < RotateAroundPitchAngleMin ||
			BetweenAngle < RotateAroundPitchAngleMin)
		{
			IsOutOfAngle = true;
		}
		//if (IsIncreaseAngle && NewBetweenAngle > RotateAroundPitchAngleMax && BetweenAngle > RotateAroundPitchAngleMax)
		//{
		//	IsOutOfAngle = true;
		//	//UE_LOG(LogTemp, Warning, TEXT("max, between angle: %f, new between angle: %f"), BetweenAngle, NewBetweenAngle);
		//}
		//if (!IsIncreaseAngle && NewBetweenAngle < RotateAroundPitchAngleMin && BetweenAngle < RotateAroundPitchAngleMin)
		//{
		//	IsOutOfAngle = true;
		//	//UE_LOG(LogTemp, Warning, TEXT("min, between angle: %f, new between angle: %f"), BetweenAngle, NewBetweenAngle);
		//}

		FVector DeltaLocationX = NewBetweenVectorX - BetweenVector;
		FVector DeltaLocationY = (IsOutOfAngle || !CouldRotateAroundPitch) ? FVector::ZeroVector : NewBetweenVectorY - BetweenVector;
		/*if (!DeltaLocationY.IsNearlyZero())
		{
			UE_LOG(LogTemp, Warning, TEXT("DeltaLocationY, x:%f, y:%f, z:%f"), DeltaLocationY.X, DeltaLocationY.Y, DeltaLocationY.Z);
			UE_LOG(LogTemp, Warning, TEXT("min, between angle: %f, new between angle: %f, is out of angle: %d"), BetweenAngle, NewBetweenAngle, (int)IsOutOfAngle);
		}*/

		FVector NewLocation = DeltaLocationX + DeltaLocationY + CameraLocation;
		/*FVector DeltaLocation = NewLocation - CameraLocation;
		DeltaLocation.Z = 0.f;
		UpdateLocation(DeltaLocation + GetActorLocation());*/
		//UE_LOG(LogTemp, Log, TEXT("delta location, x:%f, y:%f, z:%f"), DeltaLocation.X, DeltaLocation.Y, DeltaLocation.Z);
		UpdateLocation(NewLocation);
		OnInputMoveOrRotate(true);

		/*FRotator DeltaRotation = FQuat::FindBetweenVectors(BetweenVector, NewBetweenVector).Rotator();
		FRotator CameraRotation = GetController()->GetControlRotation();*/
		FVector NewForward = RotateAroundPos - NewLocation;
		FRotator NewRot = NewForward.Rotation();

		FRotator OldRotation = GetController()->GetControlRotation();
		FRotator NewRotation(NewRot.Pitch, NewRot.Yaw, OldRotation.Roll/*CameraRotation.Pitch, CameraRotation.Yaw + DeltaRotation.Yaw, CameraRotation.Roll*/);
		UpdateRotation(NewRotation);
		OnInputMoveOrRotate(false);

		MouseInputAxis = FVector::ZeroVector;
	}
}

void AMES_CameraController::CheckRotateAroundMode()
{
	switch (RotateAroundMode)
	{
	case EMESCameraControllerRotateAroundMode::CameraForward:
		RayTraceForward(RotateAroundPos);
		break;
	case EMESCameraControllerRotateAroundMode::MouseForward:
		RayTraceToMouse(RotateAroundPos);
		break;
	case EMESCameraControllerRotateAroundMode::SpecifyPoint:
		// should use UpdateRotateAroundPoint() to set this point to rotate around.
		break;
	default:
		break;
	}
}

void AMES_CameraController::RayTraceForward(FVector& OutPoint)
{
	FHitResult OutHit;

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!CameraManager)
	{
		return;
	}
	FVector TraceStart = CameraManager->GetCameraLocation();
	FVector CameraForward = CameraManager->GetActorForwardVector();
	FVector TraceEnd = TraceStart + CameraForward * TraceMaxDistance;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByProfile(OutHit, TraceStart, TraceEnd, LineTraceProfileName, Params))
	{
		OutPoint = OutHit.Actor.Get()->GetComponentsBoundingBox().GetCenter();;
		//UE_LOG(LogTemp, Log, TEXT("hit location, x:%f, y:%f, z:%f"), OutPoint.X, OutPoint.Y, OutPoint.Z);
		//OutDistance = OutHit.Distance/*(OutPoint - TraceStart).Size()*/;
	}
	else if (GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Camera, Params))
	{
		OutPoint = OutHit.Actor.Get()->GetComponentsBoundingBox().GetCenter();
	}
	else
	{
		//OutDistance = MiddleMouseTraceDefaultDistance;
		OutPoint = TraceStart + CameraForward * TraceDistanceNoHit;
	}
}

void AMES_CameraController::RayTraceToMouse(FVector& OutPoint)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector MouseWorldPos;
		FVector TraceDir;
		if (DeprojectMousePositionToWorld(MouseWorldPos, TraceDir))
		{
			FVector TraceStart = CameraLocation;
			FVector TraceEnd = TraceStart + (TraceDir * TraceMaxDistance);

			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);

			FHitResult OutHit;
			if (GetWorld()->LineTraceSingleByProfile(OutHit, TraceStart, TraceEnd, LineTraceProfileName, TraceParams))
			{
				OutPoint = OutHit.Actor.Get()->GetComponentsBoundingBox().GetCenter();
			}
			else if (GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Camera, TraceParams))
			{
				OutPoint = OutHit.Actor.Get()->GetComponentsBoundingBox().GetCenter();
			}
			else
			{
				OutPoint = TraceStart + TraceDir * TraceDistanceNoHit;
			}
		}
	}
}

AActor * AMES_CameraController::RayTraceToMouse()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector MouseWorldPos;
		FVector TraceDir;
		if (DeprojectMousePositionToWorld(MouseWorldPos, TraceDir))
		{
			FVector TraceStart = CameraLocation;
			FVector TraceEnd = TraceStart + (TraceDir * TraceMaxDistance);

			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);

			FHitResult OutHit;
			if (GetWorld()->LineTraceSingleByProfile(OutHit, TraceStart, TraceEnd, LineTraceProfileName, TraceParams))
			{
				return OutHit.Actor.Get();
			}
			else if(GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Camera, TraceParams))
			{
				return OutHit.Actor.Get();
			}
		}
	}
	return nullptr;
}

bool AMES_CameraController::DeprojectMousePositionToWorld(FVector & WorldLocation, FVector & WorldDirection)
{
	FVector2D MousePosition;
	GetMousePos(MousePosition);

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		return UGameplayStatics::DeprojectScreenToWorld(PlayerController, MousePosition, WorldLocation, WorldDirection);
	}

	return false;
}

void AMES_CameraController::InvokeCallback(ECameraMoveType InCameraMoveType)
{
	switch (InCameraMoveType)
	{
	case AMES_CameraController::ECameraMoveType::MoveToPos:
		if (MoveToPosDynamicCallback.IsBound())
		{
			MoveToPosDynamicCallback.Execute();
			MoveToPosDynamicCallback.Unbind();
		}
		/*if (MoveToPosCallbacks.IsBound())
		{
			MoveToPosCallbacks.Broadcast();
		}
		if (MoveToPosDynamicCallbacks.IsBound())
		{
			MoveToPosDynamicCallbacks.Broadcast();
		}*/
		break;
	case AMES_CameraController::ECameraMoveType::MoveToBound:
		if (MoveToBoundDynamicCallback.IsBound())
		{
			MoveToBoundDynamicCallback.Execute();
			MoveToBoundDynamicCallback.Unbind();
		}
		/*if (MoveToBoundCallbacks.IsBound())
		{
			MoveToBoundCallbacks.Broadcast();
		}
		if (MoveToBoundDynamicCallbacks.IsBound())
		{
			MoveToBoundDynamicCallbacks.Broadcast();
		}*/
		break;
	case AMES_CameraController::ECameraMoveType::PlayAni:
		if (PlayAniDynamicCallback.IsBound())
		{
			PlayAniDynamicCallback.Execute();
			PlayAniDynamicCallback.Unbind();
		}

		/*if (PlayAniCallbacks.IsBound())
		{
			PlayAniCallbacks.Broadcast();
		}
		if (PlayAniDynamicCallbacks.IsBound())
		{
			PlayAniDynamicCallbacks.Broadcast();
		}*/
		break;
	default:
		break;
	}
	ChangeCameraMoveType(ECameraMoveType::None);
}

void AMES_CameraController::PlayAniSelfCallback()
{
	IsPlayingAnimation = false;
	InvokeCallback(CameraMoveType);
}

void AMES_CameraController::SpawnLevelSequenceActor()
{
	if (LevelSequenceActor && !LevelSequenceActor->IsPendingKill() && LevelSequenceActor->IsValidLowLevelFast())
	{
		LevelSequenceActor->Destroy();
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.ObjectFlags |= RF_Transient;
	SpawnParams.bAllowDuringConstructionScript = true;
	SpawnParams.bDeferConstruction = true;

	LevelSequenceActor = GetWorld()->SpawnActor<ALevelSequenceActor>(SpawnParams);

	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	PlaybackSettings.bAutoPlay = false;
	PlaybackSettings.bPauseAtEnd = true;

	LevelSequenceActor->PlaybackSettings = PlaybackSettings;

	FTransform DefaultTransform;
	LevelSequenceActor->FinishSpawning(DefaultTransform);
}

void AMES_CameraController::ChangeCameraMoveType(ECameraMoveType NewMoveType)
{
	if (CameraMoveType == NewMoveType)
	{
		return;
	}

	// change before
	switch (CameraMoveType)
	{
	case AMES_CameraController::ECameraMoveType::None:
		break;
	case AMES_CameraController::ECameraMoveType::MoveToPos:
		break;
	case AMES_CameraController::ECameraMoveType::MoveToBound:
		break;
	case AMES_CameraController::ECameraMoveType::PlayAni:
	{
		IsPlayingAnimation = false;
		for (FMovieSceneBinding Binding : LevelSequenceActor->GetSequence()->GetMovieScene()->GetBindings())
		{
			const FGuid ObjectGuid = Binding.GetObjectGuid();
			FMovieSceneObjectBindingID BindingID(ObjectGuid, MovieSceneSequenceID::Root);
			for (UObject* BoundObject : LevelSequenceActor->GetSequencePlayer()->GetBoundObjects(BindingID))
			{
				if (ACineCameraActor* CineCameraActor = Cast<ACineCameraActor>(BoundObject))
				{
					SetPos(CineCameraActor->GetActorLocation(), CineCameraActor->GetActorRotation());
					//YY:add
					LevelSequenceActor->GetSequencePlayer()->Stop();
					break;
				}
			}
		}

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			PlayerController->SetViewTargetWithBlend(this);
		}

		if (DisableInputWhenPlayAni)
		{
			UnfreezeCamera();
		}
	}
		
		break;
	default:
		break;
	}

	CameraMoveType = NewMoveType;

	// change after
	switch (CameraMoveType)
	{
	case AMES_CameraController::ECameraMoveType::None:
		break;
	case AMES_CameraController::ECameraMoveType::MoveToPos:
		break;
	case AMES_CameraController::ECameraMoveType::MoveToBound:
		break;
	case AMES_CameraController::ECameraMoveType::PlayAni:
		break;
	default:
		break;
	}
}

void AMES_CameraController::InitializeSequence(FString AniName, FCameraActionCompleteDynamicDelegate Callback)
{
	for (int i = 0; i < CameraAnis.Num(); ++i)
	{
		ULevelSequence* LevelSequence = CameraAnis[i];

		if (!LevelSequence)
		{
			continue;
		}

		if (!LevelSequence->GetName().Equals(AniName))
		{
			continue;
		}

		SpawnLevelSequenceActor();
		LevelSequenceActor->SetSequence(LevelSequence);
		/*
		FGuid ActorGuid = LevelSequence->FindPossessableObjectId(*this, GetWorld());
		FGuid CameraComponentGuid = LevelSequence->FindPossessableObjectId(*FollowCamera, GetWorld());
		if (!ActorGuid.IsValid() && !CameraComponentGuid.IsValid())
		{
			ActorGuid = LevelSequence->GetMovieScene()->AddPossessable(this->GetName(), this->GetClass());
			LevelSequence->BindPossessableObject(ActorGuid, *this, GetWorld());
			FMovieSceneBinding* NewActorBinding = LevelSequence->GetMovieScene()->FindBinding(ActorGuid);

			CameraComponentGuid = LevelSequence->GetMovieScene()->AddPossessable(FollowCamera->GetName(), FollowCamera->GetClass());
			LevelSequence->BindPossessableObject(CameraComponentGuid, *FollowCamera, GetWorld());
			FMovieSceneBinding* NewCameraComponentBinding = LevelSequence->GetMovieScene()->FindBinding(CameraComponentGuid);

			//for (int32 Index = 0; Index < LevelSequence->GetMovieScene()->GetSpawnableCount(); ++Index)
			//{
			//	FGuid ThisGuid = LevelSequence->GetMovieScene()->GetSpawnable(Index).GetGuid();

			//	for (TWeakObjectPtr<> WeakObject : FindBoundObjects(ThisGuid, SequenceID))
			//	{
			//		if (WeakObject.IsValid())
			//		{
			//			AActor* Actor = Cast<AActor>(WeakObject.Get());

			//			if (Actor)
			//			{
			//				UBlueprint* Blueprint = Cast<UBlueprint>(Actor->GetClass()->ClassGeneratedBy);
			//				if (Blueprint && Blueprint->bRunConstructionScriptInSequencer)
			//				{
			//					BoundActors.Add(Actor);
			//					BoundGuids.Add(TPair<FMovieSceneSequenceID, FGuid>(SequenceID, ThisGuid));
			//				}
			//			}
			//		}
			//	}
			//}

			for (FMovieSceneBinding MovieBinding : LevelSequence->GetMovieScene()->GetBindings())
			{
				if (MovieBinding.GetName().Equals("CameraComponent"))
				{
					for (UMovieSceneTrack* Track : MovieBinding.GetTracks())
					{
						UMovieSceneFloatTrack* FloatTrack = Cast<UMovieSceneFloatTrack>(Track);
						if (FloatTrack && FloatTrack->GetPropertyName().IsEqual("CurrentFocalLength"))
						{
							UMovieSceneFloatTrack* FovTrack = Cast<UMovieSceneFloatTrack>(StaticDuplicateObject(Track, LevelSequence->GetMovieScene()));
							NewCameraComponentBinding->AddTrack(*FovTrack);

							FovTrack->SetPropertyNameAndPath("FieldOfView", "FieldOfView");

							for (UMovieSceneSection* Section : FovTrack->GetAllSections())
							{
								UMovieSceneFloatSection* FloatSection = Cast<UMovieSceneFloatSection>(Section);
								if (FloatSection)
								{
									//TArrayView<const FMovieSceneFloatValue> Values = FloatSection->GetChannel().GetValues();
									//FloatSection->GetChannel().Set(FloatSection->GetChannel().GetTimes(), Values);
									for (FMovieSceneFloatChannel* Channel : FloatSection->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>())
									{
										// Convert FocalLength --->>>  FieldOfView
										// CurrentFocalLength = (Filmback.SensorWidth / 2.f) / FMath::Tan(FMath::DegreesToRadians(InFieldOfView / 2.f));
										Channel->SetDefault(FMath::RadiansToDegrees(FMath::Atan2(Channel->GetDefault().GetValue(), 11.83f) * 2.f));
										for (FMovieSceneFloatValue FloatValue : Channel->GetValues())
										{
											FloatValue.Value = FMath::RadiansToDegrees(FMath::Atan2(FloatValue.Value, 11.83f) * 2.f);
										}
										
									}
								}
							}

							//UMovieSceneFloatTrack* NewFovTrack = Cast<UMovieSceneFloatTrack>(LevelSequence->GetMovieScene()->AddTrack(UMovieSceneFloatTrack::StaticClass(), CameraComponentGuid));
							//if (NewFovTrack)
							//{
							//	NewFovTrack->SetPropertyNameAndPath("FieldOfView", "FieldOfView");

							//	//UMovieSceneFloatSection* FovSection = Cast<UMovieSceneFloatSection>(NewFovTrack->CreateNewSection());
							//	for (UMovieSceneSection* Section : NewFovTrack->GetAllSections())
							//	{
							//		UMovieSceneFloatSection* FloatSection = Cast<UMovieSceneFloatSection>(Section);
							//		if (FloatSection)
							//		{
							//			UMovieSceneFloatSection* FovSection = Cast<UMovieSceneFloatSection>(NewFovTrack->CreateNewSection());
							//			NewFovTrack->AddSection(*FovSection);

							//			FovSection->SetRange(FloatSection->GetRange());
							//			FovSection->SetCompletionMode(FloatSection->GetCompletionMode());
							//			FovSection->SetBlendType(FloatSection->GetBlendType().Get());
							//			FovSection->SetRowIndex(FloatSection->GetRowIndex());
							//			FovSection->SetOverlapPriority(FloatSection->GetOverlapPriority());
							//		}
							//	}

							//	TArrayView<FMovieSceneFloatChannel*> FloatChannels = TransformSection->GetChannelProxy().GetChannels<FMovieSceneFloatChannel>();
							//	FloatChannels[0]->SetDefault(Location.X);
							//	FloatChannels[1]->SetDefault(Location.Y);
							//	FloatChannels[2]->SetDefault(Location.Z);
							//	FloatChannels[3]->SetDefault(Rotation.X);
							//	FloatChannels[4]->SetDefault(Rotation.Y);
							//	FloatChannels[5]->SetDefault(Rotation.Z);
							//	FloatChannels[6]->SetDefault(Scale.X);
							//	FloatChannels[7]->SetDefault(Scale.Y);
							//	FloatChannels[8]->SetDefault(Scale.Z);

							//	for (int32 Counter = 0; Counter < KeyTimes.Num(); ++Counter)
							//	{
							//		FFrameNumber KeyTime = KeyTimes[Counter];

							//		FTransform Transform(Rotations[Counter], Locations[Counter]);
							//		FTransform LocalTransform = ParentInverseTransform * Transform;
							//		FVector LocalTranslation = LocalTransform.GetTranslation();
							//		FVector LocalRotation = LocalTransform.GetRotation().Euler();

							//		FloatChannels[0]->AddLinearKey(KeyTime, LocalTranslation.X);
							//		FloatChannels[1]->AddLinearKey(KeyTime, LocalTranslation.Y);
							//		FloatChannels[2]->AddLinearKey(KeyTime, LocalTranslation.Z);
							//		FloatChannels[3]->AddLinearKey(KeyTime, LocalRotation.X);
							//		FloatChannels[4]->AddLinearKey(KeyTime, LocalRotation.Y);
							//		FloatChannels[5]->AddLinearKey(KeyTime, LocalRotation.Z);
							//		FloatChannels[6]->AddLinearKey(KeyTime, Scale.X);
							//		FloatChannels[7]->AddLinearKey(KeyTime, Scale.Y);
							//		FloatChannels[8]->AddLinearKey(KeyTime, Scale.Z);
							//	}
							//}
						}
					}
				}

				if (MovieBinding.GetName().Equals("CineCameraActor"))
				{
					for (UMovieSceneTrack* Track : MovieBinding.GetTracks())
					{
						UMovieScene3DTransformTrack* TransformTrack = Cast<UMovieScene3DTransformTrack>(Track);
						if (TransformTrack)
						{
							UMovieSceneTrack* DuplicatedTrack = Cast<UMovieSceneTrack>(StaticDuplicateObject(Track, LevelSequence->GetMovieScene()));
							NewActorBinding->AddTrack(*DuplicatedTrack);
						}
					}
				}
			}

			
		}*/

		LevelSequenceActor->GetSequencePlayer()->Play();

		OnPlayAni();
		PlayAniDynamicCallback.Unbind();
		PlayAniDynamicCallback = Callback;
		break;
	}
}

void AMES_CameraController::OnPlayAni()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->SetViewTargetWithBlend(LevelSequenceActor);
	}

	FCameraActionCompleteDynamicDelegate SelfCallback;
	SelfCallback.BindUFunction(this, "PlayAniSelfCallback");
	if (!LevelSequenceActor->GetSequencePlayer()->OnFinished.Contains(SelfCallback))
	{
		LevelSequenceActor->GetSequencePlayer()->OnFinished.Add(SelfCallback);
	}

	if (DisableInputWhenPlayAni)
	{
		FreezeCamera();
	}
	ChangeCameraMoveType(ECameraMoveType::PlayAni);
	IsPlayingAnimation = true;
}

void AMES_CameraController::OnPauseAni()
{
	ChangeCameraMoveType(ECameraMoveType::None);
}

void AMES_CameraController::CheckMoveStateOnAni()
{
	if (IsMovingToPos())
	{
		BlendTimeToGo = 0.f;
	}
}

void AMES_CameraController::CheckAniStateOnMove()
{
	if (IsPlayingAni())
	{
		ResetAni(false);
	}
}

bool AMES_CameraController::CouldMoveAndRotate()
{
	return !IsFreezing;
}

void AMES_CameraController::UpdateLocation(FVector NewLocation)
{
	SetActorLocation(NewLocation);
}

void AMES_CameraController::UpdateRotation(FRotator NewRotation)
{
	if (GetController() != nullptr)
	{
		GetController()->SetControlRotation(NewRotation);
	}
}

void AMES_CameraController::OnInputMoveOrRotate(bool IsMove)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController && PlayerController->GetViewTarget() != this)
	{
		PlayerController->SetViewTargetWithBlend(this);
	}
}

bool AMES_CameraController::ShouldFixMousePosWhenOnScreenEdge_Implementation()
{
	return true;
}
