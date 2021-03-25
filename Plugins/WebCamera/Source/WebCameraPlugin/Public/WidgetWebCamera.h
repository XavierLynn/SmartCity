/************************************************************************/
/* Author: YWT20                                                        */
/* Expected release year : 2020                                         */
/************************************************************************/

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Kismet/KismetSystemLibrary.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"	
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "Components/Image.h"
#include "WidgetWebCamera.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWebCameraState, FString, URL);

UCLASS()
class UWidgetWebCamera : public UUserWidget
{
	GENERATED_BODY()
public:
	UWidgetWebCamera(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Meta = (BindWidget), Category = "WebCamera")
		UImage* Image_WebCamera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "WebCamera")
		bool bIsAutoPlay;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "WebCamera")
		FString CameraURL;

	// If the WebCamera images should be resized every frame
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "WebCamera")
		bool ShouldResize;

	// The targeted resize width and height (width, height)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "WebCamera")
		FVector2D ResizeDeminsions;

	// The rate at which the color data array and video texture is updated (in frames per second)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, Category = "WebCamera")
		float RefreshRate;

	// The refresh timer
	UPROPERTY(BlueprintReadWrite, Category = "WebCamera")
		float RefreshTimer;

	// If the stream has successfully opened yet
	UPROPERTY(BlueprintReadOnly, Category = "WebCamera")
		bool isStreamOpen;

	// The videos width and height (width, height)
	UPROPERTY(BlueprintReadWrite, Category = "WebCamera")
		FVector2D VideoSize;

	// The current video frame's corresponding texture
	UPROPERTY(BlueprintReadOnly, Category = "WebCamera")
		UTexture2D* VideoTexture;

	// The current data array
	UPROPERTY(BlueprintReadOnly, Category = "WebCamera")
		TArray<FColor> Data;

	UPROPERTY(BlueprintAssignable, Category = "WebCamera")
		FWebCameraState OnOpenWebcamSuccessfully;

	UPROPERTY(BlueprintCallable,BlueprintAssignable, Category = "WebCamera")
		FWebCameraState OnOpenWebcamFailed;

	// OpenCV fields
	cv::Mat frame;
	cv::VideoCapture stream;
public:
	// Blueprint Event called every time the video frame is updated
	UFUNCTION(BlueprintNativeEvent, Category = "WebCamera")
		void OnUpdateWebCameraTexture(UTexture2D* InVideoTexture);

	UFUNCTION(BlueprintNativeEvent, Category = "WebCamera")
		void OpenWebCamera(const FString& InCameraURL);

	// OpenCV prototypes
	void UpdateFrame();
	void UpdateTexture();

protected:
	void UpdateTextureRegions(UTexture2D* Texture, int32 MipIndex, uint32 NumRegions, FUpdateTextureRegion2D* Regions, uint32 SrcPitch, uint32 SrcBpp, uint8* SrcData, bool bFreeData);

	// Pointer to update texture region 2D struct
	FUpdateTextureRegion2D* VideoUpdateTextureRegion;
};
