// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Windows.h"
#include "ShellWindowActor.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API AShellWindowActor : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		void LaunchExe();



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString URL = "D:\\work\\OtherProjects\\Genetec\\Genetec.SDK.ControlInterface.Test\\bin\\Debug\\Genetec.SDK.ControlInterface.Test.exe";
	// Username, Password, ServerAddress, CameraGuid
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FString> CmdArgs; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TimeToWaitWindow = 0.2f;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

private:
	static HWND hWnd_win;
	static TMap<int, TArray<HWND>> ThirdWndArray;

	WNDCLASSEX WndClassEx;
	FProcHandle ProcHandle;
	uint32 ProcessId;

	static LRESULT CALLBACK MainWndProc(HWND hWnd, uint32 Msg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
	static BOOL CALLBACK EnumChildWindowsProc(HWND hwnd, LPARAM lParam);

	static void ShowErrorMessage();

	FTimerHandle TimerHandle;
	UFUNCTION()
		void TimerCallback();
	UFUNCTION(BlueprintCallable)
		void MoveThirdWindowTo(FVector2D NewPos, int NewWidth, int NewHeight);

	void OnWindowSizeChanged(class FViewport* Viewport, uint32 Meaningless);

	void Test();
};
