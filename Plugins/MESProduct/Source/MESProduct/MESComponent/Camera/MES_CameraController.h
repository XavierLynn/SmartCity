// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MES_CameraController.generated.h"

DECLARE_DYNAMIC_DELEGATE(FCameraActionCompleteDynamicDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCameraActionCompleteDynamicMultiDelegate);
DECLARE_DELEGATE(FCameraActionCompleteDelegate);
DECLARE_MULTICAST_DELEGATE(FCameraActionCompleteMultiDelegate);


UENUM(BlueprintType)
enum class EMESCameraControllerRotateAroundMode : uint8
{
	CameraForward, // camera forward trace to actor location.
	MouseForward, // mouse forward trace to actor location.
	SpecifyPoint // should use UpdateRotateAroundPoint() to set this point to rotate around.
};


UCLASS()
class MESPRODUCT_API AMES_CameraController : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMES_CameraController();

	enum class ECameraMoveType
	{
		None,
		MoveToPos,
		MoveToBound,
		PlayAni
	};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveToBound(AActor* FocusTarget, float FocusTime);

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	UFUNCTION(BlueprintCallable)
		void SetPos(FVector Location, FRotator Rotation);
	UFUNCTION(BlueprintCallable)
		void MoveToPos(FVector Location, FRotator Rotation, float Seconds, FCameraActionCompleteDynamicDelegate Callback);
	UFUNCTION(BlueprintCallable)
		void MoveToBound(struct FBoxSphereBounds Bounds, float Seconds, FCameraActionCompleteDynamicDelegate Callback);
	UFUNCTION(BlueprintCallable)
		bool IsMovingToPos();

	UFUNCTION(BlueprintCallable)
		void PlayAni(FString AniName, FCameraActionCompleteDynamicDelegate Callback);
	UFUNCTION(BlueprintCallable)
		void PauseAni();
	UFUNCTION(BlueprintCallable)
		void ResetAni(bool IsContinueToPlay);
	UFUNCTION(BlueprintCallable)
		bool IsPlayingAni();
		// DurationPercent: 0 - 1
	UFUNCTION(BlueprintCallable)
		void JumpToAni(FString AniName, float DurationPercentage, bool IsContinueToPlay, FCameraActionCompleteDynamicDelegate Callback);
	UFUNCTION(BlueprintCallable)
		float GetAniPercentage();

	UFUNCTION(BlueprintCallable)
		void FreezeCamera();
	UFUNCTION(BlueprintCallable)
		void UnfreezeCamera();

	UFUNCTION(BlueprintCallable)
		void UpdateRotateAroundMode(EMESCameraControllerRotateAroundMode NewMode);
	UFUNCTION(BlueprintCallable)
		void UpdateRotateAroundPoint(FVector SpecifyPoint);

	UFUNCTION(BlueprintNativeEvent)
		void GetMousePos(FVector2D& MousePos);

	// Should we will fix mouse position to the other side of screen when mouse cursor is near the screen edge.
	// Default is true. Blueprint could override it.
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
		bool ShouldFixMousePosWhenOnScreenEdge();

	UFUNCTION()
		void TestDynamicCallback();

	template<typename UserClass>
	void MoveToPos(FVector WorldPos, float Seconds, UserClass* Object,
			typename FCameraActionCompleteDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr Func);
	
	template<typename UserClass>
	void MoveToBound(struct FBoxSphereBounds Bound, float Seconds, UserClass* Object,
			typename FCameraActionCompleteDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr Func);

	template<typename UserClass>
	void PlayAni(FString AniName, UserClass* Object,
			typename FCameraActionCompleteDelegate::TUObjectMethodDelegate< UserClass >::FMethodPtr Func);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCapsuleComponent* CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UMES_CameraMovement* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MoveEdgeCheckMultiplier = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MoveScaleMultiplier = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TraceMaxDistance = 1000000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TraceDistanceNoHit = 10000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotateAroundYawMultiplier = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotateAroundPitchMultiplier = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotateAroundPitchAngleMax = 85.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RotateAroundPitchAngleMin = -85.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool CouldRotateAroundPitch = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EMESCameraControllerRotateAroundMode RotateAroundMode = EMESCameraControllerRotateAroundMode::CameraForward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MoveZoomFactor = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<enum EViewTargetBlendFunction> BlendFunction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FocusToTime = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BlendExp = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<class ULevelSequence*> CameraAnis;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool DisableInputWhenPlayAni = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName LineTraceProfileName = "MESCamera";

	UPROPERTY(BlueprintReadWrite)
	bool IsPrepareFocus;

private:
	bool IsMoveKeyDown;
	bool IsRotateSelfKeyDown;
	bool IsRotateAroundKeyDown;
	bool IsRotateAroundExtraKeyDown;

	FVector RotateAroundPos;
	FVector MouseInputAxis;

	FVector FocusPosition;
	FRotator FocusRotation;
	FVector FocusStartPosition;
	FRotator FocusStartRotation;

	float BlendTimeToGo;
	float BlendTimeCache;

	FCameraActionCompleteDelegate MoveToPosCallback;
	FCameraActionCompleteDelegate MoveToBoundCallback;
	FCameraActionCompleteDelegate PlayAniCallback;

	FCameraActionCompleteDynamicDelegate MoveToPosDynamicCallback;
	FCameraActionCompleteDynamicDelegate MoveToBoundDynamicCallback;
	FCameraActionCompleteDynamicDelegate PlayAniDynamicCallback;

	FCameraActionCompleteMultiDelegate MoveToPosCallbacks;
	FCameraActionCompleteMultiDelegate MoveToBoundCallbacks;
	FCameraActionCompleteMultiDelegate PlayAniCallbacks;

	FCameraActionCompleteDynamicMultiDelegate MoveToPosDynamicCallbacks;
	FCameraActionCompleteDynamicMultiDelegate MoveToBoundDynamicCallbacks;
	FCameraActionCompleteDynamicMultiDelegate PlayAniDynamicCallbacks;

	ECameraMoveType CameraMoveType = ECameraMoveType::None;

	bool IsFreezing;
	bool IsPlayingAnimation;

	UPROPERTY()
		class ALevelSequenceActor* LevelSequenceActor;

private:
	UFUNCTION(BlueprintCallable)
		void MoveForward(float AxisValue);
	UFUNCTION(BlueprintCallable)
		void MoveRight(float AxisValue);
	UFUNCTION(BlueprintCallable)
		void MoveUp(float AxisValue);
	UFUNCTION(BlueprintCallable)
		void MoveScale(float AxisValue);

	UFUNCTION(BlueprintCallable)
		void MouseX(float AxisValue);
	UFUNCTION(BlueprintCallable)
		void MouseY(float AxisValue);

	UFUNCTION(BlueprintCallable)
		void MoveKeyDown();
	UFUNCTION(BlueprintCallable)
		void MoveKeyUp();

	UFUNCTION(BlueprintCallable)
		void RotateSelfKeyDown();
	UFUNCTION(BlueprintCallable)
		void RotateSelfKeyUp();

	UFUNCTION(BlueprintCallable)
		void RotateAroundKeyDown();
	UFUNCTION(BlueprintCallable)
		void RotateAroundKeyUp();

	UFUNCTION(BlueprintCallable)
		void RotateAroundExtraKeyDown();
	UFUNCTION(BlueprintCallable)
		void RotateAroundExtraKeyUp();

	UFUNCTION(BlueprintCallable)
		void FocusDown();
	UFUNCTION(BlueprintCallable)
		void FocusUp();

	UFUNCTION(BlueprintCallable)
		static bool IsWithEditor();

	void UpdateRotateAround();
	void CheckRotateAroundMode();
	void RayTraceForward(FVector& OutPoint);
	// Used for rotate around action when EMESCameraControllerRotateAroundMode is MouseForward.
	void RayTraceToMouse(FVector& OutPoint);
	// Used to focus to actor after focus key up.
	UFUNCTION(BlueprintCallable) AActor* RayTraceToMouse();

	bool DeprojectMousePositionToWorld(FVector& WorldLocation, FVector& WorldDirection);

	void InvokeCallback(ECameraMoveType InCameraMoveType);

	UFUNCTION()
		void PlayAniSelfCallback();

	void SpawnLevelSequenceActor();

	void ChangeCameraMoveType(ECameraMoveType NewMoveType);

	void InitializeSequence(FString AniName, FCameraActionCompleteDynamicDelegate Callback);
	void OnPlayAni();
	void OnPauseAni();

	void CheckMoveStateOnAni();
	void CheckAniStateOnMove();

	bool CouldMoveAndRotate();

	void UpdateLocation(FVector NewLocation);
	void UpdateRotation(FRotator NewRotation);
	// IsMove: true = Move, false = Rotate
	void OnInputMoveOrRotate(bool IsMove);
};